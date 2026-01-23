# 🚀 Deployment Guide

Deploy your healthcare monitoring system to production using free hosting services.

## 📋 Overview

This guide covers:
1. MongoDB Atlas (Database)
2. Railway/Render (Backend)
3. Vercel/Netlify (Frontend)
4. Custom Domain & SSL

---

## 1. MongoDB Atlas (Database)

### Setup Steps

1. **Create Account**
   - Go to https://www.mongodb.com/cloud/atlas
   - Sign up for free

2. **Create Cluster**
   - Click "Build a Database"
   - Choose **FREE** M0 Sandbox tier
   - Select region closest to users
   - Click "Create"

3. **Create Database User**
   - Navigate to "Database Access"
   - Add New Database User
   - Username: `healthcareuser`
   - Password: Generate secure password
   - User Privileges: "Atlas admin" or "Read and write to any database"
   - Click "Add User"

4. **Whitelist IP Addresses**
   - Navigate to "Network Access"
   - Add IP Address
   - For development: Add your current IP
   - For production: "Allow access from anywhere" (0.0.0.0/0)

5. **Get Connection String**
   - Navigate to "Database" → "Connect"
   - Choose "Connect your application"
   - Copy the connection string
   - Replace `<password>` with your password
   - Replace `<dbname>` with `healthcare`

Example:
```
mongodb+srv://healthcareuser:YOUR_PASSWORD@cluster0.xxxxx.mongodb.net/healthcare?retryWrites=true&w=majority
```

---

## 2. Backend Deployment

### Option A: Railway (Recommended)

**Features:**
- 500 hours/month free
- Automatic deployments
- Built-in environment variables
- Free SSL

**Steps:**

1. **Create Account**
   - Go to https://railway.app
   - Sign up with GitHub

2. **Create New Project**
   - Click "New Project"
   - Select "Deploy from GitHub repo"
   - Authorize Railway to access your repository
   - Select `healthcare_device` repository

3. **Configure Build**
   - Root Directory: `/backend`
   - Build Command: `npm install`
   - Start Command: `npm start`

4. **Add Environment Variables**
   - Click on your service → Variables
   - Add each variable from `.env`:
   ```
   PORT=5000
   MONGODB_URI=mongodb+srv://...
   JWT_SECRET=your_secret_here
   NODE_ENV=production
   EMAIL_USER=your-email@gmail.com
   EMAIL_PASSWORD=your-app-password
   CLIENT_URL=https://your-frontend.vercel.app
   SEND_EMAIL_ALERTS=true
   ALERT_EMAIL_RECIPIENTS=caregiver@example.com
   ```

5. **Deploy**
   - Click "Deploy"
   - Wait for deployment to complete
   - Get your backend URL: `https://your-app.railway.app`

6. **Test Deployment**
   ```bash
   curl https://your-app.railway.app/health
   ```

### Option B: Render

**Features:**
- Free tier available
- Automatic deployments
- Easy setup

**Steps:**

1. **Create Account**
   - Go to https://render.com
   - Sign up with GitHub

2. **Create Web Service**
   - Click "New" → "Web Service"
   - Connect your GitHub repository
   - Select `healthcare_device` repository

3. **Configure Service**
   - Name: `healthcare-backend`
   - Root Directory: `backend`
   - Environment: `Node`
   - Build Command: `npm install`
   - Start Command: `npm start`
   - Free tier: Select "Free"

4. **Add Environment Variables**
   - Scroll to "Environment Variables"
   - Add all variables from `.env`

5. **Deploy**
   - Click "Create Web Service"
   - Wait for deployment

6. **Get URL**
   - Your backend URL: `https://healthcare-backend.onrender.com`

---

## 3. Frontend Deployment

### Option A: Vercel (Recommended)

**Features:**
- Unlimited free deployments
- Automatic HTTPS
- Global CDN
- Automatic deployments from GitHub

**Steps:**

1. **Create Account**
   - Go to https://vercel.com
   - Sign up with GitHub

2. **Import Project**
   - Click "Add New" → "Project"
   - Import your `healthcare_device` repository

3. **Configure Project**
   - Framework Preset: Vite
   - Root Directory: `web-app`
   - Build Command: `npm run build`
   - Output Directory: `dist`

4. **Add Environment Variables**
   - Expand "Environment Variables"
   - Add:
   ```
   VITE_API_URL=https://your-backend.railway.app/api
   VITE_WS_URL=https://your-backend.railway.app
   ```

5. **Deploy**
   - Click "Deploy"
   - Wait for deployment (usually 1-2 minutes)
   - Get your URL: `https://your-app.vercel.app`

6. **Update Backend CORS**
   - Update `CLIENT_URL` in backend environment variables
   - Set to: `https://your-app.vercel.app`

### Option B: Netlify

**Features:**
- 100GB bandwidth/month free
- Automatic deployments
- Custom domains

**Steps:**

1. **Create Account**
   - Go to https://netlify.com
   - Sign up with GitHub

2. **Create New Site**
   - Click "Add new site" → "Import an existing project"
   - Choose GitHub
   - Select repository

