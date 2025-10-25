# Root Dockerfile for Railway - MQTT Broker
FROM eclipse-mosquitto:2.0.18

# Install bash for entrypoint
RUN apk add --no-cache bash

# Create necessary directories
RUN mkdir -p /mosquitto/config \
    && mkdir -p /mosquitto/data

# Copy custom configuration
COPY mosquitto/mosquitto.conf /mosquitto/config/mosquitto.conf
COPY mosquitto/entrypoint.sh /entrypoint.sh

# Make entrypoint executable
RUN chmod +x /entrypoint.sh

# Set proper permissions
RUN chown -R mosquitto:mosquitto /mosquitto/data \
    && chmod 755 /mosquitto/data

# Expose MQTT port
EXPOSE 1883

# Run entrypoint
ENTRYPOINT ["/entrypoint.sh"]
