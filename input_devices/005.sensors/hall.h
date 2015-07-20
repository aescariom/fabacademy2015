#ifndef _H_HALL_
#define _H_HALL_ 1

#include <avr/io.h>
#include <stdio.h>

void hall_init(volatile uint8_t);
void read_hall();

#endif // _H_HALL_