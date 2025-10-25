# 🚀 Quick Start: Deploy MQTT to Railway (10 minutes)

## 🎯 Goal
Get your Pet Tracker MQTT broker running 24/7 on the free Railway.app platform.

---

## ⚡ Quick Steps

### 1️⃣ Create Railway Account (2 min)
```
Go to: https://railway.app
Click: "Start Free"
Sign in with: GitHub
```

### 2️⃣ Deploy Your Repo (3 min)
```
Dashboard → New Project
Select: Deploy from GitHub Repo
Choose: Pet_Tracker_Project
Click: Deploy
```

Railway auto-detects Dockerfile and builds! ✅

### 3️⃣ Add Environment Variables (2 min)
```
Railway Dashboard → Your Mosquitto Service
Click: Variables tab
Add:
  MOSQUITTO_ADMIN_PASSWORD = himaet23
  MOSQUITTO_ADDITIONAL_USERS = hima:himaet23
Save → Auto-restarts
```

### 4️⃣ Get Your MQTT IP (1 min)
```
Railway Dashboard → Mosquitto Service
Settings → Look for "Railway Domain"
Copy: mosquitto-production-xxxx.up.railway.app
This is your MQTT SERVER IP! 🎉
```

### 5️⃣ Test Connection (1 min)
```bash
mosquitto_sub -h mosquitto-production-xxxx.up.railway.app \
  -p 1883 -u hima -P himaet23 -t '$SYS/broker/clients/total'
```

If you see output → ✅ Connected!

---

## 📱 Add to ESP32 Code

Edit `src/main.cpp`:

```cpp
const char* mqtt_server = "mosquitto-production-xxxx.up.railway.app";
const int mqtt_port = 1883;
const char* mqtt_user = "hima";
const char* mqtt_password = "himaet23";
```

Then upload to ESP32! 🐾

---

## 💰 Cost
**FREE!** ($5/month free credit covers everything)

---

## 📚 Full Guides
- `RAILWAY_SETUP.md` - Detailed setup guide
- `DEPLOYMENT_OPTIONS.md` - All deployment options
- `ESP32_MQTT_CODE_SETUP.md` - ESP32 code examples

---

## ✨ Done!
Your MQTT broker is now running 24/7 on Railway! 🚀
