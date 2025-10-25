FROM eclipse-mosquitto:2.0.18

# Install required tools
RUN apk add --no-cache openssl bash

# Create necessary directories
RUN mkdir -p /mosquitto/config \
    && mkdir -p /mosquitto/data \
    && mkdir -p /mosquitto/log

# Copy custom configuration
COPY mosquitto.conf /mosquitto/config/mosquitto.conf
COPY password.conf /mosquitto/config/password.conf

# Generate password file with initial credentials
# Default user: admin, password: changeme (should be overridden in production)
RUN mosquitto_passwd -c /mosquitto/config/passwords.txt admin changeme

# Set proper permissions
RUN chown -R mosquitto:mosquitto /mosquitto/config \
    && chown -R mosquitto:mosquitto /mosquitto/data \
    && chown -R mosquitto:mosquitto /mosquitto/log \
    && chmod 755 /mosquitto/config \
    && chmod 755 /mosquitto/data \
    && chmod 755 /mosquitto/log

# Expose MQTT ports
EXPOSE 1883 8883 9001

# Health check
HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 \
    CMD mosquitto_sub -h localhost -p 1883 -t '$SYS/broker/clients/total' \
    || exit 1

# Run Mosquitto
CMD ["/usr/sbin/mosquitto", "-c", "/mosquitto/config/mosquitto.conf"]
