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
#include <stdbool.h> // inkludiere bool


#define PORT_DIRECTION DDRB
#define PORT_VALUE PORTB

#define PORT_DIRECTION_SLAVE DDRD
#define PORT_VALUE_SLAVE PORTD


// Pin definitions (on Port B)
#define SCK  5
#define MISO 4
#define MOSI 3
#define SS   2

// Pin definitions (on port D)
#define SS_SLAVE_1 3
#define SS_SLAVE_2 4

// Defines for the SS pin
#define SS_UNSELECT PORT_VALUE |= (1 << SS);
#define SS_SELECT PORT_VALUE &= ~(1 << SS);

#define SS_UNSELECT_SLAVE_1 PORT_VALUE_SLAVE |= (1 << SS_SLAVE_1);
#define SS_SELECT_SLAVE_1 PORT_VALUE_SLAVE &= ~(1 << SS_SLAVE_1);

#define SS_UNSELECT_SLAVE_2 PORT_VALUE_SLAVE |= (1 << SS_SLAVE_2);
#define SS_SELECT_SLAVE_2 PORT_VALUE_SLAVE &= ~(1 << SS_SLAVE_2);

void init();
void uart_init(uint32_t baudrate);
void uart_transmit(char character);
void uart_transmit_string(char *string);
char uart_receive(void);
void SPI_MasterInit(void);
//void init_DS13xx() RTC from rtc.h
//void LCD_and_Spi_Init() from dogm_lcd.h
void check_current_hour_and_initialize_volatile_variables(); //initialize volatile variables such as 
//uint8_t get_current_hour() from rtc.h
//uint8_t get_current_minute() from rtc.h
void init_interrupts();// Initialize Interrupts 250 - ticks - 1ms 
void check_slave_message_should_action();
void check_current_state_and_do_action_if_needed();
void set_traffic_light_green();

void do_action();

static int16_t delay_for_communication_between_traffic_cycles_ms = 50;
static int16_t delay_before_first_cycle = 500;
static int16_t delay_between_green_blink_and_orange_or_red = 6000;
static int16_t delay_between_orange_and_red_or_green = 5000;
static int16_t delay_between_one_slave_red_and_other_start = 3000;

static uint8_t counter_cycle = 0;


static volatile char next_state = '0';
static int16_t counter_delay_ms=0;
static volatile uint8_t current_hour;
static volatile uint8_t current_minute;
static volatile int8_t counter_till_next_interval;
static volatile char last_state; 
static volatile bool should_action = false;
static bool is_cycling_traffic_light_cars_green = false;
static bool is_cycling_traffic_light_walkers_green = false;
static volatile bool is_day_mode;
static volatile bool is_traffic_light_cars_red ;
static volatile uint8_t counter_to_do_action_seconds;



void SPI_MasterInit(void) {
    // Set MOSI and SCK output, all others input
    PORT_DIRECTION |= (1 << MOSI) | (1 << SCK) | (1<<SS);
    PORT_DIRECTION_SLAVE |= (1<<SS_SLAVE_1) | (1<<SS_SLAVE_2);
    PORT_VALUE_SLAVE |= (1<<SS_SLAVE_1) | (1<<SS_SLAVE_2);
    
    //init SPI
	// SPI2X...Double SPI Speed Bit
	// SPE...SPI Enable Bit
	// MSTR...Master SPI Mode
	// SPR1...SCK Frequency set to Fosc/32
    //SPSR |= (1<<SPI2X);  
	//SPCR |= ((1<<SPE) | (1<<MSTR) | (1<<SPR1) | (1<<CPOL) | (1<<CPHA));
	
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
    //while (!(SPSR & (1 << SPIF)));
    if(SPSR & (1 << SPIF)) {
		/* Return Data Register */
		return SPDR;
	} else {
		return '0';
	}
    /* Return Data Register */
    //return SPDR;
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
    static uint8_t menueopencounter=0, cnt_ms=0,cnt_ms_ten=0, cnt_s=0; // gloabl lifetime, local visibillity Counter for miliseconds
    TCNT0 = 6; // counter auf 6 --> jede 256-6= 250 ticks --> 1 ms
    counter_delay_ms++;
    // SS_SELECT_SLAVE_1
    //_delay_ms(100);
    // uint8_t slaveValue = SPI_MasterTransmit('3');
    // uart_transmit(slaveValue + 48);
    // SS_UNSELECT_SLAVE_1
    
    /*SS_SELECT_SLAVE_1
    uint8_t slaveValue = SPI_MasterTransmit('x');
    SS_UNSELECT_SLAVE_1
    uart_transmit(slaveValue);*/
   

    // $$$$$$$$$$$$$$$$ Anfang check
    set_hour(current_hour);
    
    
    
    if(cnt_ms++>=100){
		cnt_ms_ten++;
		cnt_ms=0;
	} 
    if(cnt_ms_ten>10){
        cnt_s++;
        counter_to_do_action_seconds++;
        counter_till_next_interval--;
        if(counter_till_next_interval>=0){
            set_counter_till_next_interval(counter_till_next_interval);
        }
        uart_transmit_string("Updating seconds ... \n\r");

       // Every second we check if maybe someone is near the Red Traffic Light slave -> master command 6 or 7 
        check_slave_message_should_action();
        if(cnt_s>60){
            current_minute++;
            if(current_minute>=60){
                current_hour++;
                if(current_hour>=24){
                    current_hour=0;
                }
                // TODO update hour to check if nightmode or daymode.
                set_hour(current_hour);
                current_minute=0;
            }
            set_minutes(current_minute);
            cnt_s=0;
        }
        set_seconds(cnt_s);
        display_and_update_menue();
        cnt_ms_ten=0;
        //DS13xx_Read_CLK_Registers();
        
    }
    
}

