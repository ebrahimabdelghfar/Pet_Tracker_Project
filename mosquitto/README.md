# Mosquitto MQTT Broker Deployment

This directory contains the configuration and GitHub Actions workflow for deploying a Mosquitto MQTT broker with custom configuration and password authentication.

## Structure

```
mosquitto/
├── Dockerfile              # Docker image for Mosquitto
├── mosquitto.conf          # Mosquitto configuration
├── password.conf           # Password configuration documentation
├── entrypoint.sh          # Entrypoint script for dynamic password setup
└── README                 # This file
```

## Features

- ✅ MQTT protocol on port 1883
- ✅ MQTT over TLS on port 8883
- ✅ WebSocket support on port 9001
- ✅ Password-based authentication
- ✅ Persistent storage
- ✅ Health checks
- ✅ Docker containerization
- ✅ GitHub Actions CI/CD deployment

## GitHub Actions Workflow

The workflow file (`.github/workflows/deploy-mosquitto.yml`) provides:

1. **Build**: Creates a Docker image for the Mosquitto broker
2. **Push**: Pushes the image to GitHub Container Registry (ghcr.io)
3. **Deploy**: Optionally deploys to a server via SSH (requires secrets)

### GitHub Secrets Required

To enable deployment, configure these secrets in your repository settings:

- `DEPLOY_KEY`: SSH private key for server access
- `DEPLOY_HOST`: Hostname or IP of the deployment server
- `DEPLOY_USER`: SSH username for deployment

### Environment Variables

- `MOSQUITTO_ADMIN_PASSWORD`: Password for the admin user (default: `changeme`)
- `MOSQUITTO_ADDITIONAL_USERS`: Comma-separated list of additional users in format `user1:pass1,user2:pass2`

## Local Testing

### Using Docker Compose

```bash
cd /home/ebrahim/Pet_Tracker_Project
docker-compose up -d
```

This will:
- Build the Mosquitto Docker image
- Start the broker with configured ports and volumes
- Set up persistent storage for data and logs

### Verify the Broker is Running

```bash
# Check logs
docker logs mosquitto-broker

# Test MQTT connection
docker exec mosquitto-broker mosquitto_sub -h localhost -p 1883 -t '$SYS/broker/clients/total'

# Test with credentials
mosquitto_sub -h localhost -p 1883 -u admin -P your_secure_password -t 'test/topic'
```

## Production Deployment

### 1. Configure GitHub Secrets

Go to your repository → Settings → Secrets and create:

```
DEPLOY_KEY: (your-private-ssh-key)
DEPLOY_HOST: your.server.com
DEPLOY_USER: deploy_user
```

### 2. Update Environment Variables

Modify the deployment section in `.github/workflows/deploy-mosquitto.yml` to set:

```yaml
environment:
  MOSQUITTO_ADMIN_PASSWORD: ${{ secrets.MQTT_ADMIN_PASSWORD }}
  MOSQUITTO_ADDITIONAL_USERS: ${{ secrets.MQTT_USERS }}
```

### 3. Prepare Server

On your deployment server:

```bash
# Create directories
mkdir -p /etc/mosquitto/config
mkdir -p /mosquitto/data
mkdir -p /mosquitto/log

# Create password file (optional - will be created by container)
touch /mosquitto/config/passwords.txt

# Set permissions
chmod 755 /mosquitto/data
chmod 755 /mosquitto/log
```

### 4. Configure TLS/SSL (Optional but Recommended)

Place certificates in `/mosquitto/config/certs/`:

```bash
/mosquitto/config/certs/
├── ca.crt          # CA certificate
├── server.crt      # Server certificate
└── server.key      # Server private key
```

Or generate self-signed certificates:

```bash
openssl req -x509 -newkey rsa:4096 -keyout server.key \
  -out server.crt -days 365 -nodes \
  -subj "/CN=mosquitto.example.com"
```

## Configuration Options

### mosquitto.conf

Key settings you can customize:

- `listener`: Port and protocol configuration
- `max_connections`: Maximum concurrent connections
- `max_queued_messages`: Maximum queued messages per client
- `message_size_limit`: Maximum message size (0 = unlimited)
- `max_inflight_messages`: Maximum unacked messages
- `log_*`: Logging configuration

See the [official documentation](https://mosquitto.org/man/mosquitto-conf-5.html) for all options.

## Adding Users

To add new users to the password file:

### In Docker

```bash
docker exec mosquitto-broker mosquitto_passwd /mosquitto/config/passwords.txt newuser
```

### On Host

```bash
mosquitto_passwd /mosquitto/config/passwords.txt newuser
```

## Troubleshooting

### Broker won't start
- Check logs: `docker logs mosquitto-broker`
- Verify ports aren't in use: `netstat -tlnp | grep 1883`
- Check configuration syntax: `mosquitto -c mosquitto.conf -t`

### Authentication failing
- Verify password file exists and is readable
- Check user is in passwords.txt: `cat /mosquitto/config/passwords.txt`
- Ensure `allow_anonymous false` is set in mosquitto.conf

### TLS connection issues
- Verify certificate files exist and permissions are correct
- Test with: `openssl s_client -connect localhost:8883 -CAfile ca.crt`

## Integration with Pet Tracker

For the Pet Tracker project to connect:

```cpp
// MQTT connection example for Pet Tracker
#include <PubSubClient.h>

const char* mqtt_server = "your.mqtt.server";
const int mqtt_port = 1883;
const char* mqtt_user = "pet_tracker";
const char* mqtt_password = "pet_tracker_pass";

PubSubClient client(wifiClient);
client.setServer(mqtt_server, mqtt_port);

// Connect
client.connect("PetTracker", mqtt_user, mqtt_password);
```

## Security Best Practices

1. ✅ Change default passwords immediately
2. ✅ Use TLS/SSL for secure connections
3. ✅ Use strong, unique passwords for each user
4. ✅ Keep `allow_anonymous` set to `false`
5. ✅ Regularly update Mosquitto to latest version
6. ✅ Use firewall rules to restrict access
7. ✅ Rotate credentials periodically
8. ✅ Monitor logs for suspicious activity

## References

- [Mosquitto Documentation](https://mosquitto.org/)
- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [Docker Documentation](https://docs.docker.com/)
