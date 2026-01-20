#ifndef WEATHERMANAGER_H
#define WEATHERMANAGER_H

#include <Adafruit_BMP280.h>

class WeatherManager {
  private:
    Adafruit_BMP280 bmp;
    bool bmpOK = false;
    float lastPressure = 0;
    float initialAltitude = 0;
    unsigned long lastForecastUpdate = 0;
    String currentForecast = "Stabilizing...";

  public:
    WeatherManager() {}

    // Initialize the sensor
    bool begin() {
        if (bmp.begin(0x76)) { // Standard address for most modules
            bmpOK = true;
            initialAltitude = bmp.readAltitude(1013.25);
            lastPressure = bmp.readPressure() / 100.0F;
            return true;
        }
        return false;
    }

    // Main update logic (call this in your loop)
    void update() {
        if (!bmpOK) return;

        // Update forecast every 10 minutes (600,000 ms)
        if (millis() - lastForecastUpdate > 600000) {
            float currentPressure = bmp.readPressure() / 100.0F;
            
            if (currentPressure < lastPressure - 1.0) {
                currentForecast = "Storm Warning!";
            } else if (currentPressure > lastPressure + 1.0) {
                currentForecast = "Clear Skies";
            } else {
                currentForecast = "Stable";
            }
            
            lastPressure = currentPressure;
            lastForecastUpdate = millis();
        }
    }

    // --- Data Getters ---

    float getTemperature() {
        return bmpOK ? bmp.readTemperature() : 0.0;
    }

    float getPressure() {
        return bmpOK ? bmp.readPressure() / 100.0F : 0.0;
    }

    float getAltitude() {
        return bmpOK ? bmp.readAltitude(1013.25) : 0.0;
    }

    // Decision: Calculate how many floors you've climbed since start
    int getFloorsClimbed() {
        if (!bmpOK) return 0;
        float currentAlt = getAltitude();
        float diff = currentAlt - initialAltitude;
        // Average floor height is ~3 meters
        int floors = (int)(diff / 3.0);
        return (floors > 0) ? floors : 0;
    }

    String getForecast() {
        return currentForecast;
    }

    bool isConnected() {
        return bmpOK;
    }
};

#endif