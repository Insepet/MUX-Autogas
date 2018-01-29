/**
* @file LCD.c
* @Author Insepet LTDA
* @date 28/2/2016
* @brief Librería para el uso de las pantallas stone
*/
/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include <device.h>
#include "VariablesG.h"
#include "I2C.h"

/**
* set_imagen
* @brief envía identificador de imagen a la pantalla para que se muestre
* @param lcd pantalla que se va a usar
* @param id número de imágen a mostrar
*
*/
void set_imagen(uint8 lcd, uint16 id){
    if(lcd==1){
        LCD_1_PutChar(0xAA);
        LCD_1_PutChar(0x70);
        LCD_1_PutChar(((id>>8)&0xFF));
        LCD_1_PutChar((id&0xFF));
        LCD_1_PutChar(0xCC);
        LCD_1_PutChar(0x33);
        LCD_1_PutChar(0xC3);
        LCD_1_PutChar(0x3C);
    }
    else{
       	LCD_2_PutChar(0xAA);
        LCD_2_PutChar(0x70);
        LCD_2_PutChar(((id>>8)&0xFF));
        LCD_2_PutChar((id&0xFF));
        LCD_2_PutChar(0xCC);
        LCD_2_PutChar(0x33);
        LCD_2_PutChar(0xC3);
        LCD_2_PutChar(0x3C);   		 
    }
}

/**
* write_LCD
* @brief función para escribir caracteres en pantalla, escribe byte a byte
* @param lcd pantalla que se va a usar
* @param dato caracter a mostrar
* @param pos poscición en pantalla 
*
*/
void write_LCD(uint8_t lcd,uint8_t dato, uint16_t posy, uint16_t posx,uint8_t size){
	uint8_t buffer[18]={0xAA,0x98,0,0,0x01,0x39,0x23,0xC5,0x02,0x00,0x00,0xFF,0xFF,0,0xCC,0x33,0xC3,0x3C},x;
	buffer[2]=(0x0C*posx)>>8;
	buffer[3]=(0x0C*posx)&0xFF;
	buffer[4]=(0x0F*posy)>>8;
	buffer[5]=(0x0F*posy)&0xFF;
	buffer[6]=0x23+size;
	buffer[13]=dato;
	for(x=0;x<=17;x++){
		if(lcd==1){
			LCD_1_PutChar(buffer[x]);
		}
		else{
			LCD_2_PutChar(buffer[x]);
		}	
	}	
}

/**
* Hora_LCD
* @brief escribe la hora actual en la pantalla
* @param lcd pantalla que se va a usar
*
*/
void Hora_LCD(uint8 lcd){
	uint8 x;
	uint8 horal[24]={0xAA,0x98,0x00,0xd0,0x00,0xbb,0x24,0xC5,0x02,0x00,0x00,0xFF,0xFF,0X30,0x30,':',0x30,0x30,0x41,'m',0xCC,0x33,0xC3,0x3C};	
	horal[13]=(((hora[1]&0x10)>>4)+48);
	horal[14]=((hora[1]&0x0F)+48);
	horal[16]=(((hora[0]&0xF0)>>4)+48);
	horal[17]=((hora[0]&0x0F)+48);
	if((hora[1]&0x20)==0x20){
		horal[18]='p';
	}
	else{
		horal[18]='a';						
	}	
    if(lcd==1){
		for(x=0;x<=23;x++){
        	LCD_1_PutChar(horal[x]);
		}
    }
    else{
		for(x=0;x<=23;x++){
	        LCD_2_PutChar(horal[x]);
		}
    }
}

/**
* Fecha_LCD
* @brief escribe la fecha actual en la pantalla
* @param lcd pantalla que se va a usar
*
*/
void Fecha_LCD(uint8 lcd){
	uint8 x;
	uint8 fechal[25]={0xAA,0x98,0x00,0xcb,0x01,0x27,0x24,0xC5,0x02,0x00,0x00,0xFF,0xFF,0x30,0x30,0x2f,0x30,0x30,0x2f,0x30,0x30,0xCC,0x33,0xC3,0x3C};	
	fechal[13]=(((fecha[0]&0x30)>>4)+48);
	fechal[14]=((fecha[0]&0x0F)+48);
	fechal[16]=(((fecha[1]&0x10)>>4)+48);
	fechal[17]=((fecha[1]&0x0F)+48);
	fechal[19]=(((fecha[2]&0xF0)>>4)+48);
	fechal[20]=((fecha[2]&0x0F)+48);	
    if(lcd==1){
		for(x=0;x<=24;x++){
        	LCD_1_PutChar(fechal[x]);
		}
    }
    else{
		for(x=0;x<=24;x++){
	        LCD_2_PutChar(fechal[x]);
		}
    }
}

/* [] END OF FILE */
