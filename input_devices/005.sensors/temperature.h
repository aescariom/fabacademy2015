#ifndef _H_TEMPERATURE_
#define _H_TEMPERATURE_ 1

#include <avr/io.h>
#include <stdio.h>
#include <math.h>

void temperature_init(volatile uint8_t, volatile uint8_t);
void temperature_change_port(volatile uint8_t);
int read_temperature_port(volatile uint8_t);
void read_temperature();

#endif /* _H_TEMPERATURE_ */