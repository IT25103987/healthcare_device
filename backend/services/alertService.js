const Alert = require('../models/Alert');
const emailService = require('./emailService');

/**
 * Alert Service
 * Handles alert detection and notification logic
 */

class AlertService {
  /**
   * Analyze heart rate and determine alert status
   * @param {number} heartRate - Heart rate in BPM
   * @returns {object} Alert information
   */
  analyzeHeartRate(heartRate) {
    let alertType = null;
    let message = '';
    let shouldAlert = false;

    if (heartRate < 40) {
      alertType = 'CRITICAL_LOW';
      message = `Critical: Heart rate dangerously low at ${heartRate} BPM (< 40)`;
      shouldAlert = true;
    } else if (heartRate >= 40 && heartRate < 50) {
      alertType = 'WARNING_LOW';
      message = `Warning: Heart rate low at ${heartRate} BPM (40-49)`;
      shouldAlert = true;
    } else if (heartRate > 110) {
      alertType = 'CRITICAL_HIGH';
      message = `Critical: Heart rate dangerously high at ${heartRate} BPM (> 110)`;
      shouldAlert = true;
    } else if (heartRate >= 91 && heartRate <= 110) {
      alertType = 'WARNING_HIGH';
      message = `Warning: Heart rate elevated at ${heartRate} BPM (91-110)`;
      shouldAlert = true;
    } else {
      // Normal range: 50-90 BPM
      alertType = 'NORMAL';
      message = `Heart rate normal at ${heartRate} BPM`;
      shouldAlert = false;
    }

    return {
      alertType,
      message,
      shouldAlert,
      severity: this.getSeverity(alertType)
    };
  }

  /**
   * Get severity level for alert type
   */
  getSeverity(alertType) {
    if (alertType.includes('CRITICAL')) return 'critical';
    if (alertType.includes('WARNING')) return 'high';
    return 'medium';
  }

  /**
   * Create and save alert to database
   */
  async createAlert(deviceId, heartRate, alertInfo, healthDataId) {
    try {
      const alert = await Alert.create({
        deviceId,
        alertType: alertInfo.alertType,
        heartRate,
        message: alertInfo.message,
        severity: alertInfo.severity,
        healthDataId,
        emailSent: false
      });

      console.log(`✅ Alert created: ${alert.alertType} for device ${deviceId}`);
      return alert;
    } catch (error) {
      console.error('❌ Failed to create alert:', error.message);
      throw error;
    }
  }

  /**
   * Send email notification for alert
   */
  async sendAlertEmail(alert, recipients) {
    if (!recipients || recipients.length === 0) {
      console.warn('No recipients specified for alert email');
      return { success: false, error: 'No recipients' };
    }

    let result;

    if (alert.alertType.includes('CRITICAL')) {
      result = await emailService.sendCriticalAlert(
        alert.deviceId,
        alert.heartRate,
        alert.alertType,
        recipients
      );
    } else {
      result = await emailService.sendWarningAlert(
        alert.deviceId,
        alert.heartRate,
        alert.alertType,
        recipients
      );
    }

    // Update alert with email status
    try {
      alert.emailSent = result.success;
      alert.emailSentAt = result.success ? new Date() : null;
      alert.emailError = result.error || null;
      await alert.save();
    } catch (error) {
      console.error('Failed to update alert email status:', error.message);
    }

    return result;
  }

  /**
   * Process health data and handle alerts
   */
  async processHealthData(healthData) {
    const { deviceId, heartRate, _id } = healthData;

    // Analyze heart rate
    const alertInfo = this.analyzeHeartRate(heartRate);

    // If alert is needed, create and send notification
    if (alertInfo.shouldAlert) {
      const alert = await this.createAlert(deviceId, heartRate, alertInfo, _id);

      // Check if email alerts are enabled
      if (process.env.SEND_EMAIL_ALERTS === 'true') {
        const recipients = this.getAlertRecipients();
        
        if (recipients.length > 0) {
          await this.sendAlertEmail(alert, recipients);
        }
      }

      return { alert, alertInfo };
    }

    return { alert: null, alertInfo };
  }

  /**
   * Get list of alert recipients from environment
   */
  getAlertRecipients() {
    const recipientsStr = process.env.ALERT_EMAIL_RECIPIENTS || '';
    return recipientsStr.split(',').map(email => email.trim()).filter(email => email);
  }

  /**
   * Get alert statistics
   */
  async getAlertStats(deviceId, timeRange = '24h') {
    const now = new Date();
    let startDate;

    switch (timeRange) {
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

    const query = deviceId ? { deviceId, createdAt: { $gte: startDate } } : { createdAt: { $gte: startDate } };

    const [total, critical, warning, handled, unhandled] = await Promise.all([
      Alert.countDocuments(query),
      Alert.countDocuments({ ...query, alertType: { $in: ['CRITICAL_LOW', 'CRITICAL_HIGH', 'CRITICAL'] } }),
      Alert.countDocuments({ ...query, alertType: { $in: ['WARNING_LOW', 'WARNING_HIGH', 'WARNING'] } }),
      Alert.countDocuments({ ...query, handled: true }),
      Alert.countDocuments({ ...query, handled: false })
    ]);

    return {
      total,
      critical,
      warning,
      handled,
      unhandled,
      timeRange,
      startDate,
      endDate: now
    };
  }

  /**
   * Mark alert as handled
   */
  async markAlertHandled(alertId, handledBy) {
    try {
      const alert = await Alert.findByIdAndUpdate(
        alertId,
        {
          handled: true,
          handledBy,
          handledAt: new Date()
        },
        { new: true }
      );

      if (!alert) {
        throw new Error('Alert not found');
      }

      console.log(`✅ Alert ${alertId} marked as handled by ${handledBy}`);
      return alert;
    } catch (error) {
      console.error('❌ Failed to mark alert as handled:', error.message);
      throw error;
    }
  }
}

// Create singleton instance
const alertService = new AlertService();

module.exports = alertService;
