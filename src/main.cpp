
#include <Wire.h>
#include <Arduino.h>

// --- INCLUDE CUSTOM MODULES ---
#include "TimeManager.h"
#include "State.h"
#include "HartRate.h"
#include "WeatherManager.h"
#include "ECGManager.h"
#include "ActivityManager.h"
#include "Vibrate.h"
#include "DisplayManager.h"
#include "Firebase/FirebaseManager.h"
#include "Firebase/FireBaseSndOBJ.h"

// --- PIN DEFINITIONS ---
#define SDA_PIN 8
#define SCL_PIN 9

// Buttons
#define BTN_POWER 10 // Toggle Watch State
#define BTN_MODE 11  // Switch Screens
#define BTN_SOS 12   // Long Press for Emergency

// ECG Pins
#define ECG_INPUT 36 // VP
#define ECG_LO_P 38
#define ECG_LO_N 37

// --- OBJECT INSTANTIATION ---
TimeManager timeManager;
HartRate heartMonitor;
WeatherManager weather;
ActivityManager activity;
Vibrate vibrate;
ECGManager ecg(ECG_INPUT, ECG_LO_P, ECG_LO_N);
State watchState;            // Handles On/Off state
DisplayManager displayMgr;   // Handles all OLED drawing
FirebaseManager firebaseMgr; // Handles Firebase interactions
FireBaseSndOBJ fireBOBJ;

// --- GLOBAL VARIABLES ---
int window = 0; // 0=Clock, 1=Pulse, 2=Weather, 3=ECG, 4=Steps
bool lastModeBtnState = HIGH;

// Button Timers
unsigned long pwrPressedTime = 0;
bool pwrIsPressing = false;
bool pwrHandled = false;

unsigned long sosPressedTime = 0;
bool sosIsPressing = false;
bool sosHandled = false;

// Alert Flags
bool fallDetected = false;
bool fallReported = false; // Track if fall was already sent to cloud

// heartrate encapsulated Data
HeartData hData;

/* =================================================================
 * SETUP
 * ================================================================= */
void setup()
{
    Serial.begin(115200);
    delay(1000);

    // 1. Initialize I2C Bus
    // IMPORTANT: Wire.begin must happen before sensors use it
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(100000);

    Serial.println("--- SYSTEM STARTUP ---");

    // 2. Initialize Modules
    displayMgr.begin();
    timeManager.begin();

    if (weather.begin())
        Serial.println("✅ BMP280 Ready");
    else
        Serial.println("❌ BMP280 FAILED");

    ecg.begin();
    activity.begin();     // Sets up MPU6050
    heartMonitor.begin(); // Sets up MAX30102

    // 3. Setup Buttons
    pinMode(BTN_POWER, INPUT_PULLUP);
    pinMode(BTN_MODE, INPUT_PULLUP);
    pinMode(BTN_SOS, INPUT_PULLUP);

    // 4. Initial State
    watchState.setState(false); // Start Powered Off
    vibrate.vibrateFor(200);    // Startup haptic feedback
    firebaseMgr.begin();

    displayMgr.clear();
    Serial.println("🚀 SYSTEM READY");
}

/* =================================================================
 * HELPER LOGIC
 * ================================================================= */

// Logic: Check Power Button (Long Press to Toggle On/Off)
void checkPowerButton()
{
    int state = digitalRead(BTN_POWER);

    if (state == LOW)
    { // Pressed
        if (!pwrIsPressing)
        {
            pwrIsPressing = true;
            pwrPressedTime = millis();
            pwrHandled = false;
        }
        else
        {
            if ((millis() - pwrPressedTime >= 2000) && !pwrHandled)
            {
                // Long press detected (>2s)
                bool newState = !watchState.getState();
                watchState.setState(newState);

                vibrate.vibrateFor(300);
                pwrHandled = true;

                if (newState)
                {
                    Serial.println("Power ON");
                }
                else
                {
                    Serial.println("Power OFF");
                    displayMgr.clear();
                    displayMgr.update();
                }
            }
        }
    }
    else
    { // Released
        pwrIsPressing = false;
    }
}

