# 🎯 Getting Started with Mosquitto MQTT Broker

## Welcome! 👋

Your Pet Tracker project now has a **complete Mosquitto MQTT broker deployment setup** with GitHub Actions automation. This file will get you started in 5 minutes.

---

## ⚡ Super Quick Start (2 minutes)

### Step 1: Start the broker locally

```bash
cd /home/ebrahim/Pet_Tracker_Project
docker-compose up -d
```

### Step 2: Test it's working

```bash
docker exec mosquitto-broker mosquitto_sub \
  -h localhost -p 1883 \
  -u admin -P changeme \
  -t '$SYS/broker/clients/total'
```

You should see output like: `$SYS/broker/clients/total: 1`

### Step 3: Stop the broker

```bash
docker-compose down
```

✅ **Done!** Your MQTT broker is working!

---

## 📚 What's Been Created

| What | Where | Purpose |
|------|-------|---------|
| **Docker Setup** | `mosquitto/` | Configuration files for broker |
| **Workflows** | `.github/workflows/` | Automated deployment to GitHub Actions |
| **Kubernetes** | `k8s/` | Enterprise deployment configs |
| **Docker Compose** | `docker-compose.yml` | Local development setup |
| **Docs** | `MQTT_*.md` | Comprehensive guides |

---

## 🔐 Default Credentials

```
Username: admin
Password: changeme
```

⚠️ **IMPORTANT**: Change these before production use!

---

## 🛠️ Common Tasks

### Add a New User

```bash
./mosquitto/manage-passwords.sh add myuser mypassword
```

### List All Users

```bash
./mosquitto/manage-passwords.sh list
```

### Test a User

```bash
./mosquitto/manage-passwords.sh test myuser mypassword
```

### Change a Password

```bash
./mosquitto/manage-passwords.sh change myuser newpassword
```

---

## 🚀 Deploy to GitHub

1. **Commit changes**:
   ```bash
   git add .
   git commit -m "Add Mosquitto MQTT broker"
   git push origin main
   ```

2. **Watch GitHub Actions**:
   - Go to your repository
   - Click the **Actions** tab
   - See your workflow building and pushing the image

3. **Check the image**:
   - Go to **Packages** in your repository
   - Find `mosquitto` image in ghcr.io

---

## 📖 Documentation Files

Read these in order:

1. **MQTT_DOCUMENTATION_INDEX.md** - Overview of all documentation
2. **DEPLOYMENT_SUMMARY.md** - 5-minute quick start + security checklist
3. **MQTT_SETUP.md** - Detailed setup & production guide
4. **MQTT_COMMANDS_REFERENCE.md** - Quick command reference
5. **mosquitto/README.md** - Detailed configuration reference

---

## 🧪 Testing Your Setup

### Test 1: Subscribe to Messages

```bash
# Terminal 1
docker exec mosquitto-broker mosquitto_sub \
  -h localhost -p 1883 \
  -u admin -P changeme \
  -t 'test/topic' \
  -v

# Terminal 2 - Send message
docker exec mosquitto-broker mosquitto_pub \
  -h localhost -p 1883 \
  -u admin -P changeme \
  -t 'test/topic' \
  -m 'Hello from MQTT!'
```

### Test 2: Python Client

```python
import paho.mqtt.client as mqtt

def on_connect(client, userdata, flags, rc):
    print("Connected!")
    client.subscribe("test/topic")

def on_message(client, userdata, msg):
    print(f"{msg.topic}: {msg.payload.decode()}")

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.username_pw_set("admin", "changeme")
client.connect("localhost", 1883, 60)
client.loop_forever()
```

---

## 🌐 Connect Your Pet Tracker

### Arduino/ESP32 Code Example

```cpp
#include <PubSubClient.h>

const char* mqtt_server = "localhost";  // or your server IP
const char* mqtt_user = "admin";
const char* mqtt_password = "changeme";

PubSubClient client(wifiClient);

void setup() {
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    client.connect("PetTracker", mqtt_user, mqtt_password);
  }
  client.loop();
  
  // Publish GPS location
  char payload[100];
  snprintf(payload, sizeof(payload), 
    "{\"lat\":%.6f,\"lon\":%.6f}", gps_lat, gps_lon);
  client.publish("pet/location", payload);
}
```

