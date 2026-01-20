#include <Wire.h>
#include <RTClib.h>
#include "TimeManager.h" // Double quotes look in include/ or local folder
#include "State.h"
#include "HartRate.h"
#include "WeatherManager.h"

// yasiru

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


  //wether manager setup//
  if(weather.begin()){
    Serial.println("✅ BMP280 OK");
  } else {
    Serial.println("❌ BMP280 FAIL");
  } 

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
  display.print((int)data.bpm);

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

int window = 0;               // 0 = Clock, 1 = Heart Rate, 2 = Weather/BMP
bool lastButton2State = HIGH; // To detect the *moment* the button is pressed

void loop()
{
  timeManager.update(); // keep RTC fresh

  // 1. Check the power button state

  // 2. CONSTANTLY sample the heart rate (CRITICAL: No delays here!)
  // This must run every single loop cycle to "see" the pulse wave
  //   int currentBPM = heartMonitor.update();
  // Serial.print("IR: "); Serial.println(data.irValue);
  // Serial.print("BPM: "); Serial.println(data.bpm);
  // Serial.print("AVG BPM: "); Serial.println(data.avgBpm);

  // 1. Check if the Power Button is being held (updates state)
  chackState();

  // 2. ALWAYS sample heart rate (No delays allowed for accuracy)

  // 3. ONLY run display logic if the watch "state" is ON
  if (state.getState())
  {

    // --- BUTTON 2: SWITCH WINDOWS ---
    int currentButton2 = digitalRead(BUTTON2);
    if (currentButton2 == LOW && lastButton2State == HIGH)
    {
      window++;
      if (window > 2)
        window = 0;
      display.clearDisplay();
      delay(50); // Small debounce
    }
    lastButton2State = currentButton2;

    // --- WINDOW MANAGER ---
    switch (window)
    {
    case 0:
      start(); // Show Clock
      break;

    case 1:
      showHeartRateScreen();
      break;

    case 2:
      showWeatherScreen();
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

  // chackState();

  // if (state.getState())
  // {
  //   start();
  // }

  // button2 = digitalRead(BUTTON2);
  // if (button2 == LOW){
  //   if (state.getState()){
  // Serial.println(heartMonitor.update());
  // }

  // display.clearDisplay();

  // int currentBPM =

  // // --- 3. Draw Center Heart Rate ---
  // display.setTextSize(2);
  // static int displayBPM = 0;

  // String bpmStr = String(displayBPM) + " BPM";

  // int16_t x1, y1;
  // uint16_t w, h;
  // display.getTextBounds(bpmStr, 0, 0, &x1, &y1, &w, &h);

  // int centerX = (128 - w) / 2;
  // int centerY = (64 - h) / 2;

  // display.setCursor(centerX, centerY);
  // display.print(bpmStr);

  // // --- 4. Pulse Indicator ---
  // // Visual feedback that the sensor is working
  // if (currentBPM > 0)
  // {
  //   display.fillCircle(centerX - 15, centerY + 7, 4, SH110X_WHITE);
  // }

  // // ---------- FOOTER ----------
  // display.setTextSize(1);
  // display.setCursor(10, 50);
  // display.println("  SENSING PULSE...");

  // display.display();
  // }
  // button2 = digitalRead(button2);
  // if(button2 == HIGH){
  //   Serial.println("swiched");
  // }
  // /* ================= HEART RATE ================= */
  // long irValue = particleSensor.getIR();
  // if (irValue > 5000 && checkForBeat(irValue)) {
  //   long delta = millis() - lastBeat;
  //   lastBeat = millis();

  //   if (delta > 300 && delta < 2000) {
  //     bpm = 60000 / delta;
  //   }
  // }

  // /* ================= BMP280 ================= */
  // float temp = NAN, pres = NAN;
  // if (bmpOK) {
  //   temp = bmp.readTemperature();
  //   pres = bmp.readPressure() / 100.0F;
  // }

  // /* ================= MPU9250 ================= */
  // Wire.beginTransmission(MPU_ADDR);
  // Wire.write(0x3B);
  // Wire.endTransmission(false);
  // Wire.requestFrom(MPU_ADDR, 6, true);

  // AcX = Wire.read()<<8 | Wire.read();
  // AcY = Wire.read()<<8 | Wire.read();
  // AcZ = Wire.read()<<8 | Wire.read();

  // /* ================= SERIAL OUTPUT ================= */
  // Serial.println("----- SENSOR DATA -----");
  // Serial.print("Temp      : "); Serial.print(temp); Serial.println(" C");
  // Serial.print("Pressure  : "); Serial.print(pres); Serial.println(" hPa");
  // Serial.print("ACC X: "); Serial.print(AcX);
  // Serial.print(" Y: "); Serial.print(AcY);
  // Serial.print(" Z: "); Serial.println(AcZ);
  // Serial.println("-----------------------");

  // /* ================= OLED OUTPUT ================= */
}
