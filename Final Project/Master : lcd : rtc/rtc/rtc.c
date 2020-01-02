/*
 * RTC.c    RTC: Real Time Clock
 * The used chip is DS1302
 * Datasheet: https://datasheets.maximintegrated.com/en/ds/DS1302.pdf
 * 
 * 
 * Created: 1.12.2019 11:42:36
 * Author : Bushra Yasin
 * 
 * The DS1302  does not support SPI or TWI. According to Datasheet DS1302 communicates "via a simple serial interface"
 * In this code the bit-bang techniques will be used for data transmission.
 * The DS1302 uses a single bidirectional I/O pin.
 * A clock cycle is a sequence of a rising edge followed by a falling edge.
 * Writing is done on rising edge of the next eight SCLK cycles and
 * Reading is done on falling edge of the next eight SCLK SCL cycle.
 * 
 * The RTC chips count in BCD (packed)
 * 
 * Note: everytime you start minicom, the RTC will be reinitialed.
 * So if you don't want that, you have to unplug the CE-, IO- & SCL-Cabel before you start minicom
 * 
 * Copyright (C) - 2019 Bushra Yasin
 */ 

#define F_CPU 16000000

#include "rtc.h"
#include <stdio.h>
#include <avr/io.h> 
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h> //for itoa function


void DS13xx_Init(void);
void DS13xx_Reset(void);
unsigned char DS13xx_ReadByte(void); // read one byte from the device
void DS13xx_WriteByte(unsigned char); 
void DS13xx_Write_CLK_Registers(void);
void DS13xx_Read_CLK_Registers(void);

#define DS13xx_CE 5	//CHIP SELECTION
#define DS13xx_IO 6 //Data
#define DS13xx_SCLK 7 //Clock

#define BAUDRATE 115200

//TODO define a Methode where the Time is send to the slave module(lcd Display).

//0b1000000=40=@
//0b1000001=41=A
//0b1010000=50
//0b1011000=58='0'
unsigned char ClockSecond = 0b01000000; //40 100=4 0000=0 -> 40
unsigned char ClockMinute = 0b01000000;
unsigned  char ClockHour = 0b00010011; //13  0001=1 0011=3 -> 13
//unsigned char ClockDay;
//unsigned char ClockMonth;

void init_uart(uint32_t baudrate)
{
	uint16_t br = (F_CPU / 8 / baudrate)-1;
	UBRR0H = (uint8_t) (br >> 8);
	UBRR0L = (uint8_t) (br & 0xff);
	UCSR0A |= (1<< U2X0); //activate double speed mode for uart //is same as 1<<1 
	
	// frame formate is correct no change in UCSROC 
	UCSR0B |=(1<<TXEN0) + (1<<RXEN0); //1<<7  // enable receive and transmit
}
void uart_transmit(uint8_t c){
		while ((UCSR0A & (1<<UDRE0)) == 0); //or 1<<5 // waits until UDR empty
		UDR0 = c; // send character c
}
uint8_t uart_receive ()
{
	while (UCSR0A & (1<<RXC0) == 0); //waits until character received
	return(UDR0); //returns received character from function
}
uint8_t uart_receive_nb (uint8_t * received)  // non-blocking receive !!
{
	if ( (UCSR0A & (1 << RXC0)) == 0)  return(0) ;    // when it dosen't receive anything, returns 0 
	* received = UDR0;    // the received character will be read and saved in "received"
	return(1);            // returns 1, when it recived a Character d
}
void uart_sendstring (char * str){
	while (*str){
	uart_transmit(*str);
			str++;
	}
}

// Setup the DS13xx hardware
void DS13xx_Init(void) {

// Initialise PORTA for the DS13xx clock
	 DDRD = (1<<DS13xx_CE) | (1<<DS13xx_IO) | (1<<DS13xx_SCLK);

// Initialize the clock first, to prevent false triggering
	 DS13xx_WriteByte(0x8E); // Send DS13xx command
	 DS13xx_WriteByte(0x00); // Disable DS13xx Write Protect
	 DS13xx_Reset();	 	 
	 DS13xx_WriteByte(0x80); // Send DS13xx command
	 DS13xx_WriteByte(0x00); // Start DS13xx internal oscillator
	 DS13xx_Reset();

	 DS13xx_WriteByte(0x81); // Send DS13xx command
	 DS13xx_Reset();
}

/***************************************************************************/
void DS13xx_Reset(void) { // reset and enable the 3-wire interface
	 PORTD &= ~(1<<DS13xx_SCLK);// unset SCLK (low), if we want to read a byte,SCLK has to be high again
	 PORTD &= ~(1<<DS13xx_CE);  // after every call of the DS13xxx_Reset, the CE be unset then set
	 PORTD |= (1<<DS13xx_CE); // CE  input  high  initiates  all  data  transfers 
							  // -> CE is high during the reading and the writing
	 //IN BURST MODE, CE IS KEPT HIGH AND ADDITIONAL SCLK CYCLES ARE SENT UNTIL THE END OF THE BURST.
}

