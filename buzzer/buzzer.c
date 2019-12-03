#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>



volatile uint16_t frequenz;

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
	if((UCSR0A & (1<<RXC0)) !=0)
	{
	return (UDR0); // return receive character from function 
	}
	return 0;
}

void uart_sendstring (char * str)
{
	while(*str){
	 uart_transmit(*str);
	 str++;
	}
}

ISR(TIMER0_OVF_vect) //timer 0 overflow interrupt service routine
{
	static uint16_t count1=0,count2=0; //static: global lifetime, local visibility		
	TCNT0 =231;				//25 ticks bis zum overflow
	if(count2<=1000){
		if(frequenz>count1) PORTD |= (1<<5); //Pin high setzen
		else PORTD &= ~(1<<5); 				//Pin low setzen
		count1++;
		if(count1==10) count1=0;	// counter zurücksetzten
	}
	count2++;
	if(count2>=2000){
		count2=0 ;
	}
}
int main()
{
	char c;
	uint8_t check=0;
	init_uart(115200);
	uart_sendstring("Gib eine Zahl von 0-9 ein um die Helligkeit einzustellen\r\n");
	DDRD = (1<<5);
	
	TCCR0B =3; 	//b prescaler 64 -> 4µs tick time, 250 ticks == 1ms
	TCNT0= 231; 	//25 ticks bis zum overflow
	TIMSK0=1; 	// enable timer 0 overflow interrupts
	sei();    	//	enable interrupts (globally)
	
	while(1){
			
	c=uart_receive();
	check= (uint16_t)c-'0';// char to int
	
	if(check>=0&&check<=9)
	{
		frequenz=check;
		uart_transmit(c);
		
	}
	}
	
}
