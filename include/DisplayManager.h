#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// Include your data managers so the display knows how to read them
#include "TimeManager.h"
#include "WeatherManager.h"
#include "ECGManager.h"
#include "ActivityManager.h"
#include "HartRate.h"
#include "Vibrate.h"

class DisplayManager {
private:
    Adafruit_SH1106G display;
    
    // ECG Graph State Variables
    int ecgX = 0;
    int ecgLastY = 32;

    // --- HELPER: Draw Phone Icon ---
    void drawPhoneHandset(int x, int y) {
        display.fillRoundRect(x, y, 24, 8, 3, SH110X_WHITE);
        display.fillCircle(x + 2, y + 6, 5, SH110X_WHITE);
        display.fillCircle(x + 22, y + 6, 5, SH110X_WHITE);
        display.drawLine(x + 12, y, x + 12, y - 4, SH110X_WHITE);
    }

    // --- HELPER: Draw Heart Shape ---
    void drawHeart(int x, int y, int size) {
        display.fillCircle(x - size / 2, y, size / 2, SH110X_WHITE);
        display.fillCircle(x + size / 2, y, size / 2, SH110X_WHITE);
        display.fillTriangle(x - size, y, x + size, y, x, y + size, SH110X_WHITE);
    }

    // --- HELPER: Draw Warning Triangle ---
    void drawWarningIcon(int x, int y) {
        display.drawTriangle(x, y, x - 20, y + 35, x + 20, y + 35, SH110X_WHITE);
        display.fillRect(x - 2, y + 10, 5, 15, SH110X_WHITE);
        display.fillCircle(x, y + 30, 2, SH110X_WHITE);
    }

    // --- HELPER: Draw Shoe Icon ---
    void drawShoeIcon(int x, int y) {
        display.fillRoundRect(x, y + 8, 20, 5, 2, SH110X_WHITE);
        display.fillRoundRect(x + 2, y + 2, 14, 8, 2, SH110X_WHITE);
        display.fillRect(x + 4, y - 2, 8, 6, SH110X_WHITE);
        display.drawLine(x + 6, y, x + 10, y, SH110X_BLACK);
        display.drawLine(x + 6, y + 3, x + 10, y + 3, SH110X_BLACK);
    }

public:
    // Constructor initializes the SH1106G object
    DisplayManager() : display(128, 64, &Wire, -1) {}

    void begin() {
        // Initialize OLED with I2C address 0x3C
        if (!display.begin(0x3C, true)) {
            Serial.println("❌ OLED NOT FOUND");
            while (1);
        }
        display.clearDisplay();
        display.setTextColor(SH110X_WHITE);
        display.display();
    }

    void clear() {
        display.clearDisplay();
    }

    void update() {
        display.display();
    }

    // ============================================
    // SCREEN 0: START / CLOCK
    // ============================================
    void showStartScreen(String date, String time) {
        display.clearDisplay();
        
        // Date Header
        display.setTextSize(1);
        display.setCursor(35, 0);
        display.print(date);
        display.drawLine(0, 10, 128, 10, SH110X_WHITE);

        // Centered Time
        display.setTextSize(2);
        int16_t x1, y1;
        uint16_t w, h;
        display.getTextBounds(time, 0, 0, &x1, &y1, &w, &h);
        int centerX = (128 - w) / 2;
        int centerY = (64 - h) / 2;
        
        display.setCursor(centerX, centerY);
        display.print(time);

        // Footer
        display.setTextSize(1);
        display.setCursor(10, centerY + h + 4);
        display.println("  PRESS BUTTON");
        display.println("   TO CONTINUE");
        
        display.display();
    }

    // ============================================
    // SCREEN 1: HEART RATE MONITOR
    // ============================================
    void showHeartRateScreen(HeartData data) {
        display.clearDisplay();
        
        // Header
        display.setTextSize(1);
        display.setCursor(35, 0);
        display.print("PULSE");
        display.drawLine(0, 10, 128, 10, SH110X_WHITE);

        // Status
        display.setCursor(0, 14);
        display.print(">> ");
        display.print(data.irValue > 50000 ? "Sensing..." : "Place Finger");

        // BPM
        display.setCursor(0, 24);
        display.print("BPM: ");
        display.print((int)data.currentBpm);

        // Average
        display.setCursor(0, 34);
        display.print("AVG: ");
        display.print((int)data.avgBpm);

        // Visual
        drawHeart(100, 30, 10);
        display.display();
    }

