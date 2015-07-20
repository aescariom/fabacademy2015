/********************************************************
*
*  potentiometer.fabduino.c
* 
*     9600 baud FTDI interface
* 
*  Alejandro Escario Méndez
*   14/04/2015  
*
*  MIT license
*********************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "../lib/serial.h"
#include "../lib/definitions.h"

#define step 10.3 // ohm

int main(void) {
   //
   // main
   //
   usart_init(R_UBRR);
   //
   // init A/D
   //
   ADMUX = (0 << REFS1) | (0 << REFS0) // VCC ref
      | (0 << ADLAR) // right adjust for 10bit precision
      | (0 << MUX3) | (0 << MUX2) | (0 << MUX1) | (0 << MUX0); // (ADC0 = PC0)
   ADCSRA = (1 << ADEN) // enable
      | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // prescaler /128
   //
   // main loop
   //
   while (1) {
      //
      // initiate conversion
      //
      ADCSRA |= (1 << ADSC);
      //
      // wait for completion
      //
      while (ADCSRA & (1 << ADSC))
         ;
         
	  unsigned short s_adch, s_adcl;
      //
      // send result
      //
      s_adcl = ADCL;
      s_adch = ADCH;
      
      unsigned short combined = (s_adch << 8 ) | (s_adcl & 0xff);
      unsigned int ohm = combined * step;
      
      printf("%c%c%c%c%c%c%c",
                  1, 2, 3, 4, // markup
                  2, // we are sending numbers of two bytes
                  (ohm >> 8) & 0xFF, ohm & 0xFF);// information
      _delay_ms(100);
   }
}