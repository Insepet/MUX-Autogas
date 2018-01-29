/*
*********************************************************************************************************
*                                           GRP550M CODE
*
*                             (c) Copyright 2013; Sistemas Insepet LTDA
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                               GRP550M CODE
*
*                                             CYPRESS PSoC5LP
*                                                with the
*                                            CY8C5969AXI-LP035
*
* Filename      : LCD.h
* Version       : V1.00
* Programmer(s) : 
                  
*********************************************************************************************************
*/

#ifndef LCD_H
#define LCD_H
#include <device.h>
	
void set_imagen(uint8 lcd, uint16 id);
void write_LCD(uint8_t lcd,uint8_t dato, uint16_t posy, uint16_t posx,uint8_t size);
void Hora_LCD(uint8 lcd);
void Fecha_LCD(uint8 lcd);



#endif

//[] END OF FILE
