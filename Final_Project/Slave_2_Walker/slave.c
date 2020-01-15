#include "traffic_light/light_ws2812.h"
#define F_CPU 16000000 
#include<avr/io.h>
#include<util/delay.h>
#include "ultrasonicsensor/ultrasonicsensor.h"
#include <avr/interrupt.h>
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
    // SPCR |= (1 << SPE) | (1 << CPOL) | (1 << CPHA);
    SPCR |= (1 << SPE);
}

void uart_transmit_slave(char character) {
    while (!(UCSR0A & (1 << 5)));
    UDR0 = (uint8_t) character;
}

char uart_receive_slave() {
    while (!(UCSR0A & (1 << RXC0)));
    return (char) UDR0;
}

void uart_transmit_string(char *string) {
    for (int i = 0; string[i] != 0; i++) {
        uart_transmit(string[i]);
    }
}

/*char SPI_SlaveReceive(char toMaster) { //was (void)
	SPDR = toMaster;
    // Wait for reception complete
    // SPI Status Reg & 1<<SPI Interrupt Flag
    while (!(SPSR & (1 << SPIF)));
    // Return data register
    return SPDR;
}*/

char SPI_SlaveReceive(char toMaster) {
	 SPDR = toMaster;
    // Wait for reception complete
    // SPI Status Reg & 1<<SPI Interrupt Flag
    if (!(SPSR & (1 << SPIF))){
    // Return data register
			
		
		return SPDR;
	}else{
		return 0;		

	}	
}


void uart_init(uint32_t baudrate) {
    // UBRR formula from datasheet
    // 8 as we use doublespeed
    uint16_t ubrr = (F_CPU / 8 / baudrate) - 1;
    // Set baud rate, use double speed
    UBRR0H = (uint8_t) (ubrr >> 8); // get higher 8 bits
    UBRR0L = (uint8_t) (ubrr & 0xff); // get lower 8 bits
    UCSR0A |= (1 << U2X0);
    // Enable receiver and transmitter
    UCSR0B = (1 << TXEN0 | 1 << RXEN0);
    // select character size, stop and parity bits: 8N1
    // UCSR0C = (1<<UCSZ01 | 1<<UCSZ00);
    // not necessary, because UCSR0C is initialised corretly !
    // for interrupts: enable receive complete interrupt
    // UCSR0B |= (1<<RXCIE0); we do not use interrupts by now..
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

int main() {
    uart_init(115200);
    DDRD = (1<<4);
    uart_transmit_string("I bims der Slave\n\r");
    SPI_SlaveInit();
	char c = 'X';
	uint8_t status=0;
	uint16_t distance;
	//SwitchRedPL();
	TCCR0B =3; 	//b prescaler 64 -> 4µs tick time, 250 ticks == 1ms
	TCNT0= 231; 	//25 ticks bis zum overflow
	TIMSK0=1; 	// enable timer 1 overflow interrupts
	sei();    	//	enable interrupts (globally)
	char s = 'a';
    while (1) {
		/*char s=SPI_SlaveReceive(status);
        if(s!=0){
			uart_sendstring("received:");
			uart_transmit_slave(s);
			uart_transmit_string("\n\r");
           c = s;
		}*/
		s=SPI_SlaveReceive(s);
		uart_sendstring("received:");
		uart_transmit_slave(s);
		uart_transmit_string("\n\r");
        
        //logik for traffic lights - the intepretation of the cmds of the master
       /* if(c=='3'){// Switch to green pedestrian traffic light
            SwitchGreenPL();
			frequenz=6;
			
        } */
        if(c==1){
			BlinkGreenPL();
		}
        if(c=='4'){// Switch to red pedestrian traffic light
            SwitchRedPL();
       		//distance = ultrasonicsensor();
			/*if(distance <= 10){ status=1
				c= SPI_SlaveReceive(distance+48); //send a char to master to know that a person is waiting
				
				}*/
        }
        
        if(c=='2'){ // night mode = blink yellow pkw traffic light
			//NoLights();
			
			SwitchGreenPL();
        }
        if(c==5){
			//check if someone is near the traffic light
			//if true,return 6
		}
		if()
        
    }
}

