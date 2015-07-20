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

#include "distance.h"

void distance_init(volatile uint8_t pc){
   ADMUX = (0 << REFS1) | (0 << REFS0) // Vcc ref
      | (0 << ADLAR) // right adjust
      | pc;
   ADCSRA = (1 << ADEN) // enable
      | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // prescaler /128
}

void read_distance(){
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
      
      unsigned short combined = (s_adch << 8 ) | (s_adcl & 0xff);
      int dist = 13343.85 * pow(combined, -1.3);
      printf("%c%c%c%c%c%c%c",
                  1, 2, 3, 4, // markup
                  2, // we are sending numbers of two bytes
                  (dist >> 8) & 0xFF, dist & 0xFF);// information
}