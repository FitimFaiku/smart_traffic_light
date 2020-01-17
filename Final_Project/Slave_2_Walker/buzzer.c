
#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "buzzer.h"

void buzzer(uint8_t frequenz){
//frequenz=6;
static uint16_t count1=0,count2=0; //static: global lifetime, local visibility		
	if(count2<=1000){
		if(frequenz<count1++){
			PORTD |= (1<<4); //Pin high setzen
		} else{
			PORTD &= ~(1<<4); 				//Pin low setzen
		 }
		//count1++;
		if(count1==10){
			 count1=0;	// counter zurücksetzten
		 }
	}
	count2++;
	if(count2>=2000){
		count2=0 ;
	}
}
