#ifndef NRF2401L_H_
#define NRF2401L_H_

extern uint16_t NRF2401L_PACKET_COUNT;

void NRF2401L_init();
void NRF2401L_output(ControlData *data);

#endif /* NRF2401L_H_ */