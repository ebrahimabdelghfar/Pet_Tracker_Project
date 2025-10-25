# 📚 Mosquitto MQTT Deployment - Complete Documentation Index

## 📖 Start Here

### 1. **DEPLOYMENT_SUMMARY.md** 👈 **READ THIS FIRST**
   - Overview of what was created
   - 5-minute quick start
   - Security checklist
   - Next steps

### 2. **MQTT_SETUP.md**
   - Comprehensive setup guide
   - Local testing instructions
   - Production deployment guide
   - Kubernetes deployment
   - Pet Tracker integration examples
   - Troubleshooting section

### 3. **MQTT_COMMANDS_REFERENCE.md**
   - Quick command reference
   - User management
   - Testing commands
   - Client examples
   - Debugging tips

---

## 📁 Project Structure

```
Pet_Tracker_Project/
│
├── DEPLOYMENT_SUMMARY.md           ← Overview & quick start
├── MQTT_SETUP.md                   ← Detailed configuration
├── MQTT_COMMANDS_REFERENCE.md      ← Quick commands
│
├── .github/workflows/
│   ├── deploy-mosquitto.yml        ← Docker CI/CD pipeline
│   └── deploy-mosquitto-k8s.yml    ← Kubernetes pipeline
│
├── mosquitto/
│   ├── Dockerfile                  ← Custom MQTT image
│   ├── mosquitto.conf              ← Broker configuration
│   ├── entrypoint.sh               ← Startup script
│   ├── manage-passwords.sh         ← User management tool
│   ├── password.conf               ← Documentation
│   └── README.md                   ← Detailed guide
│
├── k8s/
│   ├── namespace.yaml
│   ├── configmap.yaml
│   ├── secret.yaml
│   ├── deployment.yaml
│   └── service.yaml
│
└── docker-compose.yml              ← Local development
```

---

## 🚀 Quick Commands

### Start Development Broker
```bash
docker-compose up -d
```

### Manage Users
```bash
./mosquitto/manage-passwords.sh
```

### Test Connection
```bash
docker exec mosquitto-broker mosquitto_sub -h localhost -p 1883 -u admin -P changeme -t '$SYS/broker/clients/total'
```

### Deploy to GitHub
```bash
git add .github/ mosquitto/ k8s/ docker-compose.yml
git commit -m "Add Mosquitto MQTT broker"
git push origin main
```

---

## 🔐 Default Credentials

**⚠️ CHANGE IMMEDIATELY IN PRODUCTION**

- **Username:** `admin`
- **Password:** `changeme`

---

## 📋 File Reference

### Workflows (.github/workflows/)

| File | Purpose |
|------|---------|
| `deploy-mosquitto.yml` | Docker build & push to GHCR, optional SSH deployment |
| `deploy-mosquitto-k8s.yml` | Kubernetes deployment automation |

### Mosquitto Config (mosquitto/)

| File | Purpose |
|------|---------|
| `Dockerfile` | Custom image with Mosquitto & tools |
| `mosquitto.conf` | MQTT broker configuration |
| `entrypoint.sh` | Dynamic password setup on startup |
| `manage-passwords.sh` | Interactive user management tool |
| `password.conf` | Password documentation |
| `README.md` | Detailed configuration reference |

### Kubernetes Manifests (k8s/)

| File | Purpose |
|------|---------|
| `namespace.yaml` | Create `mosquitto` namespace |
| `configmap.yaml` | MQTT configuration as code |
| `secret.yaml` | Credentials management |
| `deployment.yaml` | Pod specification & deployment |
| `service.yaml` | Service exposure (LoadBalancer + ClusterIP) |

### Configuration

| File | Purpose |
|------|---------|
| `docker-compose.yml` | Local development with volumes |
| `DEPLOYMENT_SUMMARY.md` | Quick overview & getting started |
| `MQTT_SETUP.md` | Comprehensive setup guide |
| `MQTT_COMMANDS_REFERENCE.md` | Quick command reference |

---

## 🎯 What Each File Does

### 🐳 Docker Files

**Dockerfile**
- Pulls official Eclipse Mosquitto image
- Installs additional tools
- Copies configuration files
- Sets up password file
- Configures health checks
- Exposes ports 1883, 8883, 9001

**mosquitto.conf**
- Configures MQTT listeners
- Sets up authentication
- Enables persistence
- Configures logging
- TLS/SSL support

**entrypoint.sh**
- Creates password file from environment variables
- Adds additional users
- Sets file permissions
- Starts Mosquitto broker

**manage-passwords.sh**
- Add/delete/change users
- List users
- Test connections
- Generate passwords
- Interactive or CLI mode

### 🔄 GitHub Actions

**deploy-mosquitto.yml**
1. Builds Docker image
2. Pushes to GitHub Container Registry
3. (Optional) Deploys via SSH to server

Triggers:
- Push to main (mosquitto/ or workflows/ changes)
- Manual via workflow_dispatch

**deploy-mosquitto-k8s.yml**
1. Builds Docker image
2. Deploys to Kubernetes cluster
3. Updates rolling deployment

Triggers:
- Push to main
- Manual via workflow_dispatch

