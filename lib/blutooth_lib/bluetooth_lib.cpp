#include "bluetooth_lib.h"
#include <BluetoothSerial.h>

#define BT_DISCOVER_TIME 10000

bool searchBluetoothDevices(String targetName) {
  BluetoothSerial SerialBT;
  SerialBT.begin("ESP32test");  //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!"); 
  bool deviceFound = false;
  Serial.println("Starting synchronous discovery... ");
  BTScanResults *pResults = SerialBT.discover(BT_DISCOVER_TIME);
  int count = pResults->getCount();
  for (int i = 0; i < count; i++) {
    BTAdvertisedDevice *device = pResults->getDevice(i);
    // Compare device name with target name
    if (device != nullptr) {
        const char *deviceName = device->getName().c_str();
        Serial.printf("Discovered device: %s\n", deviceName);
        if (deviceName != nullptr && strcmp(deviceName, targetName.c_str()) == 0) {
            deviceFound = true;
            break;
        }
      }
  }
  SerialBT.end();
  return deviceFound;
}