    // ============================================
    // SCREEN 2: WEATHER
    // ============================================
    void showWeatherScreen(WeatherManager &weather) {
        display.clearDisplay();

        if (!weather.isConnected()) {
            display.setTextSize(1);
            display.setCursor(20, 25);
            display.print("SENSOR ERROR");
            display.display();
            return;
        }

        // Header
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.print("ENV MONITOR");
        display.drawLine(0, 10, 128, 10, SH110X_WHITE);

        // Temp
        display.setTextSize(2);
        display.setCursor(0, 18);
        display.print((int)weather.getTemperature());
        display.setTextSize(1);
        display.print(" o"); 
        display.setTextSize(2);
        display.print("C");

        // Forecast
        display.setTextSize(1);
        display.setCursor(65, 18);
        display.print("FORECAST:");
        display.setCursor(65, 30);
        display.print(weather.getForecast());

        // Footer Data
        display.drawFastHLine(0, 42, 128, SH110X_WHITE);
        display.setCursor(0, 48);
        display.print("Prs: "); display.print((int)weather.getPressure()); display.print("hPa");
        display.setCursor(75, 48);
        display.print("FLR: "); display.print(weather.getFloorsClimbed());
        
        display.drawFastVLine(70, 45, 15, SH110X_WHITE);
        display.display();
    }

    // ============================================
    // SCREEN 3: ECG GRAPH
    // ============================================
    void showECGScreen(ECGManager &ecg) {
        display.clearDisplay();

        if (!ecg.isConnected()) {
            display.setCursor(20, 30);
            display.print("ATTACH PADS...");
            display.display();
            return;
        }

        // Get Data
        int raw = analogRead(36); 
        // Note: In main code, pin 36 is implied. Ensure ECGManager handles this pin.
        
        bool isBeat = ecg.detectBeat(raw);
        int yPos = ecg.getGraphValue(40); // Map to top 40 pixels

        // Draw UI
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.print("Rhythm: "); display.print(ecg.getRhythmStatus());
        display.drawFastHLine(0, 42, 128, SH110X_WHITE);
        display.setCursor(0, 48);
        display.print("Stress: "); display.println(ecg.getStressLevel());

        // Draw Graph Line
        if (ecgX > 0) {
            display.drawLine(ecgX - 1, ecgLastY, ecgX, yPos, SH110X_WHITE);
        }

        ecgLastY = yPos;
        ecgX++;

        if (ecgX >= 128) {
            ecgX = 0;
            display.clearDisplay();
        }

        if (isBeat) {
            display.fillCircle(120, 5, 4, SH110X_WHITE);
        }
        display.display();
    }

    // ============================================
    // SCREEN 4: STEPS / PEDOMETER
    // ============================================
    void showStepsScreen(ActivityManager &activity) {
        display.clearDisplay();
        int steps = activity.getSteps();
        int goal = 6000;
        float distanceKm = steps * 0.000762;
        int calories = steps * 0.04;

        drawShoeIcon(10, 5);
        display.setTextSize(1);
        display.setCursor(38, 5);
        display.print("PEDOMETER");
        display.drawLine(0, 18, 128, 18, SH110X_WHITE);

        // Steps Count
        display.setTextSize(2);
        int xOffset = (steps < 10) ? 55 : (steps < 100) ? 50 : (steps < 1000) ? 45 : 35;
        display.setCursor(xOffset, 25);
        display.print(steps);
        display.setTextSize(1);
        display.setCursor(xOffset + (String(steps).length() * 12) + 2, 32);
        display.print("stps");

        // Progress Bar
        display.drawRect(10, 45, 108, 6, SH110X_WHITE);
        int fillWidth = map(constrain(steps, 0, goal), 0, goal, 0, 106);
        display.fillRect(11, 46, fillWidth, 4, SH110X_WHITE);

        // Stats
        display.setCursor(5, 55);
        display.print("Dist: "); display.print(distanceKm, 1); display.print("km");
        display.setCursor(70, 55);
        display.print("Cal: "); display.print(calories);

        display.display();
    }

