#ifndef Controller_H_
#define Controller_H_

extern uint16_t CONTROLLER_READ_COUNT;
extern int16_t JOYSTICK_THROTTLE_OFFSET;
extern int16_t JOYSTICK_YAW_OFFSET;

void Controller_calibrate();
void Controller_read();
uint16_t getJoystickValue(uint8_t pin, uint8_t sampleCount, uint8_t eliminator);
uint16_t mapContollerValue(uint16_t val, uint16_t lower, uint16_t middle, uint16_t upper, bool reverse);
void sort(int *a, int n);
void printArray(int *a, int n);

#endif /* Controller_H_ */
