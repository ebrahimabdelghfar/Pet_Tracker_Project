#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"
#include "GY_MAX_30100.h"
#include "heartRate.h"
MAX30105 particleSensor;

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
// Arduino Uno doesn't have enough SRAM to store 100 samples of IR led data and red led data in 32-bit format
// To solve this problem, 16-bit MSB of the sampled data will be truncated. Samples become 16-bit data.
uint16_t irBuffer[100];  // infrared LED sensor data
uint16_t redBuffer[100]; // red LED sensor data
#else
uint32_t irBuffer[50];  // infrared LED sensor data
uint32_t redBuffer[50]; // red LED sensor data
#endif

int32_t bufferLength;           // data length
int32_t spo2;                   // SPO2 value
int8_t validSPO2;               // indicator to show if the SPO2 calculation is valid
int32_t heartRate;              // heart rate value
int8_t validHeartRate;          // indicator to show if the heart rate calculation is valid
float temp = 0;        // sample rate in samples per second
bool finish_first_read = false; // flag to indicate if the first read is done

const byte RATE_SIZE = 5;  //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE];     //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0;  //Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;



void heartRateSetup()
{
    // Initialize sensor
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) // Use default I2C port, 400kHz speed
    {
        Serial.println(F("MAX30105 was not found. Please check wiring/power."));
        while (1);
    }
    particleSensor.setup();                     //Configure sensor with default settings
    particleSensor.setPulseAmplitudeRed(0x0A);  //Turn Red LED to low to indicate sensor is running
    particleSensor.setPulseAmplitudeGreen(0);   //Turn off Green LED
}

float getTemp(){
    return particleSensor.readTemperature(); // Read the temperature from the sensor
}

void updateHeartRateOnly(){
  for (int i = 0; i < 100; i++) { // Read 50 samples
    long irValue = particleSensor.getIR();
    if (checkForBeat(irValue) == true) {
      //We sensed a beat!
      long delta = millis() - lastBeat;
      lastBeat = millis();

      beatsPerMinute = 60 / (delta / 1000.0);

      if (beatsPerMinute < 255 && beatsPerMinute > 20) {
        rates[rateSpot++] = (byte)beatsPerMinute;  //Store this reading in the array
        rateSpot %= RATE_SIZE;                     //Wrap variable

        //Take average of readings
        beatAvg = 0;
        for (byte x = 0; x < RATE_SIZE; x++)
          beatAvg += rates[x];
        beatAvg /= RATE_SIZE;
      }
    }
    if (irValue < 50000) beatAvg = 0; // If no finger detected, reset average
  }
}

void updateHeartSpo2Sensor()
{
    bufferLength = 50; // buffer length of 100 stores 4 seconds of samples running at 25sps
    if(!finish_first_read){
        // read the first 100 samples, and determine the signal range
        for (byte i = 0; i < bufferLength; i++)
        {
            while (particleSensor.available() == false) // do we have new data?
                particleSensor.check();                 // Check the sensor for new data

            redBuffer[i] = particleSensor.getRed();
            irBuffer[i] = particleSensor.getIR();
            particleSensor.nextSample(); // We're finished with this sample so move to next sample
        }
        // calculate heart rate and SpO2 after first 100 samples (first 4 seconds of samples)
        maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
        finish_first_read = true; // Set the flag to true after the first read
    }
    // dumping the first 25 sets of samples in the memory and shift the last 75 sets of samples to the top
    for (byte i = 12; i < 50; i++)
    {
        redBuffer[i - 12] = redBuffer[i];
        irBuffer[i - 12] = irBuffer[i];
    }

    // take 25 sets of samples before calculating the heart rate.
    for (byte i = 25; i < 50; i++)
    {
        while (particleSensor.available() == false) particleSensor.check(); // Check the sensor for new data
        redBuffer[i] = particleSensor.getRed();
        irBuffer[i] = particleSensor.getIR();
        particleSensor.nextSample(); // We're finished with this sample so move to next sample
    }
    if(finish_first_read){
        maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
    }
    
}

int32_t getHeartRate()
{
    if (beatAvg < 0)
    {
        return 0; // Return 0 if heart rate is invalid
    }
    return beatAvg > 300 ? 0 : beatAvg;
}

int32_t getSpo2()
{
    if (spo2 < 20)
    {
        return 0; // Return 0 if SpO2 is invalid
    }
    return spo2;
}