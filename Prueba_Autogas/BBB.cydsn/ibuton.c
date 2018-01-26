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
* Filename      : ibutton.c
* Version       : V1.00
* Programmer(s) : 
                  
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include <device.h>
#include "VariablesG.h"

/*
*********************************************************************************************************
*                                           uint8 touch_present(void)
*
* Description : 
*               
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
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

/*
*********************************************************************************************************
*                                           uint8 touch_write(uint8 dato)
*
* Description : 
*               
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
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

/*
*********************************************************************************************************
*                                           uint8 touch_read_byte()
*
* Description : 
*               
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
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

/*
*********************************************************************************************************
* 							uint8 crc_check(uint8_t crc, uint8_t data)
*
* Description : 
*               
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
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
