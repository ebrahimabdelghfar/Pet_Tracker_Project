#include "Ublox_6M_lib.h"

SoftwareSerial gpsSerial;
TinyGPSPlus gps;

float latitude = 0.0;
float longitude = 0.0;

void setupUblox6M(int RX_PIN, int TX_PIN) {
    // Initialize the SoftwareSerial with the specified RX and TX pins
    gpsSerial.begin(UBLOX_6M_BAUDRATE, SWSERIAL_8N1, RX_PIN, TX_PIN);
    // Optionally, you can set a timeout for the serial communication
    gpsSerial.setTimeout(1000); // Set timeout to 1000 milliseconds (1 second)
}


void updateGps(){
  while (gpsSerial.available() > 0){
    gps.encode(gpsSerial.read());
        latitude = gps.location.lat();
        longitude = gps.location.lng();
    }
}

double getLatitude() {
    return latitude;
}

double getLongitude() {
    return longitude;
}
