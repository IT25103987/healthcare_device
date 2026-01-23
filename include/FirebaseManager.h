#ifndef FIREBASEMANAGER_H
#define FIREBASEMANAGER_H

#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

class FirebaseManager {
  private:
    const char* wifiSSID="Dialog 4G 048";
    const char* wifiPass="426cd7F1";
    const char* apiKey="BNxRzh_3ee0QpYpL416FkIRuD3sUx28d5tcH7nudvs9-UQ2pxnthI5PNxg_RGFgqwHULBgv_X9tjClJbW6GXQzA";
    const char* dbUrl="https://health-care-watch-project-default-rtdb.asia-southeast1.firebasedatabase.app/";

    FirebaseData fbdo;
    FirebaseAuth auth;
    FirebaseConfig config;
    
    unsigned long lastSendTime = 0;
    const int sendInterval = 10000; // Send every 10 seconds
    bool signupOK = false;

//   public:
//     FirebaseManager(const char* ssid, const char* pass, const char* api, const char* url) {
//         wifiSSID = ssid;
//         wifiPass = pass;
//         apiKey = api;
//         dbUrl = url;
//     }
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
        Serial.println("\n✅ Connected: " + WiFi.localIP().toString());

        // 2. Configure Firebase
        config.api_key = apiKey;
        config.database_url = dbUrl;

        // 3. Sign up as anonymous user
        if (Firebase.signUp(&config, &auth, "", "")) {
            Serial.println("✅ Firebase Login Success");
            signupOK = true;
        } else {
            Serial.printf("❌ Firebase Login Failed: %s\n", config.signer.signupError.message.c_str());
        }

        // 4. Initialize the library
        config.token_status_callback = tokenStatusCallback; // Helper function
        Firebase.begin(&config, &auth);
        Firebase.reconnectWiFi(true);
    }

    void update(int steps, int bpm, float temp, String stress) {
        // Check if ready and if time has passed
        if (Firebase.ready() && signupOK && (millis() - lastSendTime > sendInterval || lastSendTime == 0)) {
            sendData(steps, bpm, temp, stress);
            lastSendTime = millis();
        }
    }

    void sendData(int steps, int bpm, float temp, String stress) {
        // Create a JSON object
        FirebaseJson json;
        json.set("steps", steps);
        json.set("bpm", bpm);
        json.set("temp", temp);
        json.set("stress", stress);
        
        // Add timestamp (optional, roughly based on millis)
        json.set("uptime", millis() / 1000);

        // Push to database (Push creates a new unique entry every time)
        // Path: "/HealthData"
        if (Firebase.RTDB.pushJSON(&fbdo, "/HealthData", &json)) {
            Serial.println("🔥 Data Pushed to Firebase!");
        } else {
            Serial.println("❌ Push Failed: " + fbdo.errorReason());
        }
    }
};

#endif