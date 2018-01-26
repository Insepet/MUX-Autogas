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
* Filename      : ibutton.h
* Version       : V1.00
* Programmer(s) : 
                  
*********************************************************************************************************
*/

#ifndef IBUTTON_H
#define IBUTTON_H
#include <device.h>
uint8 touch_present(uint8 ibutton);
uint8 touch_write(uint8 ibutton, uint8 dato);
uint8 touch_read_byte(uint8 ibutton);
uint8 crc_check(uint8 crc, uint8 dato);

#endif

//[] END OF FILE
