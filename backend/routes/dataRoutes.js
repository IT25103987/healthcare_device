const express = require('express');
const router = express.Router();
const { body, validationResult } = require('express-validator');
const HealthData = require('../models/HealthData');
const alertService = require('../services/alertService');
const { protect } = require('../middleware/auth');

/**
 * @route   POST /api/data
 * @desc    Receive sensor data from ESP32
 * @access  Public (ESP32 device)
 */
router.post('/', [
  body('deviceId').trim().notEmpty().withMessage('Device ID is required'),
  body('heartRate').isInt({ min: 0, max: 250 }).withMessage('Heart rate must be between 0 and 250')
], async (req, res) => {
  try {
    // Check for validation errors
    const errors = validationResult(req);
    if (!errors.isEmpty()) {
      return res.status(400).json({
        success: false,
        errors: errors.array()
      });
    }

    const { deviceId, heartRate, spo2, temperature, pressure, alertStatus, timestamp } = req.body;

    // Create health data record
    const healthData = await HealthData.create({
      deviceId,
      heartRate,
      spo2,
      temperature,
      pressure,
      alertStatus: alertStatus || 'NORMAL',
      timestamp: timestamp ? new Date(timestamp) : new Date()
    });

    console.log(`✅ Data received from device ${deviceId}: HR=${heartRate} BPM`);

    // Process alerts (backend validation)
    const { alert, alertInfo } = await alertService.processHealthData(healthData);

    // Emit real-time update via Socket.IO (if initialized)
    if (req.app.get('io')) {
      req.app.get('io').emit('healthData', {
        deviceId,
        data: healthData,
        alert: alert ? {
          type: alert.alertType,
          message: alert.message,
          severity: alert.severity
        } : null
      });
    }

    res.status(201).json({
      success: true,
      message: 'Data received successfully',
      data: {
        id: healthData._id,
        deviceId: healthData.deviceId,
        heartRate: healthData.heartRate,
        alertStatus: alertInfo.alertType
      },
      alert: alert ? {
        id: alert._id,
        type: alert.alertType,
        message: alert.message
      } : null
    });
  } catch (error) {
    console.error('Data reception error:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to save data',
      error: error.message
    });
  }
});

/**
 * @route   GET /api/data/:deviceId
 * @desc    Get latest vitals for a device
 * @access  Private
 */
router.get('/:deviceId', protect, async (req, res) => {
  try {
    const { deviceId } = req.params;

    // Get latest health data
    const latestData = await HealthData.findOne({ deviceId })
      .sort({ createdAt: -1 })
      .limit(1);

    if (!latestData) {
      return res.status(404).json({
        success: false,
        message: 'No data found for this device'
      });
    }

    res.json({
      success: true,
      data: latestData
    });
  } catch (error) {
    console.error('Get latest data error:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to get data',
      error: error.message
    });
  }
});

/**
 * @route   GET /api/data/:deviceId/history
 * @desc    Get historical data for a device
 * @access  Private
 */
router.get('/:deviceId/history', protect, async (req, res) => {
  try {
    const { deviceId } = req.params;
    const { period = '24h', limit = 100 } = req.query;

    // Calculate start date based on period
    const now = new Date();
    let startDate;

    switch (period) {
      case '24h':
        startDate = new Date(now.getTime() - 24 * 60 * 60 * 1000);
        break;
      case '7d':
        startDate = new Date(now.getTime() - 7 * 24 * 60 * 60 * 1000);
        break;
      case '30d':
        startDate = new Date(now.getTime() - 30 * 24 * 60 * 60 * 1000);
        break;
      default:
        startDate = new Date(now.getTime() - 24 * 60 * 60 * 1000);
    }

    // Query health data
    const healthData = await HealthData.find({
      deviceId,
      createdAt: { $gte: startDate }
    })
      .sort({ createdAt: -1 })
      .limit(parseInt(limit));

    // Calculate statistics
    const stats = {
      count: healthData.length,
      avgHeartRate: 0,
      minHeartRate: null,
      maxHeartRate: null,
      avgTemperature: 0,
      avgSpo2: 0
    };

    if (healthData.length > 0) {
      const heartRates = healthData.map(d => d.heartRate).filter(hr => hr > 0);
      const temperatures = healthData.map(d => d.temperature).filter(t => t !== null);
      const spo2Values = healthData.map(d => d.spo2).filter(s => s !== null);

      if (heartRates.length > 0) {
        stats.avgHeartRate = Math.round(heartRates.reduce((a, b) => a + b, 0) / heartRates.length);
        stats.minHeartRate = Math.min(...heartRates);
        stats.maxHeartRate = Math.max(...heartRates);
      }

      if (temperatures.length > 0) {
        stats.avgTemperature = (temperatures.reduce((a, b) => a + b, 0) / temperatures.length).toFixed(1);
      }

      if (spo2Values.length > 0) {
        stats.avgSpo2 = Math.round(spo2Values.reduce((a, b) => a + b, 0) / spo2Values.length);
      }
    }

    res.json({
      success: true,
      period,
      startDate,
      endDate: now,
      count: healthData.length,
      stats,
      data: healthData
    });
  } catch (error) {
    console.error('Get history error:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to get history',
      error: error.message
    });
  }
});

/**
 * @route   GET /api/data/:deviceId/latest/:count
 * @desc    Get latest N readings for a device
 * @access  Private
 */
router.get('/:deviceId/latest/:count', protect, async (req, res) => {
  try {
    const { deviceId, count } = req.params;
    const limit = Math.min(parseInt(count) || 10, 1000); // Max 1000 records

    const healthData = await HealthData.find({ deviceId })
      .sort({ createdAt: -1 })
      .limit(limit);

    res.json({
      success: true,
      count: healthData.length,
      data: healthData
    });
  } catch (error) {
    console.error('Get latest readings error:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to get latest readings',
      error: error.message
    });
  }
});

/**
 * @route   DELETE /api/data/:deviceId
 * @desc    Delete all data for a device (admin only)
 * @access  Private (Admin)
 */
router.delete('/:deviceId', protect, async (req, res) => {
  try {
    // Check if user is admin
    if (req.user.role !== 'admin') {
      return res.status(403).json({
        success: false,
        message: 'Not authorized to delete data'
      });
    }

    const { deviceId } = req.params;

    const result = await HealthData.deleteMany({ deviceId });

    res.json({
      success: true,
      message: `Deleted ${result.deletedCount} records for device ${deviceId}`,
      deletedCount: result.deletedCount
    });
  } catch (error) {
    console.error('Delete data error:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to delete data',
      error: error.message
    });
  }
});

module.exports = router;
