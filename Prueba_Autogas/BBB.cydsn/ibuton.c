/**
* @file ibuton.c
* @Author Insepet LTDA
* @date 28/2/2016
* @brief Librería para la lectura del ibutton
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include <device.h>
#include "VariablesG.h"

/**
* touch_present
* @brief indica el lado donde se encuentra el ibutton
* @param ibutton lado del lector
* @return 0 no hay ibutton , 1 si hay ibutton
*
*/
uint8 touch_present(uint8 ibutton){  
    uint8 present;
	if(ibutton==1){
	    IB1_Write(0);
	    CyDelayUs(500);
	    IB1_Write(1);
	    CyDelayUs(5);

	    if(!IB1_Read()){
	        return 0;
	    }

	    CyDelayUs(65);
	    present=!IB1_Read();
	    CyDelayUs(240);
	    if(present){
	        return 1;
	    }
	    else{
	        return 0;
	    } 
	}
	else{
	    IB2_Write(0);
	    CyDelayUs(500);
	    IB2_Write(1);
	    CyDelayUs(5);

	    if(!IB2_Read()){
	        return 0;
	    }

	    CyDelayUs(65);
	    present=!IB2_Read();
	    CyDelayUs(240);
	    if(present){
	        return 1;
	    }
	    else{
	        return 0;
	    }	
	}
}

/**
* touch_write
* @brief Escribe un byte en el Ibutton
* @param ibutton lado
* @param dato byte que se va a escribir
* @return 0 operacion correcta , 1 operacion incorrecta
*
*/
uint8 touch_write(uint8 ibutton, uint8 dato){
    uint8 i;
	if(ibutton==1){
	    for(i=0;i<=7;i++){
	        IB1_Write(0); 
	        CyDelayUs(10);
	        if(((dato>>i)&1)){
	           IB1_Write(1);
	           CyDelayUs(10);
	           if(!IB1_Read()){
	                return 0;                
	           }
	        }
	        else{
	            IB1_Write(0); 
	            CyDelayUs(10);
	           if(IB1_Read()){
	                return 0;                
	           }            
	        }
	        CyDelayUs(50);
	        IB1_Write(1);
	        CyDelayUs(50);
	    }
	    return 1;
	}
	else{
	    for(i=0;i<=7;i++){
	        IB2_Write(0); 
	        CyDelayUs(10);
	        if(((dato>>i)&1)){
	           IB2_Write(1);
	           CyDelayUs(10);
	           if(!IB2_Read()){
	                return 0;                
	           }
	        }
	        else{
	            IB2_Write(0); 
	            CyDelayUs(10);
	           if(IB2_Read()){
	                return 0;                
	           }            
	        }
	        CyDelayUs(50);
	        IB2_Write(1);
	        CyDelayUs(50);
	    }
	    return 1;	
	}
}

/**
* touch_read_byte
* @brief Lee un byte del ibutton, se usa para leer los bytes del serial
* @param ibutton lado
* @return 0 no hubo lectura , =!0 dato leido
*
*/
uint8 touch_read_byte(uint8 ibutton){
    uint8 i, dato=0;
	if(ibutton==1){
	    for(i=0;i<=7;i++){
	        IB1_Write(0);    
	        CyDelayUs(14);
	        IB1_Write(1);
	        CyDelayUs(7);
	        dato|=(IB1_Read()<<i);
	        CyDelayUs(100);
	    }
	    return dato;
	}
	else{
	    for(i=0;i<=7;i++){
	        IB2_Write(0);    
	        CyDelayUs(14);
	        IB2_Write(1);
	        CyDelayUs(7);
	        dato|=(IB2_Read()<<i);
	        CyDelayUs(100);
	    }
	    return dato;	
	}
} 

/**
* crc_check
* @brief verifica los datos leídos del ibutton
* @param crc dato obtenido
* @param dato byte de ibutton
* @return crc calculado
*
*/
uint8 crc_check(uint8 crc, uint8 dato){
    uint8 i;
    crc = crc ^ dato;
    for (i = 0; i < 8; i++)
    {
        if (crc & 0x01){
            crc = (crc >> 1) ^ 0x8C;
		}	
        else{
            crc >>= 1;
		}	
    }
    return crc;
}


/* [] END OF FILE */
