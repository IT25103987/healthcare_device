# WiFi Integration Guide

This guide explains how to **optionally** add WiFi connectivity to your healthcare device to send sensor data to a backend server. The WiFi functionality is completely optional and will not affect the existing device functionality.

## 📋 Table of Contents

- [Overview](#overview)
- [Prerequisites](#prerequisites)
- [Option A: Minimal Integration (Recommended)](#option-a-minimal-integration-recommended)
- [Option B: Full Integration with RTC Timestamps](#option-b-full-integration-with-rtc-timestamps)
- [Configuration](#configuration)
- [Testing](#testing)
- [Troubleshooting](#troubleshooting)

---

## Overview

The `WiFiSender` module allows your ESP32-S3 to:
- Connect to WiFi networks
- Send health data to a backend server via HTTP POST
- Auto-reconnect if connection is lost
- Operate non-blocking (doesn't interfere with existing code)

**Key Features:**
- ✅ Non-blocking operation
- ✅ Auto-reconnect on WiFi failure
- ✅ Configurable send intervals
- ✅ Connection statistics
- ✅ Easy to enable/disable

---

## Prerequisites

Before enabling WiFi, ensure you have:

1. **WiFi Network Credentials**
   - Network SSID (name)
   - Network password

2. **Backend Server Running**
   - Server URL (e.g., `http://192.168.1.100:5000/api/data`)
   - Server must be accessible from ESP32

3. **PlatformIO Library Dependencies**
   - Add `ArduinoJson` library to `platformio.ini`

---

## Option A: Minimal Integration (Recommended)

This is the **easiest** way to add WiFi functionality. It requires only 3 lines of code and can be easily disabled.

### Step 1: Update `platformio.ini`

Add the ArduinoJson library dependency:

```ini
[env:esp32-s3]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino

upload_protocol = esptool
upload_port = COM6
monitor_port = COM6
monitor_speed = 115200

lib_deps =
    adafruit/Adafruit GFX Library
    adafruit/Adafruit SSD1306
    adafruit/Adafruit SH110X
    adafruit/Adafruit BMP280 Library
    adafruit/Adafruit Unified Sensor
    sparkfun/SparkFun MAX3010x Pulse and Proximity Sensor Library
    adafruit/RTClib
    bblanchon/ArduinoJson@^6.21.0     # ← ADD THIS LINE
```

### Step 2: Include WiFiSender in `main.cpp`

Add this include at the top of your `main.cpp` file (around line 9, after other includes):

```cpp
#include "WiFiSender.h"  // ← ADD THIS LINE
```

### Step 3: Create WiFiSender Instance

Add this line after your other global objects (around line 63):

```cpp
WiFiSender wifiSender;  // ← ADD THIS LINE
```

### Step 4: Initialize WiFi in `setup()`

Add these lines at the **end** of your `setup()` function (after the "READY" message, around line 161):

```cpp
  /* ================= WIFI (OPTIONAL) ================= */
  // Uncomment the lines below to enable WiFi data sending
  /*
  wifiSender.begin(
    "YOUR_WIFI_SSID",           // Replace with your WiFi name
    "YOUR_WIFI_PASSWORD",       // Replace with your WiFi password
    "http://192.168.1.100:5000/api/data",  // Replace with your server URL
    "watch_001"                 // Device ID (change if you have multiple devices)
  );
  wifiSender.setSendInterval(60000);  // Send every 60 seconds (1 minute)
  */
```

### Step 5: Send Data in `loop()`

Add this code in the `loop()` function where you want to send data. A good place is after heart rate is updated (around line 545):

```cpp
  // 2. ALWAYS sample heart rate (No delays allowed for accuracy)
  if (state.getState()){
    
    // ← ADD THIS BLOCK HERE
    /* ================= WIFI DATA SENDING (OPTIONAL) ================= */
    // Uncomment to enable WiFi data transmission
    /*
    HeartData currentData = heartMonitor.update();
    if (currentData.bpm > 0) {  // Only send if we have valid data
      wifiSender.sendData(
        (int)currentData.avgBpm,           // Heart rate
        98,                                 // SpO2 (placeholder, update if you have sensor)
        weather.getTemperature(),          // Temperature from BMP280
        120,                                // Systolic BP (placeholder)
        80,                                 // Diastolic BP (placeholder)
        "NORMAL"                            // Alert status
      );
    }
    */
    
    // --- BUTTON 2: SWITCH WINDOWS --- (existing code continues)
```

### Step 6: Enable WiFi

To **enable** WiFi functionality:
1. Uncomment the code blocks in Step 4 and Step 5 (remove `/*` and `*/`)
2. Replace the placeholders with your actual values:
   - `YOUR_WIFI_SSID` → Your WiFi network name
   - `YOUR_WIFI_PASSWORD` → Your WiFi password
   - `http://192.168.1.100:5000/api/data` → Your backend server URL
3. Upload the code to your ESP32

To **disable** WiFi functionality:
1. Simply comment out the code blocks (add `/*` and `*/` back)
2. Upload the code to your ESP32

That's it! Your device will now send data to the server every minute while maintaining all existing functionality.

---

## Option B: Full Integration with RTC Timestamps

This option adds proper ISO 8601 timestamps using the RTC module.

### Additional Steps (after completing Option A):

**Modify the sendData call to include RTC timestamp:**

```cpp
/* ================= WIFI DATA SENDING (OPTIONAL) ================= */
HeartData currentData = heartMonitor.update();
if (currentData.bpm > 0) {
  // Get current time from RTC
  DateTime now = timeManager.getRTC().now();
  
  // Format ISO 8601 timestamp: "2026-01-22T10:30:00Z"
  char timestamp[25];
  sprintf(timestamp, "%04d-%02d-%02dT%02d:%02d:%02dZ",
          now.year(), now.month(), now.day(),
          now.hour(), now.minute(), now.second());
  
  // Determine alert status based on heart rate
  String alertStatus = "NORMAL";
  int hr = (int)currentData.avgBpm;
  if (hr < 40) {
    alertStatus = "CRITICAL_LOW";
  } else if (hr >= 40 && hr < 50) {
    alertStatus = "WARNING_LOW";
  } else if (hr > 110) {
    alertStatus = "CRITICAL_HIGH";
  } else if (hr >= 91 && hr <= 109) {
    alertStatus = "WARNING_HIGH";
  }
  
  wifiSender.sendData(
    hr,                                    // Heart rate
    98,                                    // SpO2
    weather.getTemperature(),             // Temperature
    120,                                   // Systolic BP
    80,                                    // Diastolic BP
    alertStatus                            // Alert status
  );
}
```

---

## Configuration

### WiFi Credentials

You can set WiFi credentials in two ways:

**Method 1: Hard-coded (Simple)**
```cpp
wifiSender.begin("MyWiFi", "password123", "http://192.168.1.100:5000/api/data");
```

**Method 2: Using defines (Better for sharing code)**
```cpp
// At the top of main.cpp
#define WIFI_SSID "MyWiFi"
#define WIFI_PASSWORD "password123"
#define SERVER_URL "http://192.168.1.100:5000/api/data"

// In setup()
wifiSender.begin(WIFI_SSID, WIFI_PASSWORD, SERVER_URL);
```

### Send Interval

Control how often data is sent (default is 60 seconds):

```cpp
wifiSender.setSendInterval(30000);   // Send every 30 seconds
wifiSender.setSendInterval(120000);  // Send every 2 minutes
wifiSender.setSendInterval(300000);  // Send every 5 minutes
```

### Device ID

Set a unique ID for each device:

```cpp
wifiSender.begin(WIFI_SSID, WIFI_PASSWORD, SERVER_URL, "watch_002");  // Second device
wifiSender.begin(WIFI_SSID, WIFI_PASSWORD, SERVER_URL, "watch_003");  // Third device
```

---

## Testing

### 1. Check Serial Monitor

After uploading the code, open the Serial Monitor (115200 baud) and look for WiFi messages:

```
[WiFiSender] Initializing...
[WiFiSender] Device ID: watch_001
[WiFiSender] Server: http://192.168.1.100:5000/api/data

[WiFiSender] Connecting to WiFi...
[WiFiSender] SSID: MyWiFi
..........
[WiFiSender] ✅ WiFi Connected!
[WiFiSender] IP Address: 192.168.1.105

[WiFiSender] Sending data:
{"deviceId":"watch_001","heartRate":75,"spo2":98,"temperature":23.5,"pressure":"120/80","alertStatus":"NORMAL","timestamp":60000}

[WiFiSender] ✅ HTTP POST Success - Code: 200
[WiFiSender] Response: {"success":true,"message":"Data received"}
```

### 2. Check Backend Server

Verify that data is being received by your backend server. Check the server logs or database.

### 3. Connection Statistics

You can add this code to display WiFi statistics on the OLED:

```cpp
int successful, failed;
wifiSender.getStats(successful, failed);
Serial.print("WiFi Stats - Success: ");
Serial.print(successful);
Serial.print(", Failed: ");
Serial.println(failed);
```

---

## Troubleshooting

### ❌ WiFi Connection Failed

**Problem:** Cannot connect to WiFi

**Solutions:**
1. **Check credentials:** Ensure SSID and password are correct
2. **Check WiFi band:** ESP32 only supports 2.4GHz WiFi (not 5GHz)
3. **Check signal strength:** Move closer to the router
4. **Check router settings:** Ensure router allows new devices to connect
5. **Check WiFi mode:** Some routers block IoT devices

### ❌ HTTP POST Failed

**Problem:** WiFi connected but cannot send data

**Solutions:**
1. **Check server URL:** Ensure the URL is correct and accessible
2. **Check firewall:** Ensure server port is not blocked
3. **Check server status:** Ensure backend server is running
4. **Check network:** Ensure ESP32 and server are on the same network (or proper routing exists)
5. **Test with curl:**
   ```bash
   curl -X POST http://192.168.1.100:5000/api/data \
     -H "Content-Type: application/json" \
     -d '{"deviceId":"watch_001","heartRate":75}'
   ```

### ⚠️ Device Freezing or Slow

**Problem:** Device becomes unresponsive after enabling WiFi

**Solutions:**
1. **Increase send interval:** Sending too frequently can cause issues
   ```cpp
   wifiSender.setSendInterval(120000);  // Send every 2 minutes instead of 1
   ```
2. **Check WiFi signal:** Poor signal causes long connection attempts
3. **Disable WiFi:** If issues persist, comment out WiFi code

### ⚠️ Data Not Updating

**Problem:** Old data being sent to server

**Solutions:**
1. **Check sampling:** Ensure heart rate monitor is running
2. **Check conditions:** Verify the `if (currentData.bpm > 0)` condition
3. **Add debug prints:**
   ```cpp
   Serial.print("Sending HR: ");
   Serial.println(currentData.avgBpm);
   ```

### 📊 Monitor Connection

Add this to your loop for continuous monitoring:

```cpp
// Add to loop() function
static unsigned long lastWiFiCheck = 0;
if (millis() - lastWiFiCheck > 30000) {  // Check every 30 seconds
  lastWiFiCheck = millis();
  
  if (wifiSender.isConnected()) {
    int rssi = wifiSender.getSignalStrength();
    Serial.print("WiFi Connected - Signal: ");
    Serial.print(rssi);
    Serial.println(" dBm");
  } else {
    Serial.println("WiFi Disconnected");
  }
  
  int success, failed;
  wifiSender.getStats(success, failed);
  Serial.print("Stats - Success: ");
  Serial.print(success);
  Serial.print(", Failed: ");
  Serial.println(failed);
}
```

---

## Performance Tips

1. **Send interval:** Don't send too frequently (minimum 30 seconds recommended)
2. **WiFi power:** WiFi consumes significant power - consider disabling for battery operation
3. **Error handling:** The module handles reconnection automatically
4. **Signal strength:** Good signal (> -70 dBm) ensures reliable operation

---

## Next Steps

After WiFi integration:
1. Set up the backend server (see `backend/README.md`)
2. Configure email alerts
3. Set up the web dashboard
4. Test end-to-end system

---

## Support

For issues or questions:
1. Check the Serial Monitor output for error messages
2. Review the troubleshooting section above
3. Check backend server logs
4. Verify network connectivity

---

## Summary

✅ **To Enable WiFi:**
1. Add ArduinoJson to platformio.ini
2. Include WiFiSender.h
3. Create wifiSender object
4. Uncomment WiFi initialization in setup()
5. Uncomment WiFi sending in loop()
6. Configure credentials and server URL

✅ **To Disable WiFi:**
1. Comment out the WiFi code blocks
2. Existing functionality remains unchanged

The WiFi module is designed to be completely optional and non-intrusive to your existing code!
