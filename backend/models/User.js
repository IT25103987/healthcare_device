const mongoose = require('mongoose');
const bcrypt = require('bcryptjs');

/**
 * User Model
 * Represents patients, caregivers, and admins
 */

const userSchema = new mongoose.Schema({
  name: {
    type: String,
    required: [true, 'Name is required'],
    trim: true
  },
  email: {
    type: String,
    required: [true, 'Email is required'],
    unique: true,
    lowercase: true,
    trim: true,
    // Simplified email regex to avoid ReDoS vulnerability
    match: [/^[^\s@]+@[^\s@]+\.[^\s@]+$/, 'Please provide a valid email']
  },
  password: {
    type: String,
    required: [true, 'Password is required'],
    minlength: 6,
    select: false  // Don't include password in queries by default
  },
  role: {
    type: String,
    enum: ['patient', 'caregiver', 'admin'],
    default: 'caregiver'
  },
  deviceIds: [{
    type: String,
    trim: true
  }],
  notificationPreferences: {
    email: {
      type: Boolean,
      default: true
    },
    criticalOnly: {
      type: Boolean,
      default: false
    }
  },
  phone: {
    type: String,
    default: null
  },
  bloodType: {
    type: String,
    enum: ['A+', 'A-', 'B+', 'B-', 'AB+', 'AB-', 'O+', 'O-', null],
    default: null
  },
  medicalHistory: {
    type: String,
    default: null
  },
  medications: [{
    name: String,
    dosage: String,
    frequency: String
  }],
  emergencyContact: {
    name: String,
    phone: String,
    relationship: String
  }
}, {
  timestamps: true
});

// Hash password before saving
userSchema.pre('save', async function(next) {
  if (!this.isModified('password')) {
    return next();
  }
  
  try {
    const salt = await bcrypt.genSalt(10);
    this.password = await bcrypt.hash(this.password, salt);
    next();
  } catch (error) {
    next(error);
  }
});

// Method to compare password
userSchema.methods.comparePassword = async function(candidatePassword) {
  return await bcrypt.compare(candidatePassword, this.password);
};

// Don't return password in JSON responses
userSchema.methods.toJSON = function() {
  const user = this.toObject();
  delete user.password;
  return user;
};

module.exports = mongoose.model('User', userSchema);
