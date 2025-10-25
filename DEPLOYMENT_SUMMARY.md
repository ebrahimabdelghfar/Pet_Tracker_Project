# 🚀 Mosquitto MQTT Broker - GitHub Actions Deployment

## ✨ What Has Been Created

Your Pet Tracker project now has a complete Mosquitto MQTT broker deployment setup with GitHub Actions automation. Here's what's included:

### 📁 Directory Structure

```
Pet_Tracker_Project/
│
├── .github/workflows/
│   ├── deploy-mosquitto.yml          ← Main Docker deployment workflow
│   └── deploy-mosquitto-k8s.yml      ← Optional Kubernetes workflow
│
├── mosquitto/                        ← Broker configuration and Docker setup
│   ├── Dockerfile                    ← Custom Mosquitto image
│   ├── mosquitto.conf                ← Broker configuration
│   ├── entrypoint.sh                 ← Dynamic password setup
│   ├── manage-passwords.sh           ← Password management tool
│   ├── password.conf                 ← Password documentation
│   └── README.md                     ← Detailed configuration guide
│
├── k8s/                              ← Kubernetes manifests
│   ├── namespace.yaml
│   ├── configmap.yaml
│   ├── secret.yaml
│   ├── deployment.yaml
│   └── service.yaml
│
├── docker-compose.yml                ← Local development setup
├── MQTT_SETUP.md                     ← Quick start guide
└── DEPLOYMENT_SUMMARY.md             ← This file
```

## 🎯 Key Features

✅ **MQTT Protocol Support**
- Standard MQTT (port 1883)
- Secure MQTT with TLS (port 8883)
- WebSocket support (port 9001)

✅ **Security**
- Password-based authentication
- User management capabilities
- TLS/SSL support for encrypted connections

✅ **Docker Integration**
- Custom Dockerfile with all dependencies
- Docker Compose for local testing
- GitHub Container Registry integration

✅ **GitHub Actions**
- Automatic build on code changes
- Docker image push to GHCR
- Optional server deployment via SSH

✅ **Kubernetes Ready**
- Complete K8s manifests
- ConfigMap for configuration
- Secrets for credentials
- Health checks and probes

✅ **Tools & Scripts**
- Password management script
- Health check configuration
- Logging setup
- Volume persistence

## 🚀 Getting Started (5 Minutes)

### Step 1: Local Testing

```bash
cd /home/ebrahim/Pet_Tracker_Project

# Start the broker locally
docker-compose up -d

# Verify it's running
docker-compose ps

# Test the connection
docker exec mosquitto-broker mosquitto_sub -h localhost -p 1883 -t '$SYS/broker/clients/total'

# View logs
docker-compose logs -f mosquitto
```

### Step 2: Commit to GitHub

```bash
git add .github/workflows/
git add mosquitto/
git add k8s/
git add docker-compose.yml
git add MQTT_SETUP.md

git commit -m "Add Mosquitto MQTT broker with GitHub Actions deployment"
git push origin main
```

### Step 3: Monitor GitHub Actions

Go to your repository → **Actions** tab to see:
- Build status
- Docker image push
- Deployment logs (if SSH secrets configured)

## 🔐 Configuration for Production

### Option A: Docker Deployment

1. **Add GitHub Secrets** (Settings → Secrets → Actions):
   ```
   DEPLOY_KEY: (SSH private key for your server)
   DEPLOY_HOST: your-server.com
   DEPLOY_USER: deploy_user
   ```

2. **On your server**:
   ```bash
   mkdir -p /etc/mosquitto/config
   mkdir -p /mosquitto/data
   mkdir -p /mosquitto/log
   chmod 755 /mosquitto/{data,log}
   ```

3. **Push to main branch** - GitHub Actions automatically deploys!

### Option B: Kubernetes Deployment

1. **Add GitHub Secret**:
   ```bash
   kubectl config view --raw | base64 | tr -d '\n'
   # Store as KUBE_CONFIG secret
   ```

2. **Use the K8s workflow**:
   ```bash
   git push origin main
   # Deploys to Kubernetes automatically
   ```

## 📝 Managing Users & Passwords

### Using the Management Script

```bash
cd /home/ebrahim/Pet_Tracker_Project/mosquitto

# Interactive menu
./manage-passwords.sh

# Or command line
./manage-passwords.sh add pet_tracker my_secure_password
./manage-passwords.sh list
./manage-passwords.sh change admin new_password
./manage-passwords.sh delete old_user
./manage-passwords.sh test admin my_password
./manage-passwords.sh generate 20  # Generate 20-char password
```

### Default Credentials
- Username: `admin`
- Password: `changeme` (⚠️ Change immediately!)

## 🧪 Testing Your MQTT Setup

### Test 1: Connection Test
```bash
docker exec mosquitto-broker mosquitto_sub \
  -h localhost -p 1883 \
  -u admin -P changeme \
  -t '$SYS/broker/clients/total'
```

### Test 2: Publish & Subscribe
```bash
# Terminal 1 - Subscribe
docker exec mosquitto-broker mosquitto_sub -h localhost -p 1883 -u admin -P changeme -t "test/topic"

# Terminal 2 - Publish
docker exec mosquitto-broker mosquitto_pub -h localhost -p 1883 -u admin -P changeme -t "test/topic" -m "Hello MQTT!"
```

