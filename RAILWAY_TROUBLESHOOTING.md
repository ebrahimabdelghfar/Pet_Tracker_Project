# Railway Deployment Troubleshooting

## Problem: Container Keeps Stopping

If you see "Stopping Container" in the logs, the Mosquitto service isn't starting properly.

### Quick Fix (5 minutes)

#### Option 1: Redeploy with Fresh Build

1. Go to Railway Dashboard → Mosquitto Service
2. Click "Settings" → "Redeploy"
3. Click "Redeploy Service"
4. Railway will rebuild the Docker image from scratch
5. Wait 2-3 minutes for deployment to complete

#### Option 2: Check Environment Variables

1. Go to Railway Dashboard → Mosquitto Service
2. Click "Variables" tab
3. Verify these variables exist:
   ```
   MOSQUITTO_ADMIN_PASSWORD = himaet23
   MOSQUITTO_ADDITIONAL_USERS = hima:himaet23
   ```
4. If missing, add them
5. Click "Redeploy Service"

#### Option 3: Manually Deploy Mosquitto

If Railway deployment isn't working, run locally or use Docker Hub:

```bash
# Build the image
docker build -t mosquitto-custom ./mosquitto

# Run locally to test
docker run -it --rm \
  -p 1883:1883 \
  -e MOSQUITTO_ADMIN_PASSWORD=himaet23 \
  -e MOSQUITTO_ADDITIONAL_USERS=hima:himaet23 \
  mosquitto-custom

# In another terminal, test
mosquitto_sub -h localhost -p 1883 -u hima -P himaet23 -t '$SYS/broker/clients/total'
```

---

## Debug: Check What's Wrong

### 1. Check Service Status

Railway Dashboard:
- Service shows **Red/Crashed** → Container not starting
- Service shows **Yellow/Building** → Still deploying
- Service shows **Green/Running** → Service is running

### 2. Check Logs for Errors

Look for these errors:

| Error | Solution |
|-------|----------|
| `HEALTHCHECK failed` | Mosquitto crashed, check config |
| `Port already in use` | Change port or stop other services |
| `Command not found` | Path issue in Docker |
| `Permission denied` | File permissions wrong |

### 3. Test Locally First

Before Railway:
```bash
# Build
docker build -t mosquitto-custom ./mosquitto

# Run
docker run -d --name mosquitto-test \
  -p 1883:1883 \
  -e MOSQUITTO_ADMIN_PASSWORD=himaet23 \
  -e MOSQUITTO_ADDITIONAL_USERS=hima:himaet23 \
  mosquitto-custom

# Check logs
docker logs mosquitto-test

# Test
mosquitto_sub -h localhost -p 1883 -u hima -P himaet23 -t '$SYS/broker/clients/total'

# Clean up
docker stop mosquitto-test && docker rm mosquitto-test
```

---

## Railway Configuration Files Added

We've created:
- `railway.json` - Railway-specific build configuration
- `Procfile` - Process definition for Railway

These help Railway understand how to run your container.

---

## Complete Reset Steps

If nothing works:

1. **Delete Railway Service**
   - Railway Dashboard → Mosquitto Service → Settings → Delete Service
   - Confirm deletion

2. **Create New Service**
   - New Project → Deploy from GitHub Repo
   - Select Pet_Tracker_Project
   - Click Deploy

3. **Configure Variables**
   - Variables tab → Add:
     ```
     MOSQUITTO_ADMIN_PASSWORD = himaet23
     MOSQUITTO_ADDITIONAL_USERS = hima:himaet23
     ```

4. **Wait for Deployment**
   - Monitor logs
   - Should see "Starting Mosquitto MQTT broker..."

5. **Test Connection**
   - Once "Running", try MQTT Explorer again

---

## If Still Not Working

Post the error message from logs and I'll debug further!

Key things to check:
- [ ] Are environment variables set in Railway?
- [ ] Is the Docker build completing successfully?
- [ ] Does the container start (check logs)?
- [ ] Can you connect locally?
