#ifndef GSM_UTILS_H
#define GSM_UTILS_H
#include "gsm_config.h"
#include <TinyGsmClient.h>
#include <SoftwareSerial.h>
#include <PubSubClient.h>
/**
 * @brief Initializes the GSM module with the specified parameters.
 * 
 * @param apn The Access Point Name for the GSM network.
 * @param user The username for the GSM network (if required).
 * @param pass The password for the GSM network (if required).
 */
void setupGSM(String apn);

/**
 * @brief Sets up the MQTT connection with the specified parameters.
 * 
 * @param broker The MQTT broker address.
 * @param port The MQTT broker port.
 * @param callback The callback function to handle incoming MQTT messages.
 */
void mqttGSMSetup(String broker, int port, MQTT_CALLBACK_SIGNATURE);

/**
 * @brief Connects to the MQTT broker.
 * 
 * @return true if the connection is successful, false otherwise.
 */
boolean mqttGSMConnect();

/**
 * @brief Retrieves the modem information.
 * 
 * @return A String containing the modem information.
 */
String getModemInfo();

/**
 * @brief Handles the MQTT loop for maintaining the connection and processing messages.
 */
void loopGSM();

/**
 * @brief Checks if the MQTT connection is established.
 * 
 * @return true if connected to the MQTT broker, false otherwise.
 */
boolean isMqttGSMConnected();

/**
 * @brief Publishes a message to the specified MQTT topic.
 * 
 * @param topic The MQTT topic to publish to.
 * @param payload The message payload to publish.
 * @return true if the message was published successfully, false otherwise.
 */
boolean mqttGsmPublish(const char* topic, const char* payload);
#endif