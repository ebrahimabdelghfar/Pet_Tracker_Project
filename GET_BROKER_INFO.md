# Getting Broker IP and Port from GitHub Deployment

## 🎯 Overview

Once your Mosquitto MQTT broker is deployed, you need to find:
- **IP Address**: Where the broker is running
- **Port**: Which port it's listening on

This guide shows you how to get this information for different deployment scenarios.

---

## 📊 Deployment Scenarios

### Scenario 1: GitHub Actions → Server Deployment (SSH)

If you configured SSH deployment secrets, the broker runs on your server.

#### Step 1: Check GitHub Actions Logs
1. Go to your repository on GitHub
2. Click **Actions** tab
3. Click the latest workflow run (`Deploy Mosquitto MQTT Broker`)
4. Click the **Deploy to server** step
5. Look for deployment output with your server details

#### Step 2: Find Server IP
```bash
# Your server IP is what you set in:
# Settings → Secrets → DEPLOY_HOST

# Example: your-server.com or 192.168.1.100
```

#### Step 3: Connect to Broker
```bash
# IP: Your DEPLOY_HOST value
# Port: 1883 (MQTT), 8883 (MQTT+TLS), 9001 (WebSocket)

mqtt_server = "your-server.com"  # or IP address
mqtt_port = 1883

# Example connection
mosquitto_sub -h your-server.com -p 1883 -u hima -P himaet23 -t 'test/topic'
```

---

### Scenario 2: Docker Compose (Local)

Running locally on your machine.

```bash
# IP: localhost or 127.0.0.1
# Ports: 1883, 8883, 9001

# Test connection
docker exec mosquitto-broker mosquitto_sub -h localhost -p 1883 -u hima -P himaet23 -t 'test/topic'

# Or directly
mosquitto_sub -h localhost -p 1883 -u hima -P himaet23 -t 'test/topic'
```

---

### Scenario 3: Kubernetes Deployment

If deployed to Kubernetes cluster.

#### Get Service IP and Port

```bash
# View the service
kubectl get svc -n mosquitto

# Example output:
# NAME               TYPE           CLUSTER-IP    EXTERNAL-IP     PORT(S)
# mosquitto          LoadBalancer   10.0.0.1      192.168.1.50    1883:30267/TCP

# The EXTERNAL-IP is your access IP
# Port: 1883 (or the exposed port)
```

#### Get Detailed Service Info

```bash
kubectl describe svc mosquitto -n mosquitto

# Look for:
# - Endpoints: IP:Port of the pod
# - LoadBalancer Ingress: External IP
# - Port mappings
```

#### Port Forward for Local Testing

```bash
# Create a tunnel to access the broker
kubectl port-forward svc/mosquitto 1883:1883 -n mosquitto

# Then connect to localhost:1883
mosquitto_sub -h localhost -p 1883 -u hima -P himaet23 -t 'test/topic'
```

---

## 📋 Quick Reference by Deployment Type

| Deployment | IP | Port | Command |
|------------|-----|------|---------|
| Local Docker | `localhost` | `1883` | `docker-compose up -d` |
| Server (SSH) | Your DEPLOY_HOST | `1883` | From GitHub Actions |
| Kubernetes | `kubectl get svc` | `1883` | `kubectl apply -f k8s/` |

---

## 🔍 How to Find IP Address for Each Setup

### For Server Deployment (SSH)

1. **From GitHub Secrets**:
   ```
   Go to: Repository → Settings → Secrets → Actions → DEPLOY_HOST
   ```

2. **SSH into server and find Docker IP**:
   ```bash
   ssh user@your-server.com
   docker inspect mosquitto | grep IPAddress
   ```

3. **Using docker commands on server**:
   ```bash
   docker ps  # Find mosquitto container
   docker inspect <container_id> | grep IPAddress
   ```

### For Kubernetes

```bash
# Method 1: Get LoadBalancer external IP
kubectl get svc -n mosquitto -o wide

# Method 2: Get pod IP (internal)
kubectl get pods -n mosquitto -o wide

# Method 3: Get detailed info
kubectl describe svc mosquitto -n mosquitto
```

### For Local Docker

