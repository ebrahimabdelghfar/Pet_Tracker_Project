#include <WiFi.h>
#include <PubSubClient.h>
#include <topic.h>
#include <bluetooth_lib.h>
#include <GY_MAX_30100.h>
#include <Ublox_6M_lib.h>
#include <local_storage_lib.h>
#include <Battery_Voltage_Sensor.h>
#include "memory_key.h"
#include "wifi_utils.h"
#include "gsm_utils.h"
#include "factory_reset_lib.h"
#include "web_server_lib.h"
#define CHECK_WIFI_INTERVAL 60000
#define PUBLISH_INTERVAL 2000
#define VOLTAGE_SENSOR_PIN A0 // Pin connected to the voltage sensor
/*variables to hold WiFi and MQTT credentials*/
/*default parameters*/
String apn  = "YourAPN";
String gprsUser = "";
String gprsPass = "";
String ssid = "asu12";                        // Replace with your WiFi SSID
String password = "12345678";                 // Replace with your WiFi password
String mqtt_server = "2.tcp.ngrok.io";        // Replace with your MQTT server address
String mqtt_username = "hima";                // Replace with your MQTT username
String mqtt_password = "1234";                // Replace with your MQTT password
String bluetooth_name = "ebrahim abdelghfar"; // Replace with your Bluetooth name
String pet_name = "PET";                      // Replace with your pet's name
/*variables to hold pet data*/
float pet_temperature = 0.0;        // pet temperature value
float pet_battery_percentage = 0.0; // pet battery percentage value
int32_t pet_spo2 = 0;               // pet SpO2 value
int32_t pet_heart_rate = 0;         // pet heart rate value
double pet_gps_latitude = 0.0;      // pet GPS latitude value
double pet_gps_longitude = 0.0;     // pet GPS longitude value
bool wifi_mode = true;              // true for WiFi mode, false for GSM mode
bool ssid_changed = false;          // flag to indicate if SSID has changed
bool password_changed = false;      // flag to indicate if password has changed
bool gsm_changed = false; // flag to indicate if GSM mode has changed
bool is_factory_reset_requested = false; // flag to indicate if factory reset is requested
WiFiClient espClient;
PubSubClient client(espClient);

void mqtt_callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  String message = "";
  for (int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }

  if (strcmp(topic, (pet_name + PET_NAME_TOPIC).c_str()) == 0)
  {
    pet_name = message;
    saveString(PET_NAME_KEY, pet_name);
    ESP.restart(); // Restart to apply the new pet name
  }
  else if (strcmp(topic, (pet_name + WIFI_SSID_TOPIC).c_str()) == 0)
  {
    ssid = message;
    saveString(SSID_KEY, ssid);
  }
  else if (strcmp(topic, (pet_name + WIFI_PASSWORD_TOPIC).c_str()) == 0)
  {
    password = message;
    saveString(WIFI_PASSWORD_KEY, password);
  }
  else if (strcmp(topic, (pet_name + MQTT_SERVER_TOPIC).c_str()) == 0)
  {
    mqtt_server = message;
    saveString(MQTT_SERVER_NAME_KEY, mqtt_server);
  }
  else if (strcmp(topic, (pet_name + MQTT_USERNAME_TOPIC).c_str()) == 0)
  {
    mqtt_username = message;
    saveString(MQTT_USER_KEY, mqtt_username);
  }
  else if (strcmp(topic, (pet_name + MQTT_PASSWORD_TOPIC).c_str()) == 0)
  {
    mqtt_password = message;
    saveString(MQTT_PASSWORD_KEY, mqtt_password);
  }
  else if (strcmp(topic, (pet_name + BLUETOOTH_NAME_TOPIC).c_str()) == 0)
  {
    bluetooth_name = message;
    saveString(BLUETOOTH_NAME_KEY, bluetooth_name);
  }
  else if (strcmp(topic, (pet_name + GSM_APN_TOPIC).c_str()) == 0)
  {
    apn = message;
    gsm_changed = true; // Set flag to indicate GSM mode change
    saveString(GSM_APN_KEY, apn);
  }
  else if (strcmp(topic, (pet_name + GSM_USER_TOPIC).c_str()) == 0)
  {
    gprsUser = message;
    saveString(GSM_USER_KEY, gprsUser);
  }
  else if (strcmp(topic, (pet_name + GSM_PASSWORD_TOPIC).c_str()) == 0)
  {
    gprsPass = message;
    saveString(GSM_PASSWORD_KEY, gprsPass);
  }
}

