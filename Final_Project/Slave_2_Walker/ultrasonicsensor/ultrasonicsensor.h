#ifndef F_CPU
#define F_CPU    16000000
#endif

void init_uart(uint32_t baudrate);

void uart_transmit (uint8_t c);

char uart_receive ();

uint8_t uart_receive_nb (uint8_t * received);

void uart_sendstring (char * str);

void inttostr(uint16_t val,char * resultstring);

int ultrasonicsensor(void);
