#ifndef WEB_SERVER_LIB_H
#define WEB_SERVER_LIB_H

#include <WiFi.h>
#include <DNSServer.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
// --- Function Prototypes ---

/**
 * @brief Starts the web server and sets up the access point.
 */
void startWebServer();

/**
 * @brief Waits until the device is set up.
 */
void waitUntillSetupDevice();

#endif