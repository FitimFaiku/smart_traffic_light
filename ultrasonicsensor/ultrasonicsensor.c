#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void init_uart(uint32_t baudrate)
{
		uint16_t br = F_CPU/8/ baudrate-1;
		UBRR0H =  (uint8_t) (br >>8);
		UBRR0L =	(uint8_t) (br & 0xff);
		UCSR0A |= (1<<U2X0); // activate double speed mode for uart
		// frame format is correct (no change in UCSR0c)
		
		UCSR0B |= (1<<TXEN0)+ (1<<RXEN0); // enable recieve  and transmitt
		 
}

void uart_transmit (uint8_t c)
{
	while ((UCSR0A & (1<<UDRE0)) ==0); // wait until UDR empty
	UDR0 = c;//send character c
}

uart_receive ()
{
	while ( (UCSR0A & (1 << RXC0)) == 0)  ;    //  warten, bis bit bit RXC0 in UCSR0A 1 wird -> received something !
	return(UDR0);							   // auslesen und zurückgeben !
}
uint8_t uart_receive_nb (uint8_t * received)  // non-blocking receive !!
{
	if ( (UCSR0A & (1 << RXC0)) == 0)  return(0) ;    //  falls nichts empfangen wurde: sofort 0 zurückgeben !
	* received = UDR0;    // empfangenes zeichen auslesen und in received ablegen
	return(1);            // 1 zurückgeben als Zeichen, dass etwas empfangen wurde!
}

void uart_sendstring (char * str)
{
	while(*str){
	 uart_transmit(*str);
	 str++;
	}
}

int main(void)
{
	init_uart(115200);
	DDRD = (1<<5); //PD5 (output)
	DDRD &= ~(1<<6);//PD6 (input)
	uint8_t counter=0; //counter für messung 
	uint16_t messung=0;// messung für ie entfernung
	
	while(1)
	{
		//Mesung ausloesen durch 10µs High time auf dem Trigger-Pin
		PORTD&=~(1<<5);	
		_delay_us(2);
		PORTD|=(1<<5);
		_delay_us(20);
		PORTD&=~(1<<5);	
		
		//Entfernung ermitteln durch messung der high time am Echo-
		while((PORTD&(1<<6))==0);
		while((PORTD&(1<<6))==(1<<6)){
			messung++;
			//uart_transmit('a');
			_delay_us(1);
		}
		uart_transmit((messung/48)+48); // /48 um die gemessene Zeit in cm umzurechnen  +48 int to char 
		uart_transmit('\r');
		uart_transmit('\n');
		messung=0;
		_delay_us(100000);
		
	}
}
