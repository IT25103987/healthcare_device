#ifndef WIFISENDER_H
#define WIFISENDER_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino.h>
#include <ArduinoJson.h>

/**
 * WiFiSender - Optional WiFi Module for Healthcare Device
 * 
 * This module allows the ESP32-S3 to send sensor data to a backend server
 * via WiFi without interfering with existing device functionality.
 * 
 * Features:
 * - WiFi connection management with auto-reconnect
 * - HTTP POST requests to backend server
 * - JSON data formatting
 * - Non-blocking operation
 * - Easy configuration
 * 
 * Usage:
 * 1. Include this header in your main.cpp
 * 2. Configure WiFi credentials and server endpoint
 * 3. Call begin() in setup()
 * 4. Call sendData() periodically in loop()
 */

class WiFiSender {
private:
    // WiFi Configuration
    const char* ssid;
    const char* password;
    
    // Server Configuration
    String serverEndpoint;
    String deviceId;
    
    // Connection State
    bool wifiConnected;
    unsigned long lastConnectionAttempt;
    unsigned long lastDataSend;
    
    // Timing Configuration (in milliseconds)
    unsigned long sendInterval;           // Default: 60000 (1 minute)
    unsigned long reconnectInterval;      // Default: 30000 (30 seconds)
    
    // Statistics
    int successCount;
    int failCount;
    
    /**
     * Internal method to connect to WiFi
     * Returns true if connected, false otherwise
     */
    bool connectWiFi() {
        if (WiFi.status() == WL_CONNECTED) {
            wifiConnected = true;
            return true;
        }
        
        // Don't attempt reconnection too frequently
        if (millis() - lastConnectionAttempt < reconnectInterval) {
            return false;
        }
        
        lastConnectionAttempt = millis();
        
        Serial.println("\n[WiFiSender] Connecting to WiFi...");
        Serial.print("[WiFiSender] SSID: ");
        Serial.println(ssid);
        
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
        
        // Wait up to 10 seconds for connection
        int timeout = 10;
        while (WiFi.status() != WL_CONNECTED && timeout > 0) {
            delay(500);
            Serial.print(".");
            timeout--;
        }
        
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\n[WiFiSender] ✅ WiFi Connected!");
            Serial.print("[WiFiSender] IP Address: ");
            Serial.println(WiFi.localIP());
            wifiConnected = true;
            return true;
        } else {
            Serial.println("\n[WiFiSender] ❌ WiFi Connection Failed");
            wifiConnected = false;
            return false;
        }
    }
    
    /**
     * Internal method to send HTTP POST request
     */
    bool sendHttpPost(const String& jsonData) {
        if (!wifiConnected) {
            return false;
        }
        
        HTTPClient http;
        http.begin(serverEndpoint);
        http.addHeader("Content-Type", "application/json");
        
        int httpResponseCode = http.POST(jsonData);
        
        if (httpResponseCode > 0) {
            Serial.print("[WiFiSender] ✅ HTTP POST Success - Code: ");
            Serial.println(httpResponseCode);
            String response = http.getString();
            Serial.print("[WiFiSender] Response: ");
            Serial.println(response);
            http.end();
            successCount++;
            return true;
        } else {
            Serial.print("[WiFiSender] ❌ HTTP POST Failed - Error: ");
            Serial.println(http.errorToString(httpResponseCode));
            http.end();
            failCount++;
            return false;
        }
    }

