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

// std libs
#include <avr/io.h>
#include <avr/interrupt.h>
// common libs
#include "../lib/serial.h"
#include "../lib/definitions.h"
#include "temperature.h"

#define LED_DIRECTION DDRB
#define LED_PORT PORTB
#define LED_PIN (1 << PB5)

volatile float target_temp = 25;
char buff[6];

void send_float(float val){
    int integer = val;
    int frac = (val - integer) * 100;
    usart_putchar(1);
    usart_putchar(2);
    usart_putchar(3);
    usart_putchar(4);
    usart_putchar(integer);
    usart_putchar(frac);
    
    integer = target_temp;
    frac = (target_temp - integer) * 100;
    usart_putchar(4);
    usart_putchar(3);
    usart_putchar(2);
    usart_putchar(1);
    usart_putchar(integer);
    usart_putchar(frac);
}

ISR (USART_RX_vect) {
	buff[0] = buff[1];
	buff[1] = buff[2];
	buff[2] = buff[3];
	buff[3] = buff[4];
	buff[4] = buff[5];
	buff[5] = UDR0;
	if(buff[0] == 1 && buff[1] == 2 && buff[2] == 3 && buff[3] == 4){
		target_temp=buff[4]+buff[5]/100;
	}
}

int main(void) {
   output(LED_DIRECTION, LED_PIN);
    
    temperature_init(
        (0 << MUX3) | (1 << MUX2) | (0 << MUX1) | (0 << MUX0), // PC4
        (0 << MUX3) | (1 << MUX2) | (0 << MUX1) | (1 << MUX0) // PC5
      );

    usart_init(R_UBRR);
    sei();
    
    int i = 0;
    int filter = 250;
    float temp = .0f;
    float last_temp = 9999;
    
    while (1) { 
    	++i;
    	temp += read_temperature()/filter;
    	if(i >= filter){
    		i = 0;
    		//temp /= filter;
    		send_float(temp);
    		last_temp = temp;
    		temp = 0;
    	}
      if(last_temp < target_temp){
         set(LED_PORT,LED_PIN);
	  } else{
         clear(LED_PORT,LED_PIN);
	  }
    }
}