3. **Configure Build**
   - Base directory: `web-app`
   - Build command: `npm run build`
   - Publish directory: `web-app/dist`

4. **Add Environment Variables**
   - Site settings → Environment variables
   - Add:
   ```
   VITE_API_URL=https://your-backend.railway.app/api
   VITE_WS_URL=https://your-backend.railway.app
   ```

5. **Deploy**
   - Click "Deploy site"
   - Get URL: `https://your-app.netlify.app`

---

## 4. Custom Domain & SSL

### Add Custom Domain (Vercel)

1. **Purchase Domain**
   - Use Namecheap, GoDaddy, or any registrar

2. **Add Domain to Vercel**
   - Project Settings → Domains
   - Add your domain: `healthcare.yourdomain.com`

3. **Configure DNS**
   - Add CNAME record:
   ```
   Type: CNAME
   Name: healthcare
   Value: cname.vercel-dns.com
   ```

4. **SSL Certificate**
   - Vercel automatically provisions SSL (Let's Encrypt)
   - Wait 24-48 hours for DNS propagation

### Add Custom Domain (Railway)

1. **Add Domain**
   - Service Settings → Networking
   - Add custom domain

2. **Configure DNS**
   - Add CNAME record pointing to Railway

---

## 5. Post-Deployment

### Update ESP32 WiFi Configuration

If using WiFi module, update server URL in code:

```cpp
wifiSender.begin(
  "YOUR_WIFI_SSID",
  "YOUR_WIFI_PASSWORD",
  "https://your-backend.railway.app/api/data",  // ← Update this
  "watch_001"
);
```

### Test Production System

1. **Test Backend API**
   ```bash
   curl https://your-backend.railway.app/health
   ```

2. **Test Frontend**
   - Visit https://your-app.vercel.app
   - Try login
   - Check dashboard

3. **Test ESP32 Connection**
   - Upload updated code
   - Check Serial Monitor
   - Verify data transmission

4. **Test Email Alerts**
   - Send critical heart rate data
   - Check email delivery

---

## 6. Monitoring & Maintenance

### Backend Monitoring

- **Railway Dashboard**: View logs and metrics
- **Uptime Monitoring**: Use UptimeRobot (free)
- **Error Tracking**: Check Railway logs

### Database Monitoring

- **Atlas Dashboard**: Monitor queries, storage
- **Alerts**: Set up Atlas alerts for high usage
- **Backups**: Atlas provides automatic backups

### Frontend Monitoring

- **Vercel Analytics**: Built-in analytics
- **Performance**: Check Lighthouse scores
- **Error Tracking**: Use browser console

---

## 7. Cost Optimization

### Free Tier Limits

**Railway:**
- 500 hours/month (20 days continuous)
- Tip: Stop service when not needed

**MongoDB Atlas:**
- 512MB storage
- Tip: Archive old data periodically

**Vercel:**
- Unlimited deployments
- 100GB bandwidth

### Cost Reduction Tips

1. **Archive old health data** (> 90 days)
2. **Limit data frequency** (send every 2-5 minutes instead of 1)
3. **Optimize images and assets**
4. **Use caching effectively**

---

## 8. Security Checklist

- [ ] Strong JWT secret (32+ characters)
- [ ] HTTPS enabled (automatic with Vercel/Railway)
- [ ] CORS configured correctly
- [ ] Rate limiting enabled
- [ ] MongoDB access restricted (IP whitelist)
- [ ] Gmail app password (not main password)
- [ ] Environment variables secured
- [ ] Default passwords changed

---

## 9. Rollback & Recovery

### Rollback Deployment

**Vercel:**
- Deployments → Select previous deployment → "Promote to Production"

**Railway:**
- Service → Deployments → Redeploy previous version

### Database Backup

**Atlas:**
- Automatic backups (retained 7 days on free tier)
- Restore: Clusters → Backup → Restore

---

## 10. Troubleshooting

### Backend Not Starting

- Check Railway/Render logs
- Verify environment variables
- Test MongoDB connection string
- Check Node.js version compatibility

### Frontend Build Failed

- Check build logs in Vercel/Netlify
- Verify environment variables have `VITE_` prefix
- Check node_modules are not committed

### CORS Errors

- Verify CLIENT_URL in backend matches frontend URL
- Check CORS origin configuration
- Ensure protocol (http/https) matches

---

## 📝 Deployment Checklist

- [ ] MongoDB Atlas cluster created
- [ ] Database user and IP whitelist configured
- [ ] Backend deployed to Railway/Render
- [ ] All backend environment variables set
- [ ] Frontend deployed to Vercel/Netlify
- [ ] Frontend environment variables configured
- [ ] Backend CORS updated with frontend URL
- [ ] Email alerts tested
- [ ] Custom domain configured (optional)
- [ ] ESP32 updated with production URL
- [ ] End-to-end testing completed

---

## 🎉 Success!

Your healthcare monitoring system is now live in production!

**Next Steps:**
- Monitor system performance
- Set up uptime monitoring
- Configure backups
- Share with users

For questions or issues, refer to the troubleshooting section or create an issue on GitHub.
