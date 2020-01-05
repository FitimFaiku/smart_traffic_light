#ifndef F_CPU
#define F_CPU    16000000
#endif

void init_uart(uint32_t baudrate);

void uart_transmit (uint8_t c);

void uart_receive(void);

uint8_t uart_receive_nb (uint8_t * received);  // non-blocking receive !!

void uart_sendstring (char * str);

void inttostr(uint16_t val,char * resultstring); // val : call by value  resultstring: call by reference