//
//
// hello.ftdi.44.ctc.c
//
// Neil Gershenfeld
// 12/8/10
//
// (c) Massachusetts Institute of Technology 2010
// Permission granted for experimental and personal use;
// license for commercial sale available from MIT.
//
// Modified by Alejandro Escario
// Mar 2015

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define set(port,pin) (port |= pin) // set port pin


ISR(PCINT0_vect) {
	_delay_ms(10); // debounce timer
	if(~PINA & (1 << PA3)){ // falling edge
    	OCR0A = 0x00;
	}
}

void toggle(void){
    PORTA ^= (1 << PA7);
}

ISR(TIM0_COMPA_vect) {
	if(OCR0A >= 0xFF){
		OCR0A = 0x00;
    }else{
    	OCR0A  += 0x01;
    }
}
	
int main(void) {
   	//
   	// main
   	//
   	// the clock divider to /8 has been set with the fuses
   	
   	//
   	// initialize output pins
   	//
   	DDRA &= ~(1 << PA3); // button as input
   	DDRA |= (1 << PA7) | (1 << PA2); // in-board led as output
    DDRB   |= (1 << PB2); // led as output


	TIMSK0 |= (1 << OCF0A);   // enable interrupt enable
   	TCCR0A = (1 << COM0A0) | (1 << COM0B0) | (1 << WGM01);  // CTC + Toggle on Compare Match
    OCR0A  = 0x00;                          // initial pulse width
    
    TCCR0B = (1 << CS02) | (1 << CS00);   // /1024 prescaling
    
   	//
   	// set up pin change interrupt on input pin
   	//
   	set(GIMSK, (1 << PCIE0)); // enabling PCINT interrupts
   	set(PCMSK0, (1 << PCINT3)); // enabling the interrupts on PCINT3
   	sei(); // enable interruptions
   	
    PORTA &= (0 << PA2); // 0V on PA2
   	while(1){
   	};
}
