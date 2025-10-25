# Quick Reference - Mosquitto MQTT Commands

## 🚀 Quick Start Commands

### Start Development Environment
```bash
# Start broker with docker-compose
docker-compose up -d

# View status
docker-compose ps

# View logs
docker-compose logs -f mosquitto

# Stop broker
docker-compose down
```

### Test Connection
```bash
# Basic test
docker exec mosquitto-broker mosquitto_sub -h localhost -p 1883 -t '$SYS/broker/clients/total'

# With authentication
docker exec mosquitto-broker mosquitto_sub -h localhost -p 1883 -u admin -P changeme -t "test/topic"

# Publish message
docker exec mosquitto-broker mosquitto_pub -h localhost -p 1883 -u admin -P changeme -t "test/topic" -m "Hello"
```

## 👥 User Management

### Add User
```bash
# Interactive
./mosquitto/manage-passwords.sh add username password

# Direct
docker exec mosquitto-broker mosquitto_passwd -b /mosquitto/config/passwords.txt username password
```

### List Users
```bash
./mosquitto/manage-passwords.sh list
docker exec mosquitto-broker cat /mosquitto/config/passwords.txt
```

### Change Password
```bash
./mosquitto/manage-passwords.sh change username new_password
docker exec mosquitto-broker mosquitto_passwd -b /mosquitto/config/passwords.txt username new_password
```

### Delete User
```bash
./mosquitto/manage-passwords.sh delete username
docker exec mosquitto-broker mosquitto_passwd -D /mosquitto/config/passwords.txt username
```

### Reset All Users
```bash
./mosquitto/manage-passwords.sh reset
```

## 🔍 Debugging

### View Logs
```bash
docker logs mosquitto-broker
docker logs -f mosquitto-broker  # Follow logs
docker logs --tail 100 mosquitto-broker  # Last 100 lines
```

### Check Configuration
```bash
docker exec mosquitto-broker mosquitto -c /mosquitto/config/mosquitto.conf -t
```

### Test Port Connectivity
```bash
# Test MQTT port
telnet localhost 1883

# Test secure MQTT port
openssl s_client -connect localhost:8883

# Test WebSocket port
wscat -c ws://localhost:9001
```

### Verify Authentication File
```bash
docker exec mosquitto-broker ls -la /mosquitto/config/passwords.txt
docker exec mosquitto-broker cat /mosquitto/config/passwords.txt
```

## 🐳 Docker Operations

### Build Image
```bash
docker build -t mosquitto-custom ./mosquitto
```

### Run Standalone
```bash
docker run -d \
  --name mosquitto \
  -p 1883:1883 \
  -p 8883:8883 \
  -p 9001:9001 \
  -e MOSQUITTO_ADMIN_PASSWORD=mypassword \
  mosquitto-custom
```

### Inspect Container
```bash
docker inspect mosquitto-broker
docker exec mosquitto-broker sh
docker exec mosquitto-broker whoami
```

### Container Logs
```bash
docker logs mosquitto-broker
docker stats mosquitto-broker  # Resource usage
```

## 📊 MQTT Client Testing

### Using mosquitto_sub (Subscribe)
```bash
# Subscribe to all messages
mosquitto_sub -h localhost -p 1883 -u admin -P changeme -t '#'

# Subscribe to specific topic
mosquitto_sub -h localhost -p 1883 -u admin -P changeme -t 'device/+/status'

# Subscribe with verbose output
mosquitto_sub -h localhost -p 1883 -u admin -P changeme -t 'test/topic' -v

# Wait max 5 seconds then exit
mosquitto_sub -h localhost -p 1883 -u admin -P changeme -t 'test/topic' -W 5
```

### Using mosquitto_pub (Publish)
```bash
# Publish message
mosquitto_pub -h localhost -p 1883 -u admin -P changeme -t 'test/topic' -m 'Hello'

# Publish from file
mosquitto_pub -h localhost -p 1883 -u admin -P changeme -t 'test/topic' -f message.txt

# Publish with Quality of Service
mosquitto_pub -h localhost -p 1883 -u admin -P changeme -t 'test/topic' -m 'Hello' -q 2

# Retain message
mosquitto_pub -h localhost -p 1883 -u admin -P changeme -t 'test/topic' -m 'Hello' -r
```

## 🐍 Python Client Example

