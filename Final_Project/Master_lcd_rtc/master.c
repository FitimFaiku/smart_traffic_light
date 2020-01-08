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


static char current_state = '0';
static volatile uint8_t current_hour;
static volatile uint8_t current_minute;
static volatile char last_state; 
static volatile bool should_action = false;
static volatile bool is_day_mode;
static volatile uint8_t counter_to_do_action_seconds;

void init();
void uart_init(uint32_t baudrate);
void uart_transmit(char character);
void uart_transmit_string();
char uart_receive(void);
void SPI_MasterInit(void);
//void init_DS13xx() RTC from rtc.h
//void LCD_and_Spi_Init() from dogm_lcd.h
void check_current_hour_and_initialize_volatile_variables();
//uint8_t get_current_hour() from rtc.h
//uint8_t get_current_minute() from rtc.h

void init_interrupts();
void check_should_action();

void do_action();
void check_current_state_and_do_action_if_needed();




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
    static uint8_t menueopencounter=0, cnt_ms=0,cnt_ms_ten=0, cnt_s=0, cnt_min=26; // gloabl lifetime, local visibillity Counter for miliseconds
    TCNT0 = 6; // counter auf 6 --> jede 256-6= 250 ticks --> 1 ms

    if(cnt_ms++>=100){
		cnt_ms_ten++;
		cnt_ms=0;
	} 
    if(cnt_ms_ten>=10){
        cnt_s++;
        counter_to_do_action_seconds++;
        // Every second we check if maybe someone is near the Red Traffic Light slave -> master command 6 or 7 
        check_should_action();
        if(cnt_s>=60){
            cnt_min++;
            if(cnt_min>=60){
                current_hour++;
                if(current_hour>=24){
                    current_hour=0;
                }
                cnt_min=0;
            }
            cnt_s=0;
            menueopencounter++;
            check_current_state_and_update_if_needed();
            uart_transmit_string("Updating ...\n\r");
            //setTime(menueopencounter, current_hour,cnt_min,cnt_s);
        }
        cnt_ms_ten=0;
        //DS13xx_Read_CLK_Registers();
    }
    
}

void check_should_action(){

    char slave_message = '0';
    // When walker is waiting and currently has red TODO
    if(datafromslave  == '6' && last_state == '4') {
        current_state = '3'; // Switch Yellow Car -(?msTODO)-> Switch Red Car -(?msTODO)->Switch green for Walkers Traffic Light
    }

    // When car is waiting and currently has red TODO 
    if(datafromslave == '7' && TODO){
        // Switch Red Walkers -(?msTODO)->  Switch Yellow Car -(?msTODO)-> Switch Green Car

    }
}

void do_action(void) {
    //TODO check if slave gives some message here and react on that.
    
    static uint8_t counter_seconds = 0;
    counter_seconds++;
    switch (current_state){
        static char last_state = '0';
        case '0'
        // Init and listening state.
        static uint8_t counter_send_night_mode_status = 0;
        static uint8_t datafromslave = 0;
        
        // Only if datafromslave is Someone is near the <b>Walkers</b> Trafic Light (Slave to Master communication) and this Light is currently red
        
        counter_send_night_mode_status=0;
		uart_transmit_string("Current Status is 0 \n\r");
		break;
        case '1' :
        uart_transmit_string("Current Status is 1 \n\r");
        //Day Mode(will not be  send) -->  Init function set traffic Light green and Walker Red

        set_traffic_light_green();
        SS_SELECT
        _delay_ms(100);
        SPI_MasterTransmit('2');
        uart_transmit_string("2 Gesendet\n\r");
        SS_UNSELECT



        break;
        case '2':
        last_state='2';
        uart_transmit_string("2 senden\n\r");
        //Night Mode --> Send to slaves code 2 
        SS_SELECT
        _delay_ms(100);
        SPI_MasterTransmit('2');
        uart_transmit_string("2 Gesendet\n\r");
        SS_UNSELECT

        current_state='0';
        break;
        
        case '3':
        last_state='3';
        //Switch to Red Traffic Light and Green Walkers Traffic Light

        break;
        case '4':
        last_state='4';
        //Switch to Green Cars Traffic Light and Red Walkers Traffic Light
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


/**
* Gets current hour from RTC - DS13xx and initializes cnr_h and current_status
*/
void check_current_hour_and_initialize_volatile_variables(){
    current_hour = get_current_hour();
    current_minute = get_current_minute(); // TODO this methode impl.
    // 21-6:00 Nachtmodus TODO right times here
    if(current_hour>=21){ 
        //Night mode
        uart_transmit_string("Nachtmodus");
        //uart_transmit(current_hour+48);
        current_state = '2';
        uart_transmit_string("Nachtmodus, status: ");
        uart_transmit(current_state);
        is_day_mode = false;
    } else {
        should_action = true;
        // Day mode
        uart_transmit_string("Tagesmodus");
        current_state = '1';
        is_day_mode = true;
    }
}

void init(){

    // Init uart with baudrate
    uart_init(115200);
    uart_transmit_string("I bims der Master\n\r");

    DDRB |= SS;
    SPI_MasterInit();
    // INIT for the real time clock
    init_DS13xx();

    // Initialize the SPI interface for the LCD display
    // Initialize the LCD display
    LCD_and_Spi_Init();
    
    //initialize volatile variables such as 
    check_current_hour_and_initialize_volatile_variables();
    
    // Initialize Interrupts 250 - ticks - 1ms 
    init_interrupts();

}

void init_interrupts(){
     //Enable interrupts
     TCCR0B = 3; // prescaler 64 -> 4us tick time, 250 ticks -- 1 ms
     TIMSK0 = 1 ; // enablen der overflow interrupts
     TCNT0 = 6; // counter auf 6 --> jede 256-6=50 ticks --> 1 ms
     sei(); //enable interrupts(globally)
}

void check_current_state_and_do_action_if_needed(){
    if(counter_to_do_action_seconds >=30 && !should_action){
        // Reverse from One Green - To Other TODO
        should_action = true;
        counter_to_do_action_seconds = 0;
    }
    if(should_action){
        do_action();
    }
}

int main() {
    // Init of uart, SS-SPI, SPI_MasterInit, init_DS13xx, LCD_and_SPI_Init, init_volatile_variables,init_interrupts
    init();
    //Sleep mode setzen(configurieren)
    set_sleep_mode(SLEEP_MODE_IDLE);
    
    while(1){
        sleep_mode(); //save power
        check_current_state_and_update_if_need_for_action();
    }
}

