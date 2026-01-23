#include <Wire.h>
#include <RTClib.h>
#include "TimeManager.h" // Double quotes look in include/ or local folder
#include "State.h"
#include "HartRate.h"
#include "WeatherManager.h"
#include "ECGManager.h"
#include "ActivityManager.h"
#include "Vibrate.h"



/* ================= OLED ================= */
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
Adafruit_SH1106G display(128, 64, &Wire, -1);

/* ================= BMP280 ================= */
#include <Adafruit_BMP280.h>
Adafruit_BMP280 bmp;
bool bmpOK = false;

// /* ================= MAX30102 ================= */
#include <MAX30105.h>
#include "heartRate.h"
// MAX30105 particleSensor;

/* ================= MPU9250 ================= */
#define MPU_ADDR 0x68
int16_t AcX, AcY, AcZ;

/* ================= I2C PINS ================= */
#define SDA_PIN 8
#define SCL_PIN 9

/* ================= HEART RATE ================= */
long lastBeat = 0;
int bpm = 0;

// start button steup
unsigned long pressedTime = 0; // Stores when the button was first pressed
bool isPressing = false;       // Tracks if the button is currently being held
int buttonState = 0;
bool longPressHandled = false; // Flag to ensure long press is only handled once

// SOS button steup
unsigned long SOSpressedTime = 0; // Stores when the button was first pressed
bool SOSisPressing = false;       // Tracks if the button is currently being held
int SOSbuttonState = 0;
bool SOSlongPressHandled = false; // Flag to ensure long press is only handled once


// State of the watch
State state;

/* ================= TIME ================= */
unsigned long epochTime = 1610000000; // Set your initial Unix timestamp here (Jan 7, 2021)
unsigned long lastMillis = 0;

#define PowerButton 10
#define BUTTON2 11
#define BUTTON3 12

int button2 = 0;

TimeManager timeManager;
HartRate heartMonitor;
WeatherManager weather;
ActivityManager activity;
Vibrate vibrate;
ECGManager ecg(36, 38, 37);

void setup()
{
  /* ================= SERIAL ================= */
  // Start serial communication for debugging
  Serial.begin(115200);
  delay(1000); // Allow serial monitor to open

  /* ================= I2C ================= */
  // IMPORTANT: I2C must be started BEFORE any sensor uses it
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000); // Standard I2C speed (safe & stable)


  // wether manager setup//
  if (weather.begin())
  {
    Serial.println("✅ BMP280 OK");
  }
  else
  {
    Serial.println("❌ BMP280 FAIL");
  }

  ecg.begin();
  activity.begin();

  /* ================= BUTTONS ================= */
  // Configure buttons as INPUT_PULLUP (LOW = pressed)
  pinMode(PowerButton, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);

  /* ================= STATE ================= */
  // Turn watch OFF initially
  state.setState(false);

  /* ================= RTC (DS3231) ================= */
  // Initialize RTC module
  timeManager.begin();

  /*
    ⚠️ IMPORTANT:
    The line below MUST be used ONLY ONCE to set RTC time,
    then COMMENT IT OUT permanently.

    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  */

  /* ================= OLED (SH1106) ================= */
  // Initialize OLED display
  if (!display.begin(0x3C, true))
  {
    Serial.println("❌ OLED NOT FOUND");
    while (1)
      ; // Stop program if OLED fails
  }

  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("System starting...");
  display.display();
  delay(1500);

  /* ================= BMP280 ================= */
  // Initialize BMP280 pressure & temperature sensor
  if (bmp.begin(0x76))
  {
    bmpOK = true;
    Serial.println("✅ BMP280 OK");
  }
  else
  {
    bmpOK = false;
    Serial.println("❌ BMP280 FAIL");
  }

  /* ================= MAX30102 ================= */
  // Initialize Heart Rate Sensor
  heartMonitor.begin();

  /* ================= MPU9250 ================= */
  // Wake up MPU9250 accelerometer/gyro
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); // Power management register
  Wire.write(0);    // Wake up sensor
  Wire.endTransmission(true);
  Serial.println("✅ MPU9250 OK");

  /* ================= READY ================= */
  Serial.println("🚀 ALL SENSORS READY");

  display.clearDisplay();
  display.setCursor(20, 25);
  display.println("READY");
  display.display();
  delay(1000);
}

