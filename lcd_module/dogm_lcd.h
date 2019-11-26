/*! 
  \file       dogm_lcd.h
  \brief      Library for a EA DOGM162L-A 16x2 display

              This library includes all functions to work with the EA DOGM162L-A 
			  16x2 display. The library is written for the Arduino Uno Board
			  with Atmega328p microcontroller with a 16MHz clock. Before the 
			  display function could be used the SPI interface of the controller
			  must be initialized in master mode with a speed of (fosc /32).The
			  functions are segmented in control functions and write functions.

  \attention  The information contained herein is confidential property of the
              Institute of Embedded Systems - Technikum Wien. The use, copying,
              transfer or disclosure of such information is prohibited except 
              by express written agreement with the Embedded Systems Institute.

  \author     H. Schuster
  \date       25/10/2011
  \version    1.0

*/

#ifndef DOGM_LCD_H
#define DOGM_LCD_H

// IO include file to use the

#include <avr/io.h>
#include <util/delay.h>

/*!
  \def        F_CPU
              Define for the CPU clock for the delay functions.
*/
#ifndef F_CPU
#define F_CPU    16000000UL // needed for the delay function (util/delay.h)
#endif

/*!
  \def        LCD_ON/OFF
              Switches the display on and off. Used for function "lcdOnOff".
*/
#define LCD_ON 1
#define LCD_OFF 0

/*!
  \def        CURSOSR_ON/OFF
              Switches the cursor on and off. Used for function "lcdCursorOnOff".
*/
#define CURSOR_ON 1
#define CURSOR_OFF 0

/*!
  \def        POSITION_ON/OFF
              Switches the position on and off. Used for function "lcdCursorOnOff".
*/
#define POSITION_ON 1
#define POSITION_OFF 0

/*!
  \def        LINE_1/2
              Defines the used row of the display. Used for function "lcdSetCursor" and "lcdWriteString".
*/
#define LINE1 0
#define LINE2 1

/*!
  \def        TWO_LINES_ON/OFF
              Defines if one or two lines are used. Used for function "lcdWriteString".
*/
#define TWO_LINES_ON 1
#define TWO_LINES_OFF 0

/*!
  \def        OK/Error
              Defines the return values of the function "lcdWriteString".
*/
#define OK 0
#define ERROR 1

/*
	define function bytes for initialisation. Found in dog-me.pdf site 7/8.
*/

#define FUNCTION_SET 0b00111000
#define BIAS_SET 0b00011100
#define POWER_CONTROL 0b01010010
#define FOLLOWER_CONTROL 0b01101001
#define CONTRAST_SET 0b01110100
#define DISPLAY_ON 0b00001111
#define DISPLAY_OFF 0b00000111
#define CLEAR_DISPLAY 0b00000001
#define ENTRY_MODE 0b00000110
#define ENABLE_CURSOR 0b00001110
#define DISPLAY_START_ADRESS 0b10000000
#define CURSOR_STATE 0b00001100

// Functions to control the display
void lcdSpiInit();
void lcdInit();
void lcdOnOff(uint8_t mode);
void lcdClear();
void lcdSetCursor(uint8_t row, uint8_t col);
void lcdCursorOnOff(uint8_t cursorOnOff, uint8_t positionOnOff);

// Functions to write to the display
void lcdWriteChar(char x);
void lcdWriteString(char *str);
#endif /*DOGM_LCD_H*/
