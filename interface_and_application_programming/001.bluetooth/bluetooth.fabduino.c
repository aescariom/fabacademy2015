/********************************************************
*
*  bluetooth.fabduino.c
* 
*     9600 baud FTDI interface
* 
*  Alejandro Escario Méndez
*   29/04/2015  
*
*  MIT license
*********************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "../lib/definitions.h"
#include "../lib/serial.h"

#define output(directions,pin) (directions |= pin) // set port direction for output
#define input(directions,pin) (directions &= (~pin)) // set port direction for input
#define set(port,pin) (port |= pin) // set port pin
#define clear(port,pin) (port &= (~pin)) // clear port pin
#define pin_test(pins,pin) (pins & pin) // test for port pin
#define bit_test(byte,bit) (byte & (1 << bit)) // test for bit set

#define RGB_PORT PORTB
#define RGB_DIRECTION DDRB
#define RGB_PIN PINB
#define RGB_RED PB1
#define RGB_GREEN PB0
#define RGB_BLUE PB2

#define BUZZER_PORT PORTD
#define BUZZER_DIRECTION DDRD
#define BUZZER_PIN PIND
#define BUZZER_I PD7

#define BUTTON_PORT PORTD
#define BUTTON_DIRECTION DDRD
#define BUTTON_PIN PIND
#define BUTTON_I PD2

#define lcd_delay() _delay_ms(10) // delay between commands
#define strobe_delay() _delay_us(1) // delay for strobe
#define LCD_PORT PORTC
#define LCD_DIRECTION DDRC
#define LCD_DB7 (1 << PC0)
#define LCD_DB6 (1 << PC1)
#define LCD_DB5 (1 << PC2)
#define LCD_DB4 (1 << PC3)
#define LCD_E (1 << PC4)
#define LCD_RS (1 << PC5)

#define JOY_X 7
#define JOY_Y 6

int x = 0, y = 0, target_temp = 3650;
char buff[6];

void button_init(){
    set(BUTTON_PORT, (1 << BUTTON_I)); // turn on pull-up
    input(BUTTON_DIRECTION, (1 << BUTTON_I));
}

void button_on_click(void (*fn)()){
    if (0 == pin_test(BUTTON_PIN, (1 << BUTTON_I))){
      (*fn)();
    }
}

void buzzer_init(){
   clear(BUZZER_PORT, (1 << BUZZER_I));
    output(BUZZER_DIRECTION, (1 << BUZZER_I));
}

void buzzer_beep(){
   set(BUZZER_PORT, (1 << BUZZER_I));
   _delay_ms(10);
   clear(BUZZER_PORT, (1 << BUZZER_I));
}

void rgb_init(){
   clear(RGB_PORT, (1 << RGB_RED));
    output(RGB_DIRECTION, (1 << RGB_RED));
   clear(RGB_PORT, (1 << RGB_GREEN));
    output(RGB_DIRECTION, (1 << RGB_GREEN));
   clear(RGB_PORT, (1 << RGB_BLUE));
    output(RGB_DIRECTION, (1 << RGB_BLUE));

    rgb_off();
}

void rgb_green(){
   clear(PORTB, (1 << RGB_GREEN));
   set(PORTB, (1 << RGB_RED));
   set(PORTB, (1 << RGB_BLUE));
}

void rgb_off(){
   set(PORTB, (1 << RGB_GREEN));
   set(PORTB, (1 << RGB_RED));
   set(PORTB, (1 << RGB_BLUE));
}

void rgb_red(){
   clear(PORTB, (1 << RGB_RED));
   set(PORTB, (1 << RGB_GREEN));
   set(PORTB, (1 << RGB_BLUE));
}

void rgb_blue(){
   set(PORTB, (1 << RGB_RED));
   set(PORTB, (1 << RGB_GREEN));
   clear(PORTB, (1 << RGB_BLUE));
}

void rgb_yellow(){
   clear(PORTB, (1 << RGB_GREEN));
   clear(PORTB, (1 << RGB_RED));
   set(PORTB, (1 << RGB_BLUE));
}

void rgb_white(){
   clear(PORTB, (1 << RGB_GREEN));
   clear(PORTB, (1 << RGB_RED));
   clear(PORTB, (1 << RGB_BLUE));
}

void joystick_init(){
   ADCSRA = (1 << ADEN) // enable
      | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // prescaler /128
}

void joystick_change_port(volatile uint8_t pc){
   ADMUX = (0 << REFS1) | (0 << REFS0) // VCC ref
      | (0 << ADLAR) // right adjust for 10bit precision
      | pc;
}

int joystick_read_x(){
   joystick_change_port(JOY_X); 
   return read_adc();
}

int joystick_read_y(){
   joystick_change_port(JOY_Y); 
   return read_adc();
}

int read_adc(){
   ADCSRA |= (1 << ADSC); // conversion init
   while (ADCSRA & (1 << ADSC)); // wait for completion
   return ADC; // return value
}

//
// lcd_putchar
//    put character in lcdbyte
//
void lcd_putchar(char lcdbyte) {
   //
   // set RS for data
   // 
   set(LCD_PORT, LCD_RS);
   //
   // output high nibble
   //
   if bit_test(lcdbyte, 7)
      set(LCD_PORT, LCD_DB7);
   else
      clear(LCD_PORT, LCD_DB7);
   if bit_test(lcdbyte, 6)
      set(LCD_PORT, LCD_DB6);
   else
      clear(LCD_PORT, LCD_DB6);
   if bit_test(lcdbyte, 5)
      set(LCD_PORT, LCD_DB5);
   else
      clear(LCD_PORT, LCD_DB5);
   if bit_test(lcdbyte, 4)
      set(LCD_PORT, LCD_DB4);
   else
      clear(LCD_PORT, LCD_DB4);
   //
   // strobe E
   //
   strobe_delay();
   set(LCD_PORT, LCD_E);
   strobe_delay();
   clear(LCD_PORT, LCD_E);
   //
   // wait
   //
   lcd_delay();
   //
   // output low nibble
   //
   if bit_test(lcdbyte, 3)
      set(LCD_PORT, LCD_DB7);
   else
      clear(LCD_PORT, LCD_DB7);
   if bit_test(lcdbyte, 2)
      set(LCD_PORT, LCD_DB6);
   else
      clear(LCD_PORT, LCD_DB6);
   if bit_test(lcdbyte, 1)
      set(LCD_PORT, LCD_DB5);
   else
      clear(LCD_PORT, LCD_DB5);
   if bit_test(lcdbyte, 0)
      set(LCD_PORT, LCD_DB4);
   else
      clear(LCD_PORT, LCD_DB4);
   //
   // strobe E
   //
   strobe_delay();
   set(LCD_PORT, LCD_E);
   strobe_delay();
   clear(LCD_PORT, LCD_E);
   //
   // wait and return
   //
   lcd_delay();
   }
//
// lcd_putcmd
//    put command in lcdbyte
//
void lcd_putcmd(char lcdbyte) {
   //
   // clear RS for command
   // 
   clear(LCD_PORT, LCD_RS);
   //
   // output command bits
   //
   PORTC = lcdbyte;
   //
   // strobe E
   //
   strobe_delay();
   set(LCD_PORT, LCD_E);
   strobe_delay();
   clear(LCD_PORT, LCD_E);
   //
   // wait and return
   //
   lcd_delay();
   }
//
// lcd_putstring
//    put a null-terminated string in flash
//
void lcd_putstring(char* message) {
   static uint8_t i;
   static char chr;
   i = 0;
   while (1) {
      chr = message[i];
      if (chr == 0)
         return;
      lcd_putchar(chr);
      ++i;
   }
}

void lcd_putline(char* message, int line){
   if(line == 1){
      lcd_putcmd(0);
      lcd_putcmd(LCD_DB5);
   }else if(line == 2){
      lcd_putcmd(LCD_DB7+LCD_DB6);
      lcd_putcmd(0);
   }
   lcd_putstring(message);
}

void lcd_clear(){
   lcd_putcmd(0);
   lcd_putcmd(LCD_DB4);   
}

void lcd_cursor_off(){
   lcd_putcmd(0);
   lcd_putcmd(LCD_DB7+LCD_DB6);
}
//
// lcd_init
//    initialize the LCD
//
void lcd_init() {
   //
   // initialize LCD pins
   //
   clear(LCD_PORT, LCD_DB7);
   output(LCD_DIRECTION, LCD_DB7);
   clear(LCD_PORT, LCD_DB6);
   output(LCD_DIRECTION, LCD_DB6);
   clear(LCD_PORT, LCD_DB5);
   output(LCD_DIRECTION, LCD_DB5);
   clear(LCD_PORT, LCD_DB4);
   output(LCD_DIRECTION, LCD_DB4);
   clear(LCD_PORT, LCD_E);
   output(LCD_DIRECTION, LCD_E);
   clear(LCD_PORT, LCD_RS);
   output(LCD_DIRECTION, LCD_RS);
   //
   // power-up delay
   //
   lcd_delay();
   //
   // initialization sequence
   //
   lcd_putcmd(LCD_DB5+LCD_DB4);
   lcd_putcmd(LCD_DB5+LCD_DB4);
   lcd_putcmd(LCD_DB5+LCD_DB4);
   //
   // 4-bit interface
   //
   lcd_putcmd(LCD_DB5);
   //
   // two lines, 5x7 font
   //
   lcd_putcmd(LCD_DB5);
   lcd_putcmd(LCD_DB7);
   //
   // display on
   //
   lcd_putcmd(0);
   lcd_putcmd(LCD_DB7+LCD_DB6+LCD_DB5);
   //
   // entry mode
   //
   lcd_putcmd(0);
   lcd_putcmd(LCD_DB6+LCD_DB5);
} 

ISR (USART_RX_vect) {
	buff[0] = buff[1];
	buff[1] = buff[2];
	buff[2] = buff[3];
	buff[3] = buff[4];
	buff[4] = buff[5];
	buff[5] = UDR0;
	if(buff[0] == 1 && buff[1] == 2 && buff[2] == 3 && buff[3] == 4){
		target_temp=buff[4]*100+buff[5];
	}
}

void send_float(int val){
    int integer = val / 100;
    int frac = val - integer * 100;
    usart_putchar(1);
    usart_putchar(2);
    usart_putchar(3);
    usart_putchar(4);
    usart_putchar(integer);
    usart_putchar(frac);
    
    integer = target_temp / 100;
    frac = target_temp - integer * 100;
    usart_putchar(4);
    usart_putchar(3);
    usart_putchar(2);
    usart_putchar(1);
    usart_putchar(integer);
    usart_putchar(frac);
}

int main(void) {
    char line[16], num[10];
    int x, y, i = 0;
    int deg = 3650;

    buzzer_init(); 
    rgb_init();
    lcd_init();
    button_init();
    joystick_init();
    lcd_init();
    lcd_clear();
    lcd_cursor_off();

    usart_init(R_UBRR);
    sei();

    while(1){   
        deg += (rand() % 9) - 4;
        if(deg < target_temp){
        	deg += 2;
        	rgb_red();
        }else if(deg > target_temp){
			deg -= 2;
			rgb_blue();
        }else{
        	rgb_green();
        }
        //dtostrf(deg, 1, 2, num);
        sprintf(line, "%d/%d", deg, target_temp);
        lcd_putline(line, 1);
        
        send_float(deg);
        _delay_ms(500);
    }
}