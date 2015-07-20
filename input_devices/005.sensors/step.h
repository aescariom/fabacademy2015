#ifndef _H_STEP_
#define _H_STEP_ 1

#include <avr/io.h>
#include <stdio.h>

#include "../lib/definitions.h"

#define settle_delay() _delay_us(100) // settle delay
#define charge_delay_1() _delay_us(1) // charge delay 1
#define charge_delay_2() _delay_us(10) // charge delay 2
#define charge_delay_3() _delay_us(100) // charge delay 3

void step_init(volatile uint8_t *, volatile uint8_t *, int, volatile uint8_t);
void charge_delay(int);
void read_step_load();
void read_step_txrx();

#endif /* _H_STEP_ */