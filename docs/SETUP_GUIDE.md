# 📘 Complete Setup Guide

This guide will walk you through setting up the entire healthcare monitoring system from scratch.

## 📋 Table of Contents

1. [Prerequisites](#prerequisites)
2. [Hardware Setup](#hardware-setup)
3. [ESP32 Code Upload](#esp32-code-upload)
4. [WiFi Configuration (Optional)](#wifi-configuration-optional)
5. [Backend Server Setup](#backend-server-setup)
6. [Database Setup](#database-setup)
7. [Email Configuration](#email-configuration)
8. [Web App Setup](#web-app-setup)
9. [Testing](#testing)
10. [Troubleshooting](#troubleshooting)

---

## 1. Prerequisites

### Hardware Requirements
- ESP32-S3 Development Board
- MAX30102 Heart Rate & SpO2 Sensor
- BMP280 Temperature & Pressure Sensor
- MPU9250 Accelerometer/Gyroscope
- SH1106 OLED Display (128x64)
- DS3231 RTC Module
- AD8232 ECG Sensor (optional)
- Vibration Motor
- Breadboard and jumper wires
- USB cable for programming

### Software Requirements
- **For ESP32:**
  - PlatformIO IDE or VS Code with PlatformIO extension
  - USB drivers for ESP32
  
- **For Backend:**
  - Node.js 18.x or higher
  - MongoDB (local or cloud)
  - npm or yarn
  
- **For Web App:**
  - Node.js 18.x or higher
  - Modern web browser (Chrome, Firefox, Edge)
  
- **For Email Alerts:**
  - Gmail account with 2FA enabled

---

## 2. Hardware Setup

### Wiring Diagram

```
ESP32-S3 Pin Connections:

I2C Bus (SDA=GPIO 8, SCL=GPIO 9):
├─ MAX30102 (Heart Rate Sensor)
│  ├─ VCC → 3.3V
│  ├─ GND → GND
│  ├─ SDA → GPIO 8
│  └─ SCL → GPIO 9
│
├─ BMP280 (Temperature/Pressure)
│  ├─ VCC → 3.3V
│  ├─ GND → GND
│  ├─ SDA → GPIO 8
│  └─ SCL → GPIO 9
│
├─ MPU9250 (Accelerometer)
│  ├─ VCC → 3.3V
│  ├─ GND → GND
│  ├─ SDA → GPIO 8
│  └─ SCL → GPIO 9
│
├─ SH1106 OLED Display
│  ├─ VCC → 3.3V
│  ├─ GND → GND
│  ├─ SDA → GPIO 8
│  └─ SCL → GPIO 9
│
└─ DS3231 RTC
   ├─ VCC → 3.3V
   ├─ GND → GND
   ├─ SDA → GPIO 8
   └─ SCL → GPIO 9

Buttons:
├─ Power Button → GPIO 10
├─ Button 2     → GPIO 11
└─ Button 3     → GPIO 12

ECG Sensor (AD8232):
├─ LO+ → GPIO 38
├─ LO- → GPIO 37
├─ OUT → GPIO 36
├─ VCC → 3.3V
└─ GND → GND

Vibration Motor:
└─ Control → (via transistor)
```

### Assembly Steps

1. **Mount Components on Breadboard**
   - Place ESP32-S3 in center
   - Add sensors around ESP32
   - Keep wiring neat and organized

2. **Connect I2C Bus**
   - All I2C devices share SDA (GPIO 8) and SCL (GPIO 9)
   - Use pull-up resistors (4.7kΩ) on SDA and SCL lines
   - Keep I2C wires short (< 20cm) for reliable communication

3. **Connect Power**
   - All sensors use 3.3V power
   - Do NOT use 5V on sensors (may damage them)
   - Add decoupling capacitors (100nF) near each sensor

4. **Connect Buttons**
   - Use INPUT_PULLUP mode (no external resistors needed)
   - Connect one side to GPIO, other to GND

5. **Double Check Connections**
   - Verify all VCC → 3.3V
   - Verify all GND → GND
   - Check I2C connections
   - Ensure no short circuits

---

## 3. ESP32 Code Upload

### Step 1: Install PlatformIO

**VS Code Method (Recommended):**
1. Install Visual Studio Code
2. Open VS Code Extensions (Ctrl+Shift+X)
3. Search for "PlatformIO IDE"
4. Click Install
5. Restart VS Code

**Standalone Method:**
1. Download PlatformIO IDE from https://platformio.org/
2. Install and launch

### Step 2: Open Project

```bash
# Clone or open the project
cd healthcare_device

# PlatformIO will automatically detect platformio.ini
```

### Step 3: Configure Upload Port

Edit `platformio.ini`:
```ini
[env:esp32-s3]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino

upload_protocol = esptool
upload_port = COM6        # ← Change to your port
monitor_port = COM6       # ← Change to your port
monitor_speed = 115200
```

**Find Your Port:**
- **Windows**: COM3, COM4, COM6, etc. (Check Device Manager)
- **macOS**: /dev/cu.usbserial-xxx or /dev/cu.SLAB_USBtoUART
- **Linux**: /dev/ttyUSB0 or /dev/ttyACM0

### Step 4: Build and Upload

1. **Connect ESP32 to Computer** via USB
2. **Build Project**: Click PlatformIO Build button (checkmark icon)
3. **Upload**: Click PlatformIO Upload button (arrow icon)
4. **Monitor**: Click Serial Monitor to see output

Expected Output:
```
✅ BMP280 OK
✅ MPU9250 OK
✅ OLED OK
✅ RTC OK
🚀 ALL SENSORS READY
```

### Step 5: Set RTC Time (First Time Only)

In `main.cpp`, uncomment this line **once**:
```cpp
// Around line 109
rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
```

Upload, then **immediately comment it out again** to prevent resetting time on every boot.

---

## 4. WiFi Configuration (Optional)

**Note:** WiFi is completely optional. The device works perfectly without it.

See detailed guide: [docs/WIFI_INTEGRATION.md](WIFI_INTEGRATION.md)

**Quick Steps:**
1. Add ArduinoJson library to `platformio.ini`
2. Include WiFiSender.h in main.cpp
3. Uncomment WiFi initialization code
4. Set your WiFi credentials
5. Upload code

---

## 5. Backend Server Setup

### Step 1: Install Dependencies

```bash
cd backend
npm install
```

### Step 2: Create Environment File

```bash
cp .env.example .env
```

Edit `.env`:
```env
PORT=5000
MONGODB_URI=mongodb://localhost:27017/healthcare
JWT_SECRET=your_very_long_secret_key_minimum_32_characters
NODE_ENV=development

# Email Configuration
EMAIL_HOST=smtp.gmail.com
EMAIL_PORT=587
EMAIL_SECURE=false
EMAIL_USER=your-email@gmail.com
EMAIL_PASSWORD=your-app-password

# Frontend URL
CLIENT_URL=http://localhost:5173

# Alerts
SEND_EMAIL_ALERTS=true
ALERT_EMAIL_RECIPIENTS=caregiver@example.com,doctor@example.com
```

### Step 3: Generate JWT Secret

```bash
# Generate a random 32-character string
node -e "console.log(require('crypto').randomBytes(32).toString('hex'))"
```

Copy the output to `JWT_SECRET` in `.env`

---

## 6. Database Setup

### Option A: Local MongoDB

**Install MongoDB:**

**macOS (using Homebrew):**
```bash
brew tap mongodb/brew
brew install mongodb-community
brew services start mongodb-community
```

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install mongodb
sudo systemctl start mongodb
sudo systemctl enable mongodb
```

**Windows:**
1. Download MongoDB Community Server from https://www.mongodb.com/try/download/community
2. Run installer
3. Start MongoDB service

**Verify Installation:**
```bash
mongosh  # Should connect to MongoDB shell
```

**.env Configuration:**
```env
MONGODB_URI=mongodb://localhost:27017/healthcare
```

### Option B: MongoDB Atlas (Cloud - Free)

1. **Create Account:**
   - Go to https://www.mongodb.com/cloud/atlas
   - Sign up for free

2. **Create Cluster:**
   - Click "Build a Database"
   - Choose "FREE" tier (M0 Sandbox)
   - Select region closest to you
   - Click "Create Cluster"

3. **Create Database User:**
   - Go to Database Access
   - Add New Database User
   - Choose password authentication
   - Username: `healthcareuser`
   - Password: `SecurePassword123!`
   - Database User Privileges: Read and write to any database

4. **Whitelist IP Address:**
   - Go to Network Access
   - Add IP Address
   - Click "Allow Access from Anywhere" (0.0.0.0/0)
   - Or add your specific IP for better security

5. **Get Connection String:**
   - Go to Databases → Connect
   - Choose "Connect your application"
   - Copy the connection string
   - Replace `<password>` with your password

Example:
```
mongodb+srv://healthcareuser:SecurePassword123!@cluster0.xxxxx.mongodb.net/healthcare?retryWrites=true&w=majority
```

**.env Configuration:**
```env
MONGODB_URI=mongodb+srv://healthcareuser:SecurePassword123!@cluster0.xxxxx.mongodb.net/healthcare?retryWrites=true&w=majority
```

---

## 7. Email Configuration

### Step 1: Enable 2-Factor Authentication

1. Go to your Gmail account
2. Click your profile → Manage your Google Account
3. Go to Security
4. Enable 2-Step Verification

### Step 2: Generate App Password

1. In Google Account → Security
2. Under "2-Step Verification", scroll down
3. Click "App passwords"
4. Select app: Mail
5. Select device: Other (Custom name) → "Healthcare Device"
6. Click Generate
7. Copy the 16-character password (no spaces)

### Step 3: Configure .env

```env
EMAIL_USER=your-email@gmail.com
EMAIL_PASSWORD=abcdefghijklmnop  # ← 16-character app password
SEND_EMAIL_ALERTS=true
ALERT_EMAIL_RECIPIENTS=caregiver@example.com,doctor@example.com
```

**Multiple Recipients:**
Separate emails with commas (no spaces):
```env
ALERT_EMAIL_RECIPIENTS=caregiver1@example.com,caregiver2@example.com,doctor@example.com
```

### Step 4: Test Email

Start the server and send test data with critical heart rate:
```bash
cd backend
npm start

# In another terminal
curl -X POST http://localhost:5000/api/data \
  -H "Content-Type: application/json" \
  -d '{
    "deviceId": "watch_001",
    "heartRate": 35,
    "spo2": 98,
    "temperature": 36.5,
    "pressure": "120/80"
  }'
```

Check your email for critical alert!

---

## 8. Web App Setup

### Step 1: Install Dependencies

```bash
cd web-app
npm install
```

### Step 2: Create Environment File

```bash
cp .env.example .env.local
```

Edit `.env.local`:
```env
VITE_API_URL=http://localhost:5000/api
VITE_WS_URL=http://localhost:5000
```

**For Production:**
```env
VITE_API_URL=https://your-backend.railway.app/api
VITE_WS_URL=https://your-backend.railway.app
```

### Step 3: Start Development Server

```bash
npm run dev
```

Visit: http://localhost:5173

---

## 9. Testing

### Test Backend

1. **Health Check:**
```bash
curl http://localhost:5000/health
```

Expected:
```json
{
  "success": true,
  "message": "Server is running",
  "timestamp": "2026-01-23T10:00:00.000Z",
  "uptime": 123.456
}
```

2. **Register User:**
```bash
curl -X POST http://localhost:5000/api/auth/register \
  -H "Content-Type: application/json" \
  -d '{
    "name": "Test Caregiver",
    "email": "caregiver@example.com",
    "password": "test123",
    "role": "caregiver"
  }'
```

3. **Login:**
```bash
curl -X POST http://localhost:5000/api/auth/login \
  -H "Content-Type: application/json" \
  -d '{
    "email": "caregiver@example.com",
    "password": "test123"
  }'
```

Copy the `token` from response.

4. **Send Health Data:**
```bash
curl -X POST http://localhost:5000/api/data \
  -H "Content-Type: application/json" \
  -d '{
    "deviceId": "watch_001",
    "heartRate": 75,
    "spo2": 98,
    "temperature": 36.5,
    "pressure": "120/80"
  }'
```

### Test Web App

1. **Open Browser:** http://localhost:5173
2. **Login:**
   - Email: demo@healthcare.com
   - Password: Demo123!
3. **Check Dashboard:** Should see vitals cards
4. **Test Alerts:** Send critical heart rate via backend
5. **Check History:** View historical data charts

### Test ESP32 (if WiFi enabled)

1. Open Serial Monitor (115200 baud)
2. Check WiFi connection:
```
[WiFiSender] Connecting to WiFi...
[WiFiSender] ✅ WiFi Connected!
[WiFiSender] IP Address: 192.168.1.105
```
3. Watch for data transmission:
```
[WiFiSender] Sending data:
{"deviceId":"watch_001","heartRate":75...}
[WiFiSender] ✅ HTTP POST Success - Code: 200
```

### End-to-End Test

1. **ESP32** collects heart rate
2. **WiFi** sends data to backend (if enabled)
3. **Backend** receives and validates data
4. **Alert** detected for abnormal values
5. **Email** sent to caregivers
6. **Web Dashboard** updates in real-time
7. **Caregiver** acknowledges alert

---

## 10. Troubleshooting

### ESP32 Issues

**Upload Failed:**
- Check USB cable (use data cable, not charging-only)
- Hold BOOT button during upload
- Verify correct COM port
- Try different USB port
- Install/update USB drivers

**Sensors Not Working:**
- Check wiring (VCC, GND, SDA, SCL)
- Verify I2C addresses (use I2C scanner)
- Check 3.3V power supply (use multimeter)
- Ensure pull-up resistors on I2C lines
- Keep I2C wires short

**Display Issues:**
- Check I2C address (0x3C or 0x3D)
- Adjust I2C speed (try 100kHz)
- Check contrast settings
- Verify display initialization

### Backend Issues

**MongoDB Connection Failed:**
- Check if MongoDB is running: `mongosh`
- Verify MONGODB_URI in .env
- Check firewall settings
- For Atlas: verify IP whitelist and credentials

**Email Not Sending:**
- Verify Gmail app password (16 chars, no spaces)
- Check 2FA is enabled
- Test SMTP connection
- Check spam folder
- Verify EMAIL_USER and EMAIL_PASSWORD

**Port Already in Use:**
```bash
# Find process using port 5000
lsof -ti:5000 | xargs kill
# Or change PORT in .env
```

### Web App Issues

**API Connection Failed:**
- Verify backend is running
- Check VITE_API_URL in .env.local
- Check CORS settings in backend
- Verify firewall/antivirus not blocking

**WebSocket Not Connecting:**
- Check VITE_WS_URL matches backend URL
- Verify Socket.IO is enabled in backend
- Check browser console for errors

**Build Failed:**
- Clear node_modules: `rm -rf node_modules && npm install`
- Clear build cache: `rm -rf dist .vite`
- Update dependencies: `npm update`

---

## 📚 Next Steps

After successful setup:

1. **Deploy to Production** - See [DEPLOYMENT_GUIDE.md](DEPLOYMENT_GUIDE.md)
2. **Learn API** - See [API_DOCUMENTATION.md](API_DOCUMENTATION.md)
3. **User Guide** - See [USER_GUIDE.md](USER_GUIDE.md)

---

## 🎉 Success!

Your healthcare monitoring system is now set up and running!

- ESP32 device is monitoring vitals
- Backend is receiving and storing data
- Web dashboard is displaying real-time information
- Email alerts are being sent for critical conditions

For questions or issues, refer to the troubleshooting section or create an issue on GitHub.