void set_traffic_light_cars_red_and_walkers_green() {
    // Switch Yellow Car -(?msTODO)-> Switch Red Car -(?msTODO)->Switch green for Walkers Traffic Light
    counter_delay_ms=0;
    counter_cycle=0;
    is_cycling_traffic_light_walkers_green = true;
    uart_transmit_string("is_cycling_traffic_light_walkers_green setting true \n\r");

}

void set_traffic_light_walkers_red_and_cars_green() {
    counter_delay_ms = 0; 
    counter_cycle=0;
    is_cycling_traffic_light_cars_green = true;
    uart_transmit_string("is_cycling_traffic_light_cars_green setting true \n\r");
    // Switch Red Walkers traffic Light -(?msTODO)-> Switch Yellow Cars Traffic Light -(?msTODO)->Switch green for Cars Traffic Light
}

void check_slave_message_should_action(){

    //char slave_message = '0';

    uint8_t slave_message_pkw = 0;
    uint8_t slave_message_walker = 0;
    
     // When Walkers is waiting and currently has red TODO slave_message_int+48
    if(is_day_mode  && counter_to_do_action_seconds>=5 && !is_cycling_traffic_light_cars_green && !is_cycling_traffic_light_walkers_green){
        uart_transmit_string("0. Gesendet: Check if Someone is near the Traffic Light master --> slave request \n\r");
        //see --> 5) Check if Someone is near the <b>Walkers -- Slave 2</b> Traffic Light master --> slave request
        SS_SELECT_SLAVE_2
        //_delay_ms(100);
        slave_message_walker = SPI_MasterTransmit(0);
        SS_UNSELECT_SLAVE_2

        SS_SELECT_SLAVE_1
        //_delay_ms(100);
        slave_message_pkw = SPI_MasterTransmit(0);
        SS_UNSELECT_SLAVE_1

		uart_transmit_string("Got from slave_pkw \n\r");
		uart_transmit(slave_message_pkw);
		uart_transmit_string("\n\r");
		
		uart_transmit_string("Got from slave_walker \n\r");
		uart_transmit(slave_message_walker);
		uart_transmit_string(" \n\r");
		if(is_traffic_light_cars_red){
			uart_transmit_string("is_traffic_light_cars_red TRUE \n\r");
			
			if(slave_message_pkw == '1' && slave_message_walker == '0' ){
            uart_transmit_string("Do action switch to next cycle 1 \n\r");
            should_action = true;
        }
       
		} else {
			uart_transmit_string("is_traffic_light_cars_red FALSE \n\r");
			if(slave_message_pkw == '0' && slave_message_walker == '1'){
				uart_transmit_string("Do action switch to next cycle 2 \n\r");
				should_action = true;
			}
		}
        

    }
}

void do_action(void) {
    if(is_traffic_light_cars_red && next_state=='4'){
        //Switch to Green Cars Traffic Light and Red Walkers Traffic Light
        set_traffic_light_walkers_red_and_cars_green();
    }
    if(!is_traffic_light_cars_red && next_state=='3'){
        //Switch to Red CarsTraffic Light and Green Walkers Traffic Light
        set_traffic_light_cars_red_and_walkers_green();
    }
}


