import React from 'react';
import { useData } from '../../context/DataContext';
import { Link } from 'react-router-dom';
import formatters from '../../utils/formatters';

const AlertsList = () => {
  const { alerts } = useData();
  const recentAlerts = alerts.slice(0, 5);

  const getSeverityColor = (severity) => {
    switch (severity) {
      case 'critical':
        return 'border-l-4 border-red-500 bg-red-50';
      case 'warning':
        return 'border-l-4 border-yellow-500 bg-yellow-50';
      case 'info':
        return 'border-l-4 border-blue-500 bg-blue-50';
      default:
        return 'border-l-4 border-gray-500 bg-gray-50';
    }
  };

  if (recentAlerts.length === 0) {
    return (
      <div className="card">
        <h3 className="text-lg font-semibold mb-4">Recent Alerts</h3>
        <p className="text-gray-500 text-center py-8">No alerts at the moment</p>
      </div>
    );
  }

  return (
    <div className="card">
      <div className="flex justify-between items-center mb-4">
        <h3 className="text-lg font-semibold">Recent Alerts</h3>
        <Link to="/alerts" className="text-medical-600 hover:text-medical-700 text-sm font-medium">
          View All
        </Link>
      </div>
      <div className="space-y-4">
        {recentAlerts.map((alert) => (
          <div key={alert.id} className={`p-4 rounded ${getSeverityColor(alert.severity)}`}>
            <div className="flex justify-between items-start">
              <div className="flex-1">
                <p className="font-semibold text-gray-900">{alert.title}</p>
                <p className="text-sm text-gray-600 mt-1">{alert.message}</p>
                <p className="text-xs text-gray-500 mt-2">
                  {formatters.formatDistanceToNow(alert.timestamp)}
                </p>
              </div>
              <span className={`badge badge-${alert.severity}`}>
                {alert.severity.charAt(0).toUpperCase() + alert.severity.slice(1)}
              </span>
            </div>
          </div>
        ))}
      </div>
    </div>
  );
};

export default AlertsList;
