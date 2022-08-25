#ifndef SIGNAL_H_
#define SIGNAL_H_

extern uint16_t SIGNAL_INPUT_COUNT;

void Signal_input_init();
void Signal_output_init();
void Signal_input_read();
void Signal_output_write();

#endif /* SIGNAL_H_ */
