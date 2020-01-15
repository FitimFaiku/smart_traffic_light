/*
* Author: Bushra (Belleza-bashora@hotmail.com)
*
* This code implements the followimng 4 functions of traffic lights:
* void SwitchRedTL();
* void SwitchGreenTL();
* void SwitchYellowTL();
* void BlinkYellowTL();
* void BlinkGreenTL();
*
* This example is configured for a atmega328p and could be used for other
* similarly functioning controllers
* the WS2812 string connected to PB1 and can be changed in ws2812_config.h.
*
*
*/
#define F_CPU 16000000 
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "light_ws2812.h"


/*#define Rot  0xff
#define Green  0xff
#define Blue  0xff
#define rot_0  0x00 // no color
#define green_0  0x00 // no color
#define blue_0  0x00 // no color
*/

//slave 1 PKW
struct cRGB rgb[23];

void SwitchRedPL(){
	for (uint8_t i = 0; i<12; i++){
		rgb[i].r = Rot;
		rgb[i].g = green_0;
		rgb[i].b = blue_0;
	}
	for (uint8_t i = 12; i<24; i++){
		rgb[i].r = rot_0;
		rgb[i].g = green_0;
		rgb[i].b = blue_0;
	}
	ws2812_setleds(rgb,24); //rot
}
void SwitchGreenPL(){
	for (uint8_t i = 0; i<12; i++){
				rgb[i].r = rot_0;
				rgb[i].g = green_0;
				rgb[i].b = blue_0;
			}
			for (uint8_t i = 12; i<24; i++){
				rgb[i].r = rot_0;
				rgb[i].g = Green;
				rgb[i].b = blue_0;
			}
			ws2812_setleds(rgb,24); // green
}
void BlinkGreenPL(){
	for (uint8_t i = 0; i<12; i++){
				rgb[i].r = rot_0;
				rgb[i].g = green_0;
				rgb[i].b = blue_0;
			}
			for (uint8_t i = 12; i<24; i++){
				rgb[i].r = rot_0;
				rgb[i].g = Green;
				rgb[i].b = blue_0;
			}
			ws2812_setleds(rgb,24); // blink green
			_delay_ms(1000);
			for (uint8_t i = 11; i<24; i++){
				rgb[i].r = rot_0;
				rgb[i].g = green_0;
				rgb[i].b = blue_0;
			}
			ws2812_setleds(rgb,24); //green ausschalten
			_delay_ms(1000);
}
void SwitchYellowTL(){
			for (uint8_t i =0; i<11; i++){
				rgb[i].r = rot_0;
				rgb[i].g = green_0;
				rgb[i].b = blue_0;
			}
			for (uint8_t i = 11; i<23; i++){
				rgb[i].r = Rot;
				rgb[i].g = Green;
				rgb[i].b = blue_0;
			}
			for (uint8_t i = 23; i<35; i++){
				rgb[i].r = rot_0;
				rgb[i].g = green_0;
				rgb[i].b = blue_0;
			}
			ws2812_setleds(rgb,35); //gelb
}
void BlinkYellowTL(){
	for (uint8_t i =0; i<11; i++){
			rgb[i].r = rot_0;
			rgb[i].g = green_0;
			rgb[i].b = blue_0;
		}
		for (uint8_t i = 11; i<23; i++){
			rgb[i].r = Rot;
			rgb[i].g = Green;
			rgb[i].b = blue_0;
		}
		for (uint8_t i = 23; i<35; i++){
			rgb[i].r = rot_0;
			rgb[i].g = green_0;
			rgb[i].b = blue_0;
		}
		ws2812_setleds(rgb,35); //gelb
		_delay_ms(1000);
		for (uint8_t i = 11; i<23; i++){
			rgb[i].r = rot_0;
			rgb[i].g = green_0;
			rgb[i].b = blue_0;
		}
		ws2812_setleds(rgb,35); // gelb ausschalten
		_delay_ms(1000);
}
void BlinkGreenTL(){
	for (uint8_t i =0; i<11; i++){
			rgb[i].r = rot_0;
			rgb[i].g = green_0;
			rgb[i].b = blue_0;
		}
		for (uint8_t i = 11; i<23; i++){
			rgb[i].r = rot_0;
			rgb[i].g = green_0;
			rgb[i].b = blue_0;
		}
		for (uint8_t i = 23; i<35; i++){
			rgb[i].r = rot_0;
			rgb[i].g = Green;
			rgb[i].b = blue_0;
		}
		ws2812_setleds(rgb,35); //gelb
		_delay_ms(1000);
		for (uint8_t i = 11; i<23; i++){
			rgb[i].r = rot_0;
			rgb[i].g = green_0;
			rgb[i].b = blue_0;
		}
		ws2812_setleds(rgb,35); // gelb ausschalten
		_delay_ms(1000);
}
void NoLights(){
	for (uint8_t i = 0; i<24; i++){
		rgb[i].r = rot_0;
		rgb[i].g = green_0;
		rgb[i].b = blue_0;
	}
	ws2812_setleds(rgb,24); // gelb ausschalten
	 
}


