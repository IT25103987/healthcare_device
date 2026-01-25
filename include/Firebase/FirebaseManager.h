#ifndef FIREBASEMANAGER_H
#define FIREBASEMANAGER_H

#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <FireBaseSndOBJ.h>


// Provide the token generation process info.
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

class FirebaseManager {
  private:
    // --- YOUR CREDENTIALS ---
    // (I have hidden your keys for security. Please paste them back here)
    const char* wifiSSID = "Dialog 4G 048";
    const char* wifiPass = "426cd7F1";
    const char* apiKey   = "AIzaSyCT50wP8Zpgi4GrzqwYgV5MJvbPeLUy360";
    const char* dbUrl    = "https://health-care-watch-project-default-rtdb.asia-southeast1.firebasedatabase.app/";

    FirebaseData fbdo;
    FirebaseAuth auth;
    FirebaseConfig config;
    
    unsigned long lastSendTime = 0;
    const int sendInterval = 1000; // Faster updates (1 second) for responsiveness
    bool signupOK = false;

  public:
    FirebaseManager() {}

    void begin() {
        // 1. Connect to Wi-Fi
        WiFi.begin(wifiSSID, wifiPass);
        Serial.print("Connecting to Wi-Fi");
        while (WiFi.status() != WL_CONNECTED) {
            Serial.print(".");
            delay(300);
        }
        Serial.println("\n✅ Wi-Fi Connected: " + WiFi.localIP().toString());

        // 2. Configure Firebase
        config.api_key = apiKey;
        config.database_url = dbUrl;

        // 3. Sign up
        if (Firebase.signUp(&config, &auth, "", "")) {
            Serial.println("✅ Firebase Auth Success");
            signupOK = true;
        } else {
            Serial.printf("❌ Auth Failed: %s\n", config.signer.signupError.message.c_str());
        }

        // 4. Initialize
        // Validates token generation (required for stable connection)
        config.token_status_callback = tokenStatusCallback; 
        
        Firebase.begin(&config, &auth);
        Firebase.reconnectWiFi(true);
    }

    // Call this from your main loop
// Updated function with 'force' parameter
    void update(FireBaseSndOBJ data, bool force = false) {
        // Check if connected
        if (Firebase.ready() && signupOK) {
            // LOGIC CHANGE: 
            // Send if 'force' is TRUE -OR- if enough time has passed
            if (force || (millis() - lastSendTime > sendInterval)) {
                lastSendTime = millis(); // Reset timer
                sendData(data);
            }
        }
    }
    void sendData(FireBaseSndOBJ data) {
        FirebaseJson json;
        
        // Matches the paths in index.html
        json.set("state", data.getState());
        json.set("steps", data.getSteps());
        json.set("heartrate", data.getAvgBpm());
        json.set("spo2", 0);  // Not in FireBaseSndOBJ, set to default
        json.set("temperature", data.getTemp());
        json.set("pressure", data.getPressure());
        json.set("fallDetected", data.getFallDetec());
        json.set("altitude", data.getAltitude());
        json.set("floorsClimbed", data.getFloorsClimbed());
        json.set("forecast", data.getForecast());
        
        // Optional: Add uptime for debugging
        json.set("uptime", millis() / 1000);

        // *** IMPORTANT CHANGE ***
        // Used 'updateNode' on path "/sensor"
        // This makes the website gauges move! 
        // (Previous 'pushJSON' creates history logs, which the dashboard can't read easily)
        if (Firebase.RTDB.updateNode(&fbdo, "/sensor", &json)) {
            Serial.print("."); // Success dot
        } else {
            Serial.println("\n❌ Upload Error: " + fbdo.errorReason());
        }
        
        // OPTIONAL: If you WANT history logs AND live view, uncomment this:
        // Firebase.RTDB.pushJSON(&fbdo, "/HealthHistory", &json);
    }
};

#endif