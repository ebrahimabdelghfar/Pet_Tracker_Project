#include "wifi_utils.h"
#define SWITCH_MODE_KEY_WIFI "S_M" // Key to store the WiFi mode in local storage
bool wifi_mode_internal = true;                     // true for WiFi mode, false for GSM mode

bool setup_wifi(const char *wifi_SSID, const char *wifi_PASS)
{
  Serial.println();
  Serial.print("Connecting to WiFi...");
  WiFi.mode(WIFI_MODE_STA); // Set WiFi mode to Station
  WiFi.begin(wifi_SSID, wifi_PASS);
  WiFi.setTxPower(WIFI_POWER_8_5dBm);
  int prev_millis = millis();
  int curr_millis = millis();
  // wait for connection or timeout after 10 seconds
  while (WiFi.status() != WL_CONNECTED && (curr_millis - prev_millis) < 10000) curr_millis = millis();
  return WiFi.status() == WL_CONNECTED;
}

bool searchForWifiName(const char* wifi_name)
{
  int retryCount = 2;
  int n = WiFi.scanNetworks();
  for (int j = 0; j < n; j++)
  {
    if (WiFi.SSID(j) == String(wifi_name))
    {
      return true; // WiFi network found
    }
  }
  return false;
}

bool IsWiFiConnected()
{
  return WiFi.status() == WL_CONNECTED;
}

void periodicCheckForWifiConnection()
{
  unsigned long current_wifi_check_time = millis();
  static unsigned long last_wifi_check_time = 0;
  if (current_wifi_check_time - last_wifi_check_time > CHECK_WIFI_INTERVAL && getBool(SWITCH_MODE_KEY_WIFI))
  {
    Serial.println("Periodic check for WiFi connection...if already connected");
    last_wifi_check_time = current_wifi_check_time;
    IsWiFiConnected() ? (wifi_mode_internal = true, saveBool(SWITCH_MODE_KEY_WIFI, wifi_mode_internal)) : (wifi_mode_internal = false, saveBool(SWITCH_MODE_KEY_WIFI, wifi_mode_internal), ESP.restart()); // Check if WiFi is connected, if not, restart to reconfigure
  }
}

void periodicCheckForAvailableWifiNetworks(const char *wifi_SSID)
{
  wifi_mode_internal = getBool(SWITCH_MODE_KEY_WIFI); // Get the current WiFi mode from storage
  unsigned long current_reconnect_to_wifi = millis();
  static unsigned long last_reconnect_to_wifi = 0;

  if (current_reconnect_to_wifi - last_reconnect_to_wifi > CHECK_AVAILABILITY_WIFI_RECONNECT && !(wifi_mode_internal)) // If WiFi is not connected check if it avaliable
  {
    Serial.println("Checking for available WiFi networks...");
    last_reconnect_to_wifi = current_reconnect_to_wifi;
    if (searchForWifiName(wifi_SSID))
    {
      wifi_mode_internal = true;
      Serial.println("WiFi network found, switching to WiFi mode.");
      saveBool(SWITCH_MODE_KEY_WIFI, wifi_mode_internal);
      ESP.restart(); // Restart to switch to WiFi mode
    }
    else
    {
      Serial.println("WiFi network not found, staying in GSM mode.");
      wifi_mode_internal = false;
      saveBool(SWITCH_MODE_KEY_WIFI, wifi_mode_internal);
    }
  }
}