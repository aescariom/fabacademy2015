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

// std libs
#include <avr/io.h>
#include <avr/interrupt.h>
// common libs
#include "../lib/serial.h"
#include "../lib/definitions.h"
// sensor libs
//#include "button.h"
//#include "hall.h"
//#include "distance.h"
#include "temperature.h"
//#include "step.h"

int main(void) {
    
    //button_init(&PORTB, &DDRB, PB1); 
    //hall_init((0 << MUX3) | (0 << MUX2) | (1 << MUX1) | (0 << MUX0)); // PC2
    //distance_init((0 << MUX3) | (0 << MUX2) | (1 << MUX1) | (1 << MUX0)); // PC3
    temperature_init(
        (0 << MUX3) | (1 << MUX2) | (0 << MUX1) | (0 << MUX0), // PC4
        (0 << MUX3) | (1 << MUX2) | (0 << MUX1) | (1 << MUX0) // PC5
      );
    /*step_init(&PORTD, &DDRD, PD7, 
        (0 << MUX3) | (0 << MUX2) | (0 << MUX1) | (0 << MUX0) // PC0
      ); // LOAD*/
    /*step_init(&PORTB, &DDRB, PB2, 
        (0 << MUX3) | (0 << MUX2) | (0 << MUX1) | (0 << MUX0) // PC0
      ); //TXRX*/

    usart_init(R_UBRR);
    
    while (1) { 
      //read_button(&PINB, PB1);
      //read_hall();
      //read_distance();
      read_temperature();
      //read_step_load();
      //read_step_txrx();
      _delay_ms(500);
    }
}