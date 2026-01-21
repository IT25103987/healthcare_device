#ifndef VIBRATE_H
#define VIBRATE_H

#include <Arduino.h>
class Vibrate{
    private:
        unsigned long vibrateStartTime = 0;
        unsigned long vibrateDuration = 0;
        bool isVibrating = false;
        int pinNum= 6;

    public:
    Vibrate() {
        pinMode(pinNum, OUTPUT);
    }

    void vibrateOn() {
        digitalWrite(pinNum, HIGH);
    }

    void vibrateOff() {
        digitalWrite(pinNum, LOW);
        isVibrating = false;
    }

    // Vibrate for duration in milliseconds
    void vibrateFor(unsigned long durationMs) {
        digitalWrite(pinNum, HIGH);
        vibrateStartTime = millis();
        vibrateDuration = durationMs;
        isVibrating = true;
    }

    // Call this in your loop to handle timing
    void update() {
        if (isVibrating && (millis() - vibrateStartTime >= vibrateDuration)) {
            vibrateOff();
        }
    }
};
#endif
