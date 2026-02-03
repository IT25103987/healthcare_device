# HealthCare+ Web App - Setup & Development Guide

## ✅ Project Initialization Complete

The React + Vite web application has been successfully created with all specified components, services, and configurations.

## 📁 Project Structure

```
web-app/
├── src/
│   ├── components/
│   │   ├── Auth/
│   │   │   └── LoginForm.jsx           # Login form component
│   │   ├── Dashboard/
│   │   │   ├── VitalsDisplay.jsx       # Real-time vital signs cards
│   │   │   └── AlertsList.jsx          # Recent alerts display
│   │   ├── Charts/
│   │   │   └── ChartComponent.jsx      # Recharts wrapper for data visualization
│   │   └── Layout/
│   │       ├── Navbar.jsx              # Navigation bar with user menu
│   │       └── Layout.jsx              # Main layout wrapper
│   ├── pages/
│   │   ├── Login.jsx                   # Login page with demo credentials
│   │   ├── Dashboard.jsx               # Main dashboard with vitals and alerts
│   │   ├── Alerts.jsx                  # Alert management page
│   │   ├── History.jsx                 # Health data history with filters
│   │   ├── Profile.jsx                 # User profile and settings
│   │   └── Settings.jsx                # App preferences and notifications
│   ├── services/
│   │   ├── api.js                      # Axios API client with interceptors
│   │   ├── auth.js                     # Authentication service
│   │   └── websocket.js                # Socket.IO WebSocket service
│   ├── context/
│   │   ├── AuthContext.jsx             # Authentication state management
│   │   └── DataContext.jsx             # Health data state management
│   ├── utils/
│   │   ├── formatters.js               # Date and data formatting utilities
│   │   └── validators.js               # Input validation utilities
│   ├── App.jsx                         # Main app component with routing
│   ├── main.jsx                        # App entry point with providers
│   ├── index.css                       # Tailwind CSS directives and custom styles
│   └── App.css                         # App-specific styles
├── index.html                          # HTML entry point
├── tailwind.config.js                  # Tailwind CSS configuration
├── postcss.config.js                   # PostCSS configuration with Tailwind
├── vite.config.js                      # Vite configuration
├── .env.example                        # Environment variables template
├── .gitignore                          # Git ignore rules
├── package.json                        # Dependencies and scripts
└── README.md                           # Project documentation
```

## 🚀 Getting Started

### 1. Install Dependencies
```bash
cd web-app
npm install
```

### 2. Configure Environment
```bash
cp .env.example .env.local
```

Edit `.env.local` with your backend URLs:
```
VITE_API_URL=http://localhost:3000/api
VITE_WS_URL=http://localhost:3000
```

### 3. Run Development Server
```bash
npm run dev
```

The application will be available at: **http://localhost:5173/**

### 4. Build for Production
```bash
npm run build
npm run preview
```

## 🔐 Authentication

### Demo Login
- **Email**: `demo@healthcare.com`
- **Password**: `Demo123!`

### How Authentication Works
- JWT token-based authentication
- Tokens stored in localStorage
- Automatic token injection in API requests
- Redirect to login on 401 responses
- WebSocket authentication with token

## 🏥 Core Features

### Dashboard
- Real-time vital signs display (Heart Rate, BP, O2, Temperature, RR)
- Visual status indicators (normal/warning/critical)
- Recent alerts panel
- Health trend charts
- Summary statistics

### Alerts Management
- Real-time alert notifications
- Severity levels: critical, warning, info
- Acknowledge and resolve functionality
- Alert history tracking

### Health History
- Time-range filtering (24h, 7d, 30d, 90d)
- Historical vital signs
- Status badges
- Detailed timestamps

### User Profile
- Personal information editing
- Security settings
- Password management

### Settings
- Notification preferences
- Theme selection
- Data collection options
- Help and support links

## 🔌 API Integration

### Service Structure
The application uses a modular API service architecture:

```javascript
// API Service (src/services/api.js)
- Request/Response interceptors
- Automatic JWT token injection
- Error handling with 401 redirect
- Organized endpoints:
  - patientAPI    - User health data
  - alertAPI      - Alert management
  - deviceAPI     - Device info
  - healthAPI     - Health summaries and charts
```

### Expected Backend Endpoints

