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


ISR(INT0_vect) {
	_delay_ms(10); // debounce timer
	toggle(PA7);
}

void toggle(int port){
    PORTA ^= (1 << port);
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
   	DDRA |= (1 << PA7); // in-board led as output
    DDRB &= ~(1 << INT0); // interruptor as input
    
   	//
   	// set up pin change interrupt on input pin
   	//
   	set(GIMSK, (1 << INT0)); // enabling PCINT interrupts
   	MCUCR |= (1 << ISC00); // Any logical request
   	//MCUCR |= (1 << ISC01); // falling edge
   	//MCUCR |= (1 << ISC01) || (1 << ISC00); // rising edge
   	sei(); // enable interruptions
   	
   	while(1);
}
