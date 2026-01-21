#ifndef State_h
#define State_h

#include <Arduino.h>
class Vibrate{
    private:
        unsigned long vibrateStartTime = 0;
        unsigned long vibrateDuration = 0;
        bool isVibrating = false;

    public:
    Vibrate() {
        pinMode(6, OUTPUT);
    }

    void vibrateOn() {
        digitalWrite(6, HIGH);
    }

    void vibrateOff() {
        digitalWrite(6, LOW);
        isVibrating = false;
    }

    // Vibrate for duration in milliseconds
    void vibrateFor(unsigned long durationMs) {
        digitalWrite(6, HIGH);
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
