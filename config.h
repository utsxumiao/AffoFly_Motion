#ifndef CONFIG_H_
#define CONFIG_H_

//#define PRINT_OUTPUT
#define DEBUG
#define PERFORMANCE
#define CHANNEL_COUNT           8
#define RADIO_PIPE              0xE8E8F0F0E1LL
#define RADIO_CHANNEL           125
#define RADIO_SECURITY_TOKEN    998789
#define MPU_ADDRESS             0x68
#define JOYS_VAL_SAMPLE_COUNT   5 
#define JOYS_VAL_SAMPLE_ELIMI   1
#define JOYS_CAL_SAMPLE_COUNT   10
#define JOYS_CAL_SAMPLE_ELIMI   2
#define GYRO_CAL_SAMPLE_COUNT   100
#define GYRO_LIMIT              200   //This limit motion control to 1500+-VALUE
#define YAW_DEADBAND            50
#define YAW_LIMIT               200
#define LOW_VOLTAGE_THRESHOLD   3.4
#define LED_COUNT               10
#define LED_TYPE                WS2812B
#define LED_COLOUR_ORDER        GRB
#define LED_BRIGHTNESS          16
#define LED_RIGHT_INDEX         3
#define LED_LEFT_INDEX          1
#define LED_TOP_INDEX           0
#define LED_BOTTOM_INDEX        2
#define LED_MIDDLE_INDEX        4
#define LED_GYRO_THRESHOLD      10
#define LED_START_DURATION      1000  //millis

// PIN Definitions
#define MPU_INTERRUPT_PIN       0
#define RELAY_DATA_PIN          2
#define RELAY_ENABLE_PIN        3
#define LED_DATA_PIN            4
#define AUX1_PIN                5
#define AUX2_PIN                6
#define NRF_CE_PIN              7
#define NRF_CSN_PIN             8
#define PPM_OUTPUT_PIN          9
#define BUZZER_PIN              10
#define JOYSTICK_THROTTLE_PIN   A0
#define JOYSTICK_YAW_PIN        A1
#define AUX3_PIN                A2
#define AUX4_PIN                A3
#define BATTERY_VOLTAGE_PIN     A6
//#define AUX4_PIN                A7

#endif /* CONFIG_H_ */
