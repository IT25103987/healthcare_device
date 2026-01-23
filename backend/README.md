# Healthcare Device Backend

Backend server for the healthcare wearable device monitoring system. Receives sensor data from ESP32 devices, manages alerts, and provides REST APIs for the web dashboard.

## 🚀 Features

- ✅ REST API for health data management
- ✅ Real-time updates via WebSocket (Socket.IO)
- ✅ MongoDB database for data persistence
- ✅ JWT authentication and authorization
- ✅ Email alert system with Nodemailer
- ✅ Automatic alert detection for abnormal vitals
- ✅ CORS and security middleware
- ✅ Rate limiting for API endpoints

## 📋 Prerequisites

- Node.js 18.x or higher
- MongoDB (local or MongoDB Atlas)
- Gmail account (for email notifications)

## 🔧 Installation

### 1. Install Dependencies

```bash
cd backend
npm install
```

### 2. Environment Configuration

Create a `.env` file in the backend directory:

```bash
cp .env.example .env
```

Edit `.env` with your configuration:

```env
PORT=5000
MONGODB_URI=mongodb://localhost:27017/healthcare
JWT_SECRET=your_jwt_secret_minimum_32_characters_long
NODE_ENV=development

# Email Configuration (Gmail SMTP)
EMAIL_HOST=smtp.gmail.com
EMAIL_PORT=587
EMAIL_SECURE=false
EMAIL_USER=your-email@gmail.com
EMAIL_PASSWORD=your-app-specific-password

# Frontend URL (for CORS)
CLIENT_URL=http://localhost:5173

# Alert Configuration
SEND_EMAIL_ALERTS=true
ALERT_EMAIL_RECIPIENTS=caregiver@example.com,doctor@example.com
```

### 3. MongoDB Setup

#### Option A: Local MongoDB
```bash
# Install MongoDB
# macOS
brew install mongodb-community

# Ubuntu
sudo apt install mongodb

# Start MongoDB
mongod --dbpath /path/to/data/directory
```

