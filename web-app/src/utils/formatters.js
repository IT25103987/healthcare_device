import { format, formatDistance, formatRelative } from 'date-fns';

export const formatters = {
  formatDate: (date) => {
    try {
      return format(new Date(date), 'MMM dd, yyyy');
    } catch {
      return 'Invalid date';
    }
  },

  formatDateTime: (date) => {
    try {
      return format(new Date(date), 'MMM dd, yyyy HH:mm:ss');
    } catch {
      return 'Invalid date';
    }
  },

  formatTime: (date) => {
    try {
      return format(new Date(date), 'HH:mm:ss');
    } catch {
      return 'Invalid time';
    }
  },

  formatDistanceToNow: (date) => {
    try {
      return formatDistance(new Date(date), new Date(), { addSuffix: true });
    } catch {
      return 'Unknown';
    }
  },

  formatRelativeTime: (date) => {
    try {
      return formatRelative(new Date(date), new Date());
    } catch {
      return 'Unknown';
    }
  },

  formatBloodPressure: (systolic, diastolic) => {
    return `${systolic}/${diastolic} mmHg`;
  },

  formatHeartRate: (rate) => {
    return `${rate} bpm`;
  },

  formatOxygenSaturation: (spo2) => {
    return `${spo2}%`;
  },

  formatTemperature: (temp) => {
    return `${temp.toFixed(1)}°C`;
  },

  formatRespiratoryRate: (rate) => {
    return `${rate} bpm`;
  },

  formatWeight: (weight) => {
    return `${weight.toFixed(1)} kg`;
  },

  formatHeight: (height) => {
    return `${height.toFixed(2)} m`;
  },

  formatBMI: (bmi) => {
    return `${bmi.toFixed(1)}`;
  },

  getVitalStatus: (vital, value) => {
    // Returns 'normal', 'warning', or 'critical'
    const ranges = {
      heartRate: { normal: [60, 100], warning: [50, 110], critical: [0, 40] },
      systolic: { normal: [90, 120], warning: [80, 140], critical: [0, 70] },
      diastolic: { normal: [60, 80], warning: [50, 90], critical: [0, 40] },
      oxygenSaturation: { normal: [95, 100], warning: [90, 94], critical: [0, 85] },
      temperature: { normal: [36.5, 37.5], warning: [36.0, 38.5], critical: [0, 35] },
      respiratoryRate: { normal: [12, 20], warning: [10, 25], critical: [0, 8] },
    };

    const range = ranges[vital];
    if (!range) return 'unknown';

    if (value >= range.normal[0] && value <= range.normal[1]) return 'normal';
    if (value >= range.warning[0] && value <= range.warning[1]) return 'warning';
    return 'critical';
  },

  getStatusColor: (status) => {
    switch (status) {
      case 'normal':
        return 'bg-green-100 text-green-800';
      case 'warning':
        return 'bg-yellow-100 text-yellow-800';
      case 'critical':
        return 'bg-red-100 text-red-800';
      default:
        return 'bg-gray-100 text-gray-800';
    }
  },

  getStatusBadgeClass: (status) => {
    return `badge ${formatters.getStatusColor(status)}`;
  },
};

export default formatters;
