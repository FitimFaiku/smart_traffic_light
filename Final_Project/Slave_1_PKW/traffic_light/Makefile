# Makefile to build light_ws2812 library examples
# This is not a very good example of a makefile - the dependencies do not work, therefore everything is rebuilt every time.

# Change these parameters for your device

F_CPU = 16000000
port = /dev/ttyACM0

# Tools:
CC = avr-gcc

LIB       = light_ws2812
EXAMPLES  = traffic_lights
DEP		  = ws2812_config.h Light_WS2812/light_ws2812.h

target = traffic_lights

CFLAGS = -g2 -I. -ILight_WS2812 -mmcu=atmega328p -DF_CPU=$(F_CPU) 
CFLAGS+= -Os -ffunction-sections -fdata-sections -fpack-struct -fno-move-loop-invariants -fno-tree-scev-cprop -fno-inline-small-functions  
CFLAGS+= -Wall -Wno-pointer-to-int-cast
#CFLAGS+= -Wa,-ahls=$<.lst

LDFLAGS = -Wl,--relax,--section-start=.text=0,-Map=main.map

all:	$(EXAMPLES) 

$(LIB): $(DEP)
	@echo Building Library 
	@$(CC) $(CFLAGS) -o Objects/$@.o -c Light_WS2812/$@.c 

$(EXAMPLES): $(LIB) 
	@echo Building $@
	@$(CC) $(CFLAGS) -o Objects/$@.o Examples/$@.c Light_WS2812/$^.c
	@avr-size Objects/$@.o
	@avr-objcopy -j .text  -j .data -O ihex Objects/$@.o $@.hex
	@avr-objdump -d -S Objects/$@.o >Objects/$@.lss

.PHONY:	clean

clean:
	rm -f *.hex Objects/*.o Objects/*.lss
	
# objects from c files 
.c.o:  
	avr-gcc -mmcu=atmega328p -Os -c $< -o $@
	#avr-gcc -c -Os -mmcu=atmega328p $< -o $@
	
 
# elf file 
$(target).elf: $(target).o $(objs) 
	avr-gcc -mmcu=atmega328p -o $@ $^ 
 
# hex file 
$(target).hex: $(target).elf  
	avr-objcopy -j .text -j .data -O ihex $< $@  
	avr-size $< 
 
flash:  
	avrdude -P $(port) -b 115200 -p atmega328p -c arduino -U flash:w:$(target).hex 
