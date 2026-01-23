const nodemailer = require('nodemailer');

/**
 * Email Service
 * Handles sending email notifications
 */

class EmailService {
  constructor() {
    this.transporter = null;
    this.initialized = false;
    this.initialize();
  }

  /**
   * Initialize email transporter
   */
  initialize() {
    try {
      // Check if email is configured
      if (!process.env.EMAIL_USER || !process.env.EMAIL_PASSWORD) {
        console.warn('⚠️  Email service not configured. Set EMAIL_USER and EMAIL_PASSWORD in .env');
        return;
      }

      this.transporter = nodemailer.createTransporter({
        host: process.env.EMAIL_HOST || 'smtp.gmail.com',
        port: parseInt(process.env.EMAIL_PORT) || 587,
        secure: process.env.EMAIL_SECURE === 'true', // true for 465, false for other ports
        auth: {
          user: process.env.EMAIL_USER,
          pass: process.env.EMAIL_PASSWORD
        }
      });

      this.initialized = true;
      console.log('✅ Email service initialized');
    } catch (error) {
      console.error('❌ Email service initialization failed:', error.message);
    }
  }

  /**
   * Send critical alert email
   */
  async sendCriticalAlert(deviceId, heartRate, alertType, recipients) {
    if (!this.initialized) {
      console.warn('Email service not initialized');
      return { success: false, error: 'Email service not configured' };
    }

    const subject = `🚨 CRITICAL ALERT - ${deviceId}`;
    const html = `
      <!DOCTYPE html>
      <html>
      <head>
        <style>
          body { font-family: Arial, sans-serif; line-height: 1.6; color: #333; }
          .container { max-width: 600px; margin: 0 auto; padding: 20px; }
          .alert-box { 
            background: #ff4444; 
            color: white; 
            padding: 20px; 
            border-radius: 8px; 
            margin: 20px 0;
            text-align: center;
          }
          .alert-box h1 { margin: 0 0 10px 0; font-size: 28px; }
          .details { 
            background: #f5f5f5; 
            padding: 15px; 
            border-radius: 8px; 
            margin: 20px 0; 
          }
          .details table { width: 100%; border-collapse: collapse; }
          .details td { padding: 8px; border-bottom: 1px solid #ddd; }
          .details td:first-child { font-weight: bold; width: 40%; }
          .footer { 
            text-align: center; 
            color: #666; 
            font-size: 12px; 
            margin-top: 30px; 
            padding-top: 20px; 
            border-top: 1px solid #ddd; 
          }
        </style>
      </head>
      <body>
        <div class="container">
          <div class="alert-box">
            <h1>🚨 CRITICAL ALERT</h1>
            <p style="font-size: 18px; margin: 0;">Immediate attention required</p>
          </div>
          
          <div class="details">
            <h2 style="margin-top: 0;">Patient Information</h2>
            <table>
              <tr>
                <td>Device ID:</td>
                <td><strong>${deviceId}</strong></td>
              </tr>
              <tr>
                <td>Alert Type:</td>
                <td><strong>${alertType}</strong></td>
              </tr>
              <tr>
                <td>Heart Rate:</td>
                <td><strong style="color: #ff4444;">${heartRate} BPM</strong></td>
              </tr>
              <tr>
                <td>Time:</td>
                <td>${new Date().toLocaleString()}</td>
              </tr>
            </table>
          </div>
          
          <div style="background: #fff3cd; padding: 15px; border-radius: 8px; border-left: 4px solid #ffc107;">
            <strong>⚠️ Action Required:</strong>
            <p>Please check on the patient immediately. This alert requires urgent attention.</p>
          </div>
          
          <div class="footer">
            <p>Healthcare Device Monitoring System</p>
            <p>This is an automated alert. Please do not reply to this email.</p>
          </div>
        </div>
      </body>
      </html>
    `;

    try {
      const info = await this.transporter.sendMail({
        from: `"Healthcare Alert System" <${process.env.EMAIL_USER}>`,
        to: recipients.join(', '),
        subject: subject,
        html: html
      });

      console.log('✅ Critical alert email sent:', info.messageId);
      return { success: true, messageId: info.messageId };
    } catch (error) {
      console.error('❌ Failed to send critical alert email:', error.message);
      return { success: false, error: error.message };
    }
  }

  /**
   * Send warning alert email
   */
  async sendWarningAlert(deviceId, heartRate, alertType, recipients) {
    if (!this.initialized) {
      console.warn('Email service not initialized');
      return { success: false, error: 'Email service not configured' };
    }

    const subject = `⚠️ Warning Alert - ${deviceId}`;
    const html = `
      <!DOCTYPE html>
      <html>
      <head>
        <style>
          body { font-family: Arial, sans-serif; line-height: 1.6; color: #333; }
          .container { max-width: 600px; margin: 0 auto; padding: 20px; }
          .alert-box { 
            background: #ff9800; 
            color: white; 
            padding: 20px; 
            border-radius: 8px; 
            margin: 20px 0;
            text-align: center;
          }
          .alert-box h1 { margin: 0 0 10px 0; font-size: 28px; }
          .details { 
            background: #f5f5f5; 
            padding: 15px; 
            border-radius: 8px; 
            margin: 20px 0; 
          }
          .details table { width: 100%; border-collapse: collapse; }
          .details td { padding: 8px; border-bottom: 1px solid #ddd; }
          .details td:first-child { font-weight: bold; width: 40%; }
          .footer { 
            text-align: center; 
            color: #666; 
            font-size: 12px; 
            margin-top: 30px; 
            padding-top: 20px; 
            border-top: 1px solid #ddd; 
          }
        </style>
      </head>
      <body>
        <div class="container">
          <div class="alert-box">
            <h1>⚠️ WARNING ALERT</h1>
            <p style="font-size: 18px; margin: 0;">Attention recommended</p>
          </div>
          
          <div class="details">
            <h2 style="margin-top: 0;">Patient Information</h2>
            <table>
              <tr>
                <td>Device ID:</td>
                <td><strong>${deviceId}</strong></td>
              </tr>
              <tr>
                <td>Alert Type:</td>
                <td><strong>${alertType}</strong></td>
              </tr>
              <tr>
                <td>Heart Rate:</td>
                <td><strong style="color: #ff9800;">${heartRate} BPM</strong></td>
              </tr>
              <tr>
                <td>Time:</td>
                <td>${new Date().toLocaleString()}</td>
              </tr>
            </table>
          </div>
          
          <div style="background: #e3f2fd; padding: 15px; border-radius: 8px; border-left: 4px solid #2196f3;">
            <strong>ℹ️ Recommendation:</strong>
            <p>Please monitor the patient. The vital signs are outside the normal range.</p>
          </div>
          
          <div class="footer">
            <p>Healthcare Device Monitoring System</p>
            <p>This is an automated alert. Please do not reply to this email.</p>
          </div>
        </div>
      </body>
      </html>
    `;

    try {
      const info = await this.transporter.sendMail({
        from: `"Healthcare Alert System" <${process.env.EMAIL_USER}>`,
        to: recipients.join(', '),
        subject: subject,
        html: html
      });

      console.log('✅ Warning alert email sent:', info.messageId);
      return { success: true, messageId: info.messageId };
    } catch (error) {
      console.error('❌ Failed to send warning alert email:', error.message);
      return { success: false, error: error.message };
    }
  }

  /**
   * Send daily summary email
   */
  async sendDailySummary(deviceId, stats, recipients) {
    if (!this.initialized) {
      console.warn('Email service not initialized');
      return { success: false, error: 'Email service not configured' };
    }

    const subject = `📊 Daily Health Summary - ${deviceId}`;
    const html = `
      <!DOCTYPE html>
      <html>
      <head>
        <style>
          body { font-family: Arial, sans-serif; line-height: 1.6; color: #333; }
          .container { max-width: 600px; margin: 0 auto; padding: 20px; }
          .header { 
            background: #4CAF50; 
            color: white; 
            padding: 20px; 
            border-radius: 8px; 
            text-align: center;
          }
          .stats { 
            background: #f5f5f5; 
            padding: 15px; 
            border-radius: 8px; 
            margin: 20px 0; 
          }
          .stat-row { 
            display: flex; 
            justify-content: space-between; 
            padding: 10px 0; 
            border-bottom: 1px solid #ddd; 
          }
          .footer { 
            text-align: center; 
            color: #666; 
            font-size: 12px; 
            margin-top: 30px; 
          }
        </style>
      </head>
      <body>
        <div class="container">
          <div class="header">
            <h1 style="margin: 0;">📊 Daily Health Summary</h1>
            <p style="margin: 10px 0 0 0;">${new Date().toDateString()}</p>
          </div>
          
          <div class="stats">
            <h2>Statistics</h2>
            <div class="stat-row">
              <span><strong>Average Heart Rate:</strong></span>
              <span>${stats.avgHeartRate || 'N/A'} BPM</span>
            </div>
            <div class="stat-row">
              <span><strong>Total Alerts:</strong></span>
              <span>${stats.totalAlerts || 0}</span>
            </div>
            <div class="stat-row">
              <span><strong>Critical Alerts:</strong></span>
              <span>${stats.criticalAlerts || 0}</span>
            </div>
          </div>
          
          <div class="footer">
            <p>Healthcare Device Monitoring System</p>
          </div>
        </div>
      </body>
      </html>
    `;

    try {
      const info = await this.transporter.sendMail({
        from: `"Healthcare Summary" <${process.env.EMAIL_USER}>`,
        to: recipients.join(', '),
        subject: subject,
        html: html
      });

      console.log('✅ Daily summary email sent:', info.messageId);
      return { success: true, messageId: info.messageId };
    } catch (error) {
      console.error('❌ Failed to send daily summary email:', error.message);
      return { success: false, error: error.message };
    }
  }

  /**
   * Verify email configuration
   */
  async verifyConnection() {
    if (!this.initialized) {
      return { success: false, error: 'Email service not configured' };
    }

    try {
      await this.transporter.verify();
      console.log('✅ Email server connection verified');
      return { success: true };
    } catch (error) {
      console.error('❌ Email server verification failed:', error.message);
      return { success: false, error: error.message };
    }
  }
}

// Create singleton instance
const emailService = new EmailService();

module.exports = emailService;
