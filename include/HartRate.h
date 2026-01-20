#ifndef HARTRATE_H
#define HARTRATE_H

#include <Arduino.h>
#include <Wire.h>
#include <MAX30105.h>
#include <heartRate.h>

#define SDA_PIN 8
#define SCL_PIN 9

// Smoothing
#define RATE_SIZE 6

// Data struct to hold heart rate readings
struct HeartData {
  long irValue;
  float bpm;
  float avgBpm;
};

class HartRate
{
private:
    MAX30105 particleSensor;
    // Beat detection
    long lastBeat = 0;
    float bpm = 0;
    float avgBpm = 0;
    byte rates[RATE_SIZE];
    byte rateSpot = 0;
    unsigned long lastPrint = 0;

public:
    void begin()
    {
        Wire.begin(SDA_PIN, SCL_PIN);
        Wire.setClock(50000);

        Serial.println("Starting MAX30102...");

        if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD))
        {
            Serial.println("❌ MAX30102 NOT FOUND");
            while (1)
                ;
        }

        Serial.println("✅ MAX30102 FOUND");

        particleSensor.setup(
            60,  // LED brightness
            4,   // sample average
            2,   // Red + IR
            100, // sample rate
            411, // pulse width
            4096 // ADC range
        );

        particleSensor.setPulseAmplitudeGreen(0);
    }

    HeartData update(){
        HeartData data;
        long irValue = particleSensor.getIR();
        data.irValue = irValue;
        data.bpm = bpm; // default, will update if beat detected
        data.avgBpm = avgBpm;

        // Finger not detected
        if (irValue < 5000)
        {
            if (millis() - lastPrint > 1000)
            {
                Serial.println("Place finger on sensor");
                lastPrint = millis();
            }
            return data;
        }

        if (checkForBeat(irValue))
        {
            long delta = millis() - lastBeat;
            lastBeat = millis();

            if (delta > 300 && delta < 2000)
            {
                bpm = 60000.0 / delta;
                rates[rateSpot++] = (byte)bpm;
                rateSpot %= RATE_SIZE;

                avgBpm = 0;
                for (byte i = 0; i < RATE_SIZE; i++)
                {
                    avgBpm += rates[i];
                }
                avgBpm /= RATE_SIZE;
            }
        }

        if (millis() - lastPrint > 1000)
        {
            lastPrint = millis();
            Serial.print(irValue);
            Serial.print(" ");
            Serial.print(bpm, 1);
            Serial.print(" ");
            Serial.println(avgBpm, 1);
        }

        data.bpm = bpm;
        data.avgBpm = avgBpm;
        return data;
    }
};

#endif
