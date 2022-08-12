#include "Arduino.h"
#include "types.h"
#include "config.h"
#include "Battery.h"

float BATTERY_VOLTAGE = 0;
bool BATTERY_VOLTAGE_LOW = false;

void Battery_init() {
  // Should not use the Arduino internal 1.1v reference since it will mess up analogRead on other pins, also this project utilises a 5v step up module.
  // dummy reading to saturate analog pin so next reading can be correct
  analogRead(BATTERY_VOLTAGE_PIN); 
}

void Battery_read() {
    float reading = analogRead(BATTERY_VOLTAGE_PIN);
    BATTERY_VOLTAGE = reading * 5 / 1023;
    BATTERY_VOLTAGE_LOW = BATTERY_VOLTAGE < LOW_VOLTAGE_THRESHOLD;
}