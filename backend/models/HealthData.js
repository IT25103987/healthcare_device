const mongoose = require('mongoose');

/**
 * HealthData Model
 * Stores sensor data from ESP32 device
 */

const healthDataSchema = new mongoose.Schema({
  deviceId: {
    type: String,
    required: [true, 'Device ID is required'],
    trim: true,
    index: true
  },
  heartRate: {
    type: Number,
    required: [true, 'Heart rate is required'],
    min: 0,
    max: 250
  },
  spo2: {
    type: Number,
    min: 0,
    max: 100,
    default: null
  },
  temperature: {
    type: Number,
    min: -10,
    max: 50,
    default: null
  },
  pressure: {
    type: String,
    default: null,
    validate: {
      validator: function(v) {
        // Validate format: "120/80"
        if (!v) return true;
        return /^\d{2,3}\/\d{2,3}$/.test(v);
      },
      message: 'Pressure must be in format: systolic/diastolic (e.g., "120/80")'
    }
  },
  alertStatus: {
    type: String,
    enum: ['NORMAL', 'WARNING_LOW', 'WARNING_HIGH', 'CRITICAL_LOW', 'CRITICAL_HIGH', 'WARNING', 'CRITICAL'],
    default: 'NORMAL'
  },
  timestamp: {
    type: Date,
    default: Date.now
  }
}, {
  timestamps: true  // Adds createdAt and updatedAt
});

// Index for faster queries
healthDataSchema.index({ deviceId: 1, createdAt: -1 });
healthDataSchema.index({ alertStatus: 1, createdAt: -1 });

// Virtual for getting systolic and diastolic separately
healthDataSchema.virtual('systolic').get(function() {
  if (!this.pressure) return null;
  return parseInt(this.pressure.split('/')[0]);
});

healthDataSchema.virtual('diastolic').get(function() {
  if (!this.pressure) return null;
  return parseInt(this.pressure.split('/')[1]);
});

module.exports = mongoose.model('HealthData', healthDataSchema);
