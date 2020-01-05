#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "ultrasonicsensor.h"
#include <string.h>

#define PORT_DIRECTION DDRB
#define PORT_VALUE PORTB

// Pin definitions (on Port B)
#define SCK  5
#define MISO 4
#define MOSI 3
#define SS   2
volatile uint8_t frequenz=0;

void SPI_SlaveInit(void) {
    // Set MISO output, all others input
    PORT_DIRECTION |= (1 << MISO);
    // Enable SPI
    SPCR |= (1 << SPE);
}
char SPI_SlaveReceive(void) {
    // Wait for reception complete
    // SPI Status Reg & 1<<SPI Interrupt Flag
    while (!(SPSR & (1 << SPIF)));
    // Return data register
    return SPDR;
}


ISR(TIMER0_OVF_vect) //timer 0 overflow interrupt service routine
{
	static uint16_t count1=0,count2=0; //static: global lifetime, local visibility		
	TCNT0 =231;				//25 ticks bis zum overflow
	if(count2<=1000){
		if(frequenz>count1) PORTD |= (1<<4); //Pin high setzen
		else PORTD &= ~(1<<4); 				//Pin low setzen
		count1++;
		if(count1==10) count1=0;	// counter zurücksetzten
	}
	count2++;
	if(count2>=2000){
		count2=0 ;
	}
}
int main(void)
{	
	uint16_t messung=20;
	DDRD=(1<<4);
	TCCR0B =3; 	//b prescaler 64 -> 4µs tick time, 250 ticks == 1ms
	TCNT0= 231; 	//25 ticks bis zum overflow
	TIMSK0=1; 	// enable timer 0 overflow interrupts
	sei();    	//	enable interrupts (globally)
	uart_init(115200);
    uart_sendstring("I bims der Slave\n\r");

    SPI_SlaveInit();
	while(1)
	{
	SPI_SlaveInit();
        char c = SPI_SlaveReceive();
        uart_transmit(c);
        uart_sendstring("\n\r");		
			
	switch(c){
		
		case 2 ://Nightmode
		
		break;
		case 3 ://Switch to green pedestrian trafficlight and red cars trafficlight
		frequenz=6;
		break;
		case 4 ://Switch to green cars trafficlight and red pedestrian trafficlight
		messung=ultrasonicsensor();
		break;
		case 5 ://Switch to yellow cars trafficlight
		
		break; 
		default: 
		break;
		}
	}
}
