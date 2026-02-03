const express = require('express');
const router = express.Router();
const Alert = require('../models/Alert');
const alertService = require('../services/alertService');
const { protect } = require('../middleware/auth');

/**
 * @route   GET /api/alerts
 * @desc    Get all alerts
 * @access  Private
 */
router.get('/', protect, async (req, res) => {
  try {
    const { deviceId, type, handled, limit = 50, page = 1 } = req.query;

    // Build query
    const query = {};
    if (deviceId) query.deviceId = deviceId;
    if (type) query.alertType = type;
    if (handled !== undefined) query.handled = handled === 'true';

    // Get alerts with pagination
    const alerts = await Alert.find(query)
      .sort({ createdAt: -1 })
      .limit(parseInt(limit))
      .skip((parseInt(page) - 1) * parseInt(limit))
      .populate('healthDataId', 'heartRate spo2 temperature pressure');

    // Get total count
    const total = await Alert.countDocuments(query);

    res.json({
      success: true,
      count: alerts.length,
      total,
      page: parseInt(page),
      pages: Math.ceil(total / parseInt(limit)),
      data: alerts
    });
  } catch (error) {
    console.error('Get alerts error:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to get alerts',
      error: error.message
    });
  }
});

/**
 * @route   GET /api/alerts/unread
 * @desc    Get unread (unhandled) alerts
 * @access  Private
 */
router.get('/unread', protect, async (req, res) => {
  try {
    const { deviceId } = req.query;

    const query = { handled: false };
    if (deviceId) query.deviceId = deviceId;

    const alerts = await Alert.find(query)
      .sort({ createdAt: -1 })
      .limit(100)
      .populate('healthDataId', 'heartRate spo2 temperature pressure');

    res.json({
      success: true,
      count: alerts.length,
      data: alerts
    });
  } catch (error) {
    console.error('Get unread alerts error:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to get unread alerts',
      error: error.message
    });
  }
});

/**
 * @route   GET /api/alerts/stats
 * @desc    Get alert statistics
 * @access  Private
 */
router.get('/stats', protect, async (req, res) => {
  try {
    const { deviceId, period = '24h' } = req.query;

    const stats = await alertService.getAlertStats(deviceId, period);

    res.json({
      success: true,
      stats
    });
  } catch (error) {
    console.error('Get alert stats error:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to get alert statistics',
      error: error.message
    });
  }
});

/**
 * @route   GET /api/alerts/:id
 * @desc    Get alert by ID
 * @access  Private
 */
router.get('/:id', protect, async (req, res) => {
  try {
    const alert = await Alert.findById(req.params.id)
      .populate('healthDataId');

    if (!alert) {
      return res.status(404).json({
        success: false,
        message: 'Alert not found'
      });
    }

    res.json({
      success: true,
      data: alert
    });
  } catch (error) {
    console.error('Get alert error:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to get alert',
      error: error.message
    });
  }
});

/**
 * @route   POST /api/alerts/:id/handle
 * @desc    Mark alert as handled
 * @access  Private
 */
router.post('/:id/handle', protect, async (req, res) => {
  try {
    const { notes } = req.body;
    
    const alert = await alertService.markAlertHandled(
      req.params.id,
      req.user.name || req.user.email
    );

    // Emit real-time update
    if (req.app.get('io')) {
      req.app.get('io').emit('alertHandled', {
        alertId: alert._id,
        handledBy: alert.handledBy,
        handledAt: alert.handledAt
      });
    }

    res.json({
      success: true,
      message: 'Alert marked as handled',
      data: alert
    });
  } catch (error) {
    console.error('Handle alert error:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to handle alert',
      error: error.message
    });
  }
});

/**
 * @route   POST /api/alerts/:id/resend-email
 * @desc    Resend alert email
 * @access  Private
 */
router.post('/:id/resend-email', protect, async (req, res) => {
  try {
    const alert = await Alert.findById(req.params.id);

    if (!alert) {
      return res.status(404).json({
        success: false,
        message: 'Alert not found'
      });
    }

    const recipients = alertService.getAlertRecipients();
    
    if (recipients.length === 0) {
      return res.status(400).json({
        success: false,
        message: 'No email recipients configured'
      });
    }

    const result = await alertService.sendAlertEmail(alert, recipients);

    res.json({
      success: result.success,
      message: result.success ? 'Email sent successfully' : 'Failed to send email',
      error: result.error
    });
  } catch (error) {
    console.error('Resend email error:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to resend email',
      error: error.message
    });
  }
});

/**
 * @route   DELETE /api/alerts/:id
 * @desc    Delete alert (admin only)
 * @access  Private (Admin)
 */
router.delete('/:id', protect, async (req, res) => {
  try {
    // Check if user is admin
    if (req.user.role !== 'admin') {
      return res.status(403).json({
        success: false,
        message: 'Not authorized to delete alerts'
      });
    }

    const alert = await Alert.findByIdAndDelete(req.params.id);

    if (!alert) {
      return res.status(404).json({
        success: false,
        message: 'Alert not found'
      });
    }

    res.json({
      success: true,
      message: 'Alert deleted successfully'
    });
  } catch (error) {
    console.error('Delete alert error:', error);
    res.status(500).json({
      success: false,
      message: 'Failed to delete alert',
      error: error.message
    });
  }
});

module.exports = router;
