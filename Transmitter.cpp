#include <Wire.h>
#include <SPI.h>
#include <RF24.h>
#include <FastLED.h>
#include "printf.h"
#include "ppm.h"
#include "PPMEncoder.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "types.h"
#include "config.h"
#include "Transmitter.h"


RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);
MPU6050 mpu;
ControlData controlData;
CRGB leds[LED_COUNT];

uint16_t relayCount = 0;
uint16_t mpuCount = 0;
uint16_t controllerCount = 0;
uint16_t radioCount = 0;
uint16_t loopCount = 0;

const uint16_t relayDataFetchInterval = 2000; //500hz
uint32_t previousRelayDataFetchMicros = 0;
const uint16_t mpuDataFetchInterval = 4000; //250hz
uint32_t previousMpuDataFetchMicros = 0;
const uint16_t controllerDataFetchInterval = 5000; //200hz
uint32_t previousControllerDataFetchMicros = 0;
const uint16_t radioSendInterval = 2000; //500hz
uint32_t previousRadioSendMicros = 0;
const uint16_t ledRefreshInterval = 100000; //10hz
uint32_t previousLedRefreshMicros = 0;
const uint32_t performanceReportInterval = 1000000; //1hz
uint32_t previousPerformanceReportMicros = 0;
const uint32_t voltageCheckInterval = 1000000; //1hz
uint32_t previousVoltageCheckMicros = 0;

int16_t joystickOffsetThrottle = 0;
int16_t joystickOffsetYaw = 0;

bool voltageLow = false;

//GYRO
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer
VectorFloat gravity;    // [x, y, z]            gravity vector
Quaternion quaternion;           // [w, x, y, z]         quaternion container
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
  mpuInterrupt = true;
}

void setup() {
  ADCSRA = (ADCSRA & 0xf8) | 0x04;  // set 16 times division to make analogRead faster
#if defined(DEBUG) || defined(PERFORMANCE) || defined(PRINT_OUTPUT)
  Serial.begin(115200);
#endif
#ifdef DEBUG
  Serial.println("System started");
#endif
  initPins();
  initRadio();
  initData();
  initLed();
  if (digitalRead(RELAY_ENABLE_PIN) == 1) {
    initMpu();
    calibrateJoystick();
  } else {
    initPpmInput();
  }
  //initBatteryVoltage();
  ppmEncoder.begin(PPM_OUTPUT_PIN);
#ifdef DEBUG
  Serial.println("System ready");
#endif
}

void loop() {
  uint32_t currentTime = micros();
  if (digitalRead(RELAY_ENABLE_PIN) == 0) {
    if (currentTime - previousRelayDataFetchMicros >= relayDataFetchInterval) {
      previousRelayDataFetchMicros = currentTime;
      getRelayData();
      relayCount++;
    }
  } else {
    if (currentTime - previousMpuDataFetchMicros >= mpuDataFetchInterval) {
      previousMpuDataFetchMicros = currentTime;
      getMpuData();
      mpuCount++;
    }

    if (currentTime - previousControllerDataFetchMicros >= controllerDataFetchInterval) {
      previousControllerDataFetchMicros = currentTime;
      getControllerData();
      controllerCount++;
    }
  }
  
  if (currentTime - previousRadioSendMicros >= radioSendInterval) {
    previousRadioSendMicros = currentTime;
    radioOutput();
    radioCount++;
  }

  if (currentTime - previousLedRefreshMicros >= ledRefreshInterval) {
    previousLedRefreshMicros = currentTime;
    ledOutput();
  }

  if (currentTime - previousVoltageCheckMicros >= voltageCheckInterval) {
    previousVoltageCheckMicros = currentTime;
    checkBatteryVoltage();
  }
  
  ppmOutput();

  if (currentTime - previousPerformanceReportMicros >= performanceReportInterval) {
    previousPerformanceReportMicros = currentTime;
    reportPerformance();
  }

  loopCount++;
}