### Test 3: From Python
```python
import paho.mqtt.client as mqtt

def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")
    client.subscribe("test/topic")

def on_message(client, userdata, msg):
    print(f"{msg.topic}: {msg.payload.decode()}")

client = mqtt.Client()
client.username_pw_set("admin", "changeme")
client.on_connect = on_connect
client.on_message = on_message
client.connect("localhost", 1883, 60)
client.loop_forever()
```

## 📊 GitHub Actions Workflow

### What Happens on Push to Main

1. **Checkout** - Gets latest code
2. **Setup Docker Buildx** - Prepares build environment
3. **Login to Registry** - Authenticates with GHCR
4. **Build & Push** - Creates and uploads Docker image
5. **Deploy (Optional)** - Deploys to server if SSH secrets configured

### Monitoring Workflow

In GitHub:
1. Go to **Actions** tab
2. Click on the workflow run
3. View build logs in real-time
4. Check deployment status

## 🔒 Security Best Practices

✅ **Implemented**
- Password-based authentication required
- Anonymous access disabled
- TLS/SSL support included

⚠️ **ToDo**
1. Change default `admin` password immediately
2. Generate strong passwords for each user
3. Enable TLS certificates for production
4. Use GitHub secrets for credentials
5. Restrict firewall access to MQTT ports
6. Regular credential rotation
7. Monitor logs for unauthorized access

## 📚 File Reference

| File | Purpose |
|------|---------|
| `.github/workflows/deploy-mosquitto.yml` | GitHub Actions workflow for Docker deployment |
| `.github/workflows/deploy-mosquitto-k8s.yml` | GitHub Actions workflow for Kubernetes |
| `mosquitto/Dockerfile` | Custom Docker image with configuration |
| `mosquitto/mosquitto.conf` | Broker configuration |
| `mosquitto/entrypoint.sh` | Container startup script |
| `mosquitto/manage-passwords.sh` | User/password management tool |
| `docker-compose.yml` | Local development setup |
| `k8s/deployment.yaml` | Kubernetes pod specification |
| `k8s/service.yaml` | Kubernetes service exposure |
| `MQTT_SETUP.md` | Comprehensive setup guide |

## 🔧 Customization

### Change Default Port
Edit `mosquitto/mosquitto.conf`:
```conf
listener 1883
# Change to:
listener 1884
```

### Add More Users
Edit `docker-compose.yml`:
```yaml
MOSQUITTO_ADDITIONAL_USERS: user1:pass1,user2:pass2,user3:pass3
```

### Enable TLS
Place certificates in `mosquitto/certs/`:
```
mosquitto/certs/
├── ca.crt
├── server.crt
└── server.key
```

Then `mosquitto.conf` automatically uses them.

## 🐛 Troubleshooting

### Broker won't start
```bash
docker logs mosquitto-broker
docker-compose up  # Without -d to see logs
```

### Authentication fails
```bash
# Check password file
docker exec mosquitto-broker cat /mosquitto/config/passwords.txt

# Verify user exists
docker exec mosquitto-broker grep "^youruser:" /mosquitto/config/passwords.txt
```

### Connection refused
```bash
# Check if container is running
docker ps | grep mosquitto

# Check ports
docker port mosquitto-broker

# Test telnet
telnet localhost 1883
```

### GitHub Actions fails
1. Check workflow logs in Actions tab
2. Verify Docker syntax
3. Check secrets are configured correctly
4. Review error messages in build output

## 🔄 Integration with Pet Tracker

### C++ Code Example
```cpp
#include <PubSubClient.h>

const char* mqtt_server = "mqtt.yourdomain.com";
const char* mqtt_user = "pet_tracker";
const char* mqtt_password = "your_password";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup() {
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("PetTracker", mqtt_user, mqtt_password)) {
      client.subscribe("pet/commands");
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  // Publish GPS location
  char payload[100];
  snprintf(payload, sizeof(payload), "{\"lat\":%.6f,\"lon\":%.6f}", gps_lat, gps_lon);
  client.publish("pet/location", payload);
}
```

## 📞 Support Resources

- **Mosquitto Docs**: https://mosquitto.org/man/
- **MQTT Protocol**: https://mqtt.org/
- **Docker Docs**: https://docs.docker.com/
- **GitHub Actions**: https://docs.github.com/en/actions
- **Kubernetes**: https://kubernetes.io/docs/

## ✅ Next Steps

1. [ ] Change default admin password
2. [ ] Set up GitHub secrets for deployment
3. [ ] Test locally with `docker-compose up`
4. [ ] Test Python/C++ client connections
5. [ ] Configure TLS certificates (production)
6. [ ] Set up server deployment (optional)
7. [ ] Monitor broker logs
8. [ ] Add additional users as needed

## 📦 What's Included in This Package

- ✅ Complete Dockerfile with best practices
- ✅ Production-ready configuration
- ✅ GitHub Actions CI/CD pipeline
- ✅ Docker Compose for local dev
- ✅ Kubernetes manifests
- ✅ Password management tools
- ✅ Health checks & monitoring
- ✅ Comprehensive documentation
- ✅ Integration examples
- ✅ Security best practices

---

**Congratulations! Your MQTT broker is ready for deployment! 🎉**

For questions or issues, check `MQTT_SETUP.md` or `mosquitto/README.md` for detailed information.
