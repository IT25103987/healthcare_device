import React from 'react';
import Layout from '../components/Layout/Layout';

const SettingsPage = () => {
  const [settings, setSettings] = React.useState({
    emailNotifications: true,
    smsNotifications: false,
    criticalAlerts: true,
    warningAlerts: true,
    dataCollection: true,
    theme: 'light',
  });

  const handleToggle = (key) => {
    setSettings(prev => ({
      ...prev,
      [key]: !prev[key],
    }));
  };

  const handleSelectChange = (key, value) => {
    setSettings(prev => ({
      ...prev,
      [key]: value,
    }));
  };

  return (
    <Layout>
      <div className="space-y-8">
        <div>
          <h1 className="text-3xl font-bold text-gray-900">Settings</h1>
          <p className="text-gray-600 mt-2">Manage your application preferences and notifications</p>
        </div>

        <div className="card max-w-2xl">
          <h2 className="text-2xl font-bold text-gray-900 mb-6">Notifications</h2>
          <div className="space-y-4">
            <div className="flex items-center justify-between p-4 border rounded-lg">
              <div>
                <p className="font-medium text-gray-900">Email Notifications</p>
                <p className="text-sm text-gray-600">Receive alerts via email</p>
              </div>
              <input
                type="checkbox"
                checked={settings.emailNotifications}
                onChange={() => handleToggle('emailNotifications')}
                className="w-5 h-5 cursor-pointer"
              />
            </div>

            <div className="flex items-center justify-between p-4 border rounded-lg">
              <div>
                <p className="font-medium text-gray-900">SMS Notifications</p>
                <p className="text-sm text-gray-600">Receive alerts via SMS</p>
              </div>
              <input
                type="checkbox"
                checked={settings.smsNotifications}
                onChange={() => handleToggle('smsNotifications')}
                className="w-5 h-5 cursor-pointer"
              />
            </div>

            <div className="flex items-center justify-between p-4 border rounded-lg">
              <div>
                <p className="font-medium text-gray-900">Critical Alerts</p>
                <p className="text-sm text-gray-600">Notifications for critical health issues</p>
              </div>
              <input
                type="checkbox"
                checked={settings.criticalAlerts}
                onChange={() => handleToggle('criticalAlerts')}
                className="w-5 h-5 cursor-pointer"
              />
            </div>

            <div className="flex items-center justify-between p-4 border rounded-lg">
              <div>
                <p className="font-medium text-gray-900">Warning Alerts</p>
                <p className="text-sm text-gray-600">Notifications for warning-level issues</p>
              </div>
              <input
                type="checkbox"
                checked={settings.warningAlerts}
                onChange={() => handleToggle('warningAlerts')}
                className="w-5 h-5 cursor-pointer"
              />
            </div>
          </div>
        </div>

        <div className="card max-w-2xl">
          <h2 className="text-2xl font-bold text-gray-900 mb-6">Preferences</h2>
          <div className="space-y-4">
            <div>
              <label className="block text-sm font-medium text-gray-900 mb-2">
                Theme
              </label>
              <select
                value={settings.theme}
                onChange={(e) => handleSelectChange('theme', e.target.value)}
                className="input-field"
              >
                <option value="light">Light</option>
                <option value="dark">Dark</option>
                <option value="auto">Auto</option>
              </select>
            </div>

            <div className="flex items-center justify-between p-4 border rounded-lg">
              <div>
                <p className="font-medium text-gray-900">Data Collection</p>
                <p className="text-sm text-gray-600">Allow collection of health data for analytics</p>
              </div>
              <input
                type="checkbox"
                checked={settings.dataCollection}
                onChange={() => handleToggle('dataCollection')}
                className="w-5 h-5 cursor-pointer"
              />
            </div>
          </div>
        </div>

        <div className="card max-w-2xl">
          <h2 className="text-2xl font-bold text-gray-900 mb-6">Help & Support</h2>
          <div className="space-y-2">
            <a href="#" className="block p-4 border rounded-lg hover:bg-gray-50 transition-colors">
              <p className="font-medium text-sky-600">Documentation</p>
              <p className="text-sm text-gray-600">Learn how to use HealthCare+</p>
            </a>
            <a href="#" className="block p-4 border rounded-lg hover:bg-gray-50 transition-colors">
              <p className="font-medium text-sky-600">Contact Support</p>
              <p className="text-sm text-gray-600">Get help from our support team</p>
            </a>
            <a href="#" className="block p-4 border rounded-lg hover:bg-gray-50 transition-colors">
              <p className="font-medium text-sky-600">Privacy Policy</p>
              <p className="text-sm text-gray-600">Read our privacy policy</p>
            </a>
          </div>
        </div>

        <div className="flex gap-2">
          <button className="btn-primary">Save Settings</button>
          <button className="btn-secondary">Reset to Defaults</button>
        </div>
      </div>
    </Layout>
  );
};

export default SettingsPage;
