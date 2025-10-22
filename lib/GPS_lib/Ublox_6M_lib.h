#ifndef UBLOX_6M_LIB_H
#define UBLOX_6M_LIB_H

#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <gps_config.h>

void setupUblox6M();
std::pair<double, double> getGpsLocation();
#endif
