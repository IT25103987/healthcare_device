#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include <Arduino.h>
#include <RTClib.h>

class TimeManager {
  private:
    RTC_DS3231 rtc;
    DateTime now;
    unsigned long lastUpdate = 0;

  public:
    TimeManager() {}

    void begin() {
      if (!rtc.begin()) {
        Serial.println("RTC Not Found!");
      }
      
      // Only adjust if the clock stopped (battery died)
      // or if you want to force a sync once.
      if (rtc.lostPower()) {
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      }
    }

    bool update() {
      if (millis() - lastUpdate >= 1000) {
        now = rtc.now();
        lastUpdate = millis();
        return true; 
      }
      return false; 
    }

    // Java-style "Getter" to return the time object
    DateTime getNow() {
      return now;
    }

    String getTimeString() {
      char buffer[6]; 
      sprintf(buffer, "%02d:%02d", now.hour(), now.minute());
      return String(buffer);
    }

    String getDateString() {
      char buffer[11]; 
      sprintf(buffer, "%02d/%02d/%04d", now.day(), now.month(), now.year());
      return String(buffer);
    }
};
#endif