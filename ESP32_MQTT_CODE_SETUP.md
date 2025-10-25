# Getting Broker IP for Your Pet Tracker ESP32/Arduino Code

## 🎯 Quick Answer

Your MQTT broker IP is: **Your DEPLOY_HOST value from GitHub secrets**

---

## 📍 Step-by-Step Guide to Get the IP

### Step 1: Find Your DEPLOY_HOST (Your Server IP)

**Option A: Check GitHub Secrets** (Recommended)
```
1. Go to: https://github.com/ebrahimabdelghfar/Pet_Tracker_Project
2. Click Settings (top right)
3. Click "Secrets and variables" on left sidebar
4. Click "Actions"
5. Look for DEPLOY_HOST secret
6. This is your broker IP/hostname
```

**Example:**
- If DEPLOY_HOST = `192.168.1.100` → Use `192.168.1.100`
- If DEPLOY_HOST = `your-server.com` → Use `your-server.com`

### Step 2: Verify the IP Works

Test from your computer:
```bash
# Replace with your actual IP
ping 192.168.1.100

# Or test MQTT connection
mosquitto_sub -h 192.168.1.100 -p 1883 -u hima -P himaet23 -t 'test/topic'
```

---

## 💻 Add to Your ESP32/Arduino Code

### Complete C++ Code Example

```cpp
#include <WiFi.h>
#include <PubSubClient.h>

// WiFi Configuration
const char* ssid = "Your_WiFi_SSID";
const char* wifi_password = "Your_WiFi_Password";

// MQTT Broker Configuration
const char* mqtt_server = "192.168.1.100";        // ← YOUR BROKER IP HERE
const int mqtt_port = 1883;                        // MQTT Port
const char* mqtt_user = "hima";                    // Username
const char* mqtt_password = "himaet23";            // Password
const char* client_id = "PetTracker";              // Device Name

// MQTT Topics
const char* location_topic = "pet/location";
const char* status_topic = "pet/status";
const char* heartbeat_topic = "pet/heartbeat";

// Create clients
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Function to connect to WiFi
void connectToWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, wifi_password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✓ WiFi Connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n✗ WiFi Connection Failed!");
  }
}

// Function to reconnect to MQTT
void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection... ");
    
    // Attempt to connect
    if (mqttClient.connect(client_id, mqtt_user, mqtt_password)) {
      Serial.println("✓ Connected to MQTT!");
      
      // Subscribe to control topics
      mqttClient.subscribe("pet/commands");
      
    } else {
      Serial.print("✗ Failed (rc=");
      Serial.print(mqttClient.state());
      Serial.println(") - Retrying in 5 seconds...");
      
      delay(5000);
    }
  }
}

// MQTT Callback - Handle incoming messages
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.print("Message received on topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.println(message);
  
  // Handle different topics
  if (String(topic) == "pet/commands") {
    // Process commands
    handleCommands(message);
  }
}

// Handle incoming commands
void handleCommands(String command) {
  if (command == "status") {
    publishStatus();
  } else if (command == "location") {
    publishLocation();
  }
}

// Publish location data
void publishLocation() {
  // Replace with actual GPS data
  float latitude = 37.7749;    // Your GPS latitude
  float longitude = -122.4194;  // Your GPS longitude
  
  // Create JSON payload
  String payload = "{";
  payload += "\"lat\":" + String(latitude, 6) + ",";
  payload += "\"lon\":" + String(longitude, 6) + ",";
  payload += "\"timestamp\":" + String(millis());
  payload += "}";
  
  // Publish
  if (mqttClient.publish(location_topic, payload.c_str())) {
    Serial.println("✓ Location published!");
  } else {
    Serial.println("✗ Location publish failed!");
  }
}

// Publish status
void publishStatus() {
  String payload = "{";
  payload += "\"battery\":" + String(getBatteryLevel()) + ",";
  payload += "\"signal\":" + String(WiFi.RSSI()) + ",";
  payload += "\"status\":\"online\"";
  payload += "}";
  
  if (mqttClient.publish(status_topic, payload.c_str())) {
    Serial.println("✓ Status published!");
  } else {
    Serial.println("✗ Status publish failed!");
  }
}

// Publish heartbeat
void publishHeartbeat() {
  if (mqttClient.publish(heartbeat_topic, "alive")) {
    Serial.println("✓ Heartbeat sent!");
  }
}

// Get battery level (example)
int getBatteryLevel() {
  // Replace with your battery reading code
  return 85;  // 85%
}

// Setup
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n=== Pet Tracker Starting ===\n");
  
  // Connect to WiFi
  connectToWiFi();
  
  // Setup MQTT
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(mqttCallback);
  
  Serial.println("Setup complete!");
}

// Main loop
void loop() {
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }
  
  // Check MQTT connection
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  
  // Process MQTT messages
  mqttClient.loop();
  
  // Publish data every 30 seconds
  static unsigned long lastPublish = 0;
  if (millis() - lastPublish > 30000) {
    publishLocation();
    publishStatus();
    publishHeartbeat();
    lastPublish = millis();
  }
  
  delay(100);
}
```

