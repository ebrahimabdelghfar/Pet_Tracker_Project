# Deploy MQTT Broker to Railway.app (Free)

## What is Railway?

Railway is a **free cloud platform** where you can:
- Run Docker containers 24/7
- Get a public IP/domain
- Manage environment variables
- View logs
- Deploy from GitHub automatically

**Cost:** FREE (includes $5/month credit)

---

## Step-by-Step Setup (10 minutes)

### Step 1: Create Railway Account

1. Go to https://railway.app
2. Click **"Start Free"**
3. Sign in with **GitHub** (recommended)
4. Authorize Railway to access your GitHub account

### Step 2: Create New Project

1. On Railway dashboard, click **"New Project"**
2. Select **"Deploy from GitHub Repo"**
3. Select your GitHub repository: `Pet_Tracker_Project`
4. Click **"Deploy"**

Railway will automatically:
- Detect your Dockerfile in `mosquitto/` folder
- Build the Docker image
- Deploy the container
- Assign you a public domain

### Step 3: Configure Environment Variables

1. In Railway dashboard, go to your **Mosquitto service**
2. Click **"Variables"** tab
3. Add environment variables:
   ```
   MOSQUITTO_ADMIN_PASSWORD = himaet23
   MOSQUITTO_ADDITIONAL_USERS = hima:himaet23
   ```
4. Click **"Save"**
5. Railway will automatically restart the service

### Step 4: Get Your Broker Domain

1. In Railway dashboard, select **Mosquitto service**
2. Go to **"Settings"** tab
3. Look for **"Railway Domain"** or **"Public URL"**
4. You'll see something like: `mosquitto-production-xxxx.up.railway.app`
5. **Copy this domain** - it's your MQTT broker IP!

### Step 5: Expose Port 1883

1. In Railway service settings
2. Look for **"Networking"** or **"Ports"**
3. Make sure port `1883` is exposed (Railway does this automatically for Docker)
4. Verify in logs that Mosquitto is listening on port 1883

---

## Verify Connection

### Method 1: From Your Computer

```bash
# Install mosquitto client if you don't have it
# macOS:
brew install mosquitto

# Ubuntu/Debian:
sudo apt-get install mosquitto-clients

# Test connection (replace domain with your Railway domain)
mosquitto_sub -h mosquitto-production-xxxx.up.railway.app -p 1883 \
  -u hima -P himaet23 -t '$SYS/broker/clients/total'

# You should see output like: 1
```

### Method 2: From Railway Logs

1. Go to Railway dashboard → Mosquitto service
2. Click **"Logs"** tab
3. You should see:
   ```
   [broker initialization complete]
   [1683012345] New client connected from...
   ```

---

## Add to ESP32 Code

Once you have your Railway domain, update your `main.cpp`:

```cpp
// MQTT Broker Configuration
const char* mqtt_server = "mosquitto-production-xxxx.up.railway.app";  // ← YOUR RAILWAY DOMAIN
const int mqtt_port = 1883;
const char* mqtt_user = "hima";
const char* mqtt_password = "himaet23";

// WiFi Configuration
const char* ssid = "Your_WiFi_SSID";
const char* wifi_password = "Your_WiFi_Password";
```

---

## Enable GitHub Actions Auto-Deployment (Optional)

To automatically deploy when you push to GitHub:

### Step 1: Get Railway Token

1. Go to Railway dashboard
2. Click your **profile icon** → **"Account"**
3. Click **"Tokens"**
4. Create new token: **"Deploy Token"**
5. Copy the token

### Step 2: Add GitHub Secret

1. Go to your GitHub repo → **Settings**
2. **Secrets and variables** → **Actions**
3. Click **"New repository secret"**
4. Name: `RAILWAY_TOKEN`
5. Value: Paste your Railway token
6. Click **"Add secret"**

### Step 3: Workflow Already Ready!

The workflow file `.github/workflows/deploy-railway.yml` is already configured!

Every time you push to GitHub:
1. GitHub Actions runs
2. Builds new Docker image
3. Deploys to Railway
4. Your MQTT broker updates automatically! 🚀

---

## Troubleshooting

### Problem: "Railway Domain not showing"

**Solution:**
1. Go to Railway dashboard
2. Select Mosquitto service
3. Look at **"Domains"** or **"URL"** section
4. If empty, click **"Generate Domain"**

### Problem: "Can't connect to MQTT broker"

**Check:**
1. Railway domain is correct
2. Port is 1883 (not 443)
3. Username is `hima` (not `admin`)
4. Password is `himaet23` (case-sensitive)
5. WiFi on ESP32 is working
6. Check Railway logs for errors

### Problem: "Logs show mosquitto: unknown user"

**Solution:**
1. Check environment variables are set correctly
2. Restart the service:
   - Railway dashboard → Service settings → **"Restart"**

---

## Useful Railway Links

| Link | Purpose |
|------|---------|
| https://railway.app/dashboard | Main dashboard |
| https://docs.railway.app | Railway documentation |
| https://docs.railway.app/deploy/dockerfile | Docker deployment guide |
| https://docs.railway.app/reference/environment-variables | Environment variables |

---

## Docker Image on Railway

**Image:** `ghcr.io/ebrahimabdelghfar/pet_tracker_project/mosquitto:main`  
**Built:** Every time you push to GitHub  
**Size:** ~150MB  
**Base:** eclipse-mosquitto 2.0.18  

---

## Cost Breakdown

- **Railway free tier:** $5/month credit
- **Mosquitto container:** ~$1/month (always free with credit)
- **Bandwidth:** ~$0.10/month (minimal)
- **Total:** **$0 - covered by free credit! 🎉**

---

## What You Get with Railway

✅ 24/7 MQTT broker running  
✅ Public IP/domain for ESP32 access  
✅ Environment variable management  
✅ Automatic log viewing  
✅ One-click GitHub integration  
✅ Auto-deployment on push  
✅ Health monitoring  
✅ Service restart capability  
✅ $5/month free credit  

---

## Next Steps

1. ✅ Create Railway account (railway.app)
2. ✅ Deploy Pet_Tracker_Project
3. ✅ Configure environment variables
4. ✅ Get your Railway domain
5. ✅ Add domain to ESP32 code
6. ✅ Upload code to ESP32
7. ✅ Watch logs in Railway dashboard
8. ✅ Your Pet Tracker is connected! 🐾

---

**Questions?** Check Railway docs or create an issue on GitHub!
