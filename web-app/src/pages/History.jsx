import React, { useState, useMemo } from 'react';
import Layout from '../components/Layout/Layout';
import formatters from '../utils/formatters';

const HistoryPage = () => {
  const [timeRange, setTimeRange] = useState('24h');

  // Sample history data - using useMemo to avoid impure function calls in render
  const historyData = useMemo(() => {
    const now = new Date();
    return [
      {
        id: 1,
        date: new Date(now.getTime() - 3600000),
        type: 'Heart Rate',
        value: '72 bpm',
        status: 'normal',
      },
      {
        id: 2,
        date: new Date(now.getTime() - 7200000),
        type: 'Blood Pressure',
        value: '120/80 mmHg',
        status: 'normal',
      },
      {
        id: 3,
        date: new Date(now.getTime() - 10800000),
        type: 'Temperature',
        value: '37.1°C',
        status: 'normal',
      },
      {
        id: 4,
        date: new Date(now.getTime() - 14400000),
        type: 'Oxygen Saturation',
        value: '98%',
        status: 'normal',
      },
      {
        id: 5,
        date: new Date(now.getTime() - 18000000),
        type: 'Heart Rate',
        value: '75 bpm',
        status: 'normal',
      },
    ];
  }, []);

  return (
    <Layout>
      <div className="space-y-8">
        <div className="flex justify-between items-center">
          <div>
            <h1 className="text-3xl font-bold text-gray-900">Health History</h1>
            <p className="text-gray-600 mt-2">View your historical vital signs and measurements</p>
          </div>
          <select
            value={timeRange}
            onChange={(e) => setTimeRange(e.target.value)}
            className="input-field w-40"
          >
            <option value="24h">Last 24 Hours</option>
            <option value="7d">Last 7 Days</option>
            <option value="30d">Last 30 Days</option>
            <option value="90d">Last 90 Days</option>
          </select>
        </div>

        <div className="space-y-4">
          {historyData.map((item) => (
            <div key={item.id} className="card">
              <div className="flex items-center justify-between">
                <div className="flex-1">
                  <h3 className="font-semibold text-gray-900">{item.type}</h3>
                  <p className="text-2xl font-bold text-sky-600 mt-1">{item.value}</p>
                  <p className="text-sm text-gray-500 mt-2">
                    {formatters.formatDateTime(item.date)}
                  </p>
                </div>
                <div>
                  <span className={`badge badge-${item.status}`}>
                    {item.status.charAt(0).toUpperCase() + item.status.slice(1)}
                  </span>
                </div>
              </div>
            </div>
          ))}
        </div>

        {historyData.length === 0 && (
          <div className="card text-center py-12">
            <p className="text-gray-500 text-lg">No history available</p>
            <p className="text-gray-400 text-sm mt-2">Data will appear here as measurements are recorded</p>
          </div>
        )}
      </div>
    </Layout>
  );
};

export default HistoryPage;
