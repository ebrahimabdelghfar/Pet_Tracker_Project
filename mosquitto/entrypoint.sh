#!/bin/bash
set -e

# Create password file
mosquitto_passwd -c -b /mosquitto/config/passwords.txt admin "$MOSQUITTO_ADMIN_PASSWORD" 2>/dev/null || true

# Add hima user
mosquitto_passwd -b /mosquitto/config/passwords.txt hima himaet23 2>/dev/null || true

# Set permissions
chown mosquitto:mosquitto /mosquitto/config/passwords.txt 2>/dev/null || true
chmod 600 /mosquitto/config/passwords.txt 2>/dev/null || true

# Start Mosquitto
exec /usr/sbin/mosquitto -c /mosquitto/config/mosquitto.conf