void start()
{
  static String lastTime = "";

  String currentTime = timeManager.getTimeString();

  lastTime = currentTime;

  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(35, 0);
  display.print(timeManager.getDateString());
  display.drawLine(0, 10, 128, 10, SH110X_WHITE);

  display.setTextSize(2);

  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(currentTime, 0, 0, &x1, &y1, &w, &h);

  int centerX = (128 - w) / 2;
  int centerY = (64 - h) / 2;

  display.setCursor(centerX, centerY);
  display.print(currentTime);

  display.setTextSize(1);
  display.setCursor(10, centerY + h + 4);
  display.println("   PRESS BUTTON");
  display.println("    TO CONTINUE");

  display.display();
}


void drawPhoneHandset(int x, int y) {
  // Main handle of the phone
  display.fillRoundRect(x, y, 24, 8, 3, SH110X_WHITE);
  // Left Earpiece
  display.fillCircle(x + 2, y + 6, 5, SH110X_WHITE);
  // Right Mouthpiece
  display.fillCircle(x + 22, y + 6, 5, SH110X_WHITE);
  // Cord / Antenna line (Optional style)
  display.drawLine(x+12, y, x+12, y-4, SH110X_WHITE); 
}


void showEmergencyCall() {
  int callDuration = 50; // 50 frames * 100ms = 5 seconds
  
  for (int i = 0; i < callDuration; i++) {
    display.clearDisplay();

    // --- 1. DRAW ICON & WAVES ---
    int iconX = 52;
    int iconY = 20;
    
    // Draw the phone slightly rotated (simulated by offset)
    drawPhoneHandset(iconX, iconY);

    // Animate Signal Waves (Expanding Circles)
    // We use (i % N) to make them loop repeatedly
    if (i % 10 > 2) display.drawCircle(iconX + 12, iconY + 4, 15, SH110X_WHITE);
    if (i % 10 > 5) display.drawCircle(iconX + 12, iconY + 4, 22, SH110X_WHITE);
    if (i % 10 > 8) display.drawCircle(iconX + 12, iconY + 4, 29, SH110X_WHITE);

    // --- 2. TEXT STATUS ---
    display.setTextSize(1);
    display.setCursor(35, 0);
    display.print("EMERGENCY");

    display.setTextSize(2);
    display.setCursor(15, 45);
    
    // Blink the "DIALING" text every few frames
    if (i % 8 < 5) {
      display.print("DIALING..");
    }

    // --- 3. PROGRESS BAR ---
    // A thin line at the bottom that fills up
    display.drawRect(14, 60, 100, 4, SH110X_WHITE); // Frame
    int progress = map(i, 0, callDuration, 0, 98);  // Calculate width
    display.fillRect(15, 61, progress, 2, SH110X_WHITE); // Fill

    display.display();
    vibrate.vibrateFor(100); // Short vibration pulse
    delay(100); // Animation speed
  }

  // --- 4. CALL CONNECTED SCREEN ---
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 25);
  display.print("CONNECTED");
  display.setTextSize(1);
  display.setCursor(30, 45);
  display.print("00:01");
  display.display();
  delay(2000); // Show "Connected" for 2 seconds
}

void SOSchackState()
{
  SOSbuttonState = digitalRead(12);

  if (SOSbuttonState == LOW)
  {
    // Button pressed
    if (!SOSisPressing)
    {
      SOSisPressing = true;
      SOSpressedTime = millis();
      SOSlongPressHandled = false; // Reset flag on new press
    } 
    else
    {
      // Check duration
      unsigned long holdDuration = millis() - SOSpressedTime;
      if (holdDuration >= 3000 && !SOSlongPressHandled)
      {
        showEmergencyCall();
        SOSlongPressHandled = true; // Prevent multiple toggles
      }
    }
  }
  else
  { // Button released
    SOSisPressing = false;
    // Don't change state when released - keep current state
  }
}

void chackState()
{
  buttonState = digitalRead(10);

  if (buttonState == LOW)
  {
    // Button pressed
    if (!isPressing)
    {
      isPressing = true;
      pressedTime = millis();
      longPressHandled = false; // Reset flag on new press
    }
    else
    {
      // Check duration
      unsigned long holdDuration = millis() - pressedTime;
      if (holdDuration >= 3000 && !longPressHandled)
      {
        // Toggle the state (on/off) only once per long press
        state.setState(!state.getState());
        longPressHandled = true; // Prevent multiple toggles
      }
    }
  }
  else
  { // Button released
    isPressing = false;
    // Don't change state when released - keep current state
  }
}

