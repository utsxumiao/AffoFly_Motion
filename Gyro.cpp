#include "Arduino.h"
#include "types.h"
#include "config.h"
#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "AffoFly_Motion.h"
#include "Gyro.h"

MPU6050 mpu;
uint16_t GYRO_READ_COUNT = 0;

bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer
VectorFloat gravity;    // [x, y, z]            gravity vector
Quaternion quaternion;  // [w, x, y, z]         quaternion container
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high

void dmpDataReady() {
  mpuInterrupt = true;
}

void getGyroValue() {
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

uint16_t mapGyroValue(float val2, int8_t lower, int8_t middle, int8_t upper, bool reverse) {
  float val = constrain(val2, lower, upper);
  if (val < middle) val = map(val, lower, middle, 1500 - GYRO_LIMIT, 1500);
  else val = map(val, middle, upper, 1500, 1500 + GYRO_LIMIT);
  uint16_t result = reverse ? 3000 - val : val;
  return result;
}

void Gyro_init(){
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
#ifdef DEBUG
    mpu.PrintActiveOffsets();
    Serial.println(F("Enabling DMP..."));
#endif
    // turn on the DMP, now that it's ready
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

void Gyro_read(){
    getGyroValue();
    // 180 / M_PI = 57.3
    RC_DATA.Roll = mapGyroValue(ypr[1] * 57.3, -45, 0, 45, true);
    RC_DATA.Pitch = mapGyroValue(ypr[2] * 57.3, -45, 0, 45, true);
#ifdef PRINT_OUTPUT
    Serial.print("Pitch: ");        Serial.print(RC_DATA.Pitch);      Serial.print("    ");
    Serial.print("Roll: ");         Serial.print(RC_DATA.Roll);       Serial.print("    ");
#endif
    GYRO_READ_COUNT++;
}
