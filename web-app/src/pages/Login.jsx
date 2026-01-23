import React from 'react';
import LoginForm from '../components/Auth/LoginForm';

const LoginPage = () => {
  return (
    <div className="min-h-screen bg-gradient-to-br from-sky-50 to-sky-100 flex items-center justify-center px-4">
      <div className="w-full max-w-md">
        <div className="bg-white rounded-lg shadow-lg p-8">
          <div className="text-center mb-8">
            <h1 className="text-4xl font-bold text-sky-600 mb-2">HealthCare+</h1>
            <p className="text-gray-600">Monitor your health in real-time</p>
          </div>

          <LoginForm />

          <div className="mt-6 text-center">
            <p className="text-gray-600 text-sm">
              Demo Credentials:
              <br/>
              Email: demo@healthcare.com
              <br/>
              Password: Demo123!
            </p>
          </div>
        </div>
      </div>
    </div>
  );
};

export default LoginPage;
