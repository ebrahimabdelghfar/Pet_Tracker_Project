#!/bin/bash

echo "════════════════════════════════════════════════════════"
echo "MOSQUITTO ENTRYPOINT DEBUG"
echo "════════════════════════════════════════════════════════"

echo ""
echo "Environment Variables:"
echo "  MOSQUITTO_ADMIN_PASSWORD=${MOSQUITTO_ADMIN_PASSWORD:-(not set)}"
echo "  MOSQUITTO_ADDITIONAL_USERS=${MOSQUITTO_ADDITIONAL_USERS:-(not set)}"
echo "  PORT=${PORT:-(not set)}"

echo ""
echo "Checking directories:"
ls -la /mosquitto/config/ 2>/dev/null || echo "  ❌ /mosquitto/config not found"
ls -la /mosquitto/data/ 2>/dev/null || echo "  ❌ /mosquitto/data not found"

echo ""
echo "Checking config file:"
if [ -f /mosquitto/config/mosquitto.conf ]; then
    echo "  ✅ mosquitto.conf exists"
    head -20 /mosquitto/config/mosquitto.conf
else
    echo "  ❌ mosquitto.conf NOT found"
fi

echo ""
echo "═══════════════════════════════════════════════════════"
echo "Starting Mosquitto..."
echo "═══════════════════════════════════════════════════════"

# Default values
ADMIN_PASSWORD="${MOSQUITTO_ADMIN_PASSWORD:-changeme}"
ADDITIONAL_USERS="${MOSQUITTO_ADDITIONAL_USERS:-}"

# Create password file
echo "Creating Mosquitto password file..."
mosquitto_passwd -c -b /mosquitto/config/passwords.txt admin "$ADMIN_PASSWORD" 2>&1 || echo "⚠️  Warning creating admin user"

# Add additional users if specified
if [ -n "$ADDITIONAL_USERS" ]; then
    echo "Processing additional users: $ADDITIONAL_USERS"
    IFS=',' read -ra users <<< "$ADDITIONAL_USERS"
    for user_pair in "${users[@]}"; do
        IFS=':' read -r username password <<< "$user_pair"
        if [ -n "$username" ] && [ -n "$password" ]; then
            echo "  Adding user: $username"
            mosquitto_passwd -b /mosquitto/config/passwords.txt "$username" "$password" 2>&1 || echo "    ⚠️  Warning adding $username"
        fi
    done
fi

# Set permissions
echo "Setting permissions..."
chown mosquitto:mosquitto /mosquitto/config/passwords.txt 2>&1 || echo "⚠️  Warning setting permissions"
chmod 600 /mosquitto/config/passwords.txt 2>&1 || echo "⚠️  Warning chmod"

echo ""
echo "Password file contents:"
cat /mosquitto/config/passwords.txt || echo "❌ Could not read password file"

echo ""
echo "════════════════════════════════════════════════════════"
echo "Starting Mosquitto MQTT broker..."
echo "════════════════════════════════════════════════════════"
echo ""

# Start Mosquitto in foreground
exec /usr/sbin/mosquitto -c /mosquitto/config/mosquitto.conf