void DS13xx_WriteByte(unsigned char W_Byte) { // write one byte to the device
	 unsigned char i;
	 
	 for (i = 0; i < 8; ++i) { 
		 if(W_Byte & 0x01) PORTD |= (1<<DS13xx_IO); //LSB //if i-bit of W_Byte=1, send 1 to PORTD, 
		 else PORTD &= ~(1<<DS13xx_IO); // if bit=0, unset bit
		 PORTD |= (1<<DS13xx_SCLK); // everytime after writting or deleting a bit, the SCL will be set & unset
		 PORTD &= ~(1<<DS13xx_SCLK);//SCLK is low,during the reading will be high.
		 //input on the rising edge of the next eight SCLK cycles
		 W_Byte >>= 1;		 
	 }
}

unsigned char DS13xx_ReadByte(void) { // read one byte from the device	
		 unsigned char i;
		 unsigned char R_Byte = 0;
		 unsigned char T_Byte = 0;
		 
		 DDRD &= ~(1<<DS13xx_IO); //DS13xx_IO IS AN OUTPUT
		 for (i = 0; i < 8; i++) {
			 PORTD |= (1<<DS13xx_SCLK);//reading on falling edge of the next eight SCLK cycles.
			 if (PIND & (1<<DS13xx_IO)) T_Byte = (1<<DS13xx_IO);
			 else T_Byte = 0x00;
		 	 PORTD &= ~(1<<DS13xx_SCLK);//SCLK is high,during the reading will be low.
			 T_Byte <<= 1; 
			 R_Byte >>= 1;//LSB
			 R_Byte |= T_Byte;
			 //1. Round send 1: T_Byte=0100 0000 -> 1000 0000 -> R_Byte=0000 0000 -> R_Byte=0000 0000 -> R_Byte=1000 0000
			 //2. Round send 1: T_Byte=0100 0000 -> 1000 0000 -> R_Byte=1000 0000 -> R_Byte=0100 0000 -> R_Byte=1100 0000
			 //...
	 	 }
		 DDRD |= (1<<DS13xx_IO);
	 	 return (R_Byte & 0x7F);
}


void DS13xx_Read_CLK_Registers(void) { // loop read & display clock registers

		//read word-at-a-time then reset // no Burst mode has been used while reading
		//read sec -> reset -> read min -> reset etc
	 	DS13xx_Reset(); //SCLK=0
	 	DS13xx_WriteByte(0x81); //SCLK=0 //0x81 for sec
	 	ClockSecond = DS13xx_ReadByte();//SCLK=1
	 	//z.B: 42 in packed BCD -> 0b0100 0010
	 	//z.B: 31 in packed BCD -> 0b0011 0001
	 	//ClockSecond = 40 in BCD = 0b0100 0000  
		//0000 + (0100=4) * (1010=10) = 0000 + (101000=40) = 101000040
		//ClockSecond = 40

	 	ClockSecond = (ClockSecond & 0x0F) + ((ClockSecond & 0x70)>>4)*10;
		DS13xx_Reset();
		char buffer[3];
		itoa(ClockSecond,buffer,10); //converting the int value of ClockSecond to decimal value (10),
									 //which will be saved in an array of char (buffer)
		uart_sendstring(" Second:");
		uart_sendstring(buffer);
	
	 	DS13xx_WriteByte(0x83);	// for reading the min
	 	ClockMinute = DS13xx_ReadByte();
	 	ClockMinute = (ClockMinute & 0x0F) + ((ClockMinute & 0x70)>>4)*10;
		DS13xx_Reset();
		char bufferMinute[3];
		itoa(ClockMinute,bufferMinute,10);
		uart_sendstring(" Minute:");
		uart_sendstring(bufferMinute);

	 	DS13xx_WriteByte(0x85);	// for reading the hour
	 	ClockHour = DS13xx_ReadByte();
	 	ClockHour = (ClockHour & 0x0F) + ((ClockHour & 0x10)>>4)*10;
		DS13xx_Reset();
		char bufferHour[3];
		itoa(ClockHour,bufferHour,10);
		uart_sendstring(" Hour:");
		uart_sendstring(bufferHour);
		uart_sendstring("\r\n");
}

void DS13xx_Write_CLK_Registers(void) { // initialize time & date from user entries
	 DS13xx_Reset();
	 //TODO: USE THE word-at-a-time mode AT READING 
	 DS13xx_WriteByte(0xbe); /* clock burst write (eight registers) */
	 DS13xx_WriteByte(ClockSecond); 
	 DS13xx_WriteByte(ClockMinute);
	 DS13xx_WriteByte(ClockHour); //if 7.bit=0 DS1302 counts from 0-23.
								  //If 7.bit=1 it counts from 1-12, in this case 5.bit=1 -> PM,  if 5.bit=0 -> AM
	 DS13xx_Reset();
}

unsigned char get_Current_Hour(void){
	DS13xx_WriteByte(0x85);	// for reading the hour
	ClockHour = DS13xx_ReadByte();
	ClockHour = (ClockHour & 0x0F) + ((ClockHour & 0x10)>>4)*10;
	return ClockHour;
}


void init_DS13xx(){
	DS13xx_Init();
	DS13xx_Write_CLK_Registers();
}

//int main(void){
	
	/* init_uart(BAUDRATE);
	uart_sendstring("Welcom!\r\n"); 	
	DS13xx_Init();
	DS13xx_Write_CLK_Registers(); */
	
	//while(1){
		/*
		_delay_ms(1000);
		DS13xx_Read_CLK_Registers(); // The Register will be read every Sec=1000ms
		uart_sendstring("\r\n");
		*/
//	}
//}
