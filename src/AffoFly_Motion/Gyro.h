#ifndef Gyro_H_
#define Gyro_H_

extern uint16_t GYRO_READ_COUNT;

void Gyro_init();
void Gyro_read();
void getGyroValue();
uint16_t mapGyroValue(float val, int8_t lower, int8_t middle, int8_t upper, bool reverse);
void dmpDataReady();

#endif /* Gyro_H_ */
