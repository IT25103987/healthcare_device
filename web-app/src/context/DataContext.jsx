/* eslint-disable react-refresh/only-export-components */
import React, { createContext, useContext, useState, useCallback } from 'react';

const DataContext = createContext();

export const useData = () => {
  const context = useContext(DataContext);
  if (!context) {
    throw new Error('useData must be used within a DataProvider');
  }
  return context;
};

export const DataProvider = ({ children }) => {
  const [vitals, setVitals] = useState({
    heartRate: 0,
    bloodPressure: { systolic: 0, diastolic: 0 },
    oxygenSaturation: 0,
    temperature: 0,
    respiratoryRate: 0,
  });

  const [alerts, setAlerts] = useState([]);
  const [devices, setDevices] = useState([]);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState(null);

  const updateVitals = useCallback((newVitals) => {
    setVitals(prev => ({ ...prev, ...newVitals }));
  }, []);

  const addAlert = useCallback((alert) => {
    setAlerts(prev => [alert, ...prev]);
  }, []);

  const removeAlert = useCallback((alertId) => {
    setAlerts(prev => prev.filter(a => a.id !== alertId));
  }, []);

  const updateAlert = useCallback((alertId, updates) => {
    setAlerts(prev => prev.map(a => a.id === alertId ? { ...a, ...updates } : a));
  }, []);

  const clearAlerts = useCallback(() => {
    setAlerts([]);
  }, []);

  const setDevicesList = useCallback((devicesList) => {
    setDevices(devicesList);
  }, []);

  const updateDevice = useCallback((deviceId, deviceData) => {
    setDevices(prev => prev.map(d => d.id === deviceId ? { ...d, ...deviceData } : d));
  }, []);

  const value = {
    vitals,
    updateVitals,
    alerts,
    addAlert,
    removeAlert,
    updateAlert,
    clearAlerts,
    devices,
    setDevicesList,
    updateDevice,
    loading,
    setLoading,
    error,
    setError,
  };

  return (
    <DataContext.Provider value={value}>
      {children}
    </DataContext.Provider>
  );
};
