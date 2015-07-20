/********************************************************
*
*  comparator.fabduino.c
* 
*     9600 baud FTDI interface
* 
*  Alejandro Escario Méndez
*   14/04/2015  
* 
*  (c) Massachusetts Institute of Technology 2015
*  Permission granted for experimental and personal use;
*  license for commercial sale available from MIT.
*********************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "../lib/serial.h"
#include "../lib/definitions.h"
   
ISR (ANALOG_COMP_vect) {
  if (ACSR & (1<<ACO)){
      printf("Lx > Pot\n");
  }else{
      printf("Pot >= Lx\n");
  }
}

int main(void) {

  usart_init(R_UBRR);

  cli();
  ACSR |= (1<<ACIE);	//Enable analog comparator interrupt
  sei();
  //
  // main loop
  //
  while (1);
}