void showECGScreen()
{
  display.clearDisplay();

  if (!ecg.isConnected())
  {
    display.setCursor(20, 30);
    display.print("ATTACH PADS...");
    display.display();
    return;
  }

  // --- 1. Waveform Drawing Variables ---
  static int xPos = 0;   // Current horizontal position
  static int lastY = 32; // Previous vertical position

  // --- 2. Get Data & Process Decisions ---
  int raw = analogRead(36);
  bool isBeat = ecg.detectBeat(raw);
  int yPos = ecg.getGraphValue(40); // Top 40 pixels for graph

  // --- 3. Draw UI Header & Decisions ---
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("ECG Status: ");
  display.print(ecg.getRhythmStatus());

  display.drawFastHLine(0, 42, 128, SH110X_WHITE); // Middle Divider
  display.setCursor(0, 48);
  display.print("Stress: ");
  display.println(ecg.getStressLevel());

  // --- 4. THE MISSING PART: Drawing the Wave ---
  if (xPos > 0)
  {
    // Connect the previous point to the new point
    display.drawLine(xPos - 1, lastY, xPos, yPos, SH110X_WHITE);
  }

  // Update variables for the next loop
  lastY = yPos;
  xPos++;

  // Wrap around logic: Reset when the line reaches the end of the screen
  if (xPos >= 128)
  {
    xPos = 0;
    display.clearDisplay(); // Clear to start fresh on the left
  }

  // Heart beat animation (flash circle)
  if (isBeat)
  {
    display.fillCircle(120, 5, 4, SH110X_WHITE);
  }

  display.display();
}
// Simple function to draw a heart shape
void drawHeart(int x, int y, int size)
{
  display.fillCircle(x - size / 2, y, size / 2, SH110X_WHITE);
  display.fillCircle(x + size / 2, y, size / 2, SH110X_WHITE);
  display.fillTriangle(x - size, y, x + size, y, x, y + size, SH110X_WHITE);
}

void showHeartRateScreen()
{
  display.clearDisplay();
  HeartData data = heartMonitor.update();

  // --- Header ---
  display.setTextSize(1);
  display.setCursor(35, 0);
  display.print("PULSE");
  display.drawLine(0, 10, 128, 10, SH110X_WHITE);

  // --- IR Value ---
  display.setTextSize(1);
  display.setCursor(0, 14);
  display.print(">> ");
  display.print(data.irValue > 150000 ? "wait....." : "place the finger");

  // --- Current BPM ---
  display.setCursor(0, 24);
  display.print("BPM: ");
  display.print((int)data.currentBpm);

  // --- Average BPM ---
  display.setCursor(0, 34);
  display.print("AVG: ");
  display.print((int)data.avgBpm);

  // Draw a "Beating" Heart
  drawHeart(100, 30, 10);
  display.display();
}

void showWeatherScreen()
{
  display.clearDisplay();

  if (!weather.isConnected())
  {
    display.setTextSize(1);
    display.setCursor(20, 25);
    display.print("SENSOR NOT FOUND");
    display.display();
    return;
  }

  // --- 1. Top Header ---
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("ENV MONITOR");
  display.drawLine(0, 10, 128, 10, SH110X_WHITE); // Horizontal Line

  // --- 2. Main Temperature (Big Text) ---
  display.setTextSize(2);
  display.setCursor(0, 18);
  display.print((int)weather.getTemperature());
  display.setTextSize(1);
  display.print(" o"); // Degree symbol
  display.setTextSize(2);
  display.print("C");

  // --- 3. Forecast Icon/Text (Right Side) ---
  display.setTextSize(1);
  display.setCursor(65, 18);
  display.print("FORECAST:");
  display.setCursor(65, 30);
  display.print(weather.getForecast());

  // --- 4. Middle Divider ---
  display.drawFastHLine(0, 42, 128, SH110X_WHITE);

  // --- 5. Bottom Data Bar ---
  // Pressure on the left
  display.setTextSize(1);
  display.setCursor(0, 48);
  display.print("Prs: ");
  display.print((int)weather.getPressure());
  display.print("hPa");

  // Floors climbed on the right
  display.setCursor(75, 48);
  display.print("FLR: ");
  display.print(weather.getFloorsClimbed());

  // --- 6. Decorative Visual: Vertical Separator ---
  display.drawFastVLine(70, 45, 15, SH110X_WHITE);

  display.display();
}

