#include "traffic_light/light_ws2812.h"
#include "ultrasonicsensor/ultrasonicsensor.h"
#define F_CPU 16000000 
#include<avr/io.h>
#include<util/delay.h>
#include <avr/interrupt.h>
#include <string.h>

#define PORT_DIRECTION DDRB
#define PORT_VALUE PORTB

// Pin definitions (on Port B)
#define SCK  5
#define MISO 4
#define MOSI 3
#define SS   2

void SPI_SlaveInit(void) {
    // Set MISO output, all others input
    PORT_DIRECTION |= (1 << MISO);
    // Enable SPI
    SPCR |= (1 << SPE) | (1<< SPIE);
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
    for (int i = 0; strcmp(string[i], '\0') != 0; i++) {
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
   
    //Wait for reception complete 
    //while (!(SPSR & (1 << SPIF)));
	if(SPSR & (1 << SPIF)) {
		//Return Data Register
		return SPDR;
	} else {
		return '0';
	}
     
}*/

void SPI_SlaveReceive(char toMaster) {
	SPDR = toMaster;
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

volatile char s = 0;
uint8_t sending = 0;
ISR(SPI_STC_vect){
	uart_transmit_string("SPI\n\r");
	sending = 0;
	s = SPDR;
}

int main() {
	uint8_t status='0';
    uart_init(115200);
    uart_transmit_string("I bims der Slave\n\r");
	uint16_t entfrernung;
	sei();
	
    SPI_SlaveInit();
	char c = 'X';
	SwitchGreenTL();
    while (1) {
		if(!sending) {
			sending = 1;
			SPI_SlaveReceive(status); // Send
		}
		
		
		if(s!=0){
			uart_sendstring("reveived");
			uart_transmit_slave(c);
			uart_transmit_string("\n\r");
			c=s;
		}
        
        //logik for traffic lights - the intepretation of the cmds of the master
        if(c=='1'){// blink green
            BlinkGreenTL();
        }
        if(c=='2'){// Switch to green
            SwitchGreenTL();
            status=0;
        }
        if(c=='3'){ // Switch to yellow
            SwitchYellowTL();
        }
        if(c=='4'){ // switch to red
		SwitchRedTL();
		ultrasonicsensor();
          if(entfrernung<=10){
			status='6';
		  }

        }
        if(c=='5'){ // ckeck if someone is near the trfficlight
          
        }
        if(c=='7'){ //Nightmode
            BlinkYellowTL();
      }
    }
}

