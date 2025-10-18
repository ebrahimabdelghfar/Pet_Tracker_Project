#include "gsm_utils.h"

HardwareSerial SerialAT(2); // RX, TX
TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
PubSubClient mqtt_gsm(client);

String MODEM_INFO;
void setupGSM(String apn) {
    SerialAT.begin(115200); // Start GSM serial communication at 115200 baud rate
    Serial.println("GSM start.");
    modem.restart(); // Restart the modem to ensure it is ready
    Serial.println("Modem: " + modem.getModemInfo());
    Serial.println("Searching for telco provider.");
    while(!modem.waitForNetwork())
    {
    if (modem.isNetworkConnected()) break;
        modem.restart();
        delay(6000);
        Serial.print(".");
    }
    Serial.println("Connected to telco.");
    Serial.println("Signal Quality: " + String(modem.getSignalQuality()));

    Serial.println("Connecting to GPRS network.");
    while(!modem.gprsConnect(apn.c_str(), GPRS_USER, GPRS_PASS))
    {
        Serial.println("fail");
        delay(5000);
    }
    Serial.println("Connected to GPRS: " + String(apn));
}

void mqttGSMSetup(String broker, int port, MQTT_CALLBACK_SIGNATURE) {
    mqtt_gsm.setServer(broker.c_str(), port);
    mqtt_gsm.setCallback(callback);
}

boolean mqttGSMConnect()
{
  if(!mqtt_gsm.connect("GsmClientTest"))
  {
    Serial.print(".");
    return false;
  }
  Serial.println("Connected to broker.");
  mqtt_gsm.subscribe("tttttsss/in");
  return mqtt_gsm.connected();
}
String getModemInfo() {
    return modem.getModemSerialNumber();
}

void loopGSM() {
    mqtt_gsm.loop();
}

boolean isMqttGSMConnected() {
    return mqtt_gsm.connected();
}

boolean mqttGsmPublish(const char* topic, const char* payload) {
    return mqtt_gsm.publish(topic, payload);
}

void GsmReconnect(String apn) {
    modem.restart(); // Restart the modem to ensure it is ready
    Serial.println("Modem: " + modem.getModemInfo());
    Serial.println("Searching for telco provider.");
    while(!modem.waitForNetwork())
    {
    if (modem.isNetworkConnected()) break;
        modem.restart();
        delay(6000);
        Serial.print(".");
    }
    Serial.println("Connected to telco.");
    Serial.println("Signal Quality: " + String(modem.getSignalQuality()));

    Serial.println("Connecting to GPRS network.");
    while(!modem.gprsConnect(apn.c_str(), GPRS_USER, GPRS_PASS))
    {
        Serial.println("fail");
        delay(5000);
    }
    Serial.println("Connected to GPRS: " + String(apn));
}