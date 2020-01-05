/*! 
  \file       rtc.h
  \brief      Library for a rtc module interactions
 
  \attention  The information contained herein is confidential property of the
              Institute of Embedded Systems - Technikum Wien. The use, copying,
              transfer or disclosure of such information is prohibited except 
              by express written agreement with the Embedded Systems Institute.

  \author     Fitim Faiku
  \date       31/12/2019
  \version    1.0

*/
#include <stdio.h> // Standard Libary for Data types

#ifndef F_CPU
#define F_CPU    16000000UL // needed for the delay function (util/delay.h)
#endif

#ifndef DS13xx_CE
#define DS13xx_CE 5    //CHIP SELECTION
#endif
#ifndef DS13xx_IO
#define DS13xx_IO 6 //Data
#endif
#ifndef DS13xx_SCLK
#define DS13xx_SCLK 7 //Clock
#endif
#ifndef BAUDRATE
#define BAUDRATE 115200
#endif

// Functions to control the rtc
void DS13xx_Write_CLK_Registers();
void DS13xx_Read_CLK_Registers();
unsigned char DS13xx_ReadByte();
void DS13xx_WriteByte(unsigned char W_Byte);
void DS13xx_Reset(void);
void DS13xx_Init(void);
uint8_t get_current_hour(void);
uint8_t unsined_char_to_int(char* data);
void init_DS13xx(void);

