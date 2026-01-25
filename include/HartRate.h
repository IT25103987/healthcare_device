#ifndef HARTRATE_H
#define HARTRATE_H

#include <Arduino.h>
#include <Wire.h>
#include <MAX30105.h>
#include <heartRate.h>

#define RATE_SIZE 4 // Reduced size for faster feedback

// --- DATA STRUCTURES ---
struct HeartData {
  long irValue;
  float currentBpm; 
  float avgBpm;     
  float threeMinAvg; 
  String status;    
  int alertLevel;   
};

class HartRate {
private:
    MAX30105 particleSensor;
    
    // Beat Detection
    long lastBeat = 0;
    float bpm = 0;
    float avgBpm = 0;
    byte rates[RATE_SIZE];
    byte rateSpot = 0;
    
    // 3-Minute Average Logic
    float sumBpm3Min = 0;
    int countBpm3Min = 0;
    unsigned long periodStartTime = 0;
    float finalThreeMinAvg = 0;
    
    // Timers
    unsigned long startTime = 0;
    bool isStabilized = false;

public:
    void begin() {
        // NOTE: Wire.begin() should happen in Main Setup, not here, to avoid conflicts.
        // If you haven't initialized Wire in main, uncomment the next line:
        // Wire.begin(); 

        if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) { // Use FAST (400kHz) for better data
            Serial.println("❌ MAX30102 NOT FOUND");
        }

        // Setup sensor for high sensitivity
        particleSensor.setup(); 
        particleSensor.setPulseAmplitudeRed(0x0A); // Low Red (visual)
        particleSensor.setPulseAmplitudeGreen(0);  // Turn off Green
        
        startTime = millis();
        periodStartTime = millis();
    }

    HeartData update() {
        HeartData data;
        long irValue = particleSensor.getIR();
        
        data.irValue = irValue;
        data.currentBpm = bpm;
        data.avgBpm = avgBpm;
        
        // Default safe values
        data.threeMinAvg = (finalThreeMinAvg > 0) ? finalThreeMinAvg : avgBpm; 
        data.alertLevel = 0;
        data.status = "ANALYSING"; // Default state

        // 1. Check for Finger (Threshold = 50,000)
        if (irValue < 50000) { 
            avgBpm = 0;
            bpm = 0;
            rateSpot = 0; // Reset smoother
            data.status = "NO FINGER";
            data.alertLevel = 0; // Ensure no alert triggers when finger is removed
            return data;
        }

        // 2. Detect Beat
        if (checkForBeat(irValue)) {
            long delta = millis() - lastBeat;
            lastBeat = millis();

            // Filter Noise: Beats must be between 30 and 220 BPM
            if (delta > 250 && delta < 2000) { 
                bpm = 60000.0 / delta;
                
                // Fill buffer for smoothing
                rates[rateSpot++] = (byte)bpm;
                rateSpot %= RATE_SIZE;
                
                // Calculate Immediate Average
                avgBpm = 0;
                for (byte x = 0; x < RATE_SIZE; x++) avgBpm += rates[x];
                avgBpm /= RATE_SIZE;

                // --- 3-MINUTE ACCUMULATION ---
                // Wait 10 seconds (not 1 min) for signal to settle
                if (millis() - startTime > 10000) {
                    sumBpm3Min += avgBpm;
                    countBpm3Min++;
                }
            }
        }

        // 3. Process 3-Minute Cycle
        if (millis() - periodStartTime > 180000) { // 3 mins passed
             if (countBpm3Min > 0) {
                 finalThreeMinAvg = sumBpm3Min / countBpm3Min;
             }
             // Reset
             sumBpm3Min = 0; 
             countBpm3Min = 0;
             periodStartTime = millis();
        }

        // 4. Decide which number to use for Status
        // If we have a long-term average, use it. If not, use the current average.
        float valToGrade = (finalThreeMinAvg > 0) ? finalThreeMinAvg : avgBpm;

        // Only evaluate if we actually have a valid number
        if (valToGrade > 10) {
            evaluateStatus(data, valToGrade);
        }

        return data;
    }

    // Logic from your specific table
    void evaluateStatus(HeartData &data, float avg) {
        data.threeMinAvg = avg;

        // SAFETY: Ignore impossibly low readings (Sensor noise often reads as < 10)
        if (avg < 30) {
            data.status = "CALCULATING"; // Don't alert yet
            data.alertLevel = 0;
            return; 
        }

        if (avg < 40) {
            data.status = "CRITICAL LOW";
            data.alertLevel = 2; 
        }
        else if (avg >= 40 && avg <= 49) {
            data.status = "LOW (Warning)";
            data.alertLevel = 1; 
        }
        else if (avg >= 50 && avg <= 90) {
            data.status = "NORMAL";
            data.alertLevel = 0; 
        }
        else if (avg >= 91 && avg <= 109) {
            data.status = "ELEVATED";
            data.alertLevel = 1; 
        }
        else if (avg >= 110) {
            data.status = "CRITICAL HIGH";
            data.alertLevel = 2; 
        }
    }
};

#endif