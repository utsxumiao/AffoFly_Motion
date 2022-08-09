#ifndef CONFIG_H_
#define CONFIG_H_

//#define DEBUG
#define PERFORMANCE
#define CHANNEL_COUNT           8
#define RADIO_PIPE              0xE8E8F0F0E1LL
#define RADIO_CHANNEL           100
#define RADIO_SECURITY_TOKEN    0
#define MPU_ADDRESS             0x68
#define JOYS_VAL_SAMPLE_COUNT   5
#define JOYS_VAL_SAMPLE_ELIMI   1
#define JOYS_CAL_SAMPLE_COUNT   100
#define JOYS_CAL_SAMPLE_ELIMI   10
#define GYRO_CAL_SAMPLE_COUNT   100
#define GYRO_LIMIT              200   //This limit motion control to 1500+-VALUE
#define LOW_VOLTAGE_THRESHOLD   3.4

// PIN Definitions
#define MPU_INTERRUPT_PIN       0
#define RELAY_DATA_PIN          2
#define RELAY_ENABLE_PIN        3
#define SR_DATA_PIN             4
#define SR_CLOCK_PIN            5
#define SR_LATCH_PIN            6
#define NRF_CE_PIN              7
#define NRF_CSN_PIN             8
#define PPM_OUTPUT_PIN          9
#define AUX1_PIN                10
#define JOYSTICK_THROTTLE_PIN   A0
#define JOYSTICK_YAW_PIN        A1
#define BATTERY_VOLTAGE_PIN     A2
#define AUX2_PIN                A3
#define AUX3_PIN                A6
#define AUX4_PIN                A7

// Shift Register 8 outputs
#define BUZZER_SR_BIT           0
#define RIGHT_LED_SR_BIT        1
#define TOP_LED_SR_BIT          2
#define MIDDLE_LED_SR_BIT       3
#define LEFT_LED_SR_BIT         4
#define BOTTOM_LED_SR_BIT       5

#endif /* CONFIG_H_ */
