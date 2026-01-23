# 📖 User Guide

Complete guide for using the Healthcare Device Web Dashboard.

## 🎯 Overview

The web dashboard allows caregivers to monitor patient vitals in real-time, receive alerts, and view historical health data.

---

## 🚪 Getting Started

### Accessing the Dashboard

1. Open your web browser
2. Navigate to: `https://your-app.vercel.app` (or your deployed URL)
3. You'll see the login page

### First Time Login

**Demo Account:**
- Email: `demo@healthcare.com`
- Password: `Demo123!`

**Create Your Own Account:**
1. Click "Don't have an account? Register"
2. Fill in your information:
   - Name
   - Email
   - Password (minimum 6 characters)
   - Role (Caregiver recommended)
3. Click "Register"
4. You'll be logged in automatically

---

## 📊 Dashboard Page

After login, you'll see the main dashboard with real-time patient vitals.

### Vitals Display

The dashboard shows four main vital signs:

**❤️ Heart Rate (BPM)**
- Normal: 50-90 BPM (green)
- Elevated: 91-109 BPM (yellow)
- Critical: <40 or >110 BPM (red)

**🫁 Blood Oxygen (SpO₂)**
- Normal: 95-100% (green)
- Low: <95% (yellow/red)

**🌡️ Temperature (°C)**
- Normal: 36-37.5°C (green)
- Fever: >37.5°C (yellow/red)
- Hypothermia: <36°C (yellow/red)

**💉 Blood Pressure (mmHg)**
- Normal: 90/60 - 140/90 (green)
- High: >140/90 (yellow/red)
- Low: <90/60 (yellow)

### Real-Time Updates

- Dashboard auto-refreshes every 30 seconds
- WebSocket connection provides instant updates
- Connection status shown in header
- New data highlighted briefly

### Quick Actions

- **Refresh**: Click refresh icon to manually update
- **View Details**: Click on any vital card for more info
- **Acknowledge Alert**: Click alert to mark as handled

---

## 🔔 Alerts Page

Monitor and manage all health alerts.

### Alert List

Alerts are shown with:
- **Icon**: Indicates severity (🔴 critical, 🟡 warning)
- **Time**: When alert was triggered
- **Device**: Which device triggered the alert
- **Vital**: The abnormal value
- **Status**: Handled or Unhandled

### Alert Actions

**Mark as Handled:**
1. Click on alert
2. Click "Mark as Handled"
3. Optionally add notes
4. Click "Confirm"

**Filter Alerts:**
- By Device: Select device from dropdown
- By Status: Show all, unhandled only, handled only
- By Date: Select date range

### Alert Notifications

When new alert arrives:
- Toast notification appears (top-right)
- Alert count badge updates
- Alert plays sound (if enabled in settings)

---

## 📈 History Page

View historical health data with charts and statistics.

### Time Range Selection

Choose period to view:
- Last 24 hours
- Last 7 days
- Last 30 days
- Custom date range

### Charts

**Heart Rate Chart:**
- Line chart showing BPM over time
- Color-coded zones (green = normal, yellow = warning, red = critical)
- Hover for exact values

**All Vitals Chart:**
- Multi-line chart with all vitals
- Toggle vitals on/off by clicking legend
- Zoom by dragging on chart

**Alert Frequency:**
- Bar chart showing alerts per day
- Color-coded by severity

### Statistics Summary

- Average values
- Minimum and maximum
- Standard deviation
- Total readings count

### Export Data

**Download CSV:**
1. Click "Export" button
2. Select time range
3. Click "Download CSV"
4. Opens in Excel/spreadsheet software

---

## 👤 Profile Page

Manage patient and caregiver information.

### Personal Information

**Edit Profile:**
1. Click "Edit Profile"
2. Update information:
   - Name
   - Phone number
   - Email (requires password)
3. Click "Save Changes"

### Medical Information

**Patient Details:**
- Blood Type
- Medical History
- Current Medications
- Allergies

**Emergency Contact:**
- Name
- Phone
- Relationship

### Device Management

**Add Device:**
1. Click "Add Device"
2. Enter device ID (e.g., watch_001)
3. Click "Add"

**Remove Device:**
1. Find device in list
2. Click "Remove"
3. Confirm deletion

---

## ⚙️ Settings Page

Configure dashboard preferences.

### Notification Preferences

**Email Notifications:**
- Toggle on/off
- Choose "All Alerts" or "Critical Only"
- Add/remove notification email addresses

**In-App Notifications:**
- Toast notifications
- Sound alerts
- Badge notifications

