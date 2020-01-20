F_CPU = 16000000
target = slave
objs = traffic_light/light_ws2812.o traffic_light/traffic_lights.o ultrasonicsensor/ultrasonicsensor.o  buzzer.o
port = /dev/ttyACM0


CFLAGS = -g2 -I. -ILight_WS2812 -mmcu=atmega328p -DF_CPU=$(F_CPU) 
CFLAGS+= -Os -ffunction-sections -fdata-sections -fpack-struct -fno-move-loop-invariants -fno-tree-scev-cprop -fno-inline-small-functions  
CFLAGS+= -Wall -Wno-pointer-to-int-cast

all: $(target).hex

$(target): $(objs)
	@echo Building Library 
	@$(CC) $(CFLAGS) -o Objects/$@.o -c traffic_light/Light_WS2812/$@.c 

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
