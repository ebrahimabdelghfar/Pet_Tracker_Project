#ifndef UBLOX_6M_LIB_H
#define UBLOX_6M_LIB_H

#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <gps_config.h>

void setupUblox6M(int RX_PIN, int TX_PIN);
void updateGps();
double getLatitude();
double getLongitude();
#endif
