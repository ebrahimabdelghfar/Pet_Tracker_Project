#include <bluetooth_lib.h>
bool BLE_initialized = false;
#define NO_READING_CHECK 3
bool searchBLEDevices(const char* targetName)
{
  if (!BLE_initialized) {
    BLE.begin()?BLE_initialized = true:BLE_initialized = false;                // Initialize BLE
  }
  BLE.scan();                 // Start scanning for BLE devices
  delay(100);            // Allow some time for scanning to start
  for (int i = 0; i < 100; i++) // Scan for a limited time
  {
    BLEDevice peripheral = BLE.available();
    if (peripheral)
    {
      // Check if the discovered peripheral matches the target name
      if (strcmp(peripheral.localName().c_str(), targetName) == 0)
      {
        BLE.stopScan(); // Stop scanning if the target is found
        return true;    // Target device found
      }
    }
    else delay(10); // Wait for a short time before checking again
  }
  BLE.stopScan(); // Stop scanning if the target is found
  return false;   // Target device not found
}

bool StabilizeBLESearchFlag(const char* targetName)
{
  int trueCount = 0; // Initialize trueCount to 0
  for (int i = 0; i < NO_READING_CHECK; i++) {
    if(searchBLEDevices(targetName)) {
      trueCount++;
    }
  }
  return trueCount >= int(ceil(NO_READING_CHECK * 3 / 4)); // Return true if at least 3 out of 4 readings are true
}