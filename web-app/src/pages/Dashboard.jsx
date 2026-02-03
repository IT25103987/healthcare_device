import React from 'react';
import Layout from '../components/Layout/Layout';
import VitalsDisplay from '../components/Dashboard/VitalsDisplay';
import AlertsList from '../components/Dashboard/AlertsList';
import ChartComponent from '../components/Charts/ChartComponent';
import { useData } from '../context/DataContext';

const DashboardPage = () => {
  const { vitals, alerts } = useData();

  // Sample chart data - in real app this would come from API
  const heartRateData = [
    { time: '00:00', heartRate: 72 },
    { time: '04:00', heartRate: 68 },
    { time: '08:00', heartRate: 75 },
    { time: '12:00', heartRate: 80 },
    { time: '16:00', heartRate: 78 },
    { time: '20:00', heartRate: 74 },
  ];

  const temperatureData = [
    { time: '00:00', temperature: 36.8 },
    { time: '06:00', temperature: 36.9 },
    { time: '12:00', temperature: 37.1 },
    { time: '18:00', temperature: 37.0 },
    { time: '24:00', temperature: 36.8 },
  ];

  return (
    <Layout>
      <div className="space-y-8">
        {/* Header */}
        <div className="mb-8">
          <h1 className="text-3xl font-bold text-gray-900 mb-2">Dashboard</h1>
          <p className="text-gray-600">Welcome back! Here's your health overview.</p>
        </div>

        {/* Vitals Overview */}
        <VitalsDisplay />

        {/* Charts and Alerts */}
        <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
          <div className="lg:col-span-2">
            <ChartComponent
              title="Heart Rate Trend"
              data={heartRateData}
              type="area"
              dataKey="heartRate"
              stroke="#0284c7"
              fill="#0284c7"
            />
          </div>
          <div>
            <ChartComponent
              title="Temperature Trend"
              data={temperatureData}
              type="line"
              dataKey="temperature"
              stroke="#ef4444"
            />
          </div>
        </div>

        {/* Recent Alerts */}
        <div className="grid grid-cols-1 gap-6">
          <AlertsList />
        </div>

        {/* Summary Stats */}
        <div className="grid grid-cols-1 md:grid-cols-4 gap-4">
          <div className="card">
            <p className="text-gray-500 text-sm">Total Alerts Today</p>
            <p className="text-3xl font-bold text-gray-900 mt-2">{alerts.length}</p>
          </div>
          <div className="card">
            <p className="text-gray-500 text-sm">Average Heart Rate</p>
            <p className="text-3xl font-bold text-gray-900 mt-2">
              {vitals.heartRate ? vitals.heartRate : 72} bpm
            </p>
          </div>
          <div className="card">
            <p className="text-gray-500 text-sm">Oxygen Saturation</p>
            <p className="text-3xl font-bold text-gray-900 mt-2">
              {vitals.oxygenSaturation ? vitals.oxygenSaturation : 98}%
            </p>
          </div>
          <div className="card">
            <p className="text-gray-500 text-sm">Last Update</p>
            <p className="text-sm font-medium text-sky-600 mt-2">Just now</p>
          </div>
        </div>
      </div>
    </Layout>
  );
};

export default DashboardPage;
