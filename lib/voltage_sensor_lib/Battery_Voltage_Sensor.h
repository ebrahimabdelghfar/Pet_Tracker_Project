#ifndef BATTERY_VOLTAGE_SENSOR_H
#define BATTERY_VOLTAGE_SENSOR_H
/*
* @file Battery_Voltage_Sensor.h
* @brief Header file for the battery voltage sensor library.
* 
* This library provides functions to set up a voltage sensor, read the voltage,
* and calculate the battery percentage based on the voltage readings.
*/

/*
* @brief Initializes the voltage sensor.
* @param connectedPin The pin to which the voltage sensor is connected.
*/
void setupVoltageSensor(int connectedPin);

/*
* @brief Reads the voltage from the sensor.
* @return The voltage reading from the sensor in volts.
*/
float readVoltage();

/*
* @brief Reads the battery percentage based on the voltage.
* @return The battery percentage (0 to 100).
*/
float readPercentage();

#endif