void reconnect()
{
  while (!client.connected() && WiFi.status() == WL_CONNECTED)
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_username.c_str(), mqtt_password.c_str()))
    {
      client.subscribe((pet_name + PET_NAME_TOPIC).c_str());
      client.subscribe((pet_name + WIFI_SSID_TOPIC).c_str());
      client.subscribe((pet_name + WIFI_PASSWORD_TOPIC).c_str());
      client.subscribe((pet_name + MQTT_SERVER_TOPIC).c_str());
      client.subscribe((pet_name + MQTT_USERNAME_TOPIC).c_str());
      client.subscribe((pet_name + MQTT_PASSWORD_TOPIC).c_str());
      client.subscribe((pet_name + BLUETOOTH_NAME_TOPIC).c_str());
      client.subscribe((pet_name + GSM_APN_TOPIC).c_str());
      client.subscribe((pet_name + GSM_USER_TOPIC).c_str());
      client.subscribe((pet_name + GSM_PASSWORD_TOPIC).c_str());
    }
    else
    {
      delay(100);
    }
  }
}

void setup()
{
  // if (!IsMemoryHaveBeenOpened())
  // {
  //   /*save default factory system values in the memory*/
  //   saveString(PET_NAME_KEY, pet_name);
  //   saveString(SSID_KEY, ssid);
  //   saveString(WIFI_PASSWORD_KEY, password);
  //   saveString(MQTT_SERVER_NAME_KEY, mqtt_server);
  //   saveString(MQTT_USER_KEY, mqtt_username);
  //   saveString(MQTT_PASSWORD_KEY, mqtt_password);
  //   saveString(BLUETOOTH_NAME_KEY, mqtt_password);
  // }
  // else
  // {
  //   /*Load the last saved parameters*/
  //   ssid = getString(SSID_KEY);
  //   password = getString(WIFI_PASSWORD_KEY);
  //   mqtt_server = getString(MQTT_SERVER_NAME_KEY);
  //   mqtt_username = getString(MQTT_USER_KEY);
  //   mqtt_password = getString(MQTT_PASSWORD_KEY);
  //   bluetooth_name = getString(BLUETOOTH_NAME_KEY);
  //   pet_name = getString(PET_NAME_KEY);
  //   wifi_mode = getBool(SWITCH_MODE_KEY);
  // }
  Serial.begin(115200);
  wifi_mode = getBool(SWITCH_MODE_KEY);
  if(wifi_mode){
    setup_wifi(ssid.c_str(), password.c_str());
    client.setServer(mqtt_server.c_str(), MQTT_PORT);
    client.setCallback(mqtt_callback);
  }
  else{
    setupGSM(apn,gprsUser,gprsPass);
  }

  heartRateSetup(); // Initialize heart rate sensor
  setupVoltageSensor(VOLTAGE_SENSOR_PIN); // Initialize voltage sensor
  initFactoryResetFeature(); // Initialize factory reset feature
  // setupUblox6M(UBLOX_6M_TX_PIN, UBLOX_6M_RX_PIN); // Initialize GPS module
  //check if factory reset is requested
  is_factory_reset_requested = getBool(FACTORY_RESET_KEY);
  is_factory_reset_requested ? startWebServer() : void(); // Start web server if factory reset is requested
}

void loop()
{
  is_factory_reset_requested ? waitUntillSetupDevice() : void(); // this will not contiue the code and make the config portal open automatically

  if (!is_factory_reset_requested) // If factory reset is requested
  {
    Serial.println("Factory reset requested. Restarting device...");
    delay(1000);
    ESP.restart(); // Restart the device to apply factory reset
    ssid_changed &&password_changed ? (wifi_mode = true, saveBool(SWITCH_MODE_KEY, wifi_mode), ESP.restart()) : void(); // Restart if SSID or password has changed to connect to the new WiFi network
    gsm_changed? (wifi_mode? (void()): ESP.restart()): void(); // If GSM mode has changed and currently in WiFi mode, do nothing
    periodicCheckForWifiConnection(); // Check if WiFi is connected
    periodicCheckForAvailableWifiNetworks(ssid.c_str()); // Check for available WiFi networks periodically if previously was connected to gsm
    if (getBool(SWITCH_MODE_KEY)) // If WiFi mode is enabled
    {
      !client.connected() ? reconnect() : void(); // Reconnect to MQTT server if not connected
      client.loop();
      // To always update heart data

      // Publish pet data periodically
      static unsigned long lastPublish = 0;
      if (WiFi.status() == WL_CONNECTED && millis() - lastPublish > PUBLISH_INTERVAL)
      {
        lastPublish = millis();
        /*publish reading with wifi*/
        unsigned long lastHeartRateUpdate = millis();
        unsigned long currentHeartMillis = millis();
        while (currentHeartMillis - lastHeartRateUpdate < 5000)
        {
          updateHeartRateOnly(); // Update heart rate and SpO2 data
          currentHeartMillis = millis();
        }
        pet_temperature = getTemp();
        pet_heart_rate = getHeartRate();
        pet_battery_percentage = readPercentage();

        client.publish((pet_name + TEMPERATURE_TOPIC).c_str(), String(pet_temperature).c_str());
        client.publish((pet_name + VOLTAGE_PERCENTAGE_TOPIC).c_str(), String(pet_battery_percentage).c_str());
        client.publish((pet_name + HEART_RATE_TOPIC).c_str(), String(pet_heart_rate).c_str());
      }
    }
  }
}
