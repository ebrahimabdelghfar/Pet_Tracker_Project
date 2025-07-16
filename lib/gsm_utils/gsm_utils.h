#ifndef GSM_UTILS_H
#define GSM_UTILS_H
#include "gsm_config.h"
#include <TinyGsmClient.h>
#include <SoftwareSerial.h>

/**
 * @brief Initializes the GSM module with the specified parameters.
 * 
 * @param apn The Access Point Name for the GSM network.
 * @param user The username for the GSM network (if required).
 * @param pass The password for the GSM network (if required).
 */
void setupGSM(String apn, String user, String pass);

/**
 * @brief Retrieves the modem information.
 * 
 * @return A String containing the modem information.
 */
String getModemInfo();
#endif