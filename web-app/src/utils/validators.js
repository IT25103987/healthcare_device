export const validators = {
  validateEmail: (email) => {
    const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    return emailRegex.test(email);
  },

  validatePassword: (password) => {
    // At least 8 characters, one uppercase, one lowercase, one number
    const passwordRegex = /^(?=.*[a-z])(?=.*[A-Z])(?=.*\d).{8,}$/;
    return passwordRegex.test(password);
  },

  validatePhone: (phone) => {
    const phoneRegex = /^[\d\s\-+()]{10,}$/;
    return phoneRegex.test(phone);
  },

  validateDate: (date) => {
    return !isNaN(new Date(date).getTime());
  },

  validateName: (name) => {
    return name && name.trim().length >= 2 && name.trim().length <= 100;
  },

  validateID: (id) => {
    return id && id.toString().trim().length > 0;
  },

  validateHeartRate: (rate) => {
    return rate >= 30 && rate <= 200;
  },

  validateBloodPressure: (systolic, diastolic) => {
    return systolic >= 50 && systolic <= 300 && diastolic >= 30 && diastolic <= 200;
  },

  validateTemperature: (temp) => {
    return temp >= 35 && temp <= 42;
  },

  validateOxygen: (o2) => {
    return o2 >= 70 && o2 <= 100;
  },

  validateRespiratoryRate: (rate) => {
    return rate >= 5 && rate <= 60;
  },

  getValidationErrors: (data, schema) => {
    const errors = {};

    Object.keys(schema).forEach(field => {
      const validator = schema[field];
      const value = data[field];

      if (validator.required && !value) {
        errors[field] = `${field} is required`;
      } else if (value && validator.validate && !validator.validate(value)) {
        errors[field] = validator.message || `${field} is invalid`;
      }
    });

    return errors;
  },

  hasErrors: (errors) => {
    return Object.keys(errors).length > 0;
  },
};

export default validators;
