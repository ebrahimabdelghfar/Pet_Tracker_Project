#ifndef WIFI_UTILS_H
#define WIFI_UTILS_H
#include <Arduino.h>
#include <WiFi.h>
#include "local_storage_lib.h"
#include "wifi_utils_config.h"
/**
    * @brief Connect to WiFi network
    * @param wifi_SSID The SSID of the WiFi network
    * @param wifi_PASS The password of the WiFi network
    * @return true if connected, false otherwise
*/
bool setup_wifi(const char *wifi_SSID, const char *wifi_PASS);

/**
    * @brief Search for a specific WiFi network
    * @param wifi_name The SSID of the WiFi network to search for
    * @return true if the WiFi network is found, false otherwise
*/
bool searchForWifiName(const char* wifi_name);

/**
    * @brief Check if the WiFi is connected
    * @return true if WiFi is connected, false otherwise
*/
bool IsWiFiConnected();

/**
 * @brief Periodically check if the WiFi is connected
 * If not connected, it will switch to GSM mode and restart the device
*/
void periodicCheckForWifiConnection();

/**
    * @brief Periodically if the wifi is avalibale again when it's on gsm mode
    * @param wifi_SSID The SSID of the WiFi network to check
    * If not connected, it will switch to GSM mode and restart the device
*/
void periodicCheckForAvailableWifiNetworks(const char *wifi_SSID);
#endif
