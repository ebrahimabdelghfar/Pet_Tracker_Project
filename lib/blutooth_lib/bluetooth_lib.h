#ifndef BLUETOOTH_LIB
#define BLUETOOTH_LIB
#include <ArduinoBLE.h>
#include <Arduino.h>
bool searchBLEDevices(const char* targetName);
bool StabilizeBLESearchFlag(const char* targetName);
#endif