#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "ultrasonicsensor.h"
#include "buzzer.h"


// TODO define which length in cm is really close and also implement methode which sends the master module the information to switch to green

int main(void)
{	
	uint16_t messung;
	
	while(1)
	{
		//messung=ultrasonicsensor();
		//if(messung>=5)
		//{
			buzzer(5);
			//_delay_ms(10000);
		//}
		//_delay_ms(10);
	}
}
