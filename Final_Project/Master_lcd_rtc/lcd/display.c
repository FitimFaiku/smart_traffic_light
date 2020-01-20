/*

Test the DOGM-Display over SPI.

*/
#define F_CPU 16000000 
#include "dogm_lcd.h"
#include <avr/io.h> // SFR definitions, bit positions etc.
#include <util/delay.h>
#include <avr/interrupt.h> // Interrupts importieren
#include <avr/sleep.h>
#include <stdbool.h> // Include bool
#include <stdio.h> // For sprintf

volatile static uint8_t hour=1;
volatile static uint8_t minutes=10;
volatile static uint8_t seconds=0;
volatile static int8_t counter_till_next_interval=0;
static uint8_t counter_menue_oppened = 0;
static bool is_green = false;
static uint8_t menue_selected = 1;
static uint8_t max_amoun_of_menues = 2;


void set_hour(uint8_t given_hour){
	hour = given_hour;
}

void set_minutes(uint8_t given_minutes){
	minutes = given_minutes;
}

void set_seconds(uint8_t given_seconds){
	seconds= given_seconds;
}

void set_counter_till_next_interval(int8_t cnt_till_next_interval){
	counter_till_next_interval = cnt_till_next_interval;
}

void display_and_update_menue(){
	
	switch(menue_selected){
		case 1:
		show_and_update_time_menue();
		break;
		case 2:
		show_and_update_counter_interval_menue();
		break;
		default:
		uart_transmit_string("I am in default");
	}
}

void show_and_update_time_menue(){
	if(counter_menue_oppened<1){
		//lcdClear();
		lcdSetCursor(-1,0);
		lcdWriteString("Aktuelle Uhrzeit");
		counter_menue_oppened++;
	}
	char timeline[8];
	lcdSetCursor(0,1);
	sprintf(timeline, "%0.2dh %0.2dm %0.2ds",hour, minutes, seconds);
    lcdWriteString(timeline);
}


void show_and_update_counter_interval_menue(){
	if(counter_menue_oppened<1){
		//lcdClear();
		lcdSetCursor(-1,0);
		lcdWriteString("Intervalzeit");
		counter_menue_oppened++;
	}
	char timeline[4];
	lcdSetCursor(0,1);
	sprintf(timeline, "%0.2ds",counter_till_next_interval);
    lcdWriteString(timeline);
}

void change_menue(){
	counter_menue_oppened=0;
	lcdClear();
	menue_selected++;
	// Reset menue counter to 1 because max length of menues is reached.
	if(menue_selected>max_amoun_of_menues){
		menue_selected=1;
	}
}


void setTime(uint8_t counter, uint8_t hour, uint8_t minutes, uint8_t seconds){
	if(counter_menue_oppened<1){
		//lcdClear();
		lcdSetCursor(-1,0);
		lcdWriteString("Aktuelle Uhrzeit");
		counter_menue_oppened++;
	}
	char timeline[9];
	lcdSetCursor(0,1);
	sprintf(timeline, "%0.2dh %0.2dm %0.2ds",hour, minutes, seconds);
    lcdWriteString(timeline);
}


void LCD_and_Spi_Init(){
    // Initialize the SPI interface for the LCD display
    lcdSpiInit();
    
    // Initialize the LCD display
    lcdInit();
}