public:
    /**
     * Constructor
     * Initialize WiFiSender with default values
     */
    WiFiSender() {
        ssid = "";
        password = "";
        serverEndpoint = "";
        deviceId = "watch_001";
        wifiConnected = false;
        lastConnectionAttempt = 0;
        lastDataSend = 0;
        sendInterval = 60000;        // 1 minute
        reconnectInterval = 30000;   // 30 seconds
        successCount = 0;
        failCount = 0;
    }
    
    /**
     * Initialize WiFiSender with configuration
     * 
     * @param wifi_ssid WiFi network name
     * @param wifi_password WiFi password
     * @param server_url Backend server endpoint (e.g., "http://192.168.1.100:5000/api/data")
     * @param device_id Unique device identifier (default: "watch_001")
     */
    void begin(const char* wifi_ssid, const char* wifi_password, 
               const String& server_url, const String& device_id = "watch_001") {
        ssid = wifi_ssid;
        password = wifi_password;
        serverEndpoint = server_url;
        deviceId = device_id;
        
        Serial.println("\n[WiFiSender] Initializing...");
        Serial.print("[WiFiSender] Device ID: ");
        Serial.println(deviceId);
        Serial.print("[WiFiSender] Server: ");
        Serial.println(serverEndpoint);
        
        // Attempt initial connection
        connectWiFi();
    }
    
    /**
     * Set the data send interval
     * 
     * @param intervalMs Interval in milliseconds (default: 60000 = 1 minute)
     */
    void setSendInterval(unsigned long intervalMs) {
        sendInterval = intervalMs;
    }
    
    /**
     * Check if WiFi is connected
     * 
     * @return true if connected, false otherwise
     */
    bool isConnected() {
        return wifiConnected && (WiFi.status() == WL_CONNECTED);
    }
    
    /**
     * Get connection statistics
     * 
     * @param successful Reference to store successful sends count
     * @param failed Reference to store failed sends count
     */
    void getStats(int& successful, int& failed) {
        successful = successCount;
        failed = failCount;
    }
    
    /**
     * Send health data to server
     * This method is non-blocking and respects the send interval
     * 
     * @param heartRate Heart rate in BPM
     * @param spo2 Blood oxygen saturation percentage
     * @param temperature Body temperature in Celsius
     * @param systolic Systolic blood pressure
     * @param diastolic Diastolic blood pressure
     * @param alertStatus Alert status string ("NORMAL", "WARNING", "CRITICAL")
     * @return true if data was sent successfully, false otherwise
     */
    bool sendData(int heartRate, int spo2, float temperature, 
                  int systolic, int diastolic, const String& alertStatus = "NORMAL") {
        
        // Check if enough time has passed since last send
        if (millis() - lastDataSend < sendInterval) {
            return false; // Not time to send yet
        }
        
        // Update last send time
        lastDataSend = millis();
        
        // Ensure WiFi is connected
        if (!isConnected()) {
            Serial.println("[WiFiSender] WiFi not connected, attempting to reconnect...");
            if (!connectWiFi()) {
                return false; // Connection failed
            }
        }
        
        // Create JSON document
        StaticJsonDocument<512> doc;
        
        doc["deviceId"] = deviceId;
        doc["heartRate"] = heartRate;
        doc["spo2"] = spo2;
        doc["temperature"] = temperature;
        
        // Format blood pressure as "systolic/diastolic"
        String pressureStr = String(systolic) + "/" + String(diastolic);
        doc["pressure"] = pressureStr;
        
        doc["alertStatus"] = alertStatus;
        
        // Add timestamp
        // NOTE: Using millis() provides milliseconds since boot (not real time)
        // For production use, integrate with RTC to get actual timestamp:
        // Example: doc["timestamp"] = rtc.now().unixtime();
        // For now, backend will use server time when receiving data
        doc["timestamp"] = millis();
        
        // Serialize JSON to string
        String jsonData;
        serializeJson(doc, jsonData);
        
        Serial.println("\n[WiFiSender] Sending data:");
        Serial.println(jsonData);
        
        // Send HTTP POST request
        return sendHttpPost(jsonData);
    }
    
    /**
     * Manual connection check and reconnection
     * Call this periodically if you want to ensure connection
     */
    void update() {
        // Check connection status
        if (WiFi.status() != WL_CONNECTED) {
            wifiConnected = false;
            connectWiFi();
        } else {
            wifiConnected = true;
        }
    }
    
    /**
     * Get WiFi signal strength (RSSI)
     * 
     * @return Signal strength in dBm, or 0 if not connected
     */
    int getSignalStrength() {
        if (isConnected()) {
            return WiFi.RSSI();
        }
        return 0;
    }
    
    /**
     * Disconnect from WiFi
     * Useful for power saving
     */
    void disconnect() {
        WiFi.disconnect();
        wifiConnected = false;
        Serial.println("[WiFiSender] WiFi Disconnected");
    }
};

#endif // WIFISENDER_H
