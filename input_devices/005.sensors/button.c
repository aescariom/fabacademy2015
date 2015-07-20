/********************************************************
*
*  sensors.fabduino.c
* 
*  Sensor shield v1
*     9600 baud FTDI interface
* 
*  Alejandro Escario Méndez
*   14/04/2015  
* 
*  (c) Massachusetts Institute of Technology 2015
*  Permission granted for experimental and personal use;
*  license for commercial sale available from MIT.
*********************************************************/

#include "button.h"

void button_init(volatile uint8_t *port, volatile uint8_t *direction, int pin){
    set(*port, (1 << pin)); // turn on pull-up
    input(*direction, (1 << pin));
}

void read_button(volatile uint8_t *pins, int pin){
	// 
    // wait for button down
    //
    while (0 != pin_test(*pins, (1 << pin)));
    printf("Button: down\n");
    //
    // wait for button up
    //
    while (0 == pin_test(*pins, (1 << pin)));
    printf("Button: up\n");
}