// Logic: Check SOS Button (Long Press for Emergency)
void checkSOSButton()
{
    int state = digitalRead(BTN_SOS);

    if (state == LOW)
    { // Pressed
        if (!sosIsPressing)
        {
            sosIsPressing = true;
            sosPressedTime = millis();
            sosHandled = false;
        }
        else
        {
            if ((millis() - sosPressedTime >= 3000) && !sosHandled)
            {
                // Trigger SOS Animation
                displayMgr.animateEmergencyCall(vibrate);
                sosHandled = true;
            }
        }
    }
    else
    { // Released
        sosIsPressing = false;
    }
}

// Logic: Process Heart Rate Alerts
void processHeartAlerts(HeartData data)
{
    if (data.threeMinAvg > 0)
    {
        // Critical Level (Flash Screen + Vibrate)
        if (data.alertLevel == 2)
        {
            displayMgr.showHeartAlert(data.status, data.threeMinAvg, 2, vibrate);
        }
        // Warning Level (Static Screen + Vibrate)
        else if (data.alertLevel == 1)
        {
            displayMgr.showHeartAlert(data.status, data.threeMinAvg, 1, vibrate);
        }
    }
}
void sendTOCLOUD(bool fallAlet, bool hartAlet) {
    fireBOBJ.setSteps(activity.getSteps());
    fireBOBJ.setState(watchState.getState());
    fireBOBJ.setTemp(weather.getTemperature());
    fireBOBJ.setPressure(weather.getPressure());
    fireBOBJ.setAltitude(weather.getAltitude());
    fireBOBJ.setFloorsClimbed(weather.getFloorsClimbed());
    fireBOBJ.setForecast(weather.getForecast());
    
    // Vital Stats
    fireBOBJ.setAvgBpm(hData.avgBpm);
    fireBOBJ.setFallDetec(fallAlet);

    // LOGIC CHANGE:
    // If this is a Fall (fallAlet) or Critical Heart (hartAlet), 
    // we set 'isEmergency' to TRUE.
    bool isEmergency = fallAlet || hartAlet;

    // Pass 'isEmergency' to the updated function
    // If TRUE, it ignores the timer and sends INSTANTLY.
    firebaseMgr.update(fireBOBJ, isEmergency);
}

/* =================================================================
 * MAIN LOOP
 * ================================================================= */
void loop()
{
    // 1. Always update background sensors (Critical for accurate readings)
    timeManager.update();
    activity.update();
    vibrate.update();

    // Get latest heart data and check for health alerts
    hData = heartMonitor.update();
    processHeartAlerts(hData);

    // 2. Check Global Events (Fall / SOS / Power)

    // Fall Detection
    if (activity.isFallDetected() && !fallReported)
    {
        sendTOCLOUD(true, false);
        fallReported = true;  // Mark as reported

        // If OFF, wake up? Optional. For now, we assume it alerts if ON.
        displayMgr.animateFallAlert(vibrate, BTN_POWER); // Pass Power Button as Cancel button
        
        activity.clearFallDetected();  // Clear the flag after handling
    }
    else if (!activity.isFallDetected())
    {
        fallReported = false;  // Reset flag when fall is no longer detected
    }   

    checkPowerButton();
    checkSOSButton();

    // 3. Main UI Logic (Only runs if Watch is "ON")
    if (watchState.getState())
    {

        sendTOCLOUD(false, false);

        // --- Mode Switching (Button 2) ---
        int currentModeBtn = digitalRead(BTN_MODE);
        if (currentModeBtn == LOW && lastModeBtnState == HIGH)
        {
            window++;
            if (window > 4)
                window = 0;

            vibrate.vibrateFor(500); // Haptic 'click'
            displayMgr.clear();      // Clean transition
            delay(50);               // Debounce
        }
        lastModeBtnState = currentModeBtn;

        // --- Render Active Screen ---
        switch (window)
        {
        case 0: // Clock / Home
            displayMgr.showStartScreen(timeManager.getDateString(), timeManager.getTimeString());
            break;

        case 1: // Heart Rate
            displayMgr.showHeartRateScreen(hData);
            break;

        case 2:               // Weather
            weather.update(); // Update sensor data before showing
            displayMgr.showWeatherScreen(weather);
            break;

        case 3: // ECG
            displayMgr.showECGScreen(ecg);
            break;

        case 4: // Steps
            displayMgr.showStepsScreen(activity);
            break;
        }
    }
    else
    {
        // Power Saving State
        // Ensure screen is black
        // (You could add deep sleep logic here later)
    }

    // Small delay to stabilize I2C and loop timing
    delay(10);
}