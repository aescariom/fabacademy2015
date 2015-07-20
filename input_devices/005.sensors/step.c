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

#include "step.h"

static volatile uint8_t *step_port;
static int step_pin;

void step_init(volatile uint8_t *port, volatile uint8_t *direction, int pin, volatile uint8_t pc){
	step_port = port;
	step_pin = pin;
   	clear(*port, 1 << pin);
   	output(*direction, 1 << pin);   
   	ADMUX = (0 << REFS1) | (0 << REFS0) // Vcc ref
    	| (0 << ADLAR) // right adjust
      	| pc;
   	ADCSRA = (1 << ADEN) // enable
      	| (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // prescaler /128
}

void charge_delay(int i){
	switch(i){
		case 1:
			charge_delay_1();
			break;
		case 2:
			charge_delay_2();
			break;
		case 3:
			charge_delay_3();
			break;
	}
}

void read_step_load(){
	static unsigned char up_lo,up_hi,down_lo,down_hi;
	int i, up, down;
	//
	// send framing
	//printf("%c%c%c%c", 1, 2, 3, 4);
	for(i = 0; i < 3; i++){
		//
		// settle, charge, and wait 1
		//
		settle_delay();
		set(*step_port, (1 << step_pin));
		charge_delay(i);
		//
		// initiate conversion
		//
		ADCSRA |= (1 << ADSC);
		//
		// wait for completion
		//
		while (ADCSRA & (1 << ADSC))
		 ;
		//
		// save result
		//
		up_lo = ADCL;
		up_hi = ADCH;
		//
		// settle, discharge, and wait 1
		//
		settle_delay();
		clear(*step_port, (1 << step_pin));
		charge_delay(i);
		//
		// initiate conversion
		//
		ADCSRA |= (1 << ADSC);
		//
		// wait for completion
		//
		while (ADCSRA & (1 << ADSC))
		 ;
		//
		// save result
		//
		down_lo = ADCL;
		down_hi = ADCH;
		//
		// send result
		//    
		up = (up_hi << 8 ) | (up_lo & 0xff);
		down = (down_hi << 8 ) | (down_lo & 0xff);
		printf("up: %i - down: %i", up, down);
	}
	printf("\n");
}

void read_step_txrx(){
	int count;
	int up = 0;
  	int down = 0;
	for (count = 0; count < 100; ++count) { 
		//
		// settle, charge
		//
		settle_delay();
		set(*step_port, 1<<step_pin);
		//
		// initiate conversion
		//
		ADCSRA |= (1 << ADSC);
		//
		// wait for completion
		//
		while (ADCSRA & (1 << ADSC))
		;
		//
		// save result
		//
		up += ADC;
		//
		// settle, discharge
		//
		settle_delay();
		clear(*step_port, 1<<step_pin);
		//
		// initiate conversion
		//
		ADCSRA |= (1 << ADSC);
		//
		// wait for completion
		//
		while (ADCSRA & (1 << ADSC))
		;
		//
		// save result
		//
		down += ADC;
 	}
	printf("%i - %i \n", up, down);
}