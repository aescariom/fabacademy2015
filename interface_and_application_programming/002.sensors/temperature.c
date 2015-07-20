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

#include "temperature.h"

static uint8_t temperature_probe;
static uint8_t temperature_bridge;

void temperature_init(volatile uint8_t probe, volatile uint8_t bridge){
      temperature_probe = probe;
      temperature_bridge = bridge;
      temperature_change_port(bridge);
      ADCSRA = (1 << ADEN) // enable
            | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // prescaler /128
}

void temperature_change_port(volatile uint8_t pc){
      ADMUX = (0 << REFS1) | (0 << REFS0) // Vcc ref
            | (0 << ADLAR) // right adjust
            | pc;
}

int read_temperature_port(volatile uint8_t port){
      temperature_change_port(port);
      //
      // initiate conversion
      //
      ADCSRA |= (1 << ADSC);
      //
      // wait for completion
      //
      while (ADCSRA & (1 << ADSC));
         
      unsigned short s_adch, s_adcl;
      //
      // send result
      //
      s_adcl = ADCL;
      s_adch = ADCH;

      return (s_adch << 8 ) | (s_adcl & 0xff);
}

float read_temperature(){
      int pc4 = read_temperature_port(temperature_probe);
      int pc5 = read_temperature_port(temperature_bridge);
      int diff = pc5-pc4;
      float B = 3750.;
      float res = (float)pc4 / (1023-pc4) * 10000;
      float k = 273.15;
      float temp = 1./(log(res/10000)/B+(1./(25+k))) - k;
      return temp;
}