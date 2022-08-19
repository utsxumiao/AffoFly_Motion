#include "Arduino.h"
#include "types.h"
#include "config.h"
#include "AffoFly_Motion.h"
#include "Controller.h"

uint16_t CONTROLLER_READ_COUNT = 0;
int16_t JOYSTICK_THROTTLE_OFFSET = 0;
int16_t JOYSTICK_YAW_OFFSET = 0;

void Controller_calibrate() {
#ifdef DEBUG
    Serial.print("Calibrating Joystick......");
#endif
    JOYSTICK_THROTTLE_OFFSET = getJoystickValue(JOYSTICK_THROTTLE_PIN, JOYS_CAL_SAMPLE_COUNT, JOYS_CAL_SAMPLE_ELIMI);
    JOYSTICK_YAW_OFFSET = getJoystickValue(JOYSTICK_YAW_PIN, JOYS_CAL_SAMPLE_COUNT, JOYS_CAL_SAMPLE_ELIMI) - 511;
#ifdef DEBUG
    Serial.print("Thr: ");  Serial.print(JOYSTICK_THROTTLE_OFFSET);   Serial.print("  ");
    Serial.print("Yaw: ");  Serial.print(JOYSTICK_YAW_OFFSET);        Serial.print("  ");
    Serial.println("Done");
#endif
}

void Controller_read() {
    int16_t throttle = getJoystickValue(JOYSTICK_THROTTLE_PIN, JOYS_VAL_SAMPLE_COUNT, JOYS_VAL_SAMPLE_ELIMI) - JOYSTICK_THROTTLE_OFFSET;
    throttle = constrain(throttle, 0, 1023);
    RC_DATA.Throttle = mapContollerValue(throttle, 0, 511, 1023, false);

    int16_t yaw = getJoystickValue(JOYSTICK_YAW_PIN, JOYS_VAL_SAMPLE_COUNT, JOYS_VAL_SAMPLE_ELIMI) - JOYSTICK_YAW_OFFSET;
    yaw = constrain(yaw, 0, 1023);
    if(yaw > 511 + YAW_DEADBAND) {
      yaw = map(yaw, 511 + YAW_DEADBAND, 1023, 1500, 1500 + YAW_LIMIT);
    } else if (yaw < 511 - YAW_DEADBAND) {
      yaw = map(yaw, 0, 511 - YAW_DEADBAND, 1500 - YAW_LIMIT, 1500);
    } else {
      yaw =  1500;
    }
    RC_DATA.Yaw = yaw; //3000 - yaw if reversed
    
    RC_DATA.Aux1 = mapContollerValue(digitalRead(AUX1_PIN) * 1023, 0, 511, 1023, true);
    RC_DATA.Aux2 = mapContollerValue(digitalRead(AUX2_PIN) * 1023, 0, 511, 1023, true);
    RC_DATA.Aux3 = mapContollerValue(digitalRead(AUX3_PIN) * 1023, 0, 511, 1023, true);
    RC_DATA.Aux4 = mapContollerValue(digitalRead(AUX4_PIN) * 1023, 0, 511, 1023, true);
    
    CONTROLLER_READ_COUNT++;
#ifdef PRINT_OUTPUT
    Serial.print("Throttle: ");     Serial.print(RC_DATA.Throttle);   Serial.print("    ");
    Serial.print("Yaw: ");          Serial.print(RC_DATA.Yaw);        Serial.print("    ");
    Serial.print("Aux1: ");         Serial.print(RC_DATA.Aux1);       Serial.print("    ");
    Serial.print("Aux2: ");         Serial.print(RC_DATA.Aux2);       Serial.print("    ");
    Serial.print("Aux3: ");         Serial.print(RC_DATA.Aux3);       Serial.print("    ");
    Serial.print("Aux4: ");         Serial.print(RC_DATA.Aux4);       Serial.print("    ");
    Serial.println("");
#endif
}

uint16_t getJoystickValue(uint8_t pin, uint8_t sampleCount, uint8_t eliminator) {
  uint16_t result = 0;
  uint16_t values[sampleCount];

  for (uint8_t i = 0; i < sampleCount; i++) {
    values[i] = analogRead(pin);
  }
  sort(values, sampleCount);
  //printArray(values, sampleCount);
  for (uint8_t i = eliminator; i < sampleCount - eliminator; i++) {
    result += values[i];
  }
  result /= sampleCount - (eliminator * 2);
  return result;
}

uint16_t mapContollerValue(uint16_t val, uint16_t lower, uint16_t middle, uint16_t upper, bool reverse) {
  val = constrain(val, lower, upper);
  if (val < middle) val = map(val, lower, middle, 1000, 1500);
  else val = map(val, middle, upper, 1500, 2000);
  return reverse ? 3000 - val : val;
}

void sort(int *a, int n) {
  for (int i = 1; i < n; ++i) {
    int j = a[i];
    int k;
    for (k = i - 1; (k >= 0) && (j < a[k]); k--)
    {
      a[k + 1] = a[k];
    }
    a[k + 1] = j;
  }
}

void printArray(int *a, int n) {
  for (int i = 0; i < n; i++)
  {
    Serial.print(a[i], DEC);
    Serial.print(' ');
  }
  Serial.println();
}
