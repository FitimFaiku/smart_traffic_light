/*

Test the DOGM-Display over SPI.

*/
#define F_CPU 16000000 
#include "dogm_lcd.h"
#include <avr/io.h> // SFR definitions, bit positions etc.
#include <util/delay.h>
#include <avr/interrupt.h> // Interrupts importieren
#include <avr/sleep.h>


void setTime(uint8_t hour, uint8_t minutes, uint8_t seconds){
    lcdWriteChar(hour);
    lcdWriteChar(':');
    lcdWriteChar(minutes);
    lcdWriteChar(':');
    lcdWriteChar(seconds);
    
}
volatile
ISR(TIMER0_OVF_vect){ // timer 0 overflow interrupt service routine (1 ms)
    static uint8_t cnt_ms=0, cnt_s=0, cnt_min=0, cnt_hour=0;; // gloabl lifetime, local visibillity Counter for miliseconds
    TCNT0 = 6; // counter auf 6 --> jede 256-6= 250 ticks --> 1 ms
    if(cnt_ms++>=100){
        cnt_s++;
        if(cnt_s>=60){
            cnt_min++;
            if(cnt_min>=60){
                cnt_hour++;
                if(cnt_hour>=24){
                    cnt_hour=0;
                }
                cnt_min=0;
            }
            cnt_s=0;
        }
        cnt_ms=0;
        setTime(cnt_hour,cnt_min,cnt_s);
    }
    
}

void LCD_and_Spi_Init(){
    // Initialize the SPI interface for the LCD display
    lcdSpiInit();
    
    // Initialize the LCD display
    lcdInit();
}

int main(void) {
    TCCR0B = 3; // prescaler 64 -> 4us tick time, 250 ticks -- 1 ms
    TIMSK0 = 1 ; // enablen der overflow interrupts
    TCNT0 = 6; // counter auf 6 --> jede 256-6= 250 ticks --> 1 ms
    sei(); //enable interrupts(globally)
    
    LCD_and_Spi_Init();

	_delay_ms(2000);
       // TBD: implement lcdWriteString ...

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
    //Sleep mode setzen(configurieren)
    set_sleep_mode(SLEEP_MODE_IDLE);
	while (1){
        sleep_mode(); //save power
		//for(uint8_t i=0; i<5; i++) {
		//	lcdWriteString("Hello World");
		//}
	} //endlessly

/*we never reach this*/
return 0;
}





