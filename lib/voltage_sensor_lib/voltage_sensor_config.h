#ifndef VOLTAGE_SENSOR_CONFIG_H
#define VOLTAGE_SENSOR_CONFIG_H

#define RESISTOR_1 2000 // 20k ohm resistor
#define RESISTOR_2 22000 // 22k ohm resistor
#define REF_VOLTAGE 3.3 // Reference voltage for ADC, typically 3.3V for ESP32
#define MIN_VOLTAGE 3.0 // Minimum battery voltage for percentage calculation
#define MAX_VOLTAGE 4.2 // Maximum battery voltage for percentage calculation
#define CALIBRATION_OFFSET 0.07 // Offset for voltage reading, can be adjusted based on calibration
#endif // VOLTAGE_SENSOR_CONFIG_H