---

## 🔧 Configuration Summary

Replace these values in your code:

| Item | Value | Where to Find |
|------|-------|---------------|
| `mqtt_server` | Your broker IP | DEPLOY_HOST in GitHub secrets |
| `mqtt_port` | `1883` | Always this value |
| `mqtt_user` | `hima` | Your username |
| `mqtt_password` | `himaet23` | Your password |
| `ssid` | Your WiFi name | Your home WiFi |
| `wifi_password` | Your WiFi pass | Your home WiFi |
| `latitude` | Your actual GPS lat | From GPS module |
| `longitude` | Your actual GPS lon | From GPS module |

---

## 📋 Quick Configuration Checklist

Before uploading to ESP32:

- [ ] Changed `mqtt_server` to your broker IP
- [ ] Changed `ssid` to your WiFi network name
- [ ] Changed `wifi_password` to your WiFi password
- [ ] Kept `mqtt_user` as `hima`
- [ ] Kept `mqtt_password` as `himaet23`
- [ ] Kept `mqtt_port` as `1883`
- [ ] Updated GPS coordinates with actual values

---

## 🧪 Testing After Upload

After uploading to your ESP32:

1. **Open Serial Monitor** (115200 baud):
   - Should see: `✓ WiFi Connected!`
   - Should see: `✓ Connected to MQTT!`

2. **Subscribe to topics** from your computer:
   ```bash
   mosquitto_sub -h 192.168.1.100 -p 1883 -u hima -P himaet23 -t 'pet/#'
   ```

3. **Should see messages**:
   ```
   pet/location
   pet/status
   pet/heartbeat
   ```

---

## 🔍 Finding Your IP - Different Scenarios

### Scenario 1: Using a Server (Most Common)

Your server IP is in GitHub secrets:
```
Repository → Settings → Secrets → DEPLOY_HOST
```

Example: `192.168.1.100` or `example.com`

### Scenario 2: Using Local Network

If running locally (docker-compose):
```bash
# Find your computer's IP
hostname -I

# Or use
ifconfig  # macOS/Linux
ipconfig  # Windows

# Use that IP in your code (not localhost!)
const char* mqtt_server = "192.168.1.50";  // Your local IP
```

### Scenario 3: Using Cloud Server

If deployed to cloud (AWS, Azure, DigitalOcean):
```
Use your cloud server's public IP or domain
Example: "52.15.123.45" or "mqtt.example.com"
```

---

## 🐛 Troubleshooting

### Issue: Can't Connect to MQTT

**Check 1: Is the IP correct?**
```bash
ping 192.168.1.100
```

**Check 2: Is the broker running?**
```bash
# SSH into your server
ssh user@192.168.1.100

# Check Docker
docker ps | grep mosquitto
```

**Check 3: Is the port open?**
```bash
telnet 192.168.1.100 1883
```

**Check 4: Are credentials correct?**
```bash
mosquitto_sub -h 192.168.1.100 -p 1883 -u hima -P himaet23 -t 'test/topic'
```

---

## 📝 Common Code Variations

### Using a Domain Name Instead of IP

```cpp
const char* mqtt_server = "mqtt.yourdomain.com";
const int mqtt_port = 1883;
```

### Using TLS/SSL (Port 8883)

```cpp
#include <WiFiClientSecure.h>

WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);

void setup() {
  wifiClient.setInsecure();  // For self-signed certificates
  mqttClient.setServer(mqtt_server, 8883);
}
```

### Using WebSocket (Port 9001)

For web-based clients, use WebSocket libraries instead.

---

## ✅ Final Code Template

```cpp
// MQTT Configuration - CHANGE THIS
const char* mqtt_server = "192.168.1.100";      // ← YOUR BROKER IP
const char* mqtt_user = "hima";                 // ← Keep this
const char* mqtt_password = "himaet23";         // ← Keep this

// WiFi Configuration - CHANGE THIS
const char* ssid = "YourWiFiName";              // ← Your WiFi
const char* wifi_password = "YourWiFiPassword"; // ← Your WiFi password
```

---

## 🚀 Next Steps

1. **Get your broker IP** from GitHub secrets (DEPLOY_HOST)
2. **Update the code** with your IP, WiFi name, and WiFi password
3. **Compile and upload** to ESP32
4. **Open Serial Monitor** to see connection status
5. **Verify messages** are being published to MQTT

---

## 📞 Support

If you have questions:
- Check the Serial Monitor output
- Verify firewall allows port 1883
- Make sure ESP32 can reach your server's network
- Test with mosquitto_sub first to verify broker works

**You're ready to deploy! 🎉**
