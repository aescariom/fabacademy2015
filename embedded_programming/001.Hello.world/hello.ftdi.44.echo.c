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
#include <avr/pgmspace.h>

#define output(directions,pin) (directions |= pin) // set port direction for output
#define set(port,pin) (port |= pin) // set port pin
#define clear(port,pin) (port &= (~pin)) // clear port pin
#define pin_test(pins,pin) (pins & pin) // test for port pin
#define bit_test(byte,bit) (byte & (1 << bit)) // test for bit set
#define bit_delay_time 8.5 // bit delay for 115200 with overhead
#define bit_delay() _delay_us(bit_delay_time) // RS232 bit delay
#define half_bit_delay() _delay_us(bit_delay_time/2) // RS232 half bit delay
#define char_delay() _delay_ms(10) // char delay

#define serial_port PORTA
#define serial_direction DDRA
#define serial_pins PINA
#define serial_pin_in (1 << PA0)
#define serial_pin_out (1 << PA1)

#define max_buffer 25

void get_char(volatile unsigned char *pins, unsigned char pin, char *rxbyte) {
   //
   // read character into rxbyte on pins pin
   //    assumes line driver (inverts bits)
   //
   *rxbyte = 0;
   //
   // delay to middle of first data bit
   //
   bit_delay();
   //
   // unrolled loop to read data bits
   //
   if pin_test(*pins,pin)
      *rxbyte |= (1 << 0);
   else
      *rxbyte |= (0 << 0);
   bit_delay();
   if pin_test(*pins,pin)
      *rxbyte |= (1 << 1);
   else
      *rxbyte |= (0 << 1);
   bit_delay();
   if pin_test(*pins,pin)
      *rxbyte |= (1 << 2);
   else
      *rxbyte |= (0 << 2);
   bit_delay();
   if pin_test(*pins,pin)
      *rxbyte |= (1 << 3);
   else
      *rxbyte |= (0 << 3);
   bit_delay();
   if pin_test(*pins,pin)
      *rxbyte |= (1 << 4);
   else
      *rxbyte |= (0 << 4);
   bit_delay();
   if pin_test(*pins,pin)
      *rxbyte |= (1 << 5);
   else
      *rxbyte |= (0 << 5);
   bit_delay();
   if pin_test(*pins,pin)
      *rxbyte |= (1 << 6);
   else
      *rxbyte |= (0 << 6);
   bit_delay();
   if pin_test(*pins,pin)
      *rxbyte |= (1 << 7);
   else
      *rxbyte |= (0 << 7);
   //
   // wait for stop bit
   //
   bit_delay();
   half_bit_delay();
   }

void put_char(volatile unsigned char *port, unsigned char pin, char txchar) {
   //
   // send character in txchar on port pin
   //    assumes line driver (inverts bits)
   //
   // start bit
   //
   clear(*port,pin);
   bit_delay();
   //
   // unrolled loop to write data bits
   if bit_test(txchar,0)
      set(*port,pin);
   else
      clear(*port,pin);
   bit_delay();
   if bit_test(txchar,1)
      set(*port,pin);
   else
      clear(*port,pin);
   bit_delay();
   if bit_test(txchar,2)
      set(*port,pin);
   else
      clear(*port,pin);
   bit_delay();
   if bit_test(txchar,3)
      set(*port,pin);
   else
      clear(*port,pin);
   bit_delay();
   if bit_test(txchar,4)
      set(*port,pin);
   else
      clear(*port,pin);
   bit_delay();
   if bit_test(txchar,5)
      set(*port,pin);
   else
      clear(*port,pin);
   bit_delay();
   if bit_test(txchar,6)
      set(*port,pin);
   else
      clear(*port,pin);
   bit_delay();
   if bit_test(txchar,7)
      set(*port,pin);
   else
      clear(*port,pin);
   bit_delay();
   //
   // stop bit
   //
   set(*port,pin);
   bit_delay();
   //
   // char delay
   //
   char_delay();
   }

void put_string(volatile unsigned char *port, unsigned char pin, char *str) {

   	put_char(&serial_port, serial_pin_out, 0); // Hack that avoids sending strage characters
   int i = 0;
   do {
      put_char(port, pin, str[i]);
      } while (str[++i] != 0);
   }

void blink(void){
	// blinks de led
    PORTA |= (1 << PA7);
   	_delay_ms(100);
    PORTA &= (0 << PA7);
}

int erase(char *str){
	// cleans the buffer
	int i = 1;
    while(i < max_buffer) {
        str[i++]=0;
    }
   return 0;
}

ISR(PCINT0_vect) {
	// interrupt function
   	static char buffer[max_buffer] = {0};
   	static int index;
   	static char chr;
	if(~PINA & (1 << PA0)){ // character is prepared to be received
   		get_char(&serial_pins, serial_pin_in, &chr);
   		put_string(&serial_port, serial_pin_out, "echo: \"");
   		buffer[index++] = chr;
   		if (index == (max_buffer-1))
      		index = 0;
   		put_string(&serial_port, serial_pin_out, buffer);
   		put_string(&serial_port, serial_pin_out, "\"\n");
   		blink();
   	} else{
		if(~PINA & (1 << PA3)){ // button pressed
			blink();
			put_string(&serial_port, serial_pin_out, "Reset\n");
			index = erase(buffer);
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
   	set(serial_port, serial_pin_out);
   	output(serial_direction, serial_pin_out);
   	DDRA &= ~(1 << PA3);
   	DDRA |= (1 << PA7);
   	//
   	// set up pin change interrupt on input pin
   	//
   	set(GIMSK, (1 << PCIE0));
   	set(PCMSK0, (1 << PCINT0));
   	set(PCMSK0, (1 << PCINT3));
   	sei(); // enable interruptions
   	//
   	// main loop
   	//
	while (1);
}
