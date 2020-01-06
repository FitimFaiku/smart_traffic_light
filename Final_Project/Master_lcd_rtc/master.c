#include "rtc/rtc.h"
#include "lcd/dogm_lcd.h"
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h> //for itoa function
#include <stdio.h> // For sprintf
#include <avr/sleep.h>

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

void check_current_state_and_update_if_needed(void);
static char current_state = '0';

void SPI_MasterInit(void) {
    // Set MOSI and SCK output, all others input
    PORT_DIRECTION |= (1 << MOSI) | (1 << SCK) | (1<<SS);
    // Enable SPI, Master, set clock rate fck/16
    SPCR |= (1 << SPE) | (1 << MSTR) | (1 << SPR0) | (1 << SPR1);
}


uint8_t SPI_MasterTransmit(char cData) {
    // Start transmission
    SPDR = cData;
    // Wait for transmission complete
    while (!(SPSR & (1 << SPIF)));
    
    return SPDR;
}

char SPI_MasterReceive(void) {
    SPDR = 0x00;
    /* Wait for reception complete */
    while (!(SPSR & (1 << SPIF)));
    /* Return Data Register */
    return SPDR;
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

ISR(TIMER0_OVF_vect){ // timer 0 overflow interrupt service routine (1 ms)
	// TODO get those values from the master module and set them initialy and afterwards display them!!!
    static uint8_t menueopencounter=0, cnt_ms=0,cnt_ms_ten=0, cnt_s=0, cnt_min=26, cnt_hour=11; // gloabl lifetime, local visibillity Counter for miliseconds
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
        check_current_state_and_update_if_needed();
        //setTime(menueopencounter, cnt_hour,cnt_min,cnt_s);
        menueopencounter++;
        uart_transmit_string("I bims der Master huier \n\r");
        DS13xx_Read_CLK_Registers();
    }
    
}

void check_current_state_and_update_if_needed(void) {
    //TODO check if slave gives some message here and react on that.
    char slave_message = '3';
    switch (current_state){
        static uint8_t counter_send_night_mode_status = 0;
        static uint8_t datafromslave = 0;
        static char last_state = '0';
		case '0':
		if(counter_send_night_mode_status++>=5){
			SS_SELECT
			_delay_ms(100);
			datafromslave = SPI_MasterTransmit('A');
            uart_transmit_string("2 gesendet \n \r");
            uart_transmit_string("Und bekommen");
            uart_transmit(datafromslave+48);
            uart_transmit_string("\n \r");
			SS_UNSELECT
			counter_send_night_mode_status=0;
		}
		uart_transmit_string("Current Status is 0 \n\r");
		break;
        case '1' :
        uart_transmit_string("Current Status is 1 \n\r");
        //Day Mode(will not be  send) -->  Init function set traffic Light green and Walker Red 
        break;
        case '2':
        if(current_state != last_state){
            uart_transmit_string("2 senden\n\r");
            //Night Mode --> Send to slaves code 2 
            SS_SELECT
            _delay_ms(100);
            SPI_MasterTransmit('2');
            uart_transmit_string("2 Gesendet\n\r");
            last_state = '2';
            SS_UNSELECT
        }
        break;
        //Switch to Red Traffic Light and Green Walkers Traffic Light
        case '3':
        break;
        case '4':
        //Switch to Green Traffic Light and Red Walkers Traffic Light
        break;
        case '5':
        //Switch to Yellow <b>Cars</b> Traffic Light
        break;
        case '6':
        //Someone is near the <b>Walkers</b> Trafic Light (Slave to Master communication)
        break;
        case '7':
        //Someone is near the <b>Cars</b> Trafic Light (Slave to Master communication)
        break;
        default:
        uart_transmit_string("default");
        uart_transmit(current_state);
        break;
        

    }
}

void check_current_hour_and_update_current_state(){
    uint8_t current_hour = get_current_hour();
    // 21-6:00 Nachtmodus TODO right times here
    if(current_hour>=21){ 
        //Night mode
        uart_transmit_string("Nachtmodus");
        //uart_transmit(current_hour+48);
        current_state = '2';
        uart_transmit_string("Nachtmodus, status: ");
        uart_transmit(current_state);
    } else {
        // Day mode
        uart_transmit_string("Tagesmodus");
        current_state = '1';
    }
}

int main() {
	
    uart_init(115200);
    uart_transmit_string("I bims der Master\n\r");
    //unsigned char currentHour;

    DDRB |= SS;
    SPI_MasterInit();
    // INIT for the real time clock
    init_DS13xx();

    // Initialize the SPI interface for the LCD display
    // Initialize the LCD display
    //LCD_and_Spi_Init();
    
    check_current_hour_and_update_current_state();

    //Enable interrupts
    TCCR0B = 3; // prescaler 64 -> 4us tick time, 250 ticks -- 1 ms
    TIMSK0 = 1 ; // enablen der overflow interrupts
    TCNT0 = 6; // counter auf 6 --> jede 256-6= 250 ticks --> 1 ms
    sei(); //enable interrupts(globally)
    
    //Sleep mode setzen(configurieren)
    set_sleep_mode(SLEEP_MODE_IDLE);
    
    while(1){
        sleep_mode(); //save power
    }
}

