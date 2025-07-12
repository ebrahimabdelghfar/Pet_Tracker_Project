#ifndef GY_MAX_30100_H
#define GY_MAX_30100_H
#include <Arduino.h>
#include "heart_rate_sensor_config.h"
/**
 * @brief Initializes the MAX30100 heart rate and SpO2 sensor.
 * 
 * Configures the sensor with predefined settings and prepares it for data acquisition.
 */
void heartRateSetup();

/**
 * @brief Updates the heart rate and SpO2 sensor readings.
 * 
 * Reads new samples from the sensor, processes the data, and updates the heart rate and SpO2 values.
 */
void updateHeartSensor();

/**
 * @brief Retrieves the most recently calculated heart rate value.
 * 
 * @return int32_t The current heart rate in beats per minute (BPM). Returns 0 if invalid.
 */
int32_t getHeartRate();

/**
 * @brief Retrieves the most recently calculated SpO2 value.
 * 
 * @return int32_t The current SpO2 percentage. Returns 0 if invalid.
 */
int32_t getSpo2();

/**
 * @brief Retrieves the most recently calculated temperature value.
 * 
 * @return float The current temperature in degrees Celsius. Returns 0 if invalid.
 */
float getTemp();

/**
 * @brief Updates the heart rate sensor data without SpO2 calculation.
 * 
 * This function is used to update only the heart rate data, ignoring SpO2 calculations.
 */
void updateHeartRateOnly();

#endif
