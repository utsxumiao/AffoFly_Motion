#ifndef BATTERY_H_
#define BATTERY_H_

extern float BATTERY_VOLTAGE;
extern bool BATTERY_VOLTAGE_LOW;

void Battery_init();
void Battery_read();

#endif /* BATTERY_H_ */