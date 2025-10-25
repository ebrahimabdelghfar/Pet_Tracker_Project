# Free MQTT Server Deployment Options

## Option 1: Railway.app (Recommended) ⭐

**Cost:** Free tier includes $5/month credit  
**Best for:** Production MQTT broker with persistent storage

### Steps:
1. Go to https://railway.app
2. Sign in with GitHub
3. Create new project → Deploy from GitHub repo
4. Connect your Pet_Tracker_Project repository
5. Add environment variables:
   - `MOSQUITTO_ADMIN_PASSWORD=himaet23`
   - `MOSQUITTO_ADDITIONAL_USERS=hima:himaet23`
6. Railway will:
   - Build your Docker image from `mosquitto/Dockerfile`
   - Assign you a public domain with port
   - Run 24/7 for free
   - Give you a fixed IP/hostname

### Get Broker Address:
```bash
# Railway provides: something-production.up.railway.app
# Your MQTT broker IP: something-production.up.railway.app
# Port: 1883
```

### Add to GitHub Actions:
```yaml
name: Deploy MQTT Broker to Railway

on:
  push:
    branches: [main]
    paths:
      - 'mosquitto/**'

jobs:
  deploy:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - uses: railwayapp/action@v1
        with:
          token: ${{ secrets.RAILWAY_TOKEN }}
```

---

## Option 2: Render.com (Free Alternative)

**Cost:** Free tier with limitations  
**Best for:** Learning and testing

### Steps:
1. Go to https://render.com
2. Sign in with GitHub
3. Create Web Service
4. Deploy from GitHub
5. Add dockerfile: `mosquitto/Dockerfile`
6. Add environment variables
7. Render spins down after 15 minutes of inactivity (free tier)

### Get Broker Address:
```bash
# Render provides: your-service-name.onrender.com
# Port: 1883
```

---

## Option 3: Azure Container Instances (Free $200 Credit)

**Cost:** $200 free credit (good for 1-2 months)  
**Best for:** Learning with good resources

### Steps:
1. Go to https://azure.microsoft.com/free
2. Sign up with GitHub account
3. Create Azure Container Instances from Docker image
4. Docker image: Your GHCR image
5. Set environment variables
6. Expose port 1883

---

## Option 4: HiveMQ Cloud (Free Tier)

**Cost:** Free tier included  
**Best for:** Fully managed MQTT as a Service

### Steps:
1. Go to https://www.hivemq.cloud
2. Create free cluster
3. HiveMQ provides broker URL automatically
4. Get credentials from dashboard
5. No Docker needed - just use their hosted broker

### Pros:
- Fully managed (no maintenance)
- High availability
- WebSocket support
- Perfect for IoT projects

---

## Option 5: Deploy to GitHub with Docker Hub (Current Setup)

**Cost:** Free  
**Best for:** Building/testing, not running persistent service

Your current setup:
1. Docker image builds on GitHub Actions ✅
2. Pushed to ghcr.io automatically ✅
3. You pull it whenever needed ✅

To use this approach:
```bash
# On any machine (your laptop, etc.):
docker run -d \
  --name mosquitto \
  -p 1883:1883 \
  -e MOSQUITTO_ADMIN_PASSWORD=himaet23 \
  -e MOSQUITTO_ADDITIONAL_USERS=hima:himaet23 \
  ghcr.io/ebrahimabdelghfar/pet_tracker_project/mosquitto:main
```

---

## RECOMMENDED PATH FOR YOU:

### Best Option: **Railway.app**

Why?
1. ✅ Completely free (more than $5 monthly credit)
2. ✅ One-click GitHub integration
3. ✅ Persistent 24/7 broker
4. ✅ Public MQTT IP/domain
5. ✅ Built for Docker containers
6. ✅ Perfect for IoT projects
7. ✅ Environment variable support built-in

### Quick Setup (10 minutes):
```
1. Go to railway.app
2. Click "New Project"
3. Select "Deploy from GitHub Repo"
4. Choose Pet_Tracker_Project
5. Railway auto-detects Dockerfile
6. Add 2 environment variables
7. Deploy!
8. Copy the domain → Add to ESP32 code
```

---

## Comparison Table

| Feature | Railway | Render | HiveMQ | Azure |
|---------|---------|--------|--------|-------|
| **Free Tier** | $5/mo | Limited | Yes | $200 credit |
| **Persistent** | ✅ Yes | ⚠️ Sleeps | ✅ Yes | ✅ Yes |
| **Easy Setup** | ✅ Easiest | ✅ Easy | ⚠️ Different | ⚠️ Complex |
| **Public IP** | ✅ Yes | ✅ Yes | ✅ Yes | ✅ Yes |
| **Docker Support** | ✅ Native | ✅ Native | ❌ SaaS | ✅ Native |
| **24/7 Running** | ✅ Yes | ❌ Sleeps | ✅ Yes | ✅ Yes |
| **Best for IoT** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐ |

---

## Next Steps:

1. Choose your preferred option (I recommend Railway.app)
2. Sign up with GitHub
3. Deploy container
4. Get broker IP/domain
5. Update your GitHub Actions to retrieve and output the connection info
6. Add IP to ESP32 code
7. Test connection!

---

## Already Built & Ready:

✅ Your Mosquitto Docker image: `ghcr.io/ebrahimabdelghfar/pet_tracker_project/mosquitto:main`  
✅ Credentials configured: `hima / himaet23`  
✅ Ports open: 1883, 8883, 9001  
✅ Just need to deploy to a persistent host!
