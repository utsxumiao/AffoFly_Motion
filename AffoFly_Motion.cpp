#include "Arduino.h"
#include "types.h"
#include "config.h"
#include "printf.h"
#include "Controller.h"
#include "Gyro.h"
#include "Radio.h"
#include "Signal.h"
#include "Battery.h"
#include "LED.h"
#include "AffoFly_Motion.h"

ControlData RC_DATA;
uint16_t loopCount = 0;

const uint16_t relayDataFetchInterval = 2000; //500hz
uint32_t previousRelayDataFetchMicros = 0;
const uint16_t mpuDataFetchInterval = 4000; //250hz
uint32_t previousMpuDataFetchMicros = 0;
const uint16_t controllerDataFetchInterval = 5000; //200hz
uint32_t previousControllerDataFetchMicros = 0;
const uint16_t radioSendInterval = 2000; //500hz
uint32_t previousRadioSendMicros = 0;
const uint32_t ledRefreshInterval = 100000; //10hz
uint32_t previousLedRefreshMicros = 0;
const uint32_t performanceReportInterval = 1000000; //1hz
uint32_t previousPerformanceReportMicros = 0;
const uint32_t voltageCheckInterval = 1000000; //1hz
uint32_t previousVoltageCheckMicros = 0;

void setup() {
  ADCSRA = (ADCSRA & 0xf8) | 0x04;  // set 16 times division to make analogRead faster
#if defined(DEBUG) || defined(PERFORMANCE) || defined(PRINT_OUTPUT)
  Serial.begin(115200);
#endif
#ifdef DEBUG
  Serial.println("System started");
#endif
  pinInit();
  dataInit();
  Radio_init();
  LED_init();
  Battery_init();
  if (digitalRead(RELAY_ENABLE_PIN) == 1) {
    Gyro_init();
    Controller_calibrate();
  } else {
    Signal_input_init();
  }
  Signal_output_init();
#ifdef DEBUG
  Serial.println("System ready");
#endif
}

void loop() {
  uint32_t currentTime = micros();
  if (digitalRead(RELAY_ENABLE_PIN) == 0) {
    if (currentTime - previousRelayDataFetchMicros >= relayDataFetchInterval) {
      previousRelayDataFetchMicros = currentTime;
      Signal_input_read();
    }
  } else {
    if (currentTime - previousMpuDataFetchMicros >= mpuDataFetchInterval) {
      previousMpuDataFetchMicros = currentTime;
      Gyro_read();
    }

    if (currentTime - previousControllerDataFetchMicros >= controllerDataFetchInterval) {
      previousControllerDataFetchMicros = currentTime;
      Controller_read();
    }
  }
  
  if (currentTime - previousRadioSendMicros >= radioSendInterval) {
    previousRadioSendMicros = currentTime;
    Radio_output();
  }

  if (currentTime - previousLedRefreshMicros >= ledRefreshInterval) {
    previousLedRefreshMicros = currentTime;
    LED_refresh();
  }

  if (currentTime - previousVoltageCheckMicros >= voltageCheckInterval) {
    previousVoltageCheckMicros = currentTime;
    Battery_read();
  }
  
  Signal_output_write();

  if (currentTime - previousPerformanceReportMicros >= performanceReportInterval) {
    previousPerformanceReportMicros = currentTime;
    reportPerformance();
  }

  loopCount++;
}

void pinInit() {
#ifdef DEBUG
  Serial.print("Initialising PINs......");
#endif
  pinMode(MPU_INTERRUPT_PIN, INPUT);
  pinMode(RELAY_ENABLE_PIN, INPUT_PULLUP);
  pinMode(BATTERY_VOLTAGE_PIN, INPUT);
  pinMode(AUX1_PIN, INPUT_PULLUP);
  pinMode(AUX2_PIN, INPUT_PULLUP);
  pinMode(AUX3_PIN, INPUT_PULLUP);
  pinMode(AUX4_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
#ifdef DEBUG
  Serial.println("Done");
#endif
}

void dataInit() {
#ifdef DEBUG
  Serial.print("Initialising system data......");
#endif
  RC_DATA.Token = RADIO_SECURITY_TOKEN;
  RC_DATA.Throttle = 1000;
  RC_DATA.Yaw = 1500;
  RC_DATA.Pitch = 1500;
  RC_DATA.Roll = 1500;
  RC_DATA.Aux1 = 1000;
  RC_DATA.Aux2 = 1000;
  RC_DATA.Aux3 = 1000;
  RC_DATA.Aux4 = 1000;
#ifdef DEBUG
  Serial.println("Done");
#endif
}

void reportPerformance() {
#ifdef PERFORMANCE
  Serial.print("Signal: ");     Serial.print(SIGNAL_INPUT_COUNT);     Serial.print("    ");
  Serial.print("Gyro: ");       Serial.print(GYRO_READ_COUNT);        Serial.print("    ");
  Serial.print("Controller: "); Serial.print(CONTROLLER_READ_COUNT);  Serial.print("    ");
  Serial.print("Radio: ");      Serial.print(RADIO_PACKET_COUNT);     Serial.print("    ");
  Serial.print("Loop: ");       Serial.print(loopCount);              Serial.print("    ");
  Serial.print("Battery: ");    Serial.print(BATTERY_VOLTAGE);        Serial.print("    ");
  Serial.println("");
#endif
  SIGNAL_INPUT_COUNT = 0;
  GYRO_READ_COUNT = 0;
  CONTROLLER_READ_COUNT = 0;
  RADIO_PACKET_COUNT = 0;
  loopCount = 0;
}
