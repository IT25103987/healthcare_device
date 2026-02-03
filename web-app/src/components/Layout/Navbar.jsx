import React from 'react';
import { Link, useLocation } from 'react-router-dom';
import { useAuth } from '../../context/AuthContext';

const Navbar = () => {
  const { user, logout } = useAuth();
  const location = useLocation();

  const isActive = (path) => location.pathname === path;

  return (
    <nav className="bg-white shadow-md sticky top-0 z-50">
      <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
        <div className="flex justify-between items-center h-16">
          <div className="flex items-center">
            <Link to="/dashboard" className="text-2xl font-bold text-medical-600">
              HealthCare+
            </Link>
          </div>

          <div className="hidden md:flex space-x-8">
            <Link
              to="/dashboard"
              className={`px-3 py-2 rounded-md text-sm font-medium transition-colors ${
                isActive('/dashboard')
                  ? 'bg-medical-100 text-medical-600'
                  : 'text-gray-700 hover:text-medical-600'
              }`}
            >
              Dashboard
            </Link>
            <Link
              to="/alerts"
              className={`px-3 py-2 rounded-md text-sm font-medium transition-colors ${
                isActive('/alerts')
                  ? 'bg-medical-100 text-medical-600'
                  : 'text-gray-700 hover:text-medical-600'
              }`}
            >
              Alerts
            </Link>
            <Link
              to="/history"
              className={`px-3 py-2 rounded-md text-sm font-medium transition-colors ${
                isActive('/history')
                  ? 'bg-medical-100 text-medical-600'
                  : 'text-gray-700 hover:text-medical-600'
              }`}
            >
              History
            </Link>
            <Link
              to="/profile"
              className={`px-3 py-2 rounded-md text-sm font-medium transition-colors ${
                isActive('/profile')
                  ? 'bg-medical-100 text-medical-600'
                  : 'text-gray-700 hover:text-medical-600'
              }`}
            >
              Profile
            </Link>
          </div>

          <div className="flex items-center space-x-4">
            <span className="text-sm text-gray-600">{user?.name || 'User'}</span>
            <button
              onClick={logout}
              className="btn-secondary text-sm"
            >
              Logout
            </button>
          </div>
        </div>
      </div>
    </nav>
  );
};

export default Navbar;
