#include "Arduino.h"
#include "types.h"
#include "config.h"
#include <Wire.h>
#include <FastLED.h>
#include "Battery.h"
#include "WS2812B.h"

CRGB leds[LED_COUNT];

void WS2812B_init() {
#ifdef DEBUG
    Serial.print("Initialising LED......");
#endif
    FastLED.addLeds<LED_TYPE, LED_DATA_PIN, LED_COLOUR_ORDER>(leds, LED_COUNT);
    FastLED.setBrightness(LED_BRIGHTNESS);
    for(uint8_t i = 0; i < LED_COUNT; i++) {
        leds[i] = CRGB::Black;
    }
    FastLED.show();
#ifdef DEBUG
    Serial.println("Done");
#endif
}

void WS2812B_refresh(ControlData *data) {
    //TODO: use colour temperature to reflect value
    leds[LED_MIDDLE_INDEX]  = BATTERY_VOLTAGE_LOW ? CRGB::Red : CRGB::Green;

    leds[LED_RIGHT_INDEX]   = data->Roll > 1500 + LED_GYRO_THRESHOLD ? CRGB::Green : CRGB::Black;
    leds[LED_LEFT_INDEX]    = data->Roll < 1500 - LED_GYRO_THRESHOLD ? CRGB::Green : CRGB::Black;
    leds[LED_TOP_INDEX]     = data->Pitch > 1500 + LED_GYRO_THRESHOLD ? CRGB::Green : CRGB::Black;
    leds[LED_BOTTOM_INDEX]  = data->Pitch < 1500 - LED_GYRO_THRESHOLD ? CRGB::Green : CRGB::Black;
    FastLED.show();
}