```bash
# Docker internal IP
docker inspect mosquitto-broker | grep IPAddress

# Local access
localhost or 127.0.0.1

# Network access (if running on network)
hostname -I  # shows your machine IP
```

---

## 🚀 Test Connection Examples

### From Another Machine (Server Deployment)

```bash
# Replace with your actual server IP/hostname
MQTT_SERVER="192.168.1.100"
MQTT_PORT="1883"
MQTT_USER="hima"
MQTT_PASSWORD="himaet23"

mosquitto_sub -h $MQTT_SERVER -p $MQTT_PORT -u $MQTT_USER -P $MQTT_PASSWORD -t 'test/topic'
```

### From GitHub Actions Itself

You can add a test step to your workflow:

```yaml
- name: Test MQTT Connection
  run: |
    docker pull eclipse-mosquitto
    docker run --rm eclipse-mosquitto mosquitto_sub \
      -h ${{ secrets.DEPLOY_HOST }} \
      -p 1883 \
      -u hima \
      -P himaet23 \
      -t '$SYS/broker/clients/total' \
      -W 2
```

---

## 📝 Useful Commands

### SSH to Server and Check Broker

```bash
# SSH into your server
ssh $DEPLOY_USER@$DEPLOY_HOST

# Check if Docker container is running
docker ps | grep mosquitto

# View broker logs
docker logs mosquitto

# Get container IP
docker inspect mosquitto | grep IPAddress

# Test connection from server
docker exec mosquitto-broker mosquitto_sub -h localhost -p 1883 -u hima -P himaet23 -t 'test/topic'

# Get port mappings
docker port mosquitto
```

### Check Kubernetes Details

```bash
# List all resources in mosquitto namespace
kubectl get all -n mosquitto

# Get service details
kubectl describe svc/mosquitto -n mosquitto

# Get pod details
kubectl describe pod -n mosquitto

# Check logs
kubectl logs -n mosquitto -l app=mosquitto -f

# Get events (for debugging)
kubectl get events -n mosquitto
```

---

## 🐛 Troubleshooting

### Can't Connect to Broker

1. **Verify broker is running**:
   ```bash
   # For Docker
   docker ps | grep mosquitto
   
   # For Kubernetes
   kubectl get pods -n mosquitto
   ```

2. **Check if port is open**:
   ```bash
   # Test port connectivity
   telnet <ip> 1883
   ```

3. **Verify credentials**:
   ```bash
   docker exec mosquitto-broker cat /mosquitto/config/passwords.txt
   ```

4. **Check firewall**:
   ```bash
   # May need to allow port 1883
   sudo ufw allow 1883
   ```

### Wrong IP or Port

- Verify `DEPLOY_HOST` in GitHub secrets
- Verify service is exposed correctly in Kubernetes
- Check docker port mappings: `docker port mosquitto`

---

## 📱 Connection Details for Your Pet Tracker

Use these details to connect your Pet Tracker:

```cpp
// Replace with your actual values
const char* mqtt_server = "192.168.1.100";      // Get from above methods
const int mqtt_port = 1883;                     // Or 8883 for TLS, 9001 for WebSocket
const char* mqtt_user = "hima";                 // Your username
const char* mqtt_password = "himaet23";         // Your password

// Connect
PubSubClient client(wifiClient);
client.setServer(mqtt_server, mqtt_port);
client.connect("PetTracker", mqtt_user, mqtt_password);
```

---

## ✅ Quick Checklist

- [ ] Broker deployed and running
- [ ] Found the IP address (server, localhost, or K8s)
- [ ] Confirmed port is accessible (1883, 8883, or 9001)
- [ ] Tested connection with `mosquitto_sub`
- [ ] Updated Pet Tracker code with correct IP/port
- [ ] Successfully published/subscribed messages

---

## 🎯 Next Steps

1. **Identify your deployment method** (Server, Local, or Kubernetes)
2. **Find the IP** using the appropriate method above
3. **Test the connection** with the provided commands
4. **Update your Pet Tracker code** with the IP and port
5. **Start publishing MQTT messages** 🚀

---

*Created: October 25, 2025*  
*For: Pet Tracker Project MQTT Broker*
