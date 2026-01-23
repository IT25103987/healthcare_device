#ifndef DISPLAY_H
#define DISPLAY_H

/* ================= OLED ================= */
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
Adafruit_SH1106G display(128, 64, &Wire, -1);

#include <Arduino.h>
class Display {
      private:
    bool state;

    public:
    Display() {
        if (!display.begin(0x3C, true)){
    Serial.println("❌ OLED NOT FOUND");
    while (1); // Stop program if OLED fails
    }

    display.clearDisplay();
    display.setTextColor(SH110X_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("System starting...");
    display.display();
    delay(1500);

    }



};
#endif
