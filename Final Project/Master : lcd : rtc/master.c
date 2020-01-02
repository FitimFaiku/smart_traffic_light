#include "rtc/rtc.h"
#include "lcd/dogm_lcd.h"
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h> //for itoa function

#define PORT_DIRECTION DDRB
#define PORT_VALUE PORTB

// Pin definitions (on Port B)
#define SCK  5
#define MISO 4
#define MOSI 3
#define SS   2

// Defines for the SS pin
#define SS_UNSELECT PORT_VALUE |= (1 << SS);
#define SS_SELECT PORT_VALUE &= ~(1 << SS);

void SPI_MasterInit(void) {
    // Set MOSI and SCK output, all others input
    PORT_DIRECTION |= (1 << MOSI) | (1 << SCK);
    // Enable SPI, Master, set clock rate fck/16
    SPCR |= (1 << SPE) | (1 << MSTR) | (1 << SPR0) | (1 << SPR1);
}


void SPI_MasterTransmit(char cData) {
    // Start transmission
    SPDR = cData;

    // Wait for transmission complete
    while (!(SPSR & (1 << SPIF)));
}

void uart_transmit(char character) {
    while (!(UCSR0A & (1 << 5)));
    UDR0 = (uint8_t) character;
}

void uart_transmit_string(char *string) {
    while (*string){
        uart_transmit(*string);
            string++;
        }
}

char uart_receive() {
    while (!(UCSR0A & (1 << RXC0)));
    return (char) UDR0;
}

void uart_init(uint32_t baudrate) {
    // UBRR formula from datasheet
    // 8 as we use doublespeed
    uint16_t ubrr = (F_CPU / 8 / baudrate) - 1;
    // Set baud rate, use double speed
    UBRR0H = (uint8_t)(ubrr >> 8); // get higher 8 bits
    UBRR0L = (uint8_t)(ubrr & 0xff); // get lower 8 bits
    UCSR0A |= (1 << U2X0);
    // Enable receiver and transmitter
    UCSR0B = (1 << TXEN0 | 1 << RXEN0);
    // select character size, stop and parity bits: 8N1
    // UCSR0C = (1<<UCSZ01 | 1<<UCSZ00);
    // not necessary, because UCSR0C is initialised corretly !
    // for interrupts: enable receive complete interrupt
    // UCSR0B |= (1<<RXCIE0); we do not use interrupts by now..
}

char SPI_MasterReceive(void) {
    SPDR = 0x00;
    /* Wait for reception complete */
    while (!(SPSR & (1 << SPIF)));
    /* Return Data Register */
    return SPDR;
}

ISR(TIMER0_OVF_vect){ // timer 0 overflow interrupt service routine (1 ms)
	// TODO get those values from the master module and set them initialy and afterwards display them!!!
    static uint8_t cnt_ms=0,cnt_ms_ten=0, cnt_s=0, cnt_min=26, cnt_hour=11; // gloabl lifetime, local visibillity Counter for miliseconds
    TCNT0 = 6; // counter auf 6 --> jede 256-6= 250 ticks --> 1 ms
    if(cnt_ms++>=100){
		cnt_ms_ten++;
		cnt_ms=0;
	} 
    if(cnt_ms_ten>=10){
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
        cnt_ms_ten=0;
        setTime(cnt_hour,cnt_min,cnt_s);
    }
    
}

void check_Current_State_And_Update_If_Needed(void) {

}

int main() {
    uart_init(115200);
    uart_transmit_string("I bims der Master\n\r");
    unsigned char currentHour;

    DDRB |= SS;
    SPI_MasterInit();

    // INIT for the real time clock
    init_DS13xx();

    // Initialize the SPI interface for the LCD display
    // Initialize the LCD display
    LCD_and_Spi_Init();


    currentHour = get_Current_Hour();

    char bufferHour[3];
    itoa(currentHour,bufferHour,10);
    uart_transmit_string("Current Time: ");
    uart_transmit_string(bufferHour);

    TCCR0B = 3; // prescaler 64 -> 4us tick time, 250 ticks -- 1 ms
    TIMSK0 = 1 ; // enablen der overflow interrupts
    TCNT0 = 6; // counter auf 6 --> jede 256-6= 250 ticks --> 1 ms
    sei(); //enable interrupts(globally)
    
    SS_SELECT
    _delay_ms(100);
    SPI_MasterTransmit('A');
    SS_UNSELECT


    while (1) {
       /* SS_SELECT
        _delay_ms(100);
        SPI_MasterTransmit('A');
        SS_UNSELECT
        uart_transmit_string("255 Gesendet\n\r");
        _delay_ms(100);


        SS_SELECT
        _delay_ms(100);
        SPI_MasterTransmit('Z');
        uart_transmit_string("0 Gesendet\n\r");
        SS_UNSELECT
        _delay_ms(100);
        */
    }
}

