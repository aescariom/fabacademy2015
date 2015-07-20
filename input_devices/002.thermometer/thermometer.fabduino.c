/********************************************************
*
*  thermometer.fabduino.c
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
#include <util/delay.h>
#include "../lib/serial.h"
#include "../lib/definitions.h"

#define step 0.488

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
      int i = 0;
      unsigned short combined = 0;
      int samples = 10;
      for(i = 0; i < samples; i++){
         //
         // initiate conversion
         //
         ADCSRA |= (1 << ADSC);
         //
         // wait for completion
         //
         while (ADCSRA & (1 << ADSC));
            
         unsigned short s_adch, s_adcl;
         //
         // send result
         //
         s_adcl = ADCL;
         s_adch = ADCH;
         
         combined += (s_adch << 8 ) | (s_adcl & 0xff);
      }

      int deg = combined / samples * step;
      
      printf("%c%c%c%c%c%c%c",
                  1, 2, 3, 4, // markup
                  2, // we are sending numbers of two bytes
                  (deg >> 8) & 0xFF, deg & 0xFF);// information
      _delay_ms(100);
      }
   }