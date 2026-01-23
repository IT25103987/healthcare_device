# HealthCare+ Web App - Quick Start Guide

## 🚀 Start Development in 30 Seconds

### Step 1: Navigate to Project
```bash
cd web-app
```

### Step 2: Configure Environment
```bash
cp .env.example .env.local
```

### Step 3: Start Dev Server
```bash
npm run dev
```

Open browser to: **http://localhost:5173/**

### Step 4: Login with Demo Account
```
Email: demo@healthcare.com
Password: Demo123!
```

---

## 📋 What You Get

✅ **Real-time Health Monitoring**
- Heart Rate, Blood Pressure, O2 Saturation, Temperature, Respiratory Rate
- Status indicators (normal/warning/critical)
- Health trend charts

✅ **Alert Management**
- Real-time alerts with severity levels
- Acknowledge and resolve functionality
- Alert history

✅ **User Features**
- Profile management
- Health history with filters
- Settings and preferences

✅ **Professional UI**
- Responsive mobile-first design
- Medical blue theme
- Smooth animations and transitions

---

## 🔗 Integration with Backend

### 1. Update Environment Variables
Edit `.env.local`:
```
VITE_API_URL=http://your-backend:3000/api
VITE_WS_URL=http://your-backend:3000
```

### 2. Expected Backend Endpoints
```
POST   /api/auth/login
GET    /api/patients/profile
GET    /api/alerts
GET    /api/health/vitals-summary
... and more (see documentation)
```

### 3. WebSocket Events
Backend should emit:
- `vitals-update` - New vital signs
- `alert-created` - New alert
- `device-status` - Device status changes

---

## 📦 Available Commands

```bash
# Start development server
npm run dev

# Build for production
npm run build

# Preview production build
npm run preview

# Run linter
npm run lint

# Install dependencies (if needed)
npm install
```

---

## 📂 Project Structure

```
src/
├── pages/           # Page components (Login, Dashboard, etc.)
├── components/      # Reusable components (Auth, Layout, Charts)
├── services/        # API, Auth, WebSocket services
├── context/         # State management (Auth, Data)
├── utils/           # Formatters, validators
├── App.jsx          # Main app with routing
└── main.jsx         # App entry point
```

---

## 🎨 Customization

### Change Color Theme
Edit `tailwind.config.js` and update the color scheme

### Add New Pages
1. Create component in `src/pages/`
2. Add route to `src/App.jsx`
3. Add nav link to `src/components/Layout/Navbar.jsx`

### Add API Endpoints
Add methods to `src/services/api.js`

---

## 🔒 Security

The app uses JWT tokens for authentication. Tokens are:
- Stored securely in localStorage
- Auto-injected in API requests
- Refreshed automatically
- Cleared on logout

---

## 🆘 Troubleshooting

### Port Already in Use
```bash
npm run dev -- --port 3000
```

### Dependencies Issues
```bash
rm -rf node_modules package-lock.json
npm install
```

### Build Errors
```bash
npm run build -- --verbose
```

---

## 📚 Documentation

- `README.md` - Project overview
- `WEB_APP_SUMMARY.md` - Detailed documentation
- `WEBAPP_SETUP.md` - Setup instructions
- `CHECKLIST.md` - Implementation checklist

---

## ✨ Features at a Glance

| Feature | Status |
|---------|--------|
| Authentication | ✅ JWT-based |
| Real-time Updates | ✅ WebSocket |
| Data Visualization | ✅ Recharts |
| Forms & Validation | ✅ Complete |
| Responsive Design | ✅ Mobile-first |
| Error Handling | ✅ Toast notifications |
| State Management | ✅ Context API |
| Code Quality | ✅ ESLint passing |

---

## 🎯 Next Steps

1. **Integrate Backend** - Connect to your API endpoints
2. **Customize Theme** - Adjust colors and branding
3. **Add More Pages** - Extend functionality
4. **Deploy** - Build and deploy to production

---

## 💡 Tips

- Use `npm run lint` before committing code
- Check browser console for detailed errors
- Enable browser DevTools for React debugging
- Use VSCode with ES7+ snippets for faster coding

---

**Ready to build amazing healthcare apps?** 🚀

Start developing now with `npm run dev`!