void drawWarningIcon(int x, int y)
{
  // Draw Triangle Outline
  display.drawTriangle(x, y, x - 20, y + 35, x + 20, y + 35, SH110X_WHITE);

  // Draw Exclamation Mark
  display.fillRect(x - 2, y + 10, 5, 15, SH110X_WHITE); // The stick
  display.fillCircle(x, y + 30, 2, SH110X_WHITE);       // The dot
}

void fallAlert()
{
  int countdown = 100; // 10 seconds (if delay is 100ms)
  bool flashState = false;

  for (int i = 0; i < countdown; i++)
  {
    display.clearDisplay();

    // 1. Draw the Warning Icon in Center
    drawWarningIcon(64, 5);

    // 2. Big Text
    display.setTextSize(2);
    display.setCursor(10, 45); // Adjust based on your screen
    display.print("FALL DETECTED");

    // 3. Countdown Progress Bar (Fills up)
    // Map 'i' (0-100) to screen width (0-128)
    int barWidth = map(i, 0, 100, 0, 128);
    display.fillRect(0, 0, barWidth, 4, SH110X_WHITE); // Top loading bar

    // 4. "SOS" Text at bottom
    display.setTextSize(1);
    display.setCursor(35, 30);
    display.print("SENDING SOS...");

    // 5. Visual Flash Effect (Siren style)
    if (i % 5 == 0)
    { // Every 5th frame, flip the flash state
      flashState = !flashState;
      display.invertDisplay(flashState); // Inverts Black/White
    }

    display.display();

    // 6. Allow User to Cancel (Button Press)
    // If user presses any button, STOP the alert
    if (digitalRead(10) == LOW || digitalRead(11) == LOW)
    {
      display.invertDisplay(false); // Reset screen
      display.clearDisplay();
      display.setCursor(25, 25);
      display.print("CANCELLED");
      display.display();
      delay(1000);
      return; // Exit the function
    }

    delay(100); // Speed of the loop
  }

  // --- AFTER LOOP (If not cancelled) ---
  display.invertDisplay(false); // Reset normal colors
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 20);
  display.print("SOS SENT!");
  display.display();
  delay(2000);
}

void drawShoeIcon(int x, int y) {
    // Sole of the shoe
    display.fillRoundRect(x, y + 8, 20, 5, 2, SH110X_WHITE);
    // Main body
    display.fillRoundRect(x + 2, y + 2, 14, 8, 2, SH110X_WHITE);
    // Ankle part
    display.fillRect(x + 4, y - 2, 8, 6, SH110X_WHITE);
    // Laces (black lines on white body)
    display.drawLine(x + 6, y, x + 10, y, SH110X_BLACK);
    display.drawLine(x + 6, y + 3, x + 10, y + 3, SH110X_BLACK);
}
void showStepsScreen(){
  display.clearDisplay();

  // --- 1. Fetch Data ---
  int steps = activity.getSteps();
  int goal = 6000; // Daily Goal

  // Estimate Stats:
  // Avg step is 0.762 meters | Avg cal per step is 0.04
  float distanceKm = steps * 0.000762;
  int calories = steps * 0.04;

  // --- 2. Header & Icon ---
  drawShoeIcon(10, 5); // Draw shoe at top left
  display.setTextSize(1);
  display.setCursor(38, 5);
  display.print("PEDOMETER");
  display.drawLine(0, 18, 128, 18, SH110X_WHITE);

  // --- 3. Main Step Count (Big & Bold) ---
  display.setTextSize(2);
  // Center the number based on how many digits
  int xOffset = (steps < 10) ? 55 : (steps < 100) ? 50
                                : (steps < 1000)  ? 45
                                                  : 35;
  display.setCursor(xOffset, 25);
  display.print(steps);

  display.setTextSize(1);
  display.setCursor(xOffset + (String(steps).length() * 12) + 2, 32);
  display.print("stps");

  // --- 4. Goal Progress Bar ---
  // Draw frame
  display.drawRect(10, 45, 108, 6, SH110X_WHITE);

  // Calculate fill width (mapped to 0-106 pixels)
  int fillWidth = map(constrain(steps, 0, goal), 0, goal, 0, 106);
  display.fillRect(11, 46, fillWidth, 4, SH110X_WHITE);

  // --- 5. Stats Footer (Dist / Kcal) ---
  display.setCursor(5, 55);
  display.print("Dist: ");
  display.print(distanceKm, 1); // 1 decimal place
  display.print("km");

  display.setCursor(70, 55);
  display.print("Cal: ");
  display.print(calories);

  display.display();
}

