//
// hello.arduino.168.blink.c
//
// test blinking LED
//
// Neil Gershenfeld
// 10/21/13
//
// Modified by Alejandro Escario - FabAcademy 2015

#include <avr/io.h>
#include <util/delay.h>


#define output(directions,pin) (directions |= pin) // set port direction for output
#define set(port,pin) (port |= pin) // set port pin
#define clear(port,pin) (port &= (~pin)) // clear port pin
#define pin_test(pins,pin) (pins & pin) // test for port pin
#define bit_test(byte,bit) (byte & (1 << bit)) // test for bit set
#define led_delay() _delay_ms(100) // LED delay
#define num_delay() _delay_ms(500) // Iteration delay

#define led_port PORTB
#define led_direction DDRB
#define led_pin (1 << PB5)

int main(void) {
   //
   // main
   //
   // set clock divider to /1
   //
   CLKPR = (1 << CLKPCE);
   CLKPR = (0 << CLKPS3) | (0 << CLKPS2) | (0 << CLKPS1) | (0 << CLKPS0); // 8MHz (CLK) vs 8MHz (desired freq) (scaling factor 1)
   //
   // initialize LED pin
   //
   clear(led_port, led_pin);
   output(led_direction, led_pin);
   //
   // main loop
   //
   int i = 0;
   int j = 0;
   while (1) {
   		if(++i >= 255){
   			i = 0;
   		}
   		j = 0;
   		while(j < i){
      		set(led_port, led_pin);
      		led_delay();
      		clear(led_port, led_pin);
      		led_delay();
      		j++;
      	}
      	num_delay();
   }
}