    // ============================================
    // ANIMATION: SOS EMERGENCY
    // ============================================
    void animateEmergencyCall(Vibrate &vibrate) {
        int callDuration = 50; 
        
        for (int i = 0; i < callDuration; i++) {
            display.clearDisplay();
            int iconX = 52, iconY = 20;
            drawPhoneHandset(iconX, iconY);

            // Expanding waves
            if (i % 10 > 2) display.drawCircle(iconX + 12, iconY + 4, 15, SH110X_WHITE);
            if (i % 10 > 5) display.drawCircle(iconX + 12, iconY + 4, 22, SH110X_WHITE);
            if (i % 10 > 8) display.drawCircle(iconX + 12, iconY + 4, 29, SH110X_WHITE);

            // Text
            display.setTextSize(1);
            display.setCursor(35, 0);
            display.print("EMERGENCY");
            display.setTextSize(2);
            display.setCursor(15, 45);
            if (i % 8 < 5) display.print("DIALING..");

            // Progress Bar
            display.drawRect(14, 60, 100, 4, SH110X_WHITE); 
            int progress = map(i, 0, callDuration, 0, 98);
            display.fillRect(15, 61, progress, 2, SH110X_WHITE);

            display.display();
            vibrate.vibrateFor(100);
            delay(100);
        }

        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(10, 25);
        display.print("CONNECTED");
        display.display();
        delay(2000);
    }

    // ============================================
    // ANIMATION: FALL DETECTION
    // ============================================
    void animateFallAlert(Vibrate &vibrate, int cancelPin) {
        int countdown = 100;
        bool flashState = false;

        for (int i = 0; i < countdown; i++) {
            display.clearDisplay();
            drawWarningIcon(64, 5);

            display.setTextSize(2);
            display.setCursor(10, 45);
            display.print("FALL DETECTED");

            int barWidth = map(i, 0, 100, 0, 128);
            display.fillRect(0, 0, barWidth, 4, SH110X_WHITE);

            display.setTextSize(1);
            display.setCursor(35, 30);
            display.print("SENDING SOS...");

            if (i % 5 == 0) {
                flashState = !flashState;
                display.invertDisplay(flashState);
            }
            display.display();

            // Cancel check
            if (digitalRead(cancelPin) == LOW) {
                display.invertDisplay(false);
                display.clearDisplay();
                display.setCursor(25, 25);
                display.print("CANCELLED");
                display.display();
                delay(1000);
                return;
            }
            delay(100);
        }

        display.invertDisplay(false);
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(10, 20);
        display.print("SOS SENT!");
        display.display();
        delay(2000);
    }
    
    // ============================================
    // ANIMATION: HEART ALERT
    // ============================================
    void showHeartAlert(String status, float bpm, int alertLevel, Vibrate &vibrate) {
        int duration = 30;
        bool flashState = false;

        for (int i = 0; i < duration; i++) {
            display.clearDisplay();
            
            // Icon
            int x = 58; int y = 5;
            display.fillCircle(x, y, 5, SH110X_WHITE);
            display.fillCircle(x + 10, y, 5, SH110X_WHITE);
            display.fillTriangle(x - 5, y, x + 15, y, x + 5, y + 15, SH110X_WHITE);

            // Data
            display.setTextSize(1);
            display.setCursor(10, 25); 
            display.print(status);
            display.setTextSize(2);
            display.setCursor(45, 40);
            display.print((int)bpm);
            display.setTextSize(1);
            display.setCursor(85, 47);
            display.print("BPM");

            // Flashing for Critical (Level 2)
            if (alertLevel == 2 && i % 5 == 0) {
                flashState = !flashState;
                display.invertDisplay(flashState);
            } else if (alertLevel != 2) {
                 display.invertDisplay(false);
            }

            display.display();
            vibrate.vibrateFor(100);
            delay(100);
        }
        display.invertDisplay(false);
    }
};

#endif