#### Option B: MongoDB Atlas (Free Cloud)
1. Go to [MongoDB Atlas](https://www.mongodb.com/cloud/atlas)
2. Create a free account
3. Create a new cluster
4. Get your connection string
5. Update `MONGODB_URI` in `.env`

### 4. Email Configuration (Gmail)

To send email alerts:

1. Enable 2-Factor Authentication on your Gmail account
2. Generate an App Password:
   - Go to Google Account Settings → Security
   - Under "2-Step Verification", click "App passwords"
   - Generate a new app password for "Mail"
3. Use the generated password in `EMAIL_PASSWORD`

## 🏃‍♂️ Running the Server

### Development Mode (with auto-reload)
```bash
npm run dev
```

### Production Mode
```bash
npm start
```

The server will start on `http://localhost:5000` (or the PORT specified in .env)

## 📚 API Endpoints

### Health Check
```
GET /health
```

### Authentication

**Register User**
```
POST /api/auth/register
Body: {
  "name": "John Doe",
  "email": "john@example.com",
  "password": "securepassword",
  "role": "caregiver"
}
```

**Login**
```
POST /api/auth/login
Body: {
  "email": "john@example.com",
  "password": "securepassword"
}
```

**Get Current User**
```
GET /api/auth/me
Headers: {
  "Authorization": "Bearer <token>"
}
```

### Health Data

**Receive Data from ESP32**
```
POST /api/data
Body: {
  "deviceId": "watch_001",
  "heartRate": 75,
  "spo2": 98,
  "temperature": 36.5,
  "pressure": "120/80",
  "alertStatus": "NORMAL",
  "timestamp": "2026-01-22T10:30:00Z"
}
```

**Get Latest Vitals**
```
GET /api/data/:deviceId
Headers: {
  "Authorization": "Bearer <token>"
}
```

**Get Historical Data**
```
GET /api/data/:deviceId/history?period=24h&limit=100
Headers: {
  "Authorization": "Bearer <token>"
}
Query Parameters:
- period: 24h, 7d, 30d
- limit: number of records (max 1000)
```

### Alerts

**Get All Alerts**
```
GET /api/alerts
Headers: {
  "Authorization": "Bearer <token>"
}
```

**Get Unread Alerts**
```
GET /api/alerts/unread
Headers: {
  "Authorization": "Bearer <token>"
}
```

**Get Alert Statistics**
```
GET /api/alerts/stats?period=24h
Headers: {
  "Authorization": "Bearer <token>"
}
```

**Mark Alert as Handled**
```
POST /api/alerts/:id/handle
Headers: {
  "Authorization": "Bearer <token>"
}
```

## 🚨 Alert Thresholds

The backend automatically detects abnormal vitals:

### Heart Rate
- **< 40 BPM** → 🚨 CRITICAL_LOW (Immediate email)
- **40-49 BPM** → ⚠️ WARNING_LOW (Email notification)
- **50-90 BPM** → ✅ NORMAL (No action)
- **91-109 BPM** → ⚠️ WARNING_HIGH (Email notification)
- **110+ BPM** → 🚨 CRITICAL_HIGH (Immediate email)

## 🔌 WebSocket Events

The server emits real-time events via Socket.IO:

### Client Events
```javascript
// Connect to server
socket.on('connected', (data) => {
  console.log('Connected:', data.message);
});

// Receive health data updates
socket.on('healthData', (data) => {
  console.log('New data:', data);
  // data = { deviceId, data, alert }
});

// Alert handled notification
socket.on('alertHandled', (data) => {
  console.log('Alert handled:', data);
  // data = { alertId, handledBy, handledAt }
});

// Subscribe to specific device
socket.emit('subscribeDevice', 'watch_001');
```

## 🗄️ Database Models

### HealthData
```javascript
{
  deviceId: String,
  heartRate: Number,
  spo2: Number,
  temperature: Number,
  pressure: String,
  alertStatus: String,
  timestamp: Date,
  createdAt: Date,
  updatedAt: Date
}
```

### Alert
```javascript
{
  deviceId: String,
  alertType: String,
  heartRate: Number,
  message: String,
  severity: String,
  handled: Boolean,
  handledBy: String,
  handledAt: Date,
  emailSent: Boolean,
  emailSentAt: Date,
  healthDataId: ObjectId,
  createdAt: Date,
  updatedAt: Date
}
```

### User
```javascript
{
  name: String,
  email: String,
  password: String (hashed),
  role: String,
  deviceIds: [String],
  notificationPreferences: {
    email: Boolean,
    criticalOnly: Boolean
  },
  phone: String,
  bloodType: String,
  medicalHistory: String,
  medications: Array,
  emergencyContact: Object,
  createdAt: Date,
  updatedAt: Date
}
```

## 🧪 Testing

### Test with cURL

**Health Check:**
```bash
curl http://localhost:5000/health
```

**Register User:**
```bash
curl -X POST http://localhost:5000/api/auth/register \
  -H "Content-Type: application/json" \
  -d '{
    "name": "Test User",
    "email": "test@example.com",
    "password": "test123",
    "role": "caregiver"
  }'
```

**Send Health Data (simulate ESP32):**
```bash
curl -X POST http://localhost:5000/api/data \
  -H "Content-Type: application/json" \
  -d '{
    "deviceId": "watch_001",
    "heartRate": 75,
    "spo2": 98,
    "temperature": 36.5,
    "pressure": "120/80",
    "alertStatus": "NORMAL"
  }'
```

## 🚀 Deployment

### Deploy to Railway

1. Install Railway CLI:
```bash
npm i -g @railway/cli
```

2. Login to Railway:
```bash
railway login
```

3. Initialize project:
```bash
railway init
```

4. Add environment variables in Railway dashboard

5. Deploy:
```bash
railway up
```

### Deploy to Render

1. Create account at [Render.com](https://render.com)
2. Create new Web Service
3. Connect your GitHub repository
4. Set build command: `cd backend && npm install`
5. Set start command: `cd backend && npm start`
6. Add environment variables
7. Deploy

### Deploy to Heroku

1. Install Heroku CLI
2. Login:
```bash
heroku login
```

3. Create app:
```bash
heroku create healthcare-backend
```

4. Add MongoDB addon:
```bash
heroku addons:create mongolab:sandbox
```

5. Set environment variables:
```bash
heroku config:set JWT_SECRET=your_secret
heroku config:set EMAIL_USER=your_email
# ... etc
```

6. Deploy:
```bash
git push heroku main
```

## 🔒 Security

- JWT tokens expire after 30 days
- Passwords hashed with bcryptjs (salt rounds: 10)
- Helmet.js for security headers
- Rate limiting on authentication endpoints
- CORS configured for frontend origin only
- Input validation with express-validator

## 📝 Environment Variables

| Variable | Description | Required | Default |
|----------|-------------|----------|---------|
| PORT | Server port | No | 5000 |
| MONGODB_URI | MongoDB connection string | Yes | - |
| JWT_SECRET | Secret for JWT signing | Yes | - |
| NODE_ENV | Environment (development/production) | No | development |
| EMAIL_HOST | SMTP server hostname | No | smtp.gmail.com |
| EMAIL_PORT | SMTP server port | No | 587 |
| EMAIL_USER | Email account username | No | - |
| EMAIL_PASSWORD | Email account password | No | - |
| CLIENT_URL | Frontend URL for CORS | No | http://localhost:5173 |
| SEND_EMAIL_ALERTS | Enable/disable email alerts | No | true |
| ALERT_EMAIL_RECIPIENTS | Comma-separated email list | No | - |

## 🐛 Troubleshooting

### MongoDB Connection Failed
- Check if MongoDB is running: `mongod --version`
- Verify MONGODB_URI in .env
- Check network connectivity (if using Atlas)

### Email Not Sending
- Verify EMAIL_USER and EMAIL_PASSWORD
- Check if 2FA is enabled and app password is generated
- Test SMTP connection

### CORS Errors
- Ensure CLIENT_URL matches your frontend URL
- Check if frontend is running on the specified port

### Port Already in Use
- Change PORT in .env
- Kill process using the port: `lsof -ti:5000 | xargs kill`

## 📄 License

MIT

## 👥 Contributors

Healthcare Device Team

## 📞 Support

For issues and questions, please create an issue on GitHub.
