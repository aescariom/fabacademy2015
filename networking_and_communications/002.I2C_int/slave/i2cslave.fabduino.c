/********************************************************
*
*  i2cslave.fabduino.c
* 
*  Alejandro Escario Méndez
*   03/05/2015  
* 
*  (c) Massachusetts Institute of Technology 2015
*  Permission granted for experimental and personal use;
*  license for commercial sale available from MIT.
*********************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h>
#include <avr/interrupt.h>

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
unsigned char regaddr, regdata;

void i2c_init_slave(){
    TWAR = my_addr;
    TWDR = 0x00;
    // SCL freq = F_CPU/(16+2(TWBR)*prescalerValue)
    TWBR = 32;  // Bit rate
    TWSR = (0 << TWPS1) | (0 << TWPS0); // Setting prescalar bits
    TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN) | (1<<TWIE);    // get ACK, en TWI, clear int flag
}

void blink(unsigned char num){
  int i;
  for(i = 0; i < num; i++){
     _delay_ms(50);
     set(LED_PORT,LED_PIN);
     _delay_ms(50);
     clear(LED_PORT, LED_PIN);
  }
}

ISR(TWI_vect){
  static unsigned char last_val = 1;
  // Disable Global Interrupt
  cli();
  switch(TW_STATUS){
    case TW_START:  // 0x08
    case TW_REP_START: // 0x10
    case TW_MT_SLA_ACK: // 0x18
    case TW_MT_SLA_NACK: // 0x20
    case TW_MT_DATA_ACK: // 0x28
    case TW_MT_DATA_NACK: // 0x30
    case TW_MR_SLA_ACK:   // 0x40
    case TW_MR_SLA_NACK:   // 0x48
    case TW_MR_DATA_ACK:  // 0x50
    case TW_MR_DATA_NACK:  //  0x58
    case TW_ST_ARB_LOST_SLA_ACK:  //  0xB0
    case TW_SR_ARB_LOST_SLA_ACK: //  0x68
    case TW_SR_GCALL_ACK:    //0x70
    case TW_SR_ARB_LOST_GCALL_ACK:  //  0x78
    case TW_SR_GCALL_DATA_ACK:   // 0x90
    case TW_SR_GCALL_DATA_NACK:  //  0x98
    case TW_NO_INFO:   // 0xF8
      TWCR |= (1<<TWINT);    // Clear TWINT Flag
      break;
    case TW_SR_SLA_ACK:      // 0x60: SLA+W received, ACK returned
      TWCR |= (1<<TWINT);    // Clear TWINT Flag
      break;
    case TW_SR_DATA_ACK:     // data received
      last_val = TWDR;
      blink(last_val);
      TWCR |= (1<<TWINT);    // Clear TWINT Flag
      break;
    case TW_SR_STOP:         // 0xA0: stop or repeated start condition received while selected
      TWCR |= (1<<TWINT);    // Clear TWINT Flag
      break;
    case TW_ST_SLA_ACK:      // 0xA8: SLA+R received, ACK returned
      TWDR= 10-last_val;           // Fill TWDR register whith the data to be sent 
      TWCR = ((1 << TWEA) | (1 << TWINT) | (1 << TWEN) | (1 << TWIE));   // Enable TWI, Clear TWI interrupt flag
      break;
    case TW_ST_DATA_ACK:     // 0xB8: data transmitted, ACK received
      TWCR |= (1<<TWINT);    // Clear TWINT Flag
      break;
    case TW_ST_DATA_NACK:    // 0xC0: data transmitted, NACK received
      TWCR |= (1<<TWINT);    // Clear TWINT Flag
      break;
    case TW_ST_LAST_DATA:    // 0xC8: last data byte transmitted, ACK received
    case TW_BUS_ERROR:       // 0x00: illegal start or stop condition
    default:
      TWCR |= (1<<TWINT);    // Clear TWINT Flag
      break;
  }

  sei();
}

int main() {
   output(LED_DIRECTION, LED_PIN);

   i2c_init_slave();
   sei();

   while (1);
}