/********************************************************
*
*  i2cslave.fabduino.c
* 
*  Alejandro Escario Méndez
*   03/05/2015  
*
*  MIT license
*********************************************************/

#include <avr/io.h>
#include <util/delay.h>

#define output(directions,pin) (directions |= pin) // set port direction for output
#define input(directions,pin) (directions &= (~pin)) // set port direction for input
#define set(port,pin) (port |= pin) // set port pin
#define clear(port,pin) (port &= (~pin)) // clear port pin
#define pin_test(pins,pin) (pins & pin) // test for port pin
#define bit_test(byte,bit) (byte & (1 << bit)) // test for bit set

#define LED_DIRECTION DDRB
#define LED_PORT PORTB
#define LED_PIN (1 << PB5)

unsigned char my_addr = 0x28;

void i2c_init_slave(){
    TWAR = my_addr;
}

void i2c_match_read_slave(){
    while((TWSR & 0xF8)!= 0x60) { // wait for ACK
        TWCR=(1<<TWEA)|(1<<TWEN)|(1<<TWINT);    // get ACK, en TWI, clear int flag
        while (!(TWCR & (1<<TWINT))); // Wait for TWINT flag
    }
}

unsigned char i2c_read_slave(){
    TWCR=(1<<TWEA)|(1<<TWEN)|(1<<TWINT);   // get ACK, en TWI, clear int flag
    while (!(TWCR & (1<<TWINT))); // Wait for TWINT flag
    while((TWSR & 0xF8)!=0x80); // Wait for acknowledgement
    return TWDR; // return received value
}

int main() {
   output(LED_DIRECTION, LED_PIN);

   i2c_init_slave();

   while (1) {
      i2c_match_read_slave(); 

      unsigned char data = i2c_read_slave();
      int i;
      for(i = 0; i < data; i++){
         _delay_ms(25);
         set(LED_PORT,LED_PIN);
         _delay_ms(25);
         clear(LED_PORT, LED_PIN);
      }
   }
}