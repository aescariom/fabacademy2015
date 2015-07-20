#ifndef _H_DISTANCE_
#define _H_DISTANCE_ 1

#include <avr/io.h>
#include <stdio.h>
#include <math.h>

void distance_init(volatile uint8_t);
void read_distance();

#endif /* _H_DISTANCE_ */