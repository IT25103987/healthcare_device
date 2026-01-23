#ifndef HARTRATE_H
#define HARTRATE_H

#include <Arduino.h>
#include <Wire.h>
#include <MAX30105.h>
#include <heartRate.h>

#define SDA_PIN 8
#define SCL_PIN 9
#define RATE_SIZE 6 // For immediate smoothing

// --- DATA STRUCTURES ---
struct HeartData {
  long irValue;
  float currentBpm; // Instant reading
  float avgBpm;     // Smooth immediate average
  float threeMinAvg; // The important 3-minute average
  String status;    // "NORMAL", "CRITICAL HIGH", etc.
  int alertLevel;   // 0=None, 1=Warning (Vibrate), 2=Critical (Vibrate+Buzz)
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
        Wire.begin(SDA_PIN, SCL_PIN);
        Wire.setClock(50000); // 400kHz might be too fast for long wires, 50k is safe

        if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD)) {
            Serial.println("❌ MAX30102 NOT FOUND");
            while (1);
        }

        particleSensor.setup(60, 4, 2, 100, 411, 4096); 
        particleSensor.setPulseAmplitudeGreen(0);
        
        startTime = millis();
        periodStartTime = millis();
    }

    HeartData update() {
        HeartData data;
        long irValue = particleSensor.getIR();
        
        data.irValue = irValue;
        data.currentBpm = bpm;
        data.avgBpm = avgBpm;
        data.threeMinAvg = finalThreeMinAvg;
        data.alertLevel = 0;
        data.status = "WAITING";

        // 1. Check for Finger
        if (irValue < 50000) { // Adjusted threshold (5000 is often too low)
            avgBpm = 0;
            bpm = 0;
            data.status = "NO FINGER";
            return data;
        }

        // 2. Detect Beat
        if (checkForBeat(irValue)) {
            long delta = millis() - lastBeat;
            lastBeat = millis();

            if (delta > 300 && delta < 2000) { // 30 BPM to 200 BPM range
                bpm = 60000.0 / delta;
                
                // Immediate Smoothing
                rates[rateSpot++] = (byte)bpm;
                rateSpot %= RATE_SIZE;
                avgBpm = 0;
                for (byte i = 0; i < RATE_SIZE; i++) avgBpm += rates[i];
                avgBpm /= RATE_SIZE;

                // --- 3-MINUTE ACCUMULATION ---
                // Only count if stabilized (after 1 min)
                if (millis() - startTime > 60000) {
                    isStabilized = true;
                    sumBpm3Min += avgBpm;
                    countBpm3Min++;
                }
            }
        }

        // 3. Process 3-Minute Cycle
        if (isStabilized && (millis() - periodStartTime > 180000)) { // 180,000ms = 3 mins
             if (countBpm3Min > 0) {
                 finalThreeMinAvg = sumBpm3Min / countBpm3Min;
                 
                 // Reset for next 3 minutes
                 sumBpm3Min = 0;
                 countBpm3Min = 0;
                 periodStartTime = millis();
                 
                 // DETERMINE STATUS BASED ON YOUR TABLE
                 evaluateStatus(data, finalThreeMinAvg);
             }
        } else if (finalThreeMinAvg > 0) {
            // Keep returning the last calculated status until the next 3 mins update
            evaluateStatus(data, finalThreeMinAvg);
        }

        return data;
    }

    
 

    // Logic from your specific table
    void evaluateStatus(HeartData &data, float avg) {
        data.threeMinAvg = avg;

        if (avg < 40) {
            data.status = "CRITICAL LOW";
            data.alertLevel = 2; // Vibrate + Buzzer
        }
        else if (avg >= 40 && avg <= 49) {
            data.status = "LOW (Warning)";
            data.alertLevel = 1; // Vibrate Only
        }
        else if (avg >= 50 && avg <= 90) {
            data.status = "NORMAL";
            data.alertLevel = 0; // All Good
        }
        else if (avg >= 91 && avg <= 109) {
            data.status = "ELEVATED";
            data.alertLevel = 1; // Vibrate Only
        }
        else if (avg >= 110) {
            data.status = "CRITICAL HIGH";
            data.alertLevel = 2; // Vibrate + Buzzer
        }
    }
};

#endif