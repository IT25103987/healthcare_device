#ifndef ACTIVITYMANAGER_H
#define ACTIVITYMANAGER_H

#include <Arduino.h>
#include <Wire.h>

class ActivityManager {
  private:
    // --- COMMON VARIABLES ---
    float magnitudePrev = 0;

    // --- FALL DETECTION VARIABLES ---
    bool fallDetected = false;
    unsigned long freefallStartTime = 0;
    int fallState = 0; 
    const float fallLowThreshold = 6000;    // "Freefall"
    const float fallHighThreshold = 25000;  // "Impact"

    // --- STEP COUNTING VARIABLES ---
    int steps = 0;
    unsigned long lastStepTime = 0;
    const float stepThreshold = 15000; // Sensitivity for walking
    const int stepDelay = 300;         // Minimum ms between steps (debounce)

  public:
    void begin() {
        Wire.beginTransmission(0x68);
        Wire.write(0x6B); 
        Wire.write(0);    
        Wire.endTransmission(true);
    }

    void update() {
        Wire.beginTransmission(0x68);
        Wire.write(0x3B); 
        Wire.endTransmission(false);
        Wire.requestFrom(0x68, 6, true);

        int16_t ax = Wire.read() << 8 | Wire.read();
        int16_t ay = Wire.read() << 8 | Wire.read();
        int16_t az = Wire.read() << 8 | Wire.read();

        // Calculate Magnitude (Total G-Force)
        float magnitude = sqrt(pow(ax, 2) + pow(ay, 2) + pow(az, 2));

        // ===========================
        // 1. STEP COUNTING LOGIC
        // ===========================
        // Check if we crossed the threshold from low to high (a "step")
        if (magnitude > stepThreshold && magnitudePrev <= stepThreshold) {
            // Check if enough time has passed since the last step
            if (millis() - lastStepTime > stepDelay) {
                steps++;
                lastStepTime = millis();
            }
        }

        // ===========================
        // 2. FALL DETECTION LOGIC
        // ===========================
        
        // A. Detect Freefall
        if (magnitude < fallLowThreshold && fallState == 0) {
            fallState = 1;
            freefallStartTime = millis();
        }

        // B. Detect Impact
        if (fallState == 1) {
            if (magnitude > fallHighThreshold) {
                fallDetected = true; 
                fallState = 0;       
            }
            
            // Timeout (Reset if impact doesn't happen quickly)
            if (millis() - freefallStartTime > 200) {
                fallState = 0;
            }
        }

        // Store current magnitude for the next loop's comparison
        magnitudePrev = magnitude;
    }

    // --- GETTERS ---

    bool isFallDetected() {
        if (fallDetected) {
            fallDetected = false; 
            return true;
        }
        return false;
    }

    // This is the method you asked for:
    int getSteps() {
        return steps;
    }
    
    // Optional: Reset steps at midnight or via menu
    void resetSteps() {
        steps = 0;
    }
};

#endif