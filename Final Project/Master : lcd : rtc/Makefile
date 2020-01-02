
target = master
objs = rtc/rtc.o, lcd/dogm_lcd.o, lcd/display.o
port = /dev/ttyACM0

all: $(target).hex

# remove compiled files
clean:
	rm -f *.hex *.elf *.o

# objects from c files
.c.o:
	avr-gcc -mmcu=atmega328p -Os -c -g3 $< -o $@

# elf file
$(target).elf: $(target).o $(objs)
	avr-gcc -mmcu=atmega328p -o $@ $^ 

# hex file
$(target).hex: $(target).elf
	avr-objcopy -j .text -j .data -O ihex $< $@
	avr-size $<

flash:
	avrdude -P $(port) -b 115200 -p atmega328p -c arduino -U flash:w:$(target).hex 
