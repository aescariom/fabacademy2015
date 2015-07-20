//
//
// hello.stepper.bipolar.44.full.c
//
// bipolar full stepping hello-world
//
// Neil Gershenfeld
// 11/21/12
//
// (c) Massachusetts Institute of Technology 2012
// This work may be reproduced, modified, distributed,
// performed, and displayed for any purpose. Copyright is
// retained and must be preserved. The work is provided
// as is; no warranty is provided, and users accept all 
// liability.
//

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define output(directions,pin) (directions |= pin) // set port direction for output
#define set(port,pin) (port |= pin) // set port pin
#define clear(port,pin) (port &= (~pin)) // clear port pin
#define pin_test(pins,pin) (pins & pin) // test for port pin
#define bit_test(byte,bit) (byte & (1 << bit)) // test for bit set

#define bridge_port PORTD // H-bridge port
#define bridge_direction DDRD // H-bridge direction
#define A2 (1 << PD2) // H-bridge output pins
#define A1 (1 << PD3) // "
#define B2 (1 << PD5) // "
#define B1 (1 << PD6) // "
#define on_delay() _delay_us(200) // PWM on time
#define off_delay() _delay_us(40) // PWM off time
#define PWM_count 10 // number of PWM cycles
#define step_count 50 // number of steps

static uint8_t count;

//
// A+ B+ PWM pulse
//
void pulse_ApBp() {
  clear(bridge_port, A2);
  clear(bridge_port, B2);
  set(bridge_port, A1);
  set(bridge_port, B1);
   for (count = 0; count < PWM_count; ++count) {
      set(bridge_port, A1);
      set(bridge_port, B1);
      on_delay();
      clear(bridge_port, A1);
      clear(bridge_port, B1);
      off_delay();
      }
   }
//
// A+ B- PWM pulse
//
void pulse_ApBm() {
  clear(bridge_port, A2);
  clear(bridge_port, B1);
  set(bridge_port, A1);
  set(bridge_port, B2);
   for (count = 0; count < PWM_count; ++count) {
      set(bridge_port, A1);
      set(bridge_port, B2);
      on_delay();
      clear(bridge_port, A1);
      clear(bridge_port, B2);
      off_delay();
      }
   }
//
// A- B+ PWM pulse
//
void pulse_AmBp() {
  clear(bridge_port, A1);
  clear(bridge_port, B2);
  set(bridge_port, A2);
  set(bridge_port, B1);
   for (count = 0; count < PWM_count; ++count) {
      set(bridge_port, A2);
      set(bridge_port, B1);
      on_delay();
      clear(bridge_port, A2);
      clear(bridge_port, B1);
      off_delay();
      }
   }
//
// A- B- PWM pulse
//
void pulse_AmBm() {
  clear(bridge_port, A1);
  clear(bridge_port, B1);
  set(bridge_port, A2);
  set(bridge_port, B2);
   for (count = 0; count < PWM_count; ++count) {
      set(bridge_port, A2);
      set(bridge_port, B2);
      on_delay();
      clear(bridge_port, A2);
      clear(bridge_port, B2);
      off_delay();
      }
   }
//
// clockwise step
//
void step_cw() {
   pulse_ApBp();
   pulse_AmBp();
   pulse_AmBm();
   pulse_ApBm();
   }
//
// counter-clockwise step
//
void step_ccw() {
   pulse_ApBm();
   pulse_AmBm();
   pulse_AmBp();
   pulse_ApBp();
   }

 
ISR(TIMER1_COMPA_vect){
    PORTB ^= (1 << PB5);
}

int main(void) {
   //
   // main
   //
   static uint8_t i,j;
   //
   // initialize bridge pins
   //
  clear(bridge_port, A1);
  output(bridge_direction, A1);
  clear(bridge_port, A2);
  output(bridge_direction, A2);
  clear(bridge_port, B1);
  output(bridge_direction, B1);
  clear(bridge_port, B2);
  output(bridge_direction, B2);
  clear(PORTB, (1 << PB5));
  output(DDRB, (1 << PB5));
   //
   // main loop
   //
  /*TIMSK1 = _BV(OCIE1A);  // Enable Interrupt TimerCounter1 Compare Match A (SIG_OUTPUT_COMPARE0A)
  TCCR1A = _BW() | _BV(WGM10);  // Mode = PWM, Phase Correct
  TCCR1B = _BV(CS12) | _BV(CS10);   // Clock/1024, 0.001024 seconds per tick
  ICR1 = 1;
  //OCR1A = 1023;          // 0.001024*244 ~= .25 SIG_OUTPUT_COMPARE0A will be triggered 4 times per second.
   
  sei();*/

  clear(bridge_port, A2);
  clear(bridge_port, B2);
  set(bridge_port, A1);
  set(bridge_port, B1);
  /*while(1){
    for(i = 0; i < step_count/2; i++){
        step_ccw();
    }
    for(i = 0; i < step_count; i++){
      step_cw();
    }
    for(i = 0; i < step_count/2; i++){
      step_ccw();
    }
  }*/
}