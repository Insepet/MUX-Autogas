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
* Filename      : I2C.h
* Version       : V1.00
* Programmer(s) : 
                  
*********************************************************************************************************
*/
#ifndef I2C_H
#define I2C_H
#include <device.h>
	
uint8 leer_hora();
uint8 leer_fecha();
uint8 write_hora( void );
uint8 write_fecha( void );
uint8 write_psoc1( uint8 puerto, uint8 valor );

#endif

/* [] END OF FILE */