### ☸️ Kubernetes Manifests

**namespace.yaml**
- Creates `mosquitto` namespace for isolation

**configmap.yaml**
- Stores mosquitto.conf as ConfigMap
- Non-sensitive configuration

**secret.yaml**
- Stores passwords securely
- Admin password
- User passwords

**deployment.yaml**
- Pod specification
- Resource limits
- Health checks
- Volume mounts
- Environment variables

**service.yaml**
- LoadBalancer service (external access)
- ClusterIP service (internal access)
- Port mappings

---

## 🛠️ Common Tasks

### Task 1: Local Development
```bash
cd /home/ebrahim/Pet_Tracker_Project
docker-compose up -d
# Broker runs on localhost:1883
```

### Task 2: Add New User
```bash
./mosquitto/manage-passwords.sh add newuser newpassword
# Or
docker exec mosquitto-broker mosquitto_passwd -b /mosquitto/config/passwords.txt newuser newpassword
```

### Task 3: Deploy with GitHub Actions
```bash
git push origin main
# Watch Actions tab in GitHub
```

### Task 4: Deploy to Kubernetes
```bash
kubectl apply -f k8s/
```

### Task 5: Test MQTT Connection
```bash
mosquitto_sub -h localhost -u admin -P changeme -t 'test/topic'
# In another terminal:
mosquitto_pub -h localhost -u admin -P changeme -t 'test/topic' -m 'Hello'
```

---

## 🔒 Security Checklist

- [ ] Changed default admin password
- [ ] Added strong user passwords
- [ ] Configured GitHub secrets for deployment
- [ ] Set up TLS certificates (optional)
- [ ] Restricted firewall access to MQTT ports
- [ ] Reviewed mosquitto.conf security settings
- [ ] Set `allow_anonymous: false`
- [ ] Enabled logging
- [ ] Configured password file permissions

---

## 📊 Architecture Overview

```
┌─────────────────────────────────────────────────────────┐
│  GitHub Repository                                      │
│  ┌─────────────────────────────────────────────────────┤
│  │ Push to main                                        │
│  │ ↓                                                   │
│  │ .github/workflows/deploy-mosquitto.yml            │
│  │ ├─ Build Dockerfile                               │
│  │ ├─ Push to GHCR                                   │
│  │ └─ Deploy to server (optional)                    │
│  └─────────────────────────────────────────────────────┤
└─────────────────────────────────────────────────────────┘
           │
           ├─ Option 1: Docker Deployment
           │  ├─ ghcr.io/.../mosquitto:latest
           │  └─ Docker/Docker-compose
           │
           └─ Option 2: Kubernetes Deployment
              ├─ kubectl apply -f k8s/
              └─ mosquitto namespace + deployment
```

---

## 🐛 Debugging

### Broker Not Starting
```bash
docker logs mosquitto-broker
docker-compose logs mosquitto
```

### Authentication Not Working
```bash
docker exec mosquitto-broker cat /mosquitto/config/passwords.txt
docker exec mosquitto-broker mosquitto_passwd -D /mosquitto/config/passwords.txt admin
docker exec mosquitto-broker mosquitto_passwd -b /mosquitto/config/passwords.txt admin newpass
```

### Connection Issues
```bash
telnet localhost 1883
netstat -tlnp | grep 1883
```

### Workflow Failures
- Check GitHub Actions logs
- Verify Docker syntax
- Confirm secrets configured
- Review error messages

---

## 🔗 Related Links

- **Mosquitto**: https://mosquitto.org/
- **MQTT Protocol**: https://mqtt.org/
- **Docker**: https://docs.docker.com/
- **Kubernetes**: https://kubernetes.io/
- **GitHub Actions**: https://docs.github.com/en/actions

---

## 📞 Support

1. **Check logs** - Most issues are visible in logs
2. **Review documentation** - MQTT_SETUP.md has detailed troubleshooting
3. **Test locally** - Use docker-compose for testing
4. **Search online** - Mosquitto has excellent documentation

---

## ✨ What's Included

✅ Production-ready Mosquitto configuration  
✅ GitHub Actions CI/CD pipeline  
✅ Docker containerization  
✅ Kubernetes manifests  
✅ Password management tools  
✅ Health checks  
✅ Persistence configuration  
✅ TLS/SSL support  
✅ WebSocket support  
✅ Comprehensive documentation  

---

## 📝 Quick Start (TL;DR)

1. **Local testing:**
   ```bash
   docker-compose up -d
   docker exec mosquitto-broker mosquitto_sub -h localhost -p 1883 -u admin -P changeme -t '$SYS/broker/clients/total'
   ```

2. **Add users:**
   ```bash
   ./mosquitto/manage-passwords.sh
   ```

3. **Deploy to GitHub:**
   ```bash
   git add .
   git commit -m "Add Mosquitto MQTT broker"
   git push origin main
   ```

4. **Monitor:**
   - GitHub Actions tab shows build progress
   - Check broker logs: `docker-compose logs -f`

---

**Ready to deploy your MQTT broker? Start with DEPLOYMENT_SUMMARY.md! 🚀**
