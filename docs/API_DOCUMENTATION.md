# 📡 API Documentation

Complete API reference for the Healthcare Device Backend Server.

## Base URL

```
Development: http://localhost:5000/api
Production: https://your-domain.com/api
```

## Authentication

Most endpoints require JWT authentication. Include the token in the Authorization header:

```
Authorization: Bearer <your-jwt-token>
```

Get token by logging in via `/api/auth/login`.

---

## 📚 Table of Contents

- [Authentication Endpoints](#authentication-endpoints)
- [Health Data Endpoints](#health-data-endpoints)
- [Alert Endpoints](#alert-endpoints)
- [Error Responses](#error-responses)
- [WebSocket Events](#websocket-events)

---

## Authentication Endpoints

### Register User

Create a new user account.

**Endpoint:** `POST /api/auth/register`

**Access:** Public

**Request Body:**
```json
{
  "name": "John Doe",
  "email": "john@example.com",
  "password": "securepassword123",
  "role": "caregiver",
  "phone": "+1234567890",
  "bloodType": "O+"
}
```

**Response (201):**
```json
{
  "success": true,
  "message": "User registered successfully",
  "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
  "user": {
    "id": "507f1f77bcf86cd799439011",
    "name": "John Doe",
    "email": "john@example.com",
    "role": "caregiver",
    "deviceIds": []
  }
}
```

---

### Login

Authenticate and receive JWT token.

**Endpoint:** `POST /api/auth/login`

**Access:** Public

**Request Body:**
```json
{
  "email": "john@example.com",
  "password": "securepassword123"
}
```

**Response (200):**
```json
{
  "success": true,
  "message": "Login successful",
  "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
  "user": {
    "id": "507f1f77bcf86cd799439011",
    "name": "John Doe",
    "email": "john@example.com",
    "role": "caregiver",
    "deviceIds": ["watch_001"],
    "notificationPreferences": {
      "email": true,
      "criticalOnly": false
    }
  }
}
```

---

### Get Current User

Get currently logged-in user information.

**Endpoint:** `GET /api/auth/me`

**Access:** Private

**Headers:**
```
Authorization: Bearer <token>
```

**Response (200):**
```json
{
  "success": true,
  "user": {
    "id": "507f1f77bcf86cd799439011",
    "name": "John Doe",
    "email": "john@example.com",
    "role": "caregiver",
    "deviceIds": ["watch_001"],
    "phone": "+1234567890",
    "bloodType": "O+",
    "notificationPreferences": {
      "email": true,
      "criticalOnly": false
    },
    "createdAt": "2026-01-20T10:00:00.000Z",
    "updatedAt": "2026-01-23T10:00:00.000Z"
  }
}
```

---

### Update Profile

Update user profile information.

**Endpoint:** `PUT /api/auth/me`

**Access:** Private

**Request Body:**
```json
{
  "name": "John Smith",
  "phone": "+1234567890",
  "bloodType": "O+",
  "notificationPreferences": {
    "email": true,
    "criticalOnly": true
  },
  "deviceIds": ["watch_001", "watch_002"]
}
```

**Response (200):**
```json
{
  "success": true,
  "message": "Profile updated successfully",
  "user": { /* updated user object */ }
}
```

---

### Update Password

Change user password.

**Endpoint:** `PUT /api/auth/password`

**Access:** Private

**Request Body:**
```json
{
  "currentPassword": "oldpassword123",
  "newPassword": "newpassword456"
}
```

**Response (200):**
```json
{
  "success": true,
  "message": "Password updated successfully"
}
```

---

## Health Data Endpoints

### Receive Sensor Data

Receive health data from ESP32 device.

**Endpoint:** `POST /api/data`

**Access:** Public (for ESP32 devices)

**Request Body:**
```json
{
  "deviceId": "watch_001",
  "heartRate": 75,
  "spo2": 98,
  "temperature": 36.5,
  "pressure": "120/80",
  "alertStatus": "NORMAL",
  "timestamp": "2026-01-23T10:30:00Z"
}
```

**Response (201):**
```json
{
  "success": true,
  "message": "Data received successfully",
  "data": {
    "id": "507f1f77bcf86cd799439012",
    "deviceId": "watch_001",
    "heartRate": 75,
    "alertStatus": "NORMAL"
  },
  "alert": null
}
```

**With Alert (201):**
```json
{
  "success": true,
  "message": "Data received successfully",
  "data": {
    "id": "507f1f77bcf86cd799439012",
    "deviceId": "watch_001",
    "heartRate": 115,
    "alertStatus": "CRITICAL_HIGH"
  },
  "alert": {
    "id": "507f1f77bcf86cd799439013",
    "type": "CRITICAL_HIGH",
    "message": "Critical: Heart rate dangerously high at 115 BPM (> 110)"
  }
}
```

---

### Get Latest Vitals

Get the most recent health data for a device.

**Endpoint:** `GET /api/data/:deviceId`

**Access:** Private

**Example:** `GET /api/data/watch_001`

**Response (200):**
```json
{
  "success": true,
  "data": {
    "_id": "507f1f77bcf86cd799439012",
    "deviceId": "watch_001",
    "heartRate": 75,
    "spo2": 98,
    "temperature": 36.5,
    "pressure": "120/80",
    "alertStatus": "NORMAL",
    "timestamp": "2026-01-23T10:30:00.000Z",
    "createdAt": "2026-01-23T10:30:05.000Z",
    "updatedAt": "2026-01-23T10:30:05.000Z"
  }
}
```

---

### Get Historical Data

Get historical health data with statistics.

**Endpoint:** `GET /api/data/:deviceId/history`

**Access:** Private

**Query Parameters:**
- `period` (optional): Time period - `24h`, `7d`, `30d` (default: `24h`)
- `limit` (optional): Max number of records (default: 100, max: 1000)

**Example:** `GET /api/data/watch_001/history?period=24h&limit=50`

**Response (200):**
```json
{
  "success": true,
  "period": "24h",
  "startDate": "2026-01-22T10:30:00.000Z",
  "endDate": "2026-01-23T10:30:00.000Z",
  "count": 50,
  "stats": {
    "count": 50,
    "avgHeartRate": 74,
    "minHeartRate": 62,
    "maxHeartRate": 88,
    "avgTemperature": "36.4",
    "avgSpo2": 97
  },
  "data": [
    {
      "_id": "507f1f77bcf86cd799439012",
      "deviceId": "watch_001",
      "heartRate": 75,
      "spo2": 98,
      "temperature": 36.5,
      "pressure": "120/80",
      "alertStatus": "NORMAL",
      "createdAt": "2026-01-23T10:30:00.000Z"
    },
    // ... more records
  ]
}
```

---

### Get Latest N Readings

Get the most recent N health data readings.

**Endpoint:** `GET /api/data/:deviceId/latest/:count`

**Access:** Private

**Example:** `GET /api/data/watch_001/latest/10`

**Response (200):**
```json
{
  "success": true,
  "count": 10,
  "data": [ /* array of health data objects */ ]
}
```

---

## Alert Endpoints

### Get All Alerts

Get alerts with optional filtering.

**Endpoint:** `GET /api/alerts`

**Access:** Private

**Query Parameters:**
- `deviceId` (optional): Filter by device
- `type` (optional): Filter by alert type
- `handled` (optional): Filter by handled status (`true`/`false`)
- `limit` (optional): Results per page (default: 50)
- `page` (optional): Page number (default: 1)

**Example:** `GET /api/alerts?deviceId=watch_001&handled=false&limit=20`

**Response (200):**
```json
{
  "success": true,
  "count": 20,
  "total": 45,
  "page": 1,
  "pages": 3,
  "data": [
    {
      "_id": "507f1f77bcf86cd799439013",
      "deviceId": "watch_001",
      "alertType": "CRITICAL_HIGH",
      "heartRate": 115,
      "message": "Critical: Heart rate dangerously high at 115 BPM (> 110)",
      "severity": "critical",
      "handled": false,
      "handledBy": null,
      "handledAt": null,
      "emailSent": true,
      "emailSentAt": "2026-01-23T10:30:10.000Z",
      "createdAt": "2026-01-23T10:30:05.000Z",
      "updatedAt": "2026-01-23T10:30:05.000Z"
    },
    // ... more alerts
  ]
}
```

---

### Get Unread Alerts

Get all unhandled (unread) alerts.

**Endpoint:** `GET /api/alerts/unread`

**Access:** Private

**Query Parameters:**
- `deviceId` (optional): Filter by device

**Example:** `GET /api/alerts/unread?deviceId=watch_001`

**Response (200):**
```json
{
  "success": true,
  "count": 3,
  "data": [ /* array of unhandled alert objects */ ]
}
```

---

### Get Alert Statistics

Get alert statistics for a time period.

**Endpoint:** `GET /api/alerts/stats`

**Access:** Private

**Query Parameters:**
- `deviceId` (optional): Filter by device
- `period` (optional): Time period - `24h`, `7d`, `30d` (default: `24h`)

**Example:** `GET /api/alerts/stats?deviceId=watch_001&period=7d`

**Response (200):**
```json
{
  "success": true,
  "stats": {
    "total": 15,
    "critical": 3,
    "warning": 12,
    "handled": 10,
    "unhandled": 5,
    "timeRange": "7d",
    "startDate": "2026-01-16T10:30:00.000Z",
    "endDate": "2026-01-23T10:30:00.000Z"
  }
}
```

---

### Get Alert by ID

Get detailed information about a specific alert.

**Endpoint:** `GET /api/alerts/:id`

**Access:** Private

**Example:** `GET /api/alerts/507f1f77bcf86cd799439013`

**Response (200):**
```json
{
  "success": true,
  "data": { /* complete alert object with populated healthDataId */ }
}
```

---

### Mark Alert as Handled

Mark an alert as handled/acknowledged.

**Endpoint:** `POST /api/alerts/:id/handle`

**Access:** Private

**Example:** `POST /api/alerts/507f1f77bcf86cd799439013/handle`

**Request Body (optional):**
```json
{
  "notes": "Patient checked, vitals stabilized"
}
```

**Response (200):**
```json
{
  "success": true,
  "message": "Alert marked as handled",
  "data": {
    "_id": "507f1f77bcf86cd799439013",
    "handled": true,
    "handledBy": "John Doe",
    "handledAt": "2026-01-23T11:00:00.000Z",
    // ... other alert fields
  }
}
```

---

### Resend Alert Email

Manually resend an alert email.

**Endpoint:** `POST /api/alerts/:id/resend-email`

**Access:** Private

**Example:** `POST /api/alerts/507f1f77bcf86cd799439013/resend-email`

**Response (200):**
```json
{
  "success": true,
  "message": "Email sent successfully"
}
```

---

## Error Responses

All error responses follow this format:

```json
{
  "success": false,
  "message": "Error description",
  "errors": [  // Optional, for validation errors
    {
      "field": "email",
      "message": "Valid email is required"
    }
  ]
}
```

### Common HTTP Status Codes

- `200` - Success
- `201` - Created
- `400` - Bad Request (validation error)
- `401` - Unauthorized (missing or invalid token)
- `403` - Forbidden (insufficient permissions)
- `404` - Not Found
- `429` - Too Many Requests (rate limit exceeded)
- `500` - Internal Server Error

---

## WebSocket Events

Connect to WebSocket server for real-time updates.

### Connection

```javascript
import io from 'socket.io-client';

const socket = io('http://localhost:5000');

socket.on('connected', (data) => {
  console.log('Connected:', data.message);
});
```

### Subscribe to Device

```javascript
socket.emit('subscribeDevice', 'watch_001');
```

### Events from Server

**Health Data Update:**
```javascript
socket.on('healthData', (data) => {
  console.log('New health data:', data);
  // data = {
  //   deviceId: 'watch_001',
  //   data: { /* health data object */ },
  //   alert: { /* alert object if triggered */ }
  // }
});
```

**Alert Handled:**
```javascript
socket.on('alertHandled', (data) => {
  console.log('Alert handled:', data);
  // data = {
  //   alertId: '507f1f77bcf86cd799439013',
  //   handledBy: 'John Doe',
  //   handledAt: '2026-01-23T11:00:00.000Z'
  // }
});
```

### Disconnect

```javascript
socket.emit('unsubscribeDevice', 'watch_001');
socket.disconnect();
```

---

## Rate Limiting

- Authentication endpoints: 100 requests per 15 minutes per IP
- Alert endpoints: 100 requests per 15 minutes per IP
- Data POST endpoint: No limit (for ESP32 devices)

---

## Testing with cURL

### Register and Login
```bash
# Register
curl -X POST http://localhost:5000/api/auth/register \
  -H "Content-Type: application/json" \
  -d '{"name":"Test User","email":"test@example.com","password":"test123","role":"caregiver"}'

# Login
TOKEN=$(curl -X POST http://localhost:5000/api/auth/login \
  -H "Content-Type: application/json" \
  -d '{"email":"test@example.com","password":"test123"}' \
  | jq -r '.token')

echo $TOKEN
```

### Send Health Data
```bash
curl -X POST http://localhost:5000/api/data \
  -H "Content-Type: application/json" \
  -d '{"deviceId":"watch_001","heartRate":75,"spo2":98,"temperature":36.5,"pressure":"120/80"}'
```

### Get Latest Data
```bash
curl http://localhost:5000/api/data/watch_001 \
  -H "Authorization: Bearer $TOKEN"
```

### Get Alerts
```bash
curl http://localhost:5000/api/alerts \
  -H "Authorization: Bearer $TOKEN"
```

---

For questions or issues, create an issue on GitHub.