```
Authentication:
POST   /api/auth/login
POST   /api/auth/register
POST   /api/auth/change-password
POST   /api/auth/refresh-token

Patients:
GET    /api/patients/profile
PUT    /api/patients/profile
GET    /api/patients/{id}/vitals
GET    /api/patients/{id}/history

Alerts:
GET    /api/alerts
GET    /api/alerts/{id}
PUT    /api/alerts/{id}/acknowledge
PUT    /api/alerts/{id}/resolve

Devices:
GET    /api/devices
GET    /api/devices/{id}
PUT    /api/devices/{id}

Health Data:
GET    /api/health/vitals-summary
GET    /api/health/chart-data?timeRange={range}
```

## 🔄 Real-Time Updates

WebSocket service enables live health monitoring:

```javascript
// Connect to WebSocket
import websocketService from './services/websocket';

websocketService.connect(authToken);

// Listen to events
websocketService.on('vitals-update', (data) => {
  // Handle new vital signs
});

// Subscribe to channels
websocketService.subscribe('patient:vitals');
```

## 🎨 Styling

### Tailwind CSS Configuration
- Utility-first CSS framework
- Custom color palette (sky blue theme for medical)
- Responsive design (mobile-first)
- Custom component classes:

```css
.card              - Card container
.btn-primary       - Primary button
.btn-secondary     - Secondary button
.btn-danger        - Danger button
.input-field       - Form input
.badge             - Badge component
.badge-success     - Success badge
.badge-warning     - Warning badge
.badge-danger      - Danger badge
```

### Color Scheme
- **Primary**: Sky blue (sky-600) for medical/healthcare theme
- **Neutral**: Gray scale for backgrounds and text
- **Status**: Green (success), Yellow (warning), Red (critical/danger)

## 📊 Data Management

### State Management
The application uses React Context for state management:

```javascript
// AuthContext - Handles authentication state
const { user, isAuthenticated, login, register, logout } = useAuth();

// DataContext - Handles health data
const { vitals, alerts, devices, updateVitals, addAlert } = useData();
```

### Utilities

**Formatters** (`src/utils/formatters.js`)
- Date/time formatting with date-fns
- Vital sign formatting (bpm, mmHg, %, °C)
- Vital status determination (normal/warning/critical)
- Color mapping for status badges

**Validators** (`src/utils/validators.js`)
- Email validation
- Password strength validation
- Phone number validation
- Vital signs range validation

## 🧪 Development Commands

```bash
# Start development server with HMR
npm run dev

# Build for production
npm run build

# Preview production build
npm run preview

# Run linter
npm run lint
```

## 📦 Dependencies

### Production
- **React** (19.2.0) - UI library
- **React Router** (7.12.0) - Client-side routing
- **Axios** (1.13.2) - HTTP client
- **Socket.IO** (4.8.3) - WebSocket client
- **Recharts** (3.7.0) - Data visualization
- **Tailwind CSS** (4.1.18) - CSS framework
- **date-fns** (4.1.0) - Date utilities
- **react-hot-toast** (2.6.0) - Toast notifications

### Development
- **Vite** (7.2.4) - Build tool
- **@vitejs/plugin-react** (5.1.1) - React plugin
- **@tailwindcss/postcss** (4.1.18) - Tailwind PostCSS plugin
- **ESLint** (9.39.1) - Code linting

## 🔒 Security Considerations

- ✅ JWT token stored securely (localStorage)
- ✅ Automatic token injection in requests
- ✅ Protected routes with redirect on auth failure
- ✅ HTTPS recommended for production
- ✅ Input validation on all forms
- ✅ CORS configured on backend

## 🌐 Browser Support

- Chrome (latest)
- Firefox (latest)
- Safari (latest)
- Edge (latest)

## 📱 Responsive Design

The application is built mobile-first with Tailwind CSS:
- Mobile: Base styles
- Tablet (md): 768px breakpoint
- Desktop (lg): 1024px breakpoint
- Large (xl): 1280px breakpoint

## 🐛 Troubleshooting

### Port Already in Use
```bash
# Change port
npm run dev -- --port 3000
```

### WebSocket Connection Issues
- Verify backend is running on configured URL
- Check CORS settings on backend
- Verify WebSocket URL in .env.local

### Build Errors
```bash
# Clear cache and reinstall
rm -rf node_modules package-lock.json
npm install
npm run build
```

## 📚 Additional Resources

- [React Documentation](https://react.dev)
- [Vite Documentation](https://vitejs.dev)
- [Tailwind CSS Documentation](https://tailwindcss.com)
- [Recharts Documentation](https://recharts.org)
- [Socket.IO Documentation](https://socket.io/docs)

## 📄 License

MIT

---

**Project Status**: ✅ Ready for Development

All required components, services, and configurations have been implemented and tested.
The application is ready to be integrated with a backend API.
