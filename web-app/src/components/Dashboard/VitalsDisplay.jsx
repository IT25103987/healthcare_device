import React from 'react';
import { useData } from '../../context/DataContext';
import formatters from '../../utils/formatters';

const VitalCard = ({ icon, label, value, unit, status }) => {
  const statusColor = formatters.getStatusColor(status);

  return (
    <div className="card hover:shadow-lg transition-shadow">
      <div className="flex items-center justify-between">
        <div>
          <p className="text-gray-500 text-sm mb-2">{label}</p>
          <p className="text-3xl font-bold text-gray-900">{value}</p>
          <p className="text-xs text-gray-400 mt-1">{unit}</p>
        </div>
        <div className={`text-4xl ${icon}`}>
          {icon === '❤️' && '❤️'}
          {icon === '🫀' && '🫀'}
          {icon === '🌡️' && '🌡️'}
          {icon === '💨' && '💨'}
        </div>
      </div>
      {status && (
        <div className="mt-4 pt-4 border-t">
          <span className={`badge ${statusColor}`}>
            {status.charAt(0).toUpperCase() + status.slice(1)}
          </span>
        </div>
      )}
    </div>
  );
};

const VitalsDisplay = () => {
  const { vitals } = useData();

  return (
    <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6 mb-8">
      <VitalCard
        icon="❤️"
        label="Heart Rate"
        value={vitals.heartRate || 72}
        unit="bpm"
        status={formatters.getVitalStatus('heartRate', vitals.heartRate || 72)}
      />
      <VitalCard
        icon="🫀"
        label="Blood Pressure"
        value={`${vitals.bloodPressure?.systolic || 120}/${vitals.bloodPressure?.diastolic || 80}`}
        unit="mmHg"
        status={formatters.getVitalStatus('systolic', vitals.bloodPressure?.systolic || 120)}
      />
      <VitalCard
        icon="💨"
        label="Oxygen Saturation"
        value={vitals.oxygenSaturation || 98}
        unit="%"
        status={formatters.getVitalStatus('oxygenSaturation', vitals.oxygenSaturation || 98)}
      />
      <VitalCard
        icon="🌡️"
        label="Temperature"
        value={(vitals.temperature || 37.0).toFixed(1)}
        unit="°C"
        status={formatters.getVitalStatus('temperature', vitals.temperature || 37.0)}
      />
      <VitalCard
        icon="💨"
        label="Respiratory Rate"
        value={vitals.respiratoryRate || 16}
        unit="bpm"
        status={formatters.getVitalStatus('respiratoryRate', vitals.respiratoryRate || 16)}
      />
    </div>
  );
};

export default VitalsDisplay;
