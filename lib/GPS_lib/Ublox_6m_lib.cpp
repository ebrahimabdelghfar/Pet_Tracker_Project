#include "Ublox_6M_lib.h"

SoftwareSerial gpsSerial (UBLOX_6M_RX_PIN, UBLOX_6M_TX_PIN); // RX, TX
TinyGPSPlus gps;

double latitude = 0.0;
double longitude = 0.0;

void setupUblox6M() {
    // Initialize the SoftwareSerial with the specified RX and TX pins
    gpsSerial.begin(UBLOX_6M_BAUDRATE);
}


std::pair<double, double> getGpsLocation(){
  //loop untill gps is updated
  for (int i = 0; i < 10; i++) {
    while (gpsSerial.available() > 0) {
      gps.encode(gpsSerial.read());
    }
    if (gps.location.isUpdated()) {
      latitude = gps.location.lat();
      longitude = gps.location.lng();
    }
  }
    return {latitude, longitude};
}

