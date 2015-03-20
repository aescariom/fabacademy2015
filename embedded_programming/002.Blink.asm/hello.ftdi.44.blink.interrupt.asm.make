PROJECT=hello.ftdi.44.blink.interrupt
SOURCES=$(PROJECT).asm

$(PROJECT).hex: $(SOURCES)
	avra $(SOURCES)
 
program-bsd: $(PROJECT).hex
	avrdude -p t44 -c bsd -U flash:w:$(PROJECT).hex

program-dasa: $(PROJECT).hex
	avrdude -p t44 -P /dev/ttyUSB0 -c dasa -U flash:w:$(PROJECT).hex

program-avrisp2: $(PROJECT).hex
	avrdude -p t44 -P usb -c avrisp2 -U flash:w:$(PROJECT).hex

program-avrisp2-fuses: $(PROJECT).hex
	avrdude -p t44 -P usb -c avrisp2 -U lfuse:w:0x5E:m

program-usbtiny: $(PROJECT).hex
	avrdude -p t44 -P usb -c usbtiny -U flash:w:$(PROJECT).hex

program-usbtiny-fuses: $(PROJECT).hex
	avrdude -p t44 -P usb -c usbtiny -U lfuse:w:0xFF:m

program-dragon: $(PROJECT).hex
	avrdude -p t44 -P usb -c dragon_isp -U flash:w:$(PROJECT).hex
