#ifndef HEART_RATE_SENSOR_CONFIG_H
#define HEART_RATE_SENSOR_CONFIG_H

#define MAX_BRIGHTNESS 255 // Maximum brightness for the LED
#define SAMPLING_AVERAGE 2 // Options: 1, 2, 4, 8, 16, 32
#define LED_MODE 3        // Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
#define SAMPLE_RATE 800   // Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
#define PULSE_WIDTH 411    // Options: 69, 118, 215, 411
#define ADC_RANGE 4096    // Options: 2048, 4096, 8192, 16384

#endif