#include "Arduino.h"
#include "types.h"
#include "config.h"
#include "ppm.h"
#include "PPMEncoder.h"
#include "AffoFly_Motion.h"
#include "Signal.h"

uint16_t SIGNAL_INPUT_COUNT = 0;

void Signal_input_init() {
#ifdef DEBUG
    Serial.print("Initialising PPM input......");
#endif
    ppm.begin(RELAY_DATA_PIN, false);
#ifdef DEBUG
    Serial.println("Done");
#endif
}

void Signal_output_init() {
#ifdef DEBUG
    Serial.print("Initialising PPM output......");
#endif
    ppmEncoder.begin(PPM_OUTPUT_PIN);
#ifdef DEBUG
    Serial.println("Done");
#endif
}

void Signal_input_read() {
    ControlData data;
    RC_DATA.Token     = RADIO_SECURITY_TOKEN;
    RC_DATA.Throttle  = ppm.read_channel(3);
    RC_DATA.Yaw       = ppm.read_channel(4);
    RC_DATA.Pitch     = ppm.read_channel(2);
    RC_DATA.Roll      = ppm.read_channel(1);
    RC_DATA.Aux1      = ppm.read_channel(5);
    RC_DATA.Aux2      = ppm.read_channel(6);
    RC_DATA.Aux3      = ppm.read_channel(7);
    RC_DATA.Aux4      = ppm.read_channel(8);
    SIGNAL_INPUT_COUNT++;
#ifdef PRINT_OUTPUT
    Serial.print("THR: ");  Serial.print(RC_DATA.Throttle);   Serial.print("  ");
    Serial.print("YAW: ");  Serial.print(RC_DATA.Yaw);        Serial.print("  ");
    Serial.print("PIT: ");  Serial.print(RC_DATA.Pitch);      Serial.print("  ");
    Serial.print("ROL: ");  Serial.print(RC_DATA.Roll);       Serial.print("  ");
    Serial.print("AUX1: "); Serial.print(RC_DATA.Aux1);       Serial.print("  ");
    Serial.print("AUX2: "); Serial.print(RC_DATA.Aux2);       Serial.print("  ");
    Serial.print("AUX3: "); Serial.print(RC_DATA.Aux3);       Serial.print("  ");
    Serial.print("AUX4: "); Serial.print(RC_DATA.Aux4);       Serial.print("  ");
    Serial.println();
#endif
}

void Signal_output_write() {
  ppmEncoder.setChannel(2, RC_DATA.Throttle);
  ppmEncoder.setChannel(3, RC_DATA.Yaw);
  ppmEncoder.setChannel(1, RC_DATA.Pitch);
  ppmEncoder.setChannel(0, RC_DATA.Roll);
  ppmEncoder.setChannel(4, RC_DATA.Aux1);
  ppmEncoder.setChannel(5, RC_DATA.Aux2);
  ppmEncoder.setChannel(6, RC_DATA.Aux3);
  ppmEncoder.setChannel(7, RC_DATA.Aux4);
}
