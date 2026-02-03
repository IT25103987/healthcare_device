import React, { useEffect } from 'react';
import { BrowserRouter as Router, Routes, Route, Navigate } from 'react-router-dom';
import { Toaster } from 'react-hot-toast';
import { useAuth } from './context/AuthContext';
import websocketService from './services/websocket';
import './App.css';

// Pages
import LoginPage from './pages/Login';
import DashboardPage from './pages/Dashboard';
import AlertsPage from './pages/Alerts';
import HistoryPage from './pages/History';
import ProfilePage from './pages/Profile';
import SettingsPage from './pages/Settings';

// Protected Route Component
const ProtectedRoute = ({ children }) => {
  const { isAuthenticated } = useAuth();
  
  if (!isAuthenticated) {
    return <Navigate to="/login" replace />;
  }
  
  return children;
};

function App() {
  const { isAuthenticated, user } = useAuth();

  useEffect(() => {
    // Connect to WebSocket when authenticated
    if (isAuthenticated && user) {
      const token = localStorage.getItem('authToken');
      if (token) {
        websocketService.connect(token).catch(err => {
          console.warn('Failed to connect to WebSocket:', err);
        });
      }
    }

    // Cleanup on logout
    return () => {
      if (!isAuthenticated) {
        websocketService.disconnect();
      }
    };
  }, [isAuthenticated, user]);

  return (
    <Router>
      <Toaster position="top-right" />
      <Routes>
        {/* Public routes */}
        <Route 
          path="/login" 
          element={isAuthenticated ? <Navigate to="/dashboard" replace /> : <LoginPage />} 
        />

        {/* Protected routes */}
        <Route
          path="/dashboard"
          element={
            <ProtectedRoute>
              <DashboardPage />
            </ProtectedRoute>
          }
        />
        <Route
          path="/alerts"
          element={
            <ProtectedRoute>
              <AlertsPage />
            </ProtectedRoute>
          }
        />
        <Route
          path="/history"
          element={
            <ProtectedRoute>
              <HistoryPage />
            </ProtectedRoute>
          }
        />
        <Route
          path="/profile"
          element={
            <ProtectedRoute>
              <ProfilePage />
            </ProtectedRoute>
          }
        />
        <Route
          path="/settings"
          element={
            <ProtectedRoute>
              <SettingsPage />
            </ProtectedRoute>
          }
        />

        {/* Default redirect */}
        <Route path="/" element={<Navigate to={isAuthenticated ? "/dashboard" : "/login"} replace />} />
        <Route path="*" element={<Navigate to={isAuthenticated ? "/dashboard" : "/login"} replace />} />
      </Routes>
    </Router>
  );
}

export default App;
