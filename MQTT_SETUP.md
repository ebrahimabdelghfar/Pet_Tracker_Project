# Mosquitto MQTT Broker Deployment - Quick Start Guide

## 📋 Overview

This setup provides two deployment options:

1. **Docker (Recommended for simple deployments)** - Uses Docker Compose or direct Docker deployment
2. **Kubernetes** - For scalable, production-grade deployments

## 🚀 Quick Start

### Option 1: Local Testing with Docker Compose

```bash
cd /home/ebrahim/Pet_Tracker_Project

# Start the broker
docker-compose up -d

# Check status
docker-compose ps

# View logs
docker-compose logs -f mosquitto

# Test connection
docker exec mosquitto-broker mosquitto_sub -h localhost -p 1883 -t '$SYS/broker/clients/total'

# Stop the broker
docker-compose down
```

### Option 2: GitHub Actions Deployment (Docker)

1. **Push changes to main branch:**
   ```bash
   git add mosquitto/
   git add .github/workflows/deploy-mosquitto.yml
   git add docker-compose.yml
   git commit -m "Add Mosquitto MQTT broker setup"
   git push origin main
   ```

2. **GitHub Actions automatically:**
   - Builds the Docker image
   - Pushes to GitHub Container Registry
   - Optionally deploys to your server (if secrets are configured)

## 🔐 Setting Up GitHub Secrets

For automatic deployment to your server, configure these secrets:

### In GitHub (Settings → Secrets → Actions):

1. **DEPLOY_KEY** - Your SSH private key
   ```bash
   # Generate or use existing key
   cat ~/.ssh/id_rsa | base64 | xclip -selection clipboard
   ```

2. **DEPLOY_HOST** - Server hostname/IP
   ```
   your-server.com
   ```

3. **DEPLOY_USER** - SSH username
   ```
   deploy
   ```

4. **MQTT_ADMIN_PASSWORD** - Admin password (optional, overrides default)
   ```
   your-secure-password-here
   ```

5. **MQTT_USERS** - Additional users (optional format)
   ```
   user1:password1,user2:password2
   ```

## 📦 File Structure Created

```
Pet_Tracker_Project/
├── .github/
│   └── workflows/
│       ├── deploy-mosquitto.yml        # Docker deployment workflow
│       └── deploy-mosquitto-k8s.yml    # Kubernetes deployment workflow
├── mosquitto/
│   ├── Dockerfile                      # Docker image definition
│   ├── mosquitto.conf                  # MQTT broker configuration
│   ├── password.conf                   # Password setup documentation
│   ├── entrypoint.sh                   # Dynamic password configuration
│   └── README.md                       # Detailed configuration guide
├── k8s/
│   ├── namespace.yaml                  # Kubernetes namespace
│   ├── configmap.yaml                  # Configuration as code
│   ├── secret.yaml                     # Credentials management
│   ├── deployment.yaml                 # Pod deployment spec
│   └── service.yaml                    # Service exposure
└── docker-compose.yml                  # Local development setup
```

## 🔧 Configuration

### Default Credentials

- **Username:** `admin`
- **Password:** `changeme` (change immediately!)

### Ports Exposed

| Port | Protocol      | Purpose          |
|------|---------------|------------------|
| 1883 | MQTT          | Standard MQTT    |
| 8883 | MQTT + TLS    | Secure MQTT      |
| 9001 | WebSocket     | Web clients      |

### Custom Users

Edit `docker-compose.yml`:

```yaml
environment:
  - MOSQUITTO_ADMIN_PASSWORD=your_password
  - MOSQUITTO_ADDITIONAL_USERS=pet_tracker:track_pass,dashboard:dash_pass
```

Or at runtime:
```bash
docker exec mosquitto-broker mosquitto_passwd -b /mosquitto/config/passwords.txt newuser newpass
```

## 🧪 Testing

### Test with MQTT Client

```bash
# Subscribe to test topic
mosquitto_sub -h localhost -p 1883 -u admin -P changeme -t 'test/topic'

# Publish from another terminal
mosquitto_pub -h localhost -p 1883 -u admin -P changeme -t 'test/topic' -m 'Hello MQTT!'
```

### Test with curl (WebSocket)

```bash
# Install wscat
npm install -g wscat

# Connect via WebSocket
wscat -c ws://localhost:9001
```

### Test Python Client

