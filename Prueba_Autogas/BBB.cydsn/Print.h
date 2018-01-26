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
* Filename      : Print.h
* Version       : V1.00
* Programmer(s) : 
                  
*********************************************************************************************************
*/

#ifndef PRINT_H
#define PRINT_H

#include <device.h>

void imprimir(uint8 pos, uint8 print, uint8 prod, uint8 *ppu, uint8 *volumen, uint8 *dinero, uint8 *placa, uint8 *km_p);
void print_logo(uint8 logo, uint16 ini, uint16 fin, uint8 puerto);

#endif

//[] END OF FILE