int window = 0;               // 0 = Clock, 1 = Heart Rate, 2 = Weather/BMP
bool lastButton2State = HIGH; // To detect the *moment* the button is pressed
bool fallDetected = false;

   // --- 4. HEART RATE ALERT SCREEN ---
void showHeartAlert(String status, float bpm, int alertLevel) {
        // alertLevel 2 = CRITICAL (Flash Screen)
        // alertLevel 1 = WARNING (Static Screen)

        int duration = 30; // Show for 3 seconds
        bool flashState = false;

        for (int i = 0; i < duration; i++) {
            display.clearDisplay();

            // 1. Draw Heart Icon (Top Center)
            // Simple Heart Shape
            int x = 58; int y = 5;
            display.fillCircle(x, y, 5, SH110X_WHITE);
            display.fillCircle(x + 10, y, 5, SH110X_WHITE);
            display.fillTriangle(x - 5, y, x + 15, y, x + 5, y + 15, SH110X_WHITE);

            // 2. Status Label (e.g., "CRITICAL HIGH")
            display.setTextSize(1);
            // Center text roughly based on length
            display.setCursor(10, 25); 
            display.print(status);

            // 3. BPM Value (Big & Bold)
            display.setTextSize(2);
            display.setCursor(45, 40);
            display.print((int)bpm);
            
            display.setTextSize(1);
            display.setCursor(85, 47);
            display.print("BPM");

            // 4. Visual Urgency (Flashing)
            if (alertLevel == 2) { 
                // Flash every 5 frames for Critical alerts
                if (i % 5 == 0) {
                    flashState = !flashState;
                    display.invertDisplay(flashState);
                }
            } else {
                // Ensure screen is normal for non-critical warnings
                display.invertDisplay(false);
            }

            display.display();
            vibrate.vibrateFor(100); // Short vibration pulse
            delay(100);
        }
        
        // Reset screen state after loop
        display.invertDisplay(false);
    }
void chackHeartData(HeartData data){
if (data.threeMinAvg > 0) {
        
        // --- LEVEL 2: CRITICAL (Flash Screen + Buzzer + Vibrate) ---
        if (data.alertLevel == 2) {
            showHeartAlert(data.status, data.threeMinAvg, 2); 
        }
        
        // --- LEVEL 1: WARNING (Static Screen + Vibrate) ---
        else if (data.alertLevel == 1) {
            showHeartAlert(data.status, data.threeMinAvg, 1);
        }
        
        // --- LEVEL 0: NORMAL (Silent Cloud Update) ---
        else {

        }
    }
  
}
void loop(){

  timeManager.update(); // keep RTC fresh
  activity.update();
  vibrate.update();
  chackHeartData(heartMonitor.update());

  fallDetected = activity.isFallDetected();
  if (fallDetected)
  {
    // If a fall is detected, force the watch to turn on
    fallAlert();
    fallDetected = false; // Reset fall detected after alert
  }

  // 1. Check the power button state
  chackState();
  SOSchackState();

  // 2. ALWAYS sample heart rate (No delays allowed for accuracy)
  if (state.getState()){

    // --- BUTTON 2: SWITCH WINDOWS ---
    int currentButton2 = digitalRead(BUTTON2);
    if (currentButton2 == LOW && lastButton2State == HIGH)
    {
      window++;
      if (window > 4)
        window = 0;
      display.clearDisplay();
      vibrate.vibrateFor(500);  // Short vibration pulse when switching
      delay(50); 
    }
    lastButton2State = currentButton2;

    switch (window)
    {
    case 0:
      start();
      break;

    case 1:
      showHeartRateScreen();
      break;

    case 2:
      showWeatherScreen();
      break;
    case 3:
      showECGScreen();
      break;
    case 4:
      showStepsScreen();
      break;
    }
  }
  else
  {
    // If state is FALSE (Power Off), clear the display and reset window
    display.clearDisplay();
    display.display();
    window = 0;
  }

  // CRITICAL: Reduced delay to allow sensor to work
  delay(5);
}
