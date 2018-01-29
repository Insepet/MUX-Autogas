/**
* @file I2C.c
* @Author Insepet LTDA
* @date 28/2/2016
* @brief librería para el manejo de dispositivos conectados al psoc por medio del I2C
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include <device.h>
#include "VariablesG.h"
#include "LCD.h"

/**
* @fn leer_hora
* @brief Función para leer la hora vía I2C, escribe la hora solicitada en rventa.hora
* @return 0 para error de lectura 1 si no hubo problema al leer la hora
*/
uint8 leer_hora(){
	uint8 status,i;
	for(i=1;i<=2;i++){
        I2C_1_MasterClearStatus();
        status = I2C_1_MasterSendStart(0x68, I2C_1_WRITE_XFER_MODE);
        if(I2C_1_MSTR_NO_ERROR == status)								 		/* Check if transfer completed without errors */
    	{
            status = I2C_1_MasterWriteByte(i);
            if(status != I2C_1_MSTR_NO_ERROR)
            {
                return 0;
            }
			else{
		        I2C_1_MasterSendStop(); 										/* Send Stop */
		        CyDelay(10);
		        status = I2C_1_MasterSendStart(0x68, I2C_1_READ_XFER_MODE);
		        if(I2C_1_MSTR_NO_ERROR == status){
		            hora[i-1] = I2C_1_MasterReadByte(I2C_1_NAK_DATA);
		        }
				else{
					return 0;
				}
		        I2C_1_MasterSendStop();	
			}
        }
		else{
			return 0;
		}		
	}
	return 1;
}

/**
* leer_fecha
* @brief Función para leer la hora vía I2C, escribe la hora solicitada en rventa.fecha
* @return 0 para error de lectura 1 si no hubo problema al leer la fecha
*/
uint8 leer_fecha(){
	uint8 status,i;
	for(i=4;i<=6;i++){
        I2C_1_MasterClearStatus();
        status = I2C_1_MasterSendStart(0x68, I2C_1_WRITE_XFER_MODE);
        if(I2C_1_MSTR_NO_ERROR == status)								 		/* Check if transfer completed without errors */
    	{
            status = I2C_1_MasterWriteByte(i);
            if(status != I2C_1_MSTR_NO_ERROR)
            {
                return 0;
            }
			else{
		        I2C_1_MasterSendStop(); 										/* Send Stop */
		        CyDelay(10);
		        status = I2C_1_MasterSendStart(0x68, I2C_1_READ_XFER_MODE);
		        if(I2C_1_MSTR_NO_ERROR == status){
		            fecha[i-4] = I2C_1_MasterReadByte(I2C_1_NAK_DATA);
		        }
				else{
					return 0;
				}
		        I2C_1_MasterSendStop();	
			}
        }
		else{
			return 0;
		}		
	}
	return 1;
}

/**
* write_hora
* @brief Función para escribir la hora vía I2C, escribe la hora de rventa.hora
* @return 0 para error de escritura 1 si no hubo problema al escribir la hora
*/
uint8 write_hora( void ){
	uint8 status, dato[3],i;
	dato[0]=1;
	dato[1]=hora[0];
	dato[2]=hora[1];
	I2C_1_MasterClearStatus();
    status = I2C_1_MasterSendStart(0x68, I2C_1_WRITE_XFER_MODE);
    if(I2C_1_MSTR_NO_ERROR == status) 
    {
        for(i=0; i<3; i++)
        {
            status = I2C_1_MasterWriteByte(dato[i]);
            if(status != I2C_1_MSTR_NO_ERROR)
            {
                return 0;
            }
        }
    }
    else{
		return 0;
    }
    I2C_1_MasterSendStop();	
	return 1;
}

/**
* write_fecha
* @brief Función para escribir la fecha vía I2C, escribe la hora de rventa.fecha
* @return 0 para error de escritura 1 si no hubo problema al escribir la fecha
*/
uint8 write_fecha( void ){
	uint8 status, dato[4],i;
	dato[0]=4;
	dato[1]=fecha[0];
	dato[2]=fecha[1];
	dato[3]=fecha[2];
	I2C_1_MasterClearStatus();
    status = I2C_1_MasterSendStart(0x68, I2C_1_WRITE_XFER_MODE);
    if(I2C_1_MSTR_NO_ERROR == status) 
    {
        for(i=0; i<4; i++)
        {
            status = I2C_1_MasterWriteByte(dato[i]);
            if(status != I2C_1_MSTR_NO_ERROR)
            {
                return 0;
            }
        }
    }
    else{
		return 0;
    }
    I2C_1_MasterSendStop();	
	return 1;
}

/**
* write_psoc1
* @brief Función para enviar datos al micro encargado de controlar las impresoras
* @param puerto puerto al que se desea enviar los datos (impresora lado A, impresora lado B)
* @param valor dato a enviar
* @return 0 para error de lectura 1 si no hubo problema 
*/
uint8 write_psoc1( uint8 puerto, uint8 valor ){
	uint8 status, dato[2],i;
	dato[0]=puerto;
	dato[1]=valor;
	I2C_1_MasterClearStatus();
    status = I2C_1_MasterSendStart(7u, I2C_1_WRITE_XFER_MODE);
    if(I2C_1_MSTR_NO_ERROR == status) 
    {
        for(i=0; i<2; i++)
        {
            status = I2C_1_MasterWriteByte(dato[i]);
            if(status != I2C_1_MSTR_NO_ERROR)
            {
                return 0;
            }
        }
    }
    else{
		return 0;
    }
    I2C_1_MasterSendStop();	
	return 1;
}

/* [] END OF FILE */
