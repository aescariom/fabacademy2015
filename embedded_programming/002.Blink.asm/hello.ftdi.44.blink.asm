;
; hello.ftdi.blink.44.asm
;
; Led + button - 
; Alejandro Escario - FabAcademy 2015


.include "tn44def.inc"

.equ led_pin = PA7; led pin is PA7
.equ button_pin = PA3; 
.equ led_port = PORTA; comm port
.equ led_dir = DDRA; comm direction

; program is in lower part of memory
.cseg ; code segment init
.org 0 ; sets the location counter to an absolute value
rjmp reset 


;
; main program
;
reset:
   ; set pin to output
   sbi led_dir, led_pin
   
   ;
   ; start main loop
   ;
   loop:
   		sbic PINA, button_pin
   		cbi led_port, led_pin ; set led pin
   		SBIS PINA, button_pin
   		sbi led_port, led_pin
       rjmp loop