---

## ✨ Key Features

✅ **Three Deployment Options**:
1. Docker Compose (local development)
2. Docker + GitHub Actions (single server)
3. Kubernetes (enterprise)

✅ **Security**:
- Password-based authentication
- TLS/SSL support
- User management tools
- No anonymous access

✅ **Reliability**:
- Health checks
- Persistent storage
- Logging
- Automatic restarts

✅ **Integration**:
- Python clients
- Arduino/ESP32
- JavaScript
- Any MQTT client

---

## 🆘 Troubleshooting

### Broker won't start
```bash
docker logs mosquitto-broker
```

### Authentication fails
```bash
docker exec mosquitto-broker cat /mosquitto/config/passwords.txt
./mosquitto/manage-passwords.sh test admin changeme
```

### Can't connect
```bash
telnet localhost 1883
```

### Port already in use
Change the port in `docker-compose.yml`

---

## 📋 Deployment Checklist

- [ ] Tested broker locally with `docker-compose up`
- [ ] Changed default admin password
- [ ] Added custom users for your services
- [ ] Tested MQTT connection with test message
- [ ] Committed files to git
- [ ] Pushed to main branch
- [ ] Monitored GitHub Actions workflow
- [ ] Verified Docker image in GitHub Packages
- [ ] (Optional) Configured GitHub secrets for server deployment
- [ ] (Optional) Set up TLS certificates

---

## 🎯 Next Steps

1. **Local Testing** (5 min)
   ```bash
   docker-compose up -d
   ./mosquitto/manage-passwords.sh add pet_tracker secure_password
   ```

2. **Read Full Docs** (10 min)
   - Start with `DEPLOYMENT_SUMMARY.md`
   - Then read `MQTT_SETUP.md`

3. **Deploy to GitHub** (2 min)
   ```bash
   git add .
   git commit -m "Add Mosquitto MQTT broker"
   git push origin main
   ```

4. **Connect Your Pet Tracker**
   - Use credentials: `pet_tracker` / `secure_password`
   - Publish to: `pet/location`, `pet/status`, etc.

5. **Production Setup** (30 min)
   - Configure GitHub secrets
   - Set up TLS certificates
   - Deploy to your server

---

## 📱 Ports & Access

| Port | Purpose | Username | Password |
|------|---------|----------|----------|
| 1883 | MQTT | admin | changeme |
| 8883 | MQTT+TLS | admin | changeme |
| 9001 | WebSocket | admin | changeme |

---

## 🔗 Resources

- **This Project**: `/home/ebrahim/Pet_Tracker_Project/`
- **Mosquitto Docs**: https://mosquitto.org/man/
- **MQTT Protocol**: https://mqtt.org/
- **Python Client**: https://github.com/eclipse/paho.mqtt.python

---

## 💡 Tips & Tricks

**Tip 1**: Use the password management script
```bash
./mosquitto/manage-passwords.sh
```

**Tip 2**: Generate strong passwords
```bash
./mosquitto/manage-passwords.sh generate 20
```

**Tip 3**: Follow logs in real-time
```bash
docker-compose logs -f mosquitto
```

**Tip 4**: Check broker status
```bash
docker exec mosquitto-broker mosquitto_sub -h localhost -t '$SYS/#' -v -W 1
```

---

## ✅ You're Ready!

Your MQTT broker setup is complete and ready to use. Follow the quick start section above to get running in 2 minutes.

**Questions?** Check the documentation files:
- Quick overview: `MQTT_DOCUMENTATION_INDEX.md`
- Detailed setup: `MQTT_SETUP.md`
- Commands: `MQTT_COMMANDS_REFERENCE.md`

**Happy MQTT messaging! 🚀**

---

*Created: Oct 25, 2025*  
*For: Pet Tracker Project*  
*Broker: Mosquitto MQTT*
