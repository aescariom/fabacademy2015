#ifndef _H_BUTTON_
#define _H_BUTTON_ 1

#include <stdio.h>
#include "../lib/definitions.h"

void button_init(volatile uint8_t *, volatile uint8_t *, int);
void read_button(volatile uint8_t *, int);

#endif // _H_BUTTON_