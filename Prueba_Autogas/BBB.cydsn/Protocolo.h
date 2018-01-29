/*
*********************************************************************************************************

*                                           GRP550/700 CODE
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

*                                             GRP550/700 CODE
*
*                                             CYPRESS PSoC5LP
*                                                with the
*                                            CY8C5969AXI-LP035
*
* Filename      : Protocolo.h
* Version       : V1.00
* Programmer(s) : 
                  
*********************************************************************************************************
*/
#ifndef PROTOCOLO_H
#define PROTOCOLO_H
#include <device.h>

uint8 ver_pos (void);
uint8 get_estado(uint8 dir);
uint8 estado_ex(uint8 val);
void detener(uint8 val);
uint8 venta(uint8 val);
uint8 programar(uint8 dir, uint8 grado, uint8 *valor, uint8 preset);
uint8 cambiar_precio(uint8 dir,uint8 *precio, uint8 num_man);
uint8 totales(uint8 dir, uint8 nmangueras);


#endif

//[] END OF FILE
