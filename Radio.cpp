#include "Arduino.h"
#include "types.h"
#include "config.h"
#include <SPI.h>
#include <RF24.h>
#include "AffoFly_Motion.h"
#include "Radio.h"

uint16_t RADIO_PACKET_COUNT = 0;

RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);

void Radio_init() {
#ifdef DEBUG
  Serial.print("Initialising Radio......");
#endif
  while(!radio.begin()){
    Serial.println(F("Radio device is not responding!"));
    delay(1000);
  }
  radio.setPALevel(RF24_PA_MIN);
  radio.setAutoAck(false);
  radio.setChannel(RADIO_CHANNEL);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(RADIO_PIPE);
  radio.stopListening();

#ifdef DEBUG
  //radio.printPrettyDetails();
  Serial.println("Done");
#endif
}

void Radio_output() {
  //uint32_t before = millis();
  radio.write(&RC_DATA, sizeof(ControlData));
  RADIO_PACKET_COUNT++;
//  uint32_t after = millis();
//  if(after - before > 10){
//    //TODO: once radio stuck, it will not revived by itself, intervention required.
//    Serial.print("Radio Slow! took: ");   Serial.println(after - before);
//  }
}
