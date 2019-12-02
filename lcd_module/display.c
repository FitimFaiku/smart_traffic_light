/*

Test the DOGM-Display over SPI.

*/
#define F_CPU 16000000 
#include "dogm_lcd.h"
#include <avr/io.h>
#include <util/delay.h>


void setTime(uint8_t hour, uint8_t minutes, uint8_t seconds){
    lcdWriteChar(hour);
    lcdWriteChar(':');
    lcdWriteChar(minutes);
    lcdWriteChar(':');
    lcdWriteChar(seconds);
    
}

void LCD_and_Spi_Init(){
    // Initialize the SPI interface for the LCD display
    lcdSpiInit();
    
    // Initialize the LCD display
    lcdInit();
}

int main(void) {
	int i = 5;
    LCD_and_Spi_Init();

	_delay_ms(2000);
    setTime(12,32,20);   // TBD: implement lcdWriteString ...

	/** _delay_ms(2000);
	lcdSetCursor(1,1);
	lcdWriteString("der Beste!!!");
     */
	/**
	 * 
	lcdOnOff(LCD_OFF);
	
	_delay_ms(2000);
	lcdOnOff(LCD_ON);
	lcdCursorOnOff(CURSOR_OFF, POSITION_OFF);

	_delay_ms(1000);

	for (i = 0 ; i < 30; i++)
	{
		if(i < 16)
			lcdSetCursor(0, i);
		else
			lcdSetCursor(1, i-16);
		lcdWriteChar(i+8);
		_delay_ms(500);
	}

	_delay_ms(2000);

	lcdCursorOnOff(CURSOR_ON, POSITION_OFF);

	_delay_ms(2000);

	lcdOnOff(LCD_OFF);

	_delay_ms(1000);

	lcdOnOff(LCD_ON);
*/

	while (1){
		//for(uint8_t i=0; i<5; i++) {
		//	lcdWriteString("Hello World");
		//}
	} //endlessly

/*we never reach this*/
return 0;
}





