#!/bin/bash

# Entrypoint script for Mosquitto with dynamic password configuration

trap 'echo "Received signal, terminating..." && kill -TERM $PID && wait $PID' TERM INT

# Default values
ADMIN_PASSWORD="${MOSQUITTO_ADMIN_PASSWORD:-changeme}"
ADDITIONAL_USERS="${MOSQUITTO_ADDITIONAL_USERS:-}"

# Create password file
echo "Creating Mosquitto password file..."
mosquitto_passwd -c -b /mosquitto/config/passwords.txt admin "$ADMIN_PASSWORD" 2>/dev/null || true

# Add additional users if specified (format: "user1:pass1,user2:pass2")
if [ -n "$ADDITIONAL_USERS" ]; then
    IFS=',' read -ra users <<< "$ADDITIONAL_USERS"
    for user_pair in "${users[@]}"; do
        IFS=':' read -r username password <<< "$user_pair"
        if [ -n "$username" ] && [ -n "$password" ]; then
            echo "Adding user: $username"
            mosquitto_passwd -b /mosquitto/config/passwords.txt "$username" "$password" 2>/dev/null || true
        fi
    done
fi

# Set proper permissions (ignore errors if mosquitto user doesn't exist)
chown mosquitto:mosquitto /mosquitto/config/passwords.txt 2>/dev/null || true
chmod 600 /mosquitto/config/passwords.txt 2>/dev/null || true

# Start Mosquitto in background
echo "Starting Mosquitto MQTT broker..."
/usr/sbin/mosquitto -c /mosquitto/config/mosquitto.conf &
PID=$!

# Wait for the process (this keeps the container alive)
wait $PID
exit_code=$?

echo "Mosquitto exited with code: $exit_code"
exit $exit_code