/**
* Gets current hour from RTC - DS13xx and initializes cnr_h and current_status
*/
void check_current_hour_and_initialize_volatile_variables(){
    current_hour = get_current_hour();
    //current_hour = 20;
    current_minute = get_current_minute(); 
    //current_minute = 0;
    // 21-6:00 Nachtmodus TODO right times here

    set_hour(current_hour);
    set_minutes(current_minute);
    set_seconds(0);
    if(current_hour>=21 || current_hour<6){ 
        should_action = true;
        //Night mode
        uart_transmit_string("Nachtmodus");
        //uart_transmit(current_hour+48);
        uart_transmit_string("Nachtmodus, status: ");
        SS_SELECT_SLAVE_2
        //_delay_ms(delay_for_communication_between_traffic_cycles_ms);
        SPI_MasterTransmit('7');
        uart_transmit_string("7. Gesendet: Blink <b>Cars</b> Traffic Light Orange\n\r");
        SS_UNSELECT_SLAVE_2
        is_day_mode = false;
    } else {
        should_action = true;
        // Day mode
        uart_transmit_string("Tagesmodus");
        set_traffic_light_cars_red_and_walkers_green();
        is_day_mode = true;
    }
}

void init(){

    // Init uart with baudrate
    uart_init(115200);
    uart_transmit_string("I bims der Master\n\r");

    //Is done in spi masterINIT
    //PORT_DIRECTION |= (1<<SS);
    //PORT_DIRECTION_SLAVE |= (1<SS_SELECT_SLAVE_1) | (1<<SS_SELECT_SLAVE_2);

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
     TCCR0B |= (1<<CS11) | (1<<CS10); // prescaler 64 -> 4us tick time, 250 ticks -- 1 ms
     TIMSK0 = 1 ; // enablen der overflow interrupts
     TCNT0 = 6; // counter auf 6 --> jede 256-6=250 ticks --> 1 ms
     sei(); //enable interrupts(globally)
}


void check_current_state_and_do_action_if_needed(){
    
    if(counter_to_do_action_seconds == 30 && !should_action){
        //uart_transmit_string("30 sec since last state \n \r");
        should_action = true;
    }
    if(should_action){
        do_action();
        should_action = false;
    }
}

void execute_state_mashine_cars_green(){ 
    if(is_cycling_traffic_light_cars_green){
        if(counter_delay_ms==delay_before_first_cycle && counter_cycle==0){
        // see -> 1) Blink <b>Walker - Slave 2</b> Traffic Light Green
        SS_SELECT_SLAVE_2
        //_delay_ms(delay_for_communication_between_traffic_cycles_ms);
        uint8_t slaveValue = SPI_MasterTransmit('1');
        SS_UNSELECT_SLAVE_2
        counter_cycle++;
        counter_delay_ms=0;
        uart_transmit(slaveValue);
        uart_transmit_string(": 1. Gesendet: Blink <b>Walkers</b> Traffic Light Green\n\r");
		}

		if(counter_delay_ms==delay_between_green_blink_and_orange_or_red && counter_cycle==1){
			// see -> 4) Switch to Red <b>Walker - Slave 2</b> Traffic Light
			SS_SELECT_SLAVE_2
			//_delay_ms(delay_for_communication_between_traffic_cycles_ms);
			uint8_t slaveValue = SPI_MasterTransmit('4');
			SS_UNSELECT_SLAVE_2
			counter_cycle++;
			counter_delay_ms=0;
			uart_transmit(slaveValue);
			uart_transmit_string(": 4. Gesendet: Switch to Red <b>Walkers</b> Traffic Light \n\r");
			
			
		}

		if(counter_delay_ms==delay_between_one_slave_red_and_other_start && counter_cycle==2){
			// see -> 3) Switch to Yellow <b>Cars -- Slave 1</b> Traffic Light
			SS_SELECT_SLAVE_1
			//_delay_ms(100);
			uint8_t slaveValue = SPI_MasterTransmit('3');
			SS_UNSELECT_SLAVE_1
			counter_cycle++;
			counter_delay_ms=0;
			uart_transmit(slaveValue);
			uart_transmit_string(": 3. Gesendet Switch to Yellow <b>Cars</b> Traffic Light \n\r");
			
		}
		if(counter_delay_ms==delay_between_orange_and_red_or_green && counter_cycle==3){
			//see -> 2) Switch to Green <b>Cars -- Slave 1</b> Traffic Light
			SS_SELECT_SLAVE_1
			//_delay_ms(100);
			uint8_t slaveValue = SPI_MasterTransmit('2');
			SS_UNSELECT_SLAVE_1

			// Reset values because we reached end of process
			is_traffic_light_cars_red = false;
			is_cycling_traffic_light_cars_green= false;
			counter_cycle=0; // reset counter because end
			counter_delay_ms=0;
			uart_transmit(slaveValue);
			uart_transmit_string(": 2. Gesendet: Switch to Green <b>Cars</b> Traffic Light \n\r");
			next_state = '3';
            counter_to_do_action_seconds = 0;
            counter_till_next_interval = 30;
            
		}
    }
}

