;
; hello.ftdi.44.blink.interrupt.asm
;
; Led + button (interrupt)
; Modified by Alejandro Escario - FabAcademy 2015


.include "tn44def.inc" ; constant definitions

.equ led_pin = PA7; led pin is PA7
.equ button_pin = PA3; 
.equ led_dir = DDRA; comm direction 1 => OUTPUT (PORT)    0 => INPUT (PIN)

.def temp = R16; temporary storage 

.cseg ; code segment init
.org $0000 
	rjmp reset ; sets the location counter to an absolute value
	rjmp 0 ; interrupt 0
	rjmp pcint0_handler
	

;
; main program
;
reset:
   ; set pin to output
   sbi led_dir, led_pin

   ;
   ;interrupt enable for PA3
   ;
	ldi temp, (1 << PCIE0)
	out GIMSK, temp
	ldi temp, (1 << PCINT3)
	out PCMSK0, temp
   
   sei ; set Global Interrupt Enable
   cbi PORTA, led_pin ; set led pin
   
loop: 
	sbic PINA, PA3 ; if the button is not pressed
   	cbi PORTA, led_pin ; turn off the led
	sleep
	nop
	rjmp loop;
	
pcint0_handler:
   	sbi PORTA, led_pin ; set led pin
	reti