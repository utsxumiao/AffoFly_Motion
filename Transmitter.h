#ifndef TRANSMITTER_H_
#define TRANSMITTER_H_

void initPins();
void initMpu();
void initRadio();
void initData();
void initPpmInput();
void initLed();
void initBatteryVoltage();
void getRelayData();
void getControllerData();
uint16_t getJoystickValue(uint8_t pin, uint8_t sampleCount, uint8_t eliminator);
void calibrateJoystick();
uint16_t mapContollerValue(uint16_t val, uint16_t lower, uint16_t middle, uint16_t upper, bool reverse);
void getMpuData();
void getMpuValue();
uint16_t mapMpuValue(float val, int8_t lower, int8_t middle, int8_t upper, bool reverse);
void checkBatteryVoltage();
void radioOutput();
void ppmOutput();
void ledOutput();
void reportPerformance();
void sort(int *a, int n);
void printArray(int *a, int n);

#endif /* TRANSMITTER_H_ */