```python
import paho.mqtt.client as mqtt

# Callbacks
def on_connect(client, userdata, flags, rc):
    print(f"Connected: {rc}")
    client.subscribe("test/topic")

def on_message(client, userdata, msg):
    print(f"{msg.topic}: {msg.payload.decode()}")

# Create client
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# Connect
client.username_pw_set("admin", "changeme")
client.connect("localhost", 1883, 60)

# Publish
client.publish("test/topic", "Hello from Python")

# Loop
client.loop_forever()
```

## ☸️ Kubernetes Commands

### Deploy
```bash
kubectl apply -f k8s/namespace.yaml
kubectl apply -f k8s/configmap.yaml
kubectl apply -f k8s/secret.yaml
kubectl apply -f k8s/deployment.yaml
kubectl apply -f k8s/service.yaml
```

### Check Status
```bash
kubectl get pods -n mosquitto
kubectl get svc -n mosquitto
kubectl describe pod -n mosquitto <pod-name>
```

### View Logs
```bash
kubectl logs -f deployment/mosquitto -n mosquitto
```

### Port Forward
```bash
kubectl port-forward svc/mosquitto 1883:1883 -n mosquitto
```

### Delete Deployment
```bash
kubectl delete -f k8s/
```

## 📋 Environment Variables

### Docker Compose / Docker Run
```bash
MOSQUITTO_ADMIN_PASSWORD=your_password
MOSQUITTO_ADDITIONAL_USERS=user1:pass1,user2:pass2
```

### Example
```bash
docker run -e MOSQUITTO_ADMIN_PASSWORD=secure123 -e MOSQUITTO_ADDITIONAL_USERS="tracker:track123,dash:dash123" ...
```

## 🔒 SSL/TLS Configuration

### Generate Self-Signed Certificate
```bash
openssl req -x509 -newkey rsa:4096 -keyout server.key -out server.crt -days 365 -nodes
```

### Place in mosquitto/certs/
```
mosquitto/certs/
├── ca.crt
├── server.crt
└── server.key
```

### Test SSL Connection
```bash
openssl s_client -connect localhost:8883 -CAfile mosquitto/certs/ca.crt
```

## 📝 Configuration Changes

### Edit mosquitto.conf
```bash
# Edit locally
nano mosquitto/mosquitto.conf

# Changes apply on next container restart
docker-compose restart mosquitto
```

### Common Configuration Options
```ini
# Ports
listener 1883          # Standard MQTT
listener 8883          # MQTT with TLS
listener 9001          # WebSocket

# Security
password_file /mosquitto/config/passwords.txt
allow_anonymous false

# Persistence
persistence true
persistence_location /mosquitto/data/

# Logging
log_dest stdout
log_type all
```

## 🆘 Common Issues & Solutions

### Issue: Connection refused
```bash
# Solution: Check if broker is running
docker ps | grep mosquitto
docker logs mosquitto-broker
```

### Issue: Authentication failed
```bash
# Solution: Verify password file
docker exec mosquitto-broker cat /mosquitto/config/passwords.txt
./mosquitto/manage-passwords.sh list
```

### Issue: Port already in use
```bash
# Solution: Kill process using port
lsof -i :1883
kill -9 <PID>
# Or use different port in docker-compose.yml
```

### Issue: Permissions denied
```bash
# Solution: Fix file permissions
docker exec mosquitto-broker chmod 644 /mosquitto/config/passwords.txt
```

## 🔄 Git Workflow

### Commit Changes
```bash
git add mosquitto/ .github/workflows/ k8s/
git commit -m "Update Mosquitto configuration"
git push origin main
```

### GitHub Actions Triggers
- Changes to `mosquitto/` directory
- Changes to `.github/workflows/deploy-mosquitto.yml`
- Manual trigger via `workflow_dispatch`

## 📊 Monitoring Commands

### Real-time Broker Stats
```bash
# Subscribe to broker stats
mosquitto_sub -h localhost -u admin -P changeme -t '$SYS/broker/#' -v

# Memory usage
docker stats mosquitto-broker

# Process info
docker top mosquitto-broker
```

### Check Connected Clients
```bash
mosquitto_sub -h localhost -u admin -P changeme -t '$SYS/broker/clients/total' -W 1
```

---

**Save this file for quick reference!** 📌
