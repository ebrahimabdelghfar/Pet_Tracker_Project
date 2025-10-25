#!/bin/bash
set -e

# Entrypoint script for Mosquitto with dynamic password configuration

# Default values
ADMIN_PASSWORD="${MOSQUITTO_ADMIN_PASSWORD:-changeme}"
ADDITIONAL_USERS="${MOSQUITTO_ADDITIONAL_USERS:-}"

# Create password file
echo "Creating Mosquitto password file..."
mosquitto_passwd -c -b /mosquitto/config/passwords.txt admin "$ADMIN_PASSWORD"

# Add additional users if specified (format: "user1:pass1,user2:pass2")
if [ -n "$ADDITIONAL_USERS" ]; then
    IFS=',' read -ra users <<< "$ADDITIONAL_USERS"
    for user_pair in "${users[@]}"; do
        IFS=':' read -r username password <<< "$user_pair"
        if [ -n "$username" ] && [ -n "$password" ]; then
            echo "Adding user: $username"
            mosquitto_passwd -b /mosquitto/config/passwords.txt "$username" "$password"
        fi
    done
fi

# Set proper permissions
chown mosquitto:mosquitto /mosquitto/config/passwords.txt
chmod 600 /mosquitto/config/passwords.txt

# Start Mosquitto
echo "Starting Mosquitto MQTT broker..."
exec /usr/sbin/mosquitto -c /mosquitto/config/mosquitto.conf
