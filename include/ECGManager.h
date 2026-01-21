#ifndef ECGMANAGER_H
#define ECGMANAGER_H

#include <Arduino.h>

class ECGManager {
  private:
    int outputPin;
    int loPlus;
    int loMinus;
    
    // variables for heart beat detection
    unsigned long lastPeakTime = 0;
    int threshold = 2800; // Adjust this if detection is too sensitive
    long currentInterval = 0;

  public:
    ECGManager(int output, int lp, int lm) {
        outputPin = output;
        loPlus = lp;
        loMinus = lm;
    }

    void begin() {
        pinMode(loPlus, INPUT);  // Pin 38
        pinMode(loMinus, INPUT); // Pin 37
    }

    // Decision 1: Is the hardware connected?
    bool isConnected() {
        if ((digitalRead(loPlus) == HIGH) || (digitalRead(loMinus) == HIGH)) {
            return false;
        }
        return true;
    }

    // Decision 2: Find the electrical spike (R-Peak)
    bool detectBeat(int rawVal) {
        if (rawVal > threshold && (millis() - lastPeakTime > 300)) {
            currentInterval = millis() - lastPeakTime;
            lastPeakTime = millis();
            return true; 
        }
        return false;
    }

    // Decision 3: Translate Interval into Stress Level
    String getStressLevel() {
        if (currentInterval == 0) return "Sensing...";
        if (currentInterval > 850) return "Low (Relaxed)";
        if (currentInterval > 700) return "Moderate";
        return "High Stress";
    }

    // Decision 4: Translate timing into Rhythm Status
    String getRhythmStatus() {
        static long lastStoredInterval = 0;
        if (currentInterval == 0) return "Analysing...";
        
        // Compare current beat timing to the previous one
        long diff = abs(currentInterval - lastStoredInterval);
        lastStoredInterval = currentInterval;

        if (diff < 100) return "Stable"; // Very little variation
        return "Irregular";            // High variation detected
    }

    int getGraphValue(int screenHeight) {
        if (!isConnected()) return screenHeight / 2;
        int raw = analogRead(outputPin);
        // Map 12-bit ESP32 ADC (0-4095) to screen pixels
        return map(raw, 0, 4095, screenHeight - 2, 12); 
    }
};

#endif