void execute_state_mashine_walkers_green(){
    //Part 2
    if(is_cycling_traffic_light_walkers_green){
		if(counter_delay_ms==delay_before_first_cycle && counter_cycle==0 ){
			uart_transmit_string("!!!!!!!TRUE cFirst cycle is executed !!!!!\n\r");
			// see -> 1) Blink <b>Cars -- Slave 1</b> Traffic Light Green
			SS_SELECT_SLAVE_1
			//_delay_ms(100);
			uint8_t slaveValue = SPI_MasterTransmit('1');
			SS_UNSELECT_SLAVE_1
			counter_cycle++;
			counter_delay_ms=0;
			uart_transmit(slaveValue);
			uart_transmit_string(": 1. Gesendet Blink <b>Cars</b> Traffic Light \n\r");
		}
		if(counter_delay_ms==delay_between_green_blink_and_orange_or_red && counter_cycle==1){
			// see -> 3) Switch to Yellow <b>Cars -- Slave 1</b> Traffic Light
			SS_SELECT_SLAVE_1
			//_delay_ms(100);
			uint8_t slaveValue = SPI_MasterTransmit('3');
			SS_UNSELECT_SLAVE_1
			counter_cycle++;
			counter_delay_ms=0;
			uart_transmit(slaveValue);        
			uart_transmit_string(": 3. Gesendet: Switch to Yellow <b>Cars</b> Traffic Light \n\r");
			
		}

		if(counter_delay_ms==delay_between_orange_and_red_or_green && counter_cycle==2){
			// see -> 4) Switch to Red <b>Cars -- Slave 1</b> Traffic Light
			SS_SELECT_SLAVE_1
			//_delay_ms(100);
			uint8_t slaveValue = SPI_MasterTransmit('4');
			SS_UNSELECT_SLAVE_1
			counter_cycle++;
			counter_delay_ms=0;
			uart_transmit(slaveValue);
			uart_transmit_string(": 4. Gesendet: Switch to Red <b>Cars</b> Traffic Light \n\r");
			is_traffic_light_cars_red = true;
		}

		// We do delay_between_one_slave_red_and_other_start+delay_between_orange_and_red_or_green because we dont have a yellow phase in the walkers Traffic light 
		if(counter_delay_ms==delay_between_one_slave_red_and_other_start && counter_cycle==3){
			//see ->2) Switch to Green <b>Walkers -- Slave 2</b> Traffic Light
			SS_SELECT_SLAVE_2
			//_delay_ms(100);
			uint8_t slaveValue = SPI_MasterTransmit('2');
			SS_UNSELECT_SLAVE_2
			uart_transmit(slaveValue);
			uart_transmit_string(": 2. Gesendet: Switch to Green <b>Walkers</b> Traffic Light \n\r");
			is_cycling_traffic_light_walkers_green=false;
			counter_cycle=0;
			next_state = '4'; // Next is cars schould get green
            counter_to_do_action_seconds = 0;
            counter_till_next_interval = 30;
		}
	}  
}

int main() {
    // Init of uart, SS-SPI, SPI_MasterInit, init_DS13xx, LCD_and_SPI_Init, init_volatile_variables,init_interrupts
    init();
    //Sleep mode setzen(configurieren)
    //set_sleep_mode(SLEEP_MODE_IDLE);
    
    while(1){
        //sleep_mode(); //save power
        check_current_state_and_do_action_if_needed();
        execute_state_mashine_cars_green();
        execute_state_mashine_walkers_green();
    }
}

