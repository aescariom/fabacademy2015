/********************************************************
*
*  i2cmaster.fabduino.c
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

unsigned char recipient = 0x28;
unsigned char data = 1;

void i2c_init_master(){
    // SCL freq = F_CPU/(16+2(TWBR)*prescalerValue)
    TWBR = 32;  // Bit rate
    TWSR = (0 << TWPS1) | (0 << TWPS0); // Setting prescalar bits
}

void i2c_start(){
   TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN); // put start condition
   while(!(TWCR & (1 << TWINT))); // wait until start condition is transmitted
   while((TWSR & 0xF8)!= 0x08); // wait for ACK
}

void i2c_repeated_start(){
   TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN); // put start condition
   while(!(TWCR & (1 << TWINT))); // wait until start condition is transmitted
   while((TWSR & 0xF8)!= 0x10); // wait for ACK  
}

void i2c_stop(){
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);  // put stop condition
    while(!(TWCR & (1<<TWSTO)));  // wait until stop condition is transmitted
}

void i2c_write_address(unsigned char addr)
{
    TWDR = addr;  // write addr to "call"
    TWCR = (1 << TWINT) | (1 << TWEN);    // Enable TWI and clear interrupt flag
    while(!(TWCR & (1 << TWINT))); // Wait until TWDR byte is transmitted
    while((TWSR & 0xF8)!= 0x18);  // Check for the acknowledgement
}

void i2c_write_data(unsigned char data)
{
    TWDR = data;  // write data in TWDR
    TWCR = (1 << TWINT) | (1 << TWEN);    // Clear TWI interrupt flag,Enable TWI
    while (!(TWCR & (1 << TWINT))); // Wait till complete TWDR byte transmitted
    while((TWSR & 0xF8) != 0x28); // Check for the acknowledgement
}

int main() {
   output(LED_DIRECTION, LED_PIN);

   i2c_init_master();

   while (1) {
      set(LED_PORT,LED_PIN);
      _delay_ms(100);
      clear(LED_PORT,LED_PIN);

      i2c_start();
      i2c_write_address(recipient);
      i2c_write_data(data);
      i2c_stop();

      _delay_ms(1000);

      data ++;
      data %= 10;
   }
}