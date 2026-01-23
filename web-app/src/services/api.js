import axios from 'axios';

const API_URL = import.meta.env.VITE_API_URL || 'http://localhost:3000/api';

const api = axios.create({
  baseURL: API_URL,
  headers: {
    'Content-Type': 'application/json',
  },
});

// Add token to requests
api.interceptors.request.use(
  (config) => {
    const token = localStorage.getItem('authToken');
    if (token) {
      config.headers.Authorization = `Bearer ${token}`;
    }
    return config;
  },
  (error) => {
    return Promise.reject(error);
  }
);

// Handle responses
api.interceptors.response.use(
  (response) => response,
  (error) => {
    if (error.response?.status === 401) {
      localStorage.removeItem('authToken');
      window.location.href = '/login';
    }
    return Promise.reject(error);
  }
);

// Patient endpoints
export const patientAPI = {
  getProfile: () => api.get('/patients/profile'),
  updateProfile: (data) => api.put('/patients/profile', data),
  getVitals: (patientId) => api.get(`/patients/${patientId}/vitals`),
  getHistory: (patientId) => api.get(`/patients/${patientId}/history`),
};

// Alert endpoints
export const alertAPI = {
  getAlerts: () => api.get('/alerts'),
  getAlert: (id) => api.get(`/alerts/${id}`),
  acknowledgeAlert: (id) => api.put(`/alerts/${id}/acknowledge`),
  resolveAlert: (id) => api.put(`/alerts/${id}/resolve`),
};

// Device endpoints
export const deviceAPI = {
  getDevices: () => api.get('/devices'),
  getDevice: (id) => api.get(`/devices/${id}`),
  updateDevice: (id, data) => api.put(`/devices/${id}`, data),
};

// Health data endpoints
export const healthAPI = {
  getVitalsSummary: () => api.get('/health/vitals-summary'),
  getChartData: (timeRange) => api.get(`/health/chart-data?timeRange=${timeRange}`),
};

export default api;
