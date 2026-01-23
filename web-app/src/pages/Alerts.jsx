import React from 'react';
import Layout from '../components/Layout/Layout';
import { useData } from '../context/DataContext';
import formatters from '../utils/formatters';

const AlertsPage = () => {
  const { alerts, updateAlert } = useData();

  const handleAcknowledge = (alertId) => {
    updateAlert(alertId, { acknowledged: true });
  };

  const handleResolve = (alertId) => {
    updateAlert(alertId, { resolved: true });
  };

  const getSeverityBadge = (severity) => {
    const badgeClasses = {
      critical: 'bg-red-100 text-red-800',
      warning: 'bg-yellow-100 text-yellow-800',
      info: 'bg-blue-100 text-blue-800',
    };
    return badgeClasses[severity] || 'bg-gray-100 text-gray-800';
  };

  const getSeverityIcon = (severity) => {
    switch (severity) {
      case 'critical':
        return '🔴';
      case 'warning':
        return '🟡';
      case 'info':
        return '🔵';
      default:
        return '⚪';
    }
  };

  return (
    <Layout>
      <div className="space-y-8">
        <div>
          <h1 className="text-3xl font-bold text-gray-900">Alerts</h1>
          <p className="text-gray-600 mt-2">Monitor and manage health alerts</p>
        </div>

        {alerts.length === 0 ? (
          <div className="card text-center py-12">
            <p className="text-gray-500 text-lg">No alerts at the moment</p>
            <p className="text-gray-400 text-sm mt-2">All systems are operating normally</p>
          </div>
        ) : (
          <div className="space-y-4">
            {alerts.map((alert) => (
              <div
                key={alert.id}
                className="card border-l-4"
                style={{
                  borderLeftColor:
                    alert.severity === 'critical'
                      ? '#ef4444'
                      : alert.severity === 'warning'
                      ? '#eab308'
                      : '#3b82f6',
                }}
              >
                <div className="flex items-start justify-between">
                  <div className="flex-1">
                    <div className="flex items-center gap-3">
                      <span className="text-2xl">{getSeverityIcon(alert.severity)}</span>
                      <div>
                        <h3 className="font-semibold text-gray-900">{alert.title}</h3>
                        <p className="text-sm text-gray-600 mt-1">{alert.message}</p>
                        <p className="text-xs text-gray-400 mt-2">
                          {formatters.formatDateTime(alert.timestamp)}
                        </p>
                      </div>
                    </div>
                  </div>
                  <div className="flex items-center gap-2">
                    <span className={`badge ${getSeverityBadge(alert.severity)}`}>
                      {alert.severity.charAt(0).toUpperCase() + alert.severity.slice(1)}
                    </span>
                  </div>
                </div>

                <div className="flex gap-2 mt-4 pt-4 border-t">
                  {!alert.acknowledged && (
                    <button
                      onClick={() => handleAcknowledge(alert.id)}
                      className="btn-primary text-sm"
                    >
                      Acknowledge
                    </button>
                  )}
                  {!alert.resolved && (
                    <button
                      onClick={() => handleResolve(alert.id)}
                      className="btn-secondary text-sm"
                    >
                      Resolve
                    </button>
                  )}
                  {alert.acknowledged && (
                    <span className="text-xs text-gray-500">Acknowledged</span>
                  )}
                  {alert.resolved && (
                    <span className="text-xs text-green-600">Resolved</span>
                  )}
                </div>
              </div>
            ))}
          </div>
        )}
      </div>
    </Layout>
  );
};

export default AlertsPage;
