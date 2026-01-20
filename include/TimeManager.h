#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include <Arduino.h>
#include <RTClib.h>

/*
  TimeManager
  -----------
  Handles all RTC (DS3231) operations:
  - Initialization
  - Safe time updating (once per second)
  - Formatted time & date strings for OLED display
*/

class TimeManager
{
private:
  RTC_DS3231 rtc;         // RTC object
  DateTime now;           // Current time snapshot
  unsigned long lastTick; // For 1-second update timing

public:
  TimeManager() : lastTick(0) {}

  /* ================= INITIALIZE RTC ================= */
  void begin()
  {
    if (!rtc.begin())
    {
      Serial.println("❌ RTC NOT FOUND");
      while (1)
        ; // Stop program if RTC missing
    }

    Serial.println("✅ RTC FOUND");

    /*
      ⚠️ IMPORTANT — USE ONLY ONCE!
      Uncomment the line below, upload once to set RTC time,
      then COMMENT IT AGAIN and re-upload.

      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    */

    now = rtc.now(); // Get initial time
  }

  /* ================= UPDATE TIME ================= */
  // Call this in loop() — updates once per second
  bool update()
  {
    if (millis() - lastTick >= 1000)
    {
      lastTick = millis();
      now = rtc.now();
      return true; // Time updated
    }
    return false; // No update yet
  }

  /* ================= RAW TIME ACCESS ================= */
  DateTime getNow()
  {
    return now;
  }

  /* ================= TIME STRING ================= */
  // Returns formatted time like: "12:05 PM"
  String getTimeString()
  {
    int hour = now.hour();
    int minute = now.minute();

    String period = (hour >= 12) ? " PM" : " AM";

    // Convert 24-hour to 12-hour format
    hour = hour % 12;
    if (hour == 0)
      hour = 12;

    char buffer[10];
    sprintf(buffer, "%d:%02d%s", hour, minute, period.c_str());
    return String(buffer);
  }

  /* ================= DATE STRING ================= */
  // Returns formatted date like: "07/01/2026"
  String getDateString()
  {
    char buffer[12];
    sprintf(buffer, "%02d/%02d/%04d",
            now.day(),
            now.month(),
            now.year());

    return String(buffer);
  }
};

#endif
