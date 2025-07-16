#include "gsm_utils.h"

SoftwareSerial SerialGSM(1,3); // RX, TX pins for GSM module
TinyGsm modem(SerialGSM);
String MODEM_INFO;
void setupGSM(String apn, String user, String pass) {
    SerialGSM.begin(115200); // Start GSM serial communication at 115200 baud rate
    modem.restart(); // Restart the modem to ensure it is ready
    delay(6000); // Wait for the GSM module to initialize

}

String getModemInfo() {
    return modem.getModemSerialNumber();
}