require('dotenv').config();
const express = require('express');
const cors = require('cors');
const helmet = require('helmet');
const rateLimit = require('express-rate-limit');
const http = require('http');
const socketIo = require('socket.io');

const connectDB = require('./config/db');
const errorHandler = require('./middleware/errorHandler');

// Route imports
const authRoutes = require('./routes/authRoutes');
const dataRoutes = require('./routes/dataRoutes');
const alertRoutes = require('./routes/alertRoutes');

/**
 * Healthcare Device Backend Server
 * Receives sensor data from ESP32 and provides APIs for web dashboard
 */

// Initialize Express app
const app = express();
const server = http.createServer(app);

// Initialize Socket.IO for real-time updates
const io = socketIo(server, {
  cors: {
    origin: process.env.CLIENT_URL || 'http://localhost:5173',
    methods: ['GET', 'POST']
  }
});

// Make io accessible to routes
app.set('io', io);

// Connect to MongoDB
connectDB();

// Security middleware
app.use(helmet());

// CORS configuration
const corsOptions = {
  origin: process.env.CLIENT_URL || 'http://localhost:5173',
  optionsSuccessStatus: 200
};
app.use(cors(corsOptions));

// Body parser middleware
app.use(express.json());
app.use(express.urlencoded({ extended: true }));

// Rate limiting
const limiter = rateLimit({
  windowMs: 15 * 60 * 1000, // 15 minutes
  max: 100, // Limit each IP to 100 requests per windowMs
  message: 'Too many requests from this IP, please try again later'
});

// Apply rate limiting to API routes (but not to ESP32 data endpoint)
app.use('/api/auth', limiter);
app.use('/api/alerts', limiter);

// Health check endpoint
app.get('/health', (req, res) => {
  res.json({
    success: true,
    message: 'Server is running',
    timestamp: new Date().toISOString(),
    uptime: process.uptime()
  });
});

// API Routes
app.use('/api/auth', authRoutes);
app.use('/api/data', dataRoutes);
app.use('/api/alerts', alertRoutes);

// Root endpoint
app.get('/', (req, res) => {
  res.json({
    success: true,
    message: 'Healthcare Device Backend API',
    version: '1.0.0',
    endpoints: {
      health: '/health',
      auth: {
        login: 'POST /api/auth/login',
        register: 'POST /api/auth/register',
        me: 'GET /api/auth/me'
      },
      data: {
        post: 'POST /api/data',
        getLatest: 'GET /api/data/:deviceId',
        getHistory: 'GET /api/data/:deviceId/history'
      },
      alerts: {
        getAll: 'GET /api/alerts',
        getUnread: 'GET /api/alerts/unread',
        getStats: 'GET /api/alerts/stats',
        handle: 'POST /api/alerts/:id/handle'
      }
    }
  });
});

// 404 handler
app.use((req, res) => {
  res.status(404).json({
    success: false,
    message: 'Route not found'
  });
});

// Error handler middleware (must be last)
app.use(errorHandler);

// Socket.IO connection handling
io.on('connection', (socket) => {
  console.log('✅ New client connected:', socket.id);

  // Send welcome message
  socket.emit('connected', {
    message: 'Connected to healthcare monitoring system',
    timestamp: new Date().toISOString()
  });

  // Handle client disconnect
  socket.on('disconnect', () => {
    console.log('❌ Client disconnected:', socket.id);
  });

  // Handle device subscription
  socket.on('subscribeDevice', (deviceId) => {
    socket.join(`device_${deviceId}`);
    console.log(`📱 Client ${socket.id} subscribed to device ${deviceId}`);
  });

  // Handle device unsubscription
  socket.on('unsubscribeDevice', (deviceId) => {
    socket.leave(`device_${deviceId}`);
    console.log(`📱 Client ${socket.id} unsubscribed from device ${deviceId}`);
  });
});

// Start server
const PORT = process.env.PORT || 5000;

server.listen(PORT, () => {
  console.log('\n========================================');
  console.log('🏥 Healthcare Device Backend Server');
  console.log('========================================');
  console.log(`✅ Server running on port ${PORT}`);
  console.log(`✅ Environment: ${process.env.NODE_ENV || 'development'}`);
  console.log(`✅ API URL: http://localhost:${PORT}`);
  console.log(`✅ Health check: http://localhost:${PORT}/health`);
  console.log('========================================\n');
});

// Handle unhandled promise rejections
process.on('unhandledRejection', (err) => {
  console.error('❌ Unhandled Promise Rejection:', err);
  // Log error but don't exit in production to avoid unnecessary downtime
  if (process.env.NODE_ENV !== 'production') {
    server.close(() => process.exit(1));
  }
});

// Handle SIGTERM
process.on('SIGTERM', () => {
  console.log('👋 SIGTERM received. Shutting down gracefully...');
  server.close(() => {
    console.log('✅ Process terminated');
  });
});

module.exports = { app, server, io };
