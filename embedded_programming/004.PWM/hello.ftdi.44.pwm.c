//
//
// hello.ftdi.44.echo.c
//
// 115200 baud FTDI character echo, with flash string
//
// set lfuse to 0x7E for 20 MHz xtal
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
	    if(OCR0A >= 0xF0){
    		OCR0A = 0x00;
	    }else{
	    	OCR0A  += 0x10;
	    }
	}
}
	
int main(void) {
   	//
   	// main
   	//
   	// the clock divider to /1 has been set with the fuses
   	
   	//
   	// initialize output pins
   	//
   	DDRA |= (1 << PA2); // to be set as gnd
   	DDRA &= ~(1 << PA3); // button as input
    DDRB   |= (1 << PB2); // led as output
    
   	TCCR0A = (1 << COM0A1) | (1 << WGM00);  // phase correct PWM + on compare match
    OCR0A  = 0xFF;                          // initial pulse width
    
    TCCR0B = (1 << CS01) | (1 << CS00);   // /64 prescaling
    
    
   	//
   	// set up pin change interrupt on input pin
   	//
   	set(GIMSK, (1 << PCIE0)); // enabling PCINT interrupts
   	set(PCMSK0, (1 << PCINT3)); // enabling the interrupts on PCINT3
   	sei(); // enable interruptions
   	
   	// 
    PORTA &= (0 << PA2); // 0V on PA2
   	
    while(1);
}
