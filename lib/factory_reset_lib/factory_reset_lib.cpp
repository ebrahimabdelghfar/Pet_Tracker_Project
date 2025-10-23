#include "factory_reset_lib.h"
#include "factory_reset_config.h"
hw_timer_t *Timer0_Cfg = NULL;
int buttonCounter=  0;

void IRAM_ATTR Timer0_ISR()
{
  if (digitalRead(FACTORY_RESET_PIN)== (HIGH ^ INVERT_BUTTON_STATE)) {
    buttonCounter++; // Increment counter if button is pressed
  }
  else{
    buttonCounter=0; // Reset counter if button is not pressed
  }

  if (buttonCounter >= 5) { // If button pressed for 5 seconds
    saveBool(FACTORY_RESET_KEY, true); // Save factory reset key
    ESP.restart();

  }
}

void initFactoryResetFeature() {
  pinMode(FACTORY_RESET_PIN, INPUT_PULLUP); // Set GPIO 4 as input with pull-up resistor
  Timer0_Cfg = timerBegin(0, 80, true);
  timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR, true);
  timerAlarmWrite(Timer0_Cfg, 1000*1000, true); // Set alarm to trigger every second
  timerAlarmEnable(Timer0_Cfg);
}