#ifndef F_CPU
#define F_CPU    16000000
#endif



volatile uint16_t messung=0;// messung für die entfernung

// TODO define which length in cm is really close and also implement methode which sends the master module the information to switch to green

void init_uart(uint32_t baudrate);


void uart_transmit (uint8_t c);


uart_receive ();

uint8_t uart_receive_nb (uint8_t * received);  // non-blocking receive !!


void uart_sendstring (char * str);

void inttostr(uint16_t val,char * resultstring); // val : call by value  resultstring: call by reference

ISR(TIMER0_OVF_vect); //timer 0 overflow interrupt service routine
/*{
	static uint16_t count1=0,count2=0; //static: global lifetime, local visibility		
	TCNT0 =231;				//25 ticks bis zum overflow
	if(count1==2)PORTD&=~(1<<5);
	if(count1==13)PORTD|=(1<<5);
	if(count1==33)PORTD&=~(1<<5);
		
	//if(count1>=55){
		//if((PIND&(1<<6))!=0){
		//messung++;
		//}
		if(count1>=130000)count1=0;
	//}	
	count1++;
}*/



/*int main(void)
{
	init_uart(115200);
	DDRD = (1<<5); //PD5 (output)
	DDRD &= ~(1<<6);//PD6 (input)
	uint8_t counter=0; //counter für messung 
	uint16_t messung=0;// messung für ie entfernung
	char entfernung[5];// um Die entfernung auszugeben
	TCCR0B =3; 	//b prescaler 64 -> 4µs tick time, 250 ticks == 1ms
	TCNT0= 231; 	//25 ticks bis zum overflow
	TIMSK0=1; 	// enable timer 0 overflow interrupts
	sei();    	//	enable interrupts (globally)
	while(1)
	{*/
		/* //Mesung ausloesen durch 10µs High time auf dem Trigger-Pin
		PORTD&=~(1<<5);	
		_delay_us(2);
		PORTD|=(1<<5);
		_delay_us(20);
		PORTD&=~(1<<5);	
		
		//Entfernung ermitteln durch messung der high time am Echo-
		while((PIND&(1<<6))==0);
		while((PIND&(1<<6))!=0){
			messung++;
			_delay_us(1); 
		}*/
		/*messung=messung/48; // /48 um die gemessene Zeit in cm umzurechnen
		inttostr(messung,entfernung);
		uart_sendstring(entfernung); 
		uart_transmit('\r');
		uart_transmit('\n');
		messung=0;*/
	/*}
}*/