void initPins() {
#ifdef DEBUG
  Serial.print("Initialising PINs......");
#endif
  pinMode(MPU_INTERRUPT_PIN, INPUT);
  pinMode(RELAY_ENABLE_PIN, INPUT_PULLUP);
  pinMode(BATTERY_VOLTAGE_PIN, INPUT);
  pinMode(AUX1_PIN, INPUT_PULLUP);
  pinMode(AUX2_PIN, INPUT_PULLUP);
  pinMode(AUX3_PIN, INPUT);
  pinMode(AUX4_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
#ifdef DEBUG
  Serial.println("Done");
#endif
}

void initMpu() {
#ifdef DEBUG
  Serial.print("Initialising MPU......");
#endif
  Wire.begin();
  Wire.setClock(400000);
  mpu.initialize();

#ifdef DEBUG
  Serial.println(mpu.testConnection() ? F("successful") : F("failed"));
#endif
  // load and configure the DMP
#ifdef DEBUG
  Serial.println(F("Initializing DMP..."));
#endif
  devStatus = mpu.dmpInitialize();

  // supply your own gyro offsets here, scaled for min sensitivity
  mpu.setXGyroOffset(220);
  mpu.setYGyroOffset(76);
  mpu.setZGyroOffset(-85);
  mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
    mpu.CalibrateAccel(6);
    mpu.CalibrateGyro(6);
    mpu.PrintActiveOffsets();
    // turn on the DMP, now that it's ready
#ifdef DEBUG
    Serial.println(F("Enabling DMP..."));
#endif
    mpu.setDMPEnabled(true);

    // enable Arduino interrupt detection
#ifdef DEBUG
    Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
#endif
    attachInterrupt(digitalPinToInterrupt(MPU_INTERRUPT_PIN), dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
#ifdef DEBUG
    Serial.println(F("DMP ready! Waiting for first interrupt..."));
#endif
    dmpReady = true;

    // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();
#ifdef DEBUG
    Serial.print("DMP package size: ");   Serial.println(packetSize);
#endif
  } else {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
#ifdef DEBUG
    Serial.print(F("DMP Initialization failed (code "));
    Serial.print(devStatus);
    Serial.println(F(")"));
#endif
  }
#ifdef DEBUG
  Serial.println("Done");
#endif
}

void initRadio() {
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

  printf_begin();
  radio.printPrettyDetails();
#ifdef DEBUG
  Serial.println("");
#endif
}

void initData() {
#ifdef DEBUG
  Serial.print("Initialising system data......");
#endif
  controlData.Token = RADIO_SECURITY_TOKEN;
  controlData.Throttle = 1000;
  controlData.Yaw = 1500;
  controlData.Pitch = 1500;
  controlData.Roll = 1500;
  controlData.Aux1 = 1000;
  controlData.Aux2 = 1000;
  controlData.Aux3 = 1000;
  controlData.Aux4 = 1000;
#ifdef DEBUG
  Serial.println("Done");
#endif
}

void initPpmInput() {
#ifdef DEBUG
  Serial.print("Initialising system data......");
#endif
  ppm.begin(RELAY_DATA_PIN, false);
#ifdef DEBUG
  Serial.println("Done");
#endif
}

void initLed() {
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

void initBatteryVoltage() {
  // Should not use the Arduino internal 1.1v reference since it will mess up analogRead on other pins, also this project utilises a 5v step up module.
  // dummy reading to saturate analog pin so next reading can be correct
  analogRead(BATTERY_VOLTAGE_PIN); 
}

void getRelayData() {
  controlData.Token     = RADIO_SECURITY_TOKEN;
  controlData.Throttle  = ppm.read_channel(3);
  controlData.Yaw       = ppm.read_channel(4);
  controlData.Pitch     = ppm.read_channel(2);
  controlData.Roll      = ppm.read_channel(1);
  controlData.Aux1      = ppm.read_channel(5);
  controlData.Aux2      = ppm.read_channel(6);
  controlData.Aux3      = ppm.read_channel(7);
  controlData.Aux4      = ppm.read_channel(8);
#ifdef PRINT_OUTPUT
  Serial.print("THR: ");  Serial.print(controlData.Throttle);   Serial.print("  ");
  Serial.print("YAW: ");  Serial.print(controlData.Yaw);        Serial.print("  ");
  Serial.print("PIT: ");  Serial.print(controlData.Pitch);      Serial.print("  ");
  Serial.print("ROL: ");  Serial.print(controlData.Roll);       Serial.print("  ");
  Serial.print("AUX1: "); Serial.print(controlData.Aux1);       Serial.print("  ");
  Serial.print("AUX2: "); Serial.print(controlData.Aux2);       Serial.print("  ");
  Serial.print("AUX3: "); Serial.print(controlData.Aux3);       Serial.print("  ");
  Serial.print("AUX4: "); Serial.print(controlData.Aux4);       Serial.print("  ");
  Serial.println();
#endif
}

void getControllerData() {
  controlData.Token     = RADIO_SECURITY_TOKEN;
  int16_t throttle = getJoystickValue(JOYSTICK_THROTTLE_PIN, JOYS_VAL_SAMPLE_COUNT, JOYS_VAL_SAMPLE_ELIMI) - joystickOffsetThrottle;
  if (throttle < 0) {
    throttle = 0;
  }
  uint16_t yaw = getJoystickValue(JOYSTICK_YAW_PIN, JOYS_VAL_SAMPLE_COUNT, JOYS_VAL_SAMPLE_ELIMI) - joystickOffsetYaw;
  controlData.Throttle  = mapContollerValue(throttle, 0, (1023 - joystickOffsetThrottle) / 2, 1023 - joystickOffsetThrottle, false);
  controlData.Yaw       = mapContollerValue(yaw, 0, (1023 - joystickOffsetYaw) / 2, 1023 - joystickOffsetYaw, true);
  controlData.Aux1      = mapContollerValue(digitalRead(AUX1_PIN) * 1023, 0, 511, 1023, false);
  controlData.Aux2      = mapContollerValue(digitalRead(AUX2_PIN) * 1023, 0, 511, 1023, false);
  controlData.Aux3      = mapContollerValue(analogRead(AUX3_PIN), 0, 511, 1023, false);
  controlData.Aux4      = mapContollerValue(analogRead(AUX4_PIN), 0, 511, 1023, false);
  
#ifdef PRINT_OUTPUT
    Serial.print("Throttle: ");     Serial.print(controlData.Throttle);   Serial.print("    ");
    Serial.print("Yaw: ");          Serial.print(controlData.Yaw);        Serial.print("    ");
    Serial.print("Pitch: ");        Serial.print(controlData.Pitch);      Serial.print("    ");
    Serial.print("Roll: ");         Serial.print(controlData.Roll);       Serial.print("    ");
    Serial.print("Aux1: ");         Serial.print(controlData.Aux1);       Serial.print("    ");
    Serial.print("Aux2: ");         Serial.print(controlData.Aux2);       Serial.print("    ");
    Serial.print("Aux3: ");         Serial.print(controlData.Aux3);       Serial.print("    ");
    Serial.print("Aux4: ");         Serial.print(controlData.Aux4);       Serial.print("    ");
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

void calibrateJoystick() {
#ifdef DEBUG
  Serial.print("Calibrating Joystick......");
#endif
  joystickOffsetThrottle = getJoystickValue(JOYSTICK_THROTTLE_PIN, JOYS_CAL_SAMPLE_COUNT, JOYS_CAL_SAMPLE_ELIMI);
  joystickOffsetYaw = getJoystickValue(JOYSTICK_YAW_PIN, JOYS_CAL_SAMPLE_COUNT, JOYS_CAL_SAMPLE_ELIMI) - 511;
#ifdef DEBUG
  Serial.print("Thr: ");  Serial.print(joystickOffsetThrottle);   Serial.print("  ");
  Serial.print("Yaw: ");  Serial.print(joystickOffsetYaw);        Serial.print("  ");
  Serial.println("Done");
#endif
}

uint16_t mapContollerValue(uint16_t val, uint16_t lower, uint16_t middle, uint16_t upper, bool reverse) {
  val = constrain(val, lower, upper);
  if (val < middle) val = map(val, lower, middle, 1000, 1500);
  else val = map(val, middle, upper, 1500, 2000);
  return reverse ? 3000 - val : val;
}

void getMpuData() {
  getMpuValue();
  // 180 / M_PI = 57.3
  controlData.Roll = mapMpuValue(ypr[1] * 57.3, -45, 0, 45, false);
  controlData.Pitch = mapMpuValue(ypr[2] * 57.3, -45, 0, 45, false);
#ifdef PRINT_OUTPUT
  Serial.print("Pitch: ");        Serial.print(controlData.Pitch);      Serial.print("    ");
  Serial.print("Roll: ");         Serial.print(controlData.Roll);       Serial.print("    ");
#endif
}

void getMpuValue() {
  // if programming failed, don't try to do anything
  if (!dmpReady) return;

  // read a packet from FIFO
  if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
    mpu.dmpGetQuaternion(&quaternion, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &quaternion);
    mpu.dmpGetYawPitchRoll(ypr, &quaternion, &gravity);
  }
#ifdef DEBUG
//    Serial.print("ypr\t");
//    Serial.print(ypr[0] * 180 / M_PI);
//    Serial.print("\t");
//    Serial.print(ypr[1] * 180 / M_PI);
//    Serial.print("\t");
//    Serial.println(ypr[2] * 180 / M_PI);
#endif
}

uint16_t mapMpuValue(float val, int8_t lower, int8_t middle, int8_t upper, bool reverse) {
  val = constrain(val, lower, upper);
  if (val < middle) val = map(val, lower, middle, 1500 - GYRO_LIMIT, 1500);
  else val = map(val, middle, upper, 1500, 1500 + GYRO_LIMIT);
  uint16_t result = reverse ? 3000 - val : val;

  //Patch code, should be removed after issue identified
  if(result < 1500 - GYRO_LIMIT) {
    result = 1500 - GYRO_LIMIT;
  } else if(result > 1500 + GYRO_LIMIT) {
    result = 1500 + GYRO_LIMIT;
  }

  return result;
}

void checkBatteryVoltage() {
  uint16_t reading = analogRead(BATTERY_VOLTAGE_PIN); //TODO: use average value
  float voltage = reading * 1023 / 5;
  voltageLow = voltage < LOW_VOLTAGE_THRESHOLD;
}

void radioOutput() {
  //uint32_t before = millis();
  radio.write(&controlData, sizeof(ControlData));
//  uint32_t after = millis();
//  if(after - before > 10){
//    //TODO: once radio stuck, it will not revived by itself, intervention required.
//    Serial.print("Radio Slow! took: ");   Serial.println(after - before);
//  }
}

void ppmOutput() {
  ppmEncoder.setChannel(2, controlData.Throttle);
  ppmEncoder.setChannel(3, controlData.Yaw);
  ppmEncoder.setChannel(1, controlData.Pitch);
  ppmEncoder.setChannel(0, controlData.Roll);
  ppmEncoder.setChannel(4, controlData.Aux1);
  ppmEncoder.setChannel(5, controlData.Aux2);
  ppmEncoder.setChannel(6, controlData.Aux3);
  ppmEncoder.setChannel(7, controlData.Aux4);
}


uint8_t rightLedIndex = 0;
uint8_t backwardLedIndex = 6;
uint8_t forwardLedIndex = 7;
uint8_t leftLedIndex = 8;
uint8_t middleLedIndex = 4;
void ledOutput() {
  leds[middleLedIndex] = voltageLow ? CRGB::Red : CRGB::Green;
  leds[rightLedIndex] = controlData.Roll > 1510 ? CRGB::Green : CRGB::Black;
  leds[leftLedIndex] = controlData.Roll < 1490 ? CRGB::Green : CRGB::Black;
  leds[forwardLedIndex] = controlData.Pitch > 1510 ? CRGB::Green : CRGB::Black;
  leds[backwardLedIndex] = controlData.Pitch < 1490 ? CRGB::Green : CRGB::Black;
  FastLED.show();
}

void reportPerformance() {
#ifdef PERFORMANCE
  Serial.print("Relay: ");      Serial.print(relayCount);       Serial.print("    ");
  Serial.print("MPU: ");        Serial.print(mpuCount);         Serial.print("    ");
  Serial.print("Controller: "); Serial.print(controllerCount);  Serial.print("    ");
  Serial.print("Radio: ");      Serial.print(radioCount);       Serial.print("    ");
  Serial.print("Loop: ");       Serial.print(loopCount);        Serial.print("    ");
  Serial.println("");
#endif
  relayCount = 0;
  mpuCount = 0;
  controllerCount = 0;
  radioCount = 0;
  loopCount = 0;
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
