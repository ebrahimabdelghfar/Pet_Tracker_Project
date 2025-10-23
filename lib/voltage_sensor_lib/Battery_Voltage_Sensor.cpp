#include <Arduino.h>
#include "voltage_sensor_config.h" // Include the configuration header for resistor values
#include "Battery_Voltage_Sensor.h"
int pin; // Pin number for the voltage sensor
int averageReadings = 50; // Number of readings to average for more stable voltage reading
float averageVoltage = 0.0; // Variable to store the average voltage reading
void setupVoltageSensor(int connectedPin)
{
    pin = connectedPin; // Store the pin number
    pinMode(pin, INPUT); // Set the pin mode to INPUT
}

float readVoltage()
{
    for (int i = 0; i < averageReadings; i++)
    {
        // Read the voltage multiple times and average the results
        int rawValue = analogRead(pin);
        // Convert the raw ADC value to voltage
        // Assuming a 3.3V reference voltage and a 12-bit ADC resolution
        float voltage = ((rawValue / 4095.0) * REF_VOLTAGE); // ESP32 ADC resolution is 12-bit, so max value is 4095
        // Voltage divider calculation to get the actual battery voltage
        voltage = (voltage * ((RESISTOR_1 + RESISTOR_2) / RESISTOR_1))+CALIBRATION_OFFSET;
        averageVoltage += voltage; // Accumulate the voltage readings
        delay(10); // Small delay for stability
    }
    averageVoltage /= averageReadings; // Calculate the average voltage
    return averageVoltage;
}

float readPercentage()
{
    float voltage = readVoltage();
    // Assuming a battery voltage range of 3.0V to 4.2V for percentage calculation
    if (voltage < MIN_VOLTAGE)
        return 0.0; // Below minimum voltage
    if (voltage > MAX_VOLTAGE)
        return 100.0; // Above maximum voltage
    return ((voltage - MIN_VOLTAGE) / (MAX_VOLTAGE - MIN_VOLTAGE)) * 100.0;
}