```python
import paho.mqtt.client as mqtt

client = mqtt.Client()
client.username_pw_set("admin", "changeme")
client.connect("localhost", 1883, 60)
client.subscribe("test/topic")
client.publish("test/topic", "Hello from Python!")
client.loop_start()
```

## 🐳 Docker Commands

### Build Image Locally

```bash
cd mosquitto
docker build -t mosquitto-custom .
```

### Run Standalone

```bash
docker run -d \
  --name mosquitto \
  -p 1883:1883 \
  -p 8883:8883 \
  -p 9001:9001 \
  -e MOSQUITTO_ADMIN_PASSWORD=mypassword \
  ghcr.io/ebrahimabdelghfar/pet_tracker_project/mosquitto:latest
```

### View Logs

```bash
docker logs -f mosquitto
```

### Stop & Remove

```bash
docker stop mosquitto
docker rm mosquitto
```

## ☸️ Kubernetes Deployment

### Prerequisites

```bash
# Install kubectl
curl -LO "https://dl.k8s.io/release/$(curl -L -s https://dl.k8s.io/release/stable.txt)/bin/linux/amd64/kubectl"
chmod +x kubectl
sudo mv kubectl /usr/local/bin/
```

### Manual Deployment

```bash
# Create namespace and resources
kubectl apply -f k8s/namespace.yaml
kubectl apply -f k8s/configmap.yaml
kubectl apply -f k8s/secret.yaml
kubectl apply -f k8s/deployment.yaml
kubectl apply -f k8s/service.yaml

# Verify deployment
kubectl get pods -n mosquitto
kubectl get svc -n mosquitto

# View logs
kubectl logs -f deployment/mosquitto -n mosquitto

# Port forward for local testing
kubectl port-forward svc/mosquitto 1883:1883 -n mosquitto
```

### Automatic Deployment (GitHub Actions)

1. Add Kubernetes secret to GitHub:
   ```bash
   kubectl config view --raw | base64 | tr -d '\n'
   ```
   Store as `KUBE_CONFIG` secret

2. Push changes:
   ```bash
   git push origin main
   ```

3. GitHub Actions automatically deploys to Kubernetes

## 🔒 Security Checklist

- [ ] Change default admin password
- [ ] Add user-specific credentials
- [ ] Enable TLS/SSL certificates
- [ ] Configure firewall rules
- [ ] Regularly backup configuration
- [ ] Monitor logs for unauthorized access
- [ ] Use strong passwords (minimum 12 characters)
- [ ] Rotate credentials periodically
- [ ] Keep Mosquitto updated

## 🆘 Troubleshooting

### Broker Won't Start
```bash
docker logs mosquitto-broker
# Check configuration syntax
docker exec mosquitto-broker mosquitto -c /mosquitto/config/mosquitto.conf -t
```

### Connection Refused
```bash
# Check if broker is running
docker ps | grep mosquitto

# Check ports
netstat -tlnp | grep 1883

# Test connectivity
telnet localhost 1883
```

### Authentication Issues
```bash
# Verify password file exists
docker exec mosquitto-broker ls -la /mosquitto/config/passwords.txt

# Check user exists
docker exec mosquitto-broker cat /mosquitto/config/passwords.txt
```

### Kubernetes Pod Crashes
```bash
# Check pod status
kubectl describe pod -n mosquitto

# View logs
kubectl logs -n mosquitto <pod-name>

# Check resource limits
kubectl top pods -n mosquitto
```

## 📚 Additional Resources

- [Mosquitto Official Documentation](https://mosquitto.org/man/)
- [MQTT Protocol](https://mqtt.org/)
- [Docker Documentation](https://docs.docker.com/)
- [Kubernetes Documentation](https://kubernetes.io/docs/)
- [GitHub Actions](https://docs.github.com/en/actions)

## 🔄 Integration with Pet Tracker

Update your Pet Tracker code:

```cpp
#include <PubSubClient.h>

const char* mqtt_server = "your-mqtt-server.com";
const int mqtt_port = 1883;
const char* mqtt_user = "pet_tracker";
const char* mqtt_password = "your_password";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup() {
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callback);
}

void reconnect() {
  if (!mqttClient.connected()) {
    mqttClient.connect("PetTracker", mqtt_user, mqtt_password);
  }
}

void loop() {
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();
  
  // Publish pet location
  mqttClient.publish("pet/location", location_data);
}
```

## 📞 Support

For issues or questions:
1. Check the logs
2. Review configuration files
3. Consult Mosquitto documentation
4. Open a GitHub issue

---

**Happy MQTT publishing! 🚀**
