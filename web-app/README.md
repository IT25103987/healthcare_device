# HealthCare+ Web Application

A modern React + Vite web application for real-time health monitoring and management.

## Features

- 🏥 Real-time vital signs monitoring (Heart Rate, Blood Pressure, Temperature, O2 Saturation, Respiratory Rate)
- 🚨 Alert management system
- 📊 Health data visualization with charts
- 👤 User profile management
- 🔐 Authentication with JWT tokens
- 🔄 WebSocket support for real-time updates
- 📱 Responsive mobile-first design
- �� Professional medical UI with Tailwind CSS

## Project Structure

```
src/
├── components/
│   ├── Auth/
│   │   └── LoginForm.jsx
│   ├── Dashboard/
│   │   ├── VitalsDisplay.jsx
│   │   └── AlertsList.jsx
│   ├── Charts/
│   │   └── ChartComponent.jsx
│   └── Layout/
│       ├── Navbar.jsx
│       └── Layout.jsx
├── pages/
│   ├── Login.jsx
│   ├── Dashboard.jsx
│   ├── Alerts.jsx
│   ├── History.jsx
│   ├── Profile.jsx
│   └── Settings.jsx
├── services/
│   ├── api.js           # API client with interceptors
│   ├── auth.js          # Authentication service
│   └── websocket.js     # WebSocket service
├── context/
│   ├── AuthContext.jsx  # Authentication state management
│   └── DataContext.jsx  # Health data state management
├── utils/
│   ├── formatters.js    # Date and data formatting utilities
│   └── validators.js    # Input validation utilities
├── App.jsx
├── main.jsx
├── index.css
└── App.css
```

## Installation

1. Navigate to the project directory:
```bash
cd web-app
```

2. Install dependencies:
```bash
npm install
```

3. Create environment file:
```bash
cp .env.example .env.local
```

4. Update `.env.local` with your API and WebSocket URLs:
```
VITE_API_URL=http://localhost:3000/api
VITE_WS_URL=http://localhost:3000
```

## Development

Start the development server:

```bash
npm run dev
```

The application will be available at `http://localhost:5173/`

## Building for Production

Build the application:

```bash
npm run build
```

Preview the production build:

```bash
npm run preview
```

## Dependencies

- **React** - UI library
- **React Router** - Client-side routing
- **Axios** - HTTP client
- **Socket.IO** - Real-time communication
- **Recharts** - Data visualization
- **Tailwind CSS** - Utility-first CSS framework
- **date-fns** - Date utilities
- **react-hot-toast** - Toast notifications

## Authentication

The application includes:
- JWT-based authentication
- Protected routes
- Automatic token refresh
- Secure logout

Demo credentials:
- Email: `demo@healthcare.com`
- Password: `Demo123!`

## API Integration

The API service (`services/api.js`) provides:
- Request/response interceptors
- Automatic token injection
- Error handling with redirect on 401

Endpoints:
- **Auth**: `/auth/login`, `/auth/register`, `/auth/change-password`
- **Patients**: `/patients/profile`, `/patients/:id/vitals`, `/patients/:id/history`
- **Alerts**: `/alerts`, `/alerts/:id/acknowledge`, `/alerts/:id/resolve`
- **Devices**: `/devices`, `/devices/:id`
- **Health**: `/health/vitals-summary`, `/health/chart-data`

## Real-time Updates

WebSocket connection for live health data updates:
- Auto-reconnection with exponential backoff
- Event subscription/unsubscription
- Proper cleanup on disconnect

## Tailwind CSS

Configured with custom medical color palette and utility classes:
- `.card` - Card component styling
- `.btn-primary`, `.btn-secondary`, `.btn-danger` - Button variants
- `.input-field` - Form input styling
- `.badge` - Badge component with severity variants
- Medical color theme with `medical-*` variants

## Browser Support

- Chrome (latest)
- Firefox (latest)
- Safari (latest)
- Edge (latest)

## License

MIT