### Display Preferences

**Theme:**
- Light mode (default)
- Dark mode

**Units:**
- Temperature: Celsius or Fahrenheit
- Time: 12-hour or 24-hour

**Refresh Rate:**
- 15 seconds
- 30 seconds (default)
- 60 seconds
- Manual only

### Account Settings

**Change Password:**
1. Enter current password
2. Enter new password
3. Confirm new password
4. Click "Update Password"

**Two-Factor Authentication:**
- Enable/disable 2FA (if available)

**Session Management:**
- View active sessions
- Log out other devices

---

## 🔐 Security

### Password Requirements

- Minimum 6 characters
- Combination of letters and numbers recommended
- Special characters supported

### Session Timeout

- Automatic logout after 24 hours of inactivity
- Warning before logout (if enabled)

### Data Privacy

- All data encrypted in transit (HTTPS)
- Access logs maintained
- HIPAA-compliant practices followed

---

## 🆘 Troubleshooting

### Common Issues

**Can't Login:**
- Verify email and password
- Check caps lock
- Try "Forgot Password"
- Clear browser cache

**No Data Showing:**
- Check device is online
- Verify device ID in profile
- Check WiFi connection on ESP32
- Try refreshing page

**Alerts Not Received:**
- Check notification settings
- Verify email address
- Check spam folder
- Test with manual alert

**Charts Not Loading:**
- Refresh page
- Check time range selection
- Verify device has data for selected period
- Clear browser cache

**Connection Lost:**
- Check internet connection
- Backend server may be down
- Refresh page
- Try logging out and back in

---

## 📱 Mobile Usage

### Responsive Design

The dashboard works on:
- Smartphones (iOS, Android)
- Tablets
- Desktop computers

### Mobile Tips

**Portrait Mode:**
- Vitals stack vertically
- Scroll to see all data
- Tap cards for details

**Landscape Mode:**
- Better chart viewing
- Side-by-side layout
- More data visible

**Touch Gestures:**
- Swipe to navigate
- Pinch to zoom charts
- Pull down to refresh

---

## 🎓 Best Practices

### For Caregivers

1. **Check Dashboard Regularly**
   - At least 2-3 times per day
   - Before and after patient activities
   - After medication administration

2. **Respond to Alerts Promptly**
   - Acknowledge within 15 minutes
   - Take appropriate action
   - Document response

3. **Review History Weekly**
   - Look for trends
   - Note patterns
   - Share with healthcare provider

4. **Keep Profile Updated**
   - Update medications
   - Add medical events
   - Maintain emergency contacts

### For System Administrators

1. **Monitor System Health**
   - Check backend logs daily
   - Verify email delivery
   - Test alert system weekly

2. **Maintain Data**
   - Archive old data (>90 days)
   - Export backups monthly
   - Clean up test data

3. **User Management**
   - Review user access quarterly
   - Remove inactive users
   - Update roles as needed

---

## 🔍 Advanced Features

### Keyboard Shortcuts

- `Ctrl/Cmd + R`: Refresh data
- `Ctrl/Cmd + /`: Search
- `Esc`: Close modals
- `A`: Acknowledge selected alert
- `H`: Go to history
- `P`: Go to profile

### URL Parameters

- `?device=watch_001`: Filter by device
- `?period=7d`: Set history period
- `?alert=new`: Highlight new alerts

---

## 📞 Support

### Getting Help

**Documentation:**
- Setup Guide: `docs/SETUP_GUIDE.md`
- API Documentation: `docs/API_DOCUMENTATION.md`
- Deployment Guide: `docs/DEPLOYMENT_GUIDE.md`

**Contact:**
- GitHub Issues: Create an issue
- Email Support: support@example.com

---

## 🎯 Tips & Tricks

1. **Pin Dashboard Tab**: Keep it open for instant access
2. **Enable Notifications**: Don't miss critical alerts
3. **Set Custom Thresholds**: Adjust for individual patients
4. **Use Export Feature**: Share data with doctors
5. **Check History Before Appointments**: Review trends

---

## ⚠️ Medical Disclaimer

**Important:**
- This system is for monitoring purposes only
- Not a substitute for professional medical advice
- Always consult healthcare professionals
- In emergencies, call 911 immediately
- Do not rely solely on this system for critical decisions

---

## 📝 Feedback

We welcome your feedback!
- Feature requests
- Bug reports
- Improvement suggestions
- User experience feedback

Create an issue on GitHub or contact us via email.

---

**Made with ❤️ for Better Healthcare**

For technical documentation, see other guides in the `docs/` folder.
