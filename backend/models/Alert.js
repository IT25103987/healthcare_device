const mongoose = require('mongoose');

/**
 * Alert Model
 * Stores alert history and status
 */

const alertSchema = new mongoose.Schema({
  deviceId: {
    type: String,
    required: [true, 'Device ID is required'],
    trim: true,
    index: true
  },
  alertType: {
    type: String,
    required: [true, 'Alert type is required'],
    enum: ['CRITICAL_LOW', 'CRITICAL_HIGH', 'WARNING_LOW', 'WARNING_HIGH', 'WARNING', 'CRITICAL'],
    index: true
  },
  heartRate: {
    type: Number,
    required: true
  },
  message: {
    type: String,
    required: true
  },
  severity: {
    type: String,
    enum: ['low', 'medium', 'high', 'critical'],
    default: function() {
      if (this.alertType.includes('CRITICAL')) return 'critical';
      if (this.alertType.includes('WARNING')) return 'high';
      return 'medium';
    }
  },
  handled: {
    type: Boolean,
    default: false,
    index: true
  },
  handledBy: {
    type: String,
    default: null
  },
  handledAt: {
    type: Date,
    default: null
  },
  emailSent: {
    type: Boolean,
    default: false
  },
  emailSentAt: {
    type: Date,
    default: null
  },
  emailError: {
    type: String,
    default: null
  },
  healthDataId: {
    type: mongoose.Schema.Types.ObjectId,
    ref: 'HealthData'
  }
}, {
  timestamps: true
});

// Indexes for efficient queries
alertSchema.index({ deviceId: 1, createdAt: -1 });
alertSchema.index({ handled: 1, createdAt: -1 });
alertSchema.index({ alertType: 1, createdAt: -1 });

module.exports = mongoose.model('Alert', alertSchema);
