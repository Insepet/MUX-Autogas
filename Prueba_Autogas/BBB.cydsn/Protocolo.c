/**
* @file Protocolo.c
* @Author Insepet LTDA
* @date 28/2/2016
* @brief Archivo para el manejo del dispensador, comunicación del MUX-Dispensador
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include <device.h>
#include "VariablesG.h"
#include "i2c.h"
#include "Print.h"

/*
*********************************************************************************************************
*                                            	MENSAJES
*********************************************************************************************************
*/
uint8 rx_timeout;								//0 inicia espera  1 se cumplio el tiempo
uint8 msn_tc[17]="TOTALES CORRIENTE";
uint8 msn_te[13]="TOTALES EXTRA";
uint8 msn_td[14]="TOTALES DIESEL";
uint8 msn_lecact[15]="Lectura Actual:";
uint8 msn_lecaan[17]="Lectura Anterior:";
uint8 msn_tventas[18]="Total de Ventas: $";

/**
* Interrupción de tiempo para la espera de datos del surtidor
*/
CY_ISR(Rx_Surtidor){
    Timer_RxSurtidor_ReadStatusRegister();
	rx_timeout=1;	
}


/**
* get_estado
* @brief obtiene el estado de la posicion a la que se pregunte
* @param dir direccion de la posicion
* @return estado si hay respuesta del dispensador 0 si no
*
*/
uint8 get_estado(uint8 dir){
    uint8 estado;
    Surtidor_ClearRxBuffer();	
    Surtidor_PutChar(dir);
	isr_1_StartEx(Rx_Surtidor);
    Timer_RxSurtidor_Start();
	rx_timeout=0;
    CyDelay(10);
	while(Surtidor_GetRxBufferSize()==0){
		if(rx_timeout==1){
			isr_1_Stop();
			Timer_RxSurtidor_Stop();
    		Surtidor_ClearRxBuffer();			
			return 0;
		}
	}
    estado=Surtidor_ReadRxData();
	if((estado&0x0F)==dir){
		estado=(estado&0xF0)>>4;
	    Surtidor_ClearRxBuffer();
		isr_1_Stop();
		Timer_RxSurtidor_Stop();	
	    return estado;
	}
	else{
	    Surtidor_ClearRxBuffer();
		isr_1_Stop();
		Timer_RxSurtidor_Stop();	
	    return 0;	
	}
}

/**
* ver_pos
* @brief detecta las posiciones del equipo
* @return 1 si detecta las dos posiciones 0 si no
*
*/
uint8 ver_pos (void){
uint8 x;
lado1.dir=0xff;
lado2.dir=0xff;
	CyDelay(5);
    if(get_estado(0)!=0){
		lado2.dir=0;
	}	
    for(x=1;x<=15;x++){
		CyDelay(5);
        if(get_estado(x)!=0){
			if(lado1.dir==0xFF){
				lado1.dir=x;
			}
			else if(lado2.dir==0xFF){
				lado2.dir=x;
				return 1;
			}
		}
	}
	if((lado1.dir!=0xFF)&&(lado2.dir!=0xFF)){
		return 1;
	}
	else{
		return 0;
	}
}


/**
* estado_ex
* @brief funcion para obtener el número de manija levantada
* @param val posición del surtidor, lado de la manija que se levantó
* @return 0 si error, # de manija
*
*/
uint8 estado_ex(uint8 val){
    uint8 rx_extend[19],x,comand_exten[9]={0xFF,0xE9,0xFE,0xE0,0xE1,0xE0,0xFB,0xEE,0xF0};
	Surtidor_ClearRxBuffer();
    Surtidor_PutChar(0x20|val);
	isr_1_StartEx(Rx_Surtidor);
    Timer_RxSurtidor_Start();	
    rx_timeout=0;
	while(Surtidor_GetRxBufferSize()==0){
		if(rx_timeout==1){
			isr_1_Stop();
			Timer_RxSurtidor_Stop();
    		Surtidor_ClearRxBuffer();			
			return 0;
		}
	}
	isr_1_Stop();
	Timer_RxSurtidor_Stop();	
    if(Surtidor_ReadRxData()==(0xD0|val)){
        Surtidor_ClearRxBuffer();
		CyDelay(5);
        for(x=0;x<=8;x++){
           Surtidor_PutChar(comand_exten[x]); 
        }
		isr_1_StartEx(Rx_Surtidor);
	    Timer_RxSurtidor_Start();	
	    rx_timeout=0;
		while(Surtidor_GetRxBufferSize()!=19){
			if(rx_timeout==1){
				isr_1_Stop();
				Timer_RxSurtidor_Stop();
	    		Surtidor_ClearRxBuffer();			
				return 0;
			}
		}       
		isr_1_Stop();
		Timer_RxSurtidor_Stop();		
        for(x=0;x<=18;x++){
           rx_extend[x]=Surtidor_ReadRxData(); 
        }
        Surtidor_ClearRxBuffer();
        if((rx_extend[0]==0xBA)&&(rx_extend[17]==0x8D)&&(rx_extend[18]==0x8A)&&(rx_extend[12]==0xB1)){
            switch(rx_extend[14]){
                case 0xB1:
                    return 1;
                break;
                case 0xB2:
                    return 2;
                break;
                case 0xB3:
                    return 3;
                break;
                default:
                	return 0;
				break;
            }
        }
        else{
            return 0;
        }
    }
    else{
        return 0;
    }
}

/**
* detener
* @brief Envía un stop a la posición seleccionada
* @param val posición del surtidor
*
*/
void detener(uint8 val){
    Surtidor_PutChar(0x30|val);
    CyDelay(200);
}

/**
* venta
* @brief La función se llama cuando hay un FEOT o PEOT, obtiene los datos de la última venta
* almacena la venta en eeprom para utilizar los datos en otros procesos

* @param val posición del surtidor
* @return 0 si hay error, 1 si se almacena la venta    
*
*/
uint8 venta(uint8 dir){
	uint8 x;
	Surtidor_ClearRxBuffer();
	Surtidor_PutChar(0x40|dir);
    CyDelay(300);
	if((version[1]!=7)&&(Surtidor_GetRxBufferSize()==33)){
		if((Surtidor_rxBuffer[0]==0xFF)&&(Surtidor_rxBuffer[2]==0xF8)&&(Surtidor_rxBuffer[32]==0xF0)){
			if(dir==lado1.dir){
				rventa1.producto=(Surtidor_rxBuffer[9]&0x0F)+1;
				for(x=0;x<=3;x++){
					rventa1.ppu[x]=(Surtidor_rxBuffer[x+12]&0x0F);
				}
				for(x=0;x<=5;x++){
					rventa1.volumen[x]=(Surtidor_rxBuffer[x+17]&0x0F);
				}			
				for(x=0;x<=5;x++){
					rventa1.dinero[x]=(Surtidor_rxBuffer[x+24]&0x0F);
				}
			}
			else{
				rventa2.producto=(Surtidor_rxBuffer[9]&0x0F)+1;
				for(x=0;x<=3;x++){
					rventa2.ppu[x]=(Surtidor_rxBuffer[x+12]&0x0F);
				}
				for(x=0;x<=5;x++){
					rventa2.volumen[x]=(Surtidor_rxBuffer[x+17]&0x0F);
				}			
				for(x=0;x<=5;x++){
					rventa2.dinero[x]=(Surtidor_rxBuffer[x+24]&0x0F);
				}			
			}
			return 1;
		}
		else{
			return 0;
		}
	}
	else if((version[1]==7)&&(Surtidor_GetRxBufferSize()==39)){
		if((Surtidor_rxBuffer[0]==0xFF)&&(Surtidor_rxBuffer[2]==0xF8)&&(Surtidor_rxBuffer[38]==0xF0)){
			if(dir==lado1.dir){
				rventa1.producto=(Surtidor_rxBuffer[9]&0x0F)+1;
				for(x=0;x<=5;x++){
					rventa1.ppu[x]=(Surtidor_rxBuffer[x+12]&0x0F);
				}
				for(x=0;x<=7;x++){
					rventa1.volumen[x]=(Surtidor_rxBuffer[x+19]&0x0F);
				}			
				for(x=0;x<=7;x++){
					rventa1.dinero[x]=(Surtidor_rxBuffer[x+28]&0x0F);
				}
			}
			else{
				rventa2.producto=(Surtidor_rxBuffer[9]&0x0F)+1;
				for(x=0;x<=5;x++){
					rventa2.ppu[x]=(Surtidor_rxBuffer[x+12]&0x0F);
				}
				for(x=0;x<=7;x++){
					rventa2.volumen[x]=(Surtidor_rxBuffer[x+19]&0x0F);
				}			
				for(x=0;x<=7;x++){
					rventa2.dinero[x]=(Surtidor_rxBuffer[x+28]&0x0F);
				}			
			}
			return 1;
		}
		else{
			return 0;
		}
	}
	return 0;
}

/**
* programar
* @brief Envía el preset al equipo según lo que se programe en las pantallas
* @param dir posición del surtidor
* @param grado manguera del equipo
* @param *valor valor del preset a enviar
* @param preset tipo de preset 1,2,3
* @return 0 si hay error, 1 si se realiza el preset al equipo
*
*/
uint8 programar(uint8 dir, uint8 grado, uint8 *valor, uint8 preset){
	uint8 buffer[18]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},size,temp,i,decimal;
	temp=4;
	if(preset==3){
		preset=2;
	}
	if(((version[1]==5)||(version[1]==6))&&(preset==1)){
		temp=3;
	}
	if((version[1]==5)&&(preset==2)){
		temp=5;
	}
	if((version[1]==7)&&(preset==1)){
		temp=0;
	}
	if((version[1]==7)&&(preset==2)){
		temp=2;
	}	
	decimal=0;
	for(i=1;i<=7;i++){
		if(valor[i]==','){
			decimal=i;	
		}
	}
	if(decimal>4){
		return 0;
	}
	if(version[1]!=7){		
		buffer[0]=0xFF;
		buffer[1]=(0xE0|temp);
		buffer[2]=(0xF0|preset);
		buffer[3]=0xF4;
		buffer[4]=0xF8;	
		buffer[5]=(valor[1]|0xE0);
		buffer[6]=(valor[2]|0xE0);
		buffer[7]=(valor[3]|0xE0);
		buffer[8]=(valor[4]|0xE0);
		buffer[9]=(valor[5]|0xE0);
		buffer[10]=(valor[6]|0xE0);
		if(version[2]==2){
			if(decimal==4){
				buffer[5]=(valor[2]|0xE0);
				buffer[6]=(valor[3]|0xE0);
				buffer[7]=(valor[5]|0xE0);
				buffer[8]=(valor[6]|0xE0);
				buffer[9]=(valor[7]|0xE0);
				buffer[10]=(valor[8]|0xE0);
			}
			if(decimal==3){
				buffer[5]=(valor[1]|0xE0);
				buffer[6]=(valor[2]|0xE0);
				buffer[7]=(valor[4]|0xE0);
				buffer[8]=(valor[5]|0xE0);
				buffer[9]=(valor[6]|0xE0);
				buffer[10]=(valor[7]|0xE0);
			}
			if(decimal==2){
				buffer[5]=0xE0;
				buffer[6]=(valor[1]|0xE0);
				buffer[7]=(valor[3]|0xE0);
				buffer[8]=(valor[4]|0xE0);
				buffer[9]=(valor[5]|0xE0);	
				buffer[10]=(valor[6]|0xE0);
			}
			if(decimal==0){
				buffer[5]=0xE0;
				buffer[6]=0xE0;
				buffer[7]=(valor[1]|0xE0);
				buffer[8]=(valor[2]|0xE0);
				buffer[9]=(valor[3]|0xE0);
				buffer[10]=(valor[4]|0xE0);
			}			
		}
		buffer[11]=0xFB;
		buffer[12]=0;
		for(i=0;i<=11;i++){
			buffer[12]+=(buffer[i]&0x0F);	
		}
		buffer[12]=(((~buffer[12])+1)&0x0F)|0xE0;
		buffer[13]=0xF0;
		size=13;
		if(version[1]==5){
			buffer[10]=0xFB;
			buffer[11]=0;
			for(i=0;i<=10;i++){
				buffer[11]+=(buffer[i]&0x0F);	
			}
			buffer[11]=(((~buffer[11])+1)&0x0F)|0xE0;
			buffer[12]=0xF0;
			size=12;		
		}
		if(preset==1){
			buffer[4]=0xF6;
			buffer[5]=(0xE0|(grado-1));
			buffer[6]=0xF8;
			if(decimal==4){
				buffer[7]=(valor[2]|0xE0);
				buffer[8]=(valor[3]|0xE0);
				buffer[9]=(valor[5]|0xE0);
				buffer[10]=(valor[6]|0xE0);
				buffer[11]=(valor[7]|0xE0);
			}
			if(decimal==3){
				buffer[7]=(valor[1]|0xE0);
				buffer[8]=(valor[2]|0xE0);
				buffer[9]=(valor[4]|0xE0);
				buffer[10]=(valor[5]|0xE0);
				buffer[11]=(valor[6]|0xE0);		
			}
			if(decimal==2){
				buffer[7]=0xE0;
				buffer[8]=(valor[1]|0xE0);
				buffer[9]=(valor[3]|0xE0);
				buffer[10]=(valor[4]|0xE0);
				buffer[11]=(valor[5]|0xE0);		
			}
			if(decimal==0){
				buffer[7]=0xE0;
				buffer[8]=0xE0;
				buffer[9]=(valor[1]|0xE0);
				buffer[10]=(valor[2]|0xE0);
				buffer[11]=(valor[3]|0xE0);		
			}			
			buffer[12]=0xFB;
			buffer[13]=0;
			for(i=0;i<=12;i++){
				buffer[13]+=(buffer[i]&0x0F);	
			}
			buffer[13]=(((~buffer[13])+1)&0x0F)|0xE0;			
			buffer[14]=0xF0;
			size=14;
		}	
	}	
	if(version[1]==7){
		buffer[0]=0xFF;
		buffer[1]=(0xE0|temp);
		buffer[2]=(0xF0|preset);
		buffer[3]=0xF4;
		buffer[4]=0xF8;	
		buffer[5]=(valor[1]|0xE0);
		buffer[6]=(valor[2]|0xE0);
		buffer[7]=(valor[3]|0xE0);
		buffer[8]=(valor[4]|0xE0);
		buffer[9]=(valor[5]|0xE0);
		buffer[10]=(valor[6]|0xE0);	
		buffer[11]=(valor[7]|0xE0);
		buffer[12]=0xE0;
		buffer[13]=0xFB;
		buffer[14]=0;
		for(i=0;i<=13;i++){
			buffer[14]+=(buffer[i]&0x0F);	
		}
		buffer[14]=(((~buffer[14])+1)&0x0F)|0xE0;
		buffer[15]=0xF0;
		size=15;
		if(preset==1){
			buffer[4]=0xF6;
			buffer[5]=(0xE0|(grado-1));
			buffer[6]=0xF8;
			if(decimal==4){
				buffer[7]=(valor[1]|0xE0);
				buffer[8]=(valor[2]|0xE0);
				buffer[9]=(valor[3]|0xE0);
				buffer[10]=(valor[5]|0xE0);
				buffer[11]=(valor[6]|0xE0);
				buffer[12]=(valor[7]|0xE0);	
				buffer[13]=0xE0;	
				buffer[14]=0xE0;
			}
			if(decimal==3){
				buffer[7]=0xE0;
				buffer[8]=(valor[1]|0xE0);
				buffer[9]=(valor[2]|0xE0);
				buffer[10]=(valor[4]|0xE0);
				buffer[11]=(valor[5]|0xE0);
				buffer[12]=(valor[6]|0xE0);
				buffer[13]=(valor[7]|0xE0);	
				buffer[14]=0xE0;	
			}
			if(decimal==2){
				buffer[7]=0xE0;
				buffer[8]=0xE0;
				buffer[9]=(valor[1]|0xE0);
				buffer[10]=(valor[3]|0xE0);
				buffer[11]=(valor[4]|0xE0);
				buffer[12]=(valor[5]|0xE0);
				buffer[13]=(valor[6]|0xE0);	
				buffer[14]=(valor[7]|0xE0);	
			}
			if(decimal==0){
				buffer[7]=0xE0;
				buffer[8]=0xE0;
				buffer[9]=0xE0;
				buffer[10]=(valor[1]|0xE0);
				buffer[11]=(valor[2]|0xE0);
				buffer[12]=(valor[3]|0xE0);
				buffer[13]=(valor[4]|0xE0);	
				buffer[14]=0xE0;	
			}			
			buffer[15]=0xFB;
			buffer[16]=0;
			for(i=0;i<=15;i++){
				buffer[16]+=(buffer[i]&0x0F);	
			}
			buffer[16]=(((~buffer[16])+1)&0x0F)|0xE0;			
			buffer[17]=0xF0;
			size=17;
		}		
	}
	Surtidor_ClearRxBuffer();
	Surtidor_PutChar(0x20|dir);
	isr_1_StartEx(Rx_Surtidor);
    Timer_RxSurtidor_Start();	
    rx_timeout=0;
    CyDelay(10);
	while(Surtidor_GetRxBufferSize()==0){
		if(rx_timeout==1){
			isr_1_Stop();
			Timer_RxSurtidor_Stop();
    		Surtidor_ClearRxBuffer();			
			return 0;
		}
	}
	isr_1_Stop();
	Timer_RxSurtidor_Stop();	
	if(Surtidor_ReadRxData()==(0xD0|dir)){
        Surtidor_ClearRxBuffer();
		CyDelay(10);
        for(i=0;i<=size;i++){
           	Surtidor_PutChar(buffer[i]);
        }
		CyDelay(50);		
		if(get_estado(dir)==0){
			return 0;
		}
		else{
			return 1;
		}
	}
	else{
		return 0;
	}
}

/**
* cambiar_precio
* @brief Realiza el cambio de precio al producto según lo enviado por el Beagle
* @param dir posición del surtidor
* @param *precio apuntador al precio con el que se va a realizar el cambio
* @param num_man grado del equipo (manguera)
* @return 0 si hay error, 1 si realiza el cambio de precio
*
*/
uint8 cambiar_precio(uint8 dir, uint8 *precio, uint8 num_man){
	uint8 buffer[15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},size,x;
	if(version[1]!=7){
		buffer[0]=0xFF;
		buffer[1]=0xE5;
		buffer[2]=0xF4;
		buffer[3]=0xF6;
		buffer[4]=(0xE0|(num_man-1));
		buffer[5]=0xF7;	
		for(x=6;x<=9;x++){
			buffer[x]=(0xE0|(precio[x-6]&0x0F));
		}
		buffer[10]=0xFB;
		for(x=0;x<=10;x++){
			buffer[11]+=(buffer[x]&0x0F);	
		}
		buffer[11]=(((~buffer[11])+1)&0x0F)|0xE0;
		buffer[12]=0xF0;
		size=12;
	}
	if(version[1]==7){
		buffer[0]=0xFF;
		buffer[1]=0xE3;
		buffer[2]=0xF4;
		buffer[3]=0xF6;
		buffer[4]=(0xE0|(num_man-1));
		buffer[5]=0xF7;		
		for(x=6;x<=10;x++){
			buffer[x]=(0xE0|precio[x-6]);
		}
		buffer[11]=0xE0;
		buffer[12]=0xFB;
		for(x=0;x<=12;x++){
			buffer[13]+=(buffer[x]&0x0F);	
		}
		buffer[13]=(((~buffer[13])+1)&0x0F)|0xE0;
		buffer[14]=0xF0;
		size=14;
	}
	Surtidor_ClearRxBuffer();
	Surtidor_PutChar(0x20|dir);
	isr_1_StartEx(Rx_Surtidor);
    Timer_RxSurtidor_Start();	
    rx_timeout=0;
	while(Surtidor_GetRxBufferSize()==0){
		if(rx_timeout==1){
			isr_1_Stop();
			Timer_RxSurtidor_Stop();
    		Surtidor_ClearRxBuffer();			
			return 0;
		}
	}
	isr_1_Stop();
	Timer_RxSurtidor_Stop();
	if(Surtidor_ReadRxData()==(0xD0|dir)){
        Surtidor_ClearRxBuffer();
		CyDelay(10);
        for(x=0;x<=size;x++){
           	Surtidor_PutChar(buffer[x]);
        }
		CyDelay(50);		
		if(get_estado(dir)==0){
			return 0;
		}
		else{
			return 1;
		}
	}
	else{
		return 0;
	}
}

/**
* totales
* @brief Obtiene los totales electrónicos del equipo
* almacena los valores obtenidos en las variables ladox.totales
* @param dir posición del surtidor
* @param nmangueras numero de mangueras definidas
* @return 0 si hay error, =!0 si se realizan las operaciones   
*
*/
uint8 totales(uint8 dir, uint8 nmangueras){
	uint8 x,y;
	Surtidor_ClearRxBuffer();
    Surtidor_PutChar(0x50|dir);
    rx_timeout=0;
	CyDelay(300);
	if(Surtidor_GetRxBufferSize()>=1){
		if(dir==lado1.dir){
			if(version[1]!=7){
				CyDelay(900);
				x=Surtidor_GetRxBufferSize();
				if((x==34)||(x==64)||(x==94)){
					if((Surtidor_rxBuffer[0]==0xFF)&&(Surtidor_rxBuffer[1]==0xF6)&&(Surtidor_rxBuffer[3]==0xF9))                    
                    {                     
                    	for(y=0;y<=2;y++){// MOD: inicializa valores en cero
                    		for(x=0;x<=23;x++){
                    			lado1.totales[y][x]=0;
                    		}
                    	}                        
						for(x=0;x<=7;x++){///producto 1 6xto digito
							lado1.totales[0][x]=Surtidor_rxBuffer[x+4];
                            ///////////''modificacion actual''//// Se añade esta verificacion para toda obtencion de datos de totales en esta funcion
							if(lado1.totales[0][x]==0||(lado1.totales[0][x]&0xF0)!=0xE0){
								Surtidor_ClearRxBuffer();
								return 0;
							}
                            ///////////''modificacion actual''////
						}
						for(x=0;x<=7;x++){
							lado1.totales[0][x+12]=Surtidor_rxBuffer[x+13];
							if(lado1.totales[0][x+12]==0||(lado1.totales[0][x+12]&0xF0)!=0xE0){
								Surtidor_ClearRxBuffer();
								return 0;
							}							
						}
						for(x=0;x<=3;x++){
							lado1.ppu[0][x]=Surtidor_rxBuffer[x+22];							
						}					
						if(nmangueras==2){
							for(x=0;x<=7;x++){ ///producto 2
								lado1.totales[1][x]=Surtidor_rxBuffer[x+34];
								if(lado1.totales[1][x]==0||(lado1.totales[1][x]&0xF0)!=0xE0){
									Surtidor_ClearRxBuffer();
									return 0;
								}								
							}
							for(x=0;x<=7;x++){
								lado1.totales[1][x+12]=Surtidor_rxBuffer[x+43];
								if(lado1.totales[1][x+12]==0||(lado1.totales[1][x+12]&0xF0)!=0xE0){
									Surtidor_ClearRxBuffer();
									return 0;
								}								
							}
							for(x=0;x<=3;x++){
								lado1.ppu[1][x]=Surtidor_rxBuffer[x+52];
							}	
    						Surtidor_ClearRxBuffer();
    						return 2;                            
						}
						if(nmangueras==3){
							for(x=0;x<=7;x++){///producto 2
								lado1.totales[1][x]=Surtidor_rxBuffer[x+34];
								if(lado1.totales[1][x]==0||(lado1.totales[1][x]&0xF0)!=0xE0){
									Surtidor_ClearRxBuffer();
									return 0;
								}								
							}
							for(x=0;x<=7;x++){
								lado1.totales[1][x+12]=Surtidor_rxBuffer[x+43];
								if(lado1.totales[1][x+12]==0||(lado1.totales[1][x+12]&0xF0)!=0xE0){
									Surtidor_ClearRxBuffer();
									return 0;
								}								
							}
							for(x=0;x<=3;x++){
								lado1.ppu[1][x]=Surtidor_rxBuffer[x+52];
							}					
							for(x=0;x<=7;x++){///producto 3
								lado1.totales[2][x]=Surtidor_rxBuffer[x+64];
								if(lado1.totales[2][x]==0||(lado1.totales[2][x]&0xF0)!=0xE0){
									Surtidor_ClearRxBuffer();
									return 0;
								}								
							}
							for(x=0;x<=7;x++){
								lado1.totales[2][x+12]=Surtidor_rxBuffer[x+73];
								if(lado1.totales[2][x+12]==0||(lado1.totales[2][x+12]&0xF0)!=0xE0){
									Surtidor_ClearRxBuffer();
									return 0;
								}								
							}
							for(x=0;x<=3;x++){
								lado1.ppu[2][x]=Surtidor_rxBuffer[x+82];
							}
    						Surtidor_ClearRxBuffer();
    						return 3;                            
						}
						Surtidor_ClearRxBuffer();
						return 1;
					}
					else{
						Surtidor_ClearRxBuffer();
						return 0;
					}					
				}
				else{
					Surtidor_ClearRxBuffer();
					return 0;		
				}
			}
			else{
				CyDelay(900);
				x=Surtidor_GetRxBufferSize();
				if((x==46)||(x==88)||(x==130)){				
					if((Surtidor_rxBuffer[0]==0xFF)&&(Surtidor_rxBuffer[1]==0xF6)&&(Surtidor_rxBuffer[3]==0xF9)){
                    	for(y=0;y<=2;y++){
                    		for(x=0;x<=23;x++){
                    			lado1.totales[y][x]=0;
                    		}
                    	}                        
						for(x=0;x<=11;x++){
							lado1.totales[0][x]=Surtidor_rxBuffer[x+4];
							if(lado1.totales[0][x]==0||(lado1.totales[0][x]&0xF0)!=0xE0){
								Surtidor_ClearRxBuffer();
								return 0;
							}							
						}
						for(x=0;x<=11;x++){
							lado1.totales[0][x+12]=Surtidor_rxBuffer[x+17];
							if(lado1.totales[0][x+12]==0||(lado1.totales[0][x+12]&0xF0)!=0xE0){
								Surtidor_ClearRxBuffer();
								return 0;
							}							
						}
						for(x=0;x<=4;x++){
							lado1.ppu[0][x]=Surtidor_rxBuffer[x+30];
						}				
						if(nmangueras==2){
							for(x=0;x<=11;x++){
								lado1.totales[1][x]=Surtidor_rxBuffer[x+46];
								if(lado1.totales[1][x]==0||(lado1.totales[1][x]&0xF0)!=0xE0){
									Surtidor_ClearRxBuffer();
									return 0;
								}								
							}
							for(x=0;x<=11;x++){
								lado1.totales[1][x+12]=Surtidor_rxBuffer[x+59];
								if(lado1.totales[1][x+12]==0||(lado1.totales[1][x+12]&0xF0)!=0xE0){
									Surtidor_ClearRxBuffer();
									return 0;
								}								
							}
							for(x=0;x<=4;x++){
								lado1.ppu[1][x]=Surtidor_rxBuffer[x+72];
							}
    						Surtidor_ClearRxBuffer();
    						return 2;                            
						}
						if(nmangueras==3){
							for(x=0;x<=11;x++){
								lado1.totales[1][x]=Surtidor_rxBuffer[x+46];
								if(lado1.totales[1][x]==0||(lado1.totales[1][x]&0xF0)!=0xE0){
									Surtidor_ClearRxBuffer();
									return 0;
								}								
							}
							for(x=0;x<=11;x++){
								lado1.totales[1][x+12]=Surtidor_rxBuffer[x+59];
								if(lado1.totales[1][x+12]==0||(lado1.totales[1][x+12]&0xF0)!=0xE0){
									Surtidor_ClearRxBuffer();
									return 0;
								}								
							}
							for(x=0;x<=4;x++){
								lado1.ppu[1][x]=Surtidor_rxBuffer[x+72];
							}					
							for(x=0;x<=11;x++){
								lado1.totales[2][x]=Surtidor_rxBuffer[x+88];
								if(lado1.totales[2][x]==0||(lado1.totales[2][x]&0xF0)!=0xE0){
									Surtidor_ClearRxBuffer();
									return 0;
								}								
							}
							for(x=0;x<=11;x++){
								lado1.totales[2][x+12]=Surtidor_rxBuffer[x+101];
								if(lado1.totales[2][x+12]==0||(lado1.totales[2][x+12]&0xF0)!=0xE0){
									Surtidor_ClearRxBuffer();
									return 0;
								}								
							}
							for(x=0;x<=4;x++){
								lado1.ppu[2][x]=Surtidor_rxBuffer[x+114];
							}
    						Surtidor_ClearRxBuffer();
    						return 3;                            
						}
						Surtidor_ClearRxBuffer();
						return 1;			
					}
					else{
						Surtidor_ClearRxBuffer();
						return 0;					
					}
				}
				else{
					Surtidor_ClearRxBuffer();
					return 0;		
				}	
			}
		}
		else{
			if(version[1]!=7){
				CyDelay(900);
				x=Surtidor_GetRxBufferSize();
				if((x==34)||(x==64)||(x==94)){				
					if((Surtidor_rxBuffer[0]==0xFF)&&(Surtidor_rxBuffer[1]==0xF6)&&(Surtidor_rxBuffer[3]==0xF9)){
                    	for(y=0;y<=2;y++){
                    		for(x=0;x<=23;x++){
                    			lado2.totales[y][x]=0;
                    		}
                    	}                        
						for(x=0;x<=7;x++){
							lado2.totales[0][x]=Surtidor_rxBuffer[x+4];
							if(lado2.totales[0][x]==0||(lado2.totales[0][x]&0xF0)!=0xE0){
								Surtidor_ClearRxBuffer();
								return 0;
							}							
						}
						for(x=0;x<=7;x++){
							lado2.totales[0][x+12]=Surtidor_rxBuffer[x+13];
							if(lado2.totales[0][x+12]==0||(lado2.totales[0][x+12]&0xF0)!=0xE0){
								Surtidor_ClearRxBuffer();
								return 0;
							}							
						}
						for(x=0;x<=3;x++){
							lado2.ppu[0][x]=Surtidor_rxBuffer[x+22];
						}				
						if(nmangueras==2){
							for(x=0;x<=7;x++){
								lado2.totales[1][x]=Surtidor_rxBuffer[x+34];
								if(lado2.totales[1][x]==0||(lado2.totales[1][x]&0xF0)!=0xE0){
									Surtidor_ClearRxBuffer();
									return 0;
								}								
							}
							for(x=0;x<=7;x++){
								lado2.totales[1][x+12]=Surtidor_rxBuffer[x+43];
								if(lado2.totales[1][x+12]==0||(lado2.totales[1][x+12]&0xF0)!=0xE0){
									Surtidor_ClearRxBuffer();
									return 0;
								}								
							}
							for(x=0;x<=3;x++){
								lado2.ppu[1][x]=Surtidor_rxBuffer[x+52];
							}
    						Surtidor_ClearRxBuffer();
    						return 2;                            
						}
						if(nmangueras==3){
							for(x=0;x<=7;x++){
								lado2.totales[1][x]=Surtidor_rxBuffer[x+34];
								if(lado2.totales[1][x]==0||(lado2.totales[1][x]&0xF0)!=0xE0){
									Surtidor_ClearRxBuffer();
									return 0;
								}								
							}
							for(x=0;x<=7;x++){
								lado2.totales[1][x+12]=Surtidor_rxBuffer[x+43];
								if(lado2.totales[1][x+12]==0||(lado2.totales[1][x+12]&0xF0)!=0xE0){
									Surtidor_ClearRxBuffer();
									return 0;
								}								
							}
							for(x=0;x<=3;x++){
								lado2.ppu[1][x]=Surtidor_rxBuffer[x+52];
							}						
							for(x=0;x<=7;x++){
								lado2.totales[2][x]=Surtidor_rxBuffer[x+64];
								if(lado2.totales[2][x]==0||(lado2.totales[2][x]&0xF0)!=0xE0){
									Surtidor_ClearRxBuffer();
									return 0;
								}								
							}
							for(x=0;x<=7;x++){
								lado2.totales[2][x+12]=Surtidor_rxBuffer[x+73];
								if(lado2.totales[2][x+12]==0||(lado2.totales[2][x+12]&0xF0)!=0xE0){
									Surtidor_ClearRxBuffer();
									return 0;
								}								
							}
							for(x=0;x<=3;x++){
								lado2.ppu[2][x]=Surtidor_rxBuffer[x+82];
							}
    						Surtidor_ClearRxBuffer();
    						return 3;                            
						}
						Surtidor_ClearRxBuffer();
						return 1;			
					}
					else{
						Surtidor_ClearRxBuffer();
						return 0;		
					}
				}
				else{
					Surtidor_ClearRxBuffer();
					return 0;		
				}				
			}
			else{
				CyDelay(900);
				x=Surtidor_GetRxBufferSize();
				if((x==46)||(x==88)||(x==130)){					
					if((Surtidor_rxBuffer[0]==0xFF)&&(Surtidor_rxBuffer[1]==0xF6)&&(Surtidor_rxBuffer[3]==0xF9)){
                    	for(y=0;y<=2;y++){
                    		for(x=0;x<=23;x++){
                    			lado2.totales[y][x]=0;
                    		}
                    	}                         
						for(x=0;x<=11;x++){
							lado2.totales[0][x]=Surtidor_rxBuffer[x+4];
							if(lado2.totales[0][x]==0||(lado2.totales[0][x]&0xF0)!=0xE0){
								Surtidor_ClearRxBuffer();
								return 0;
							}							
						}
						for(x=0;x<=11;x++){
							lado2.totales[0][x+12]=Surtidor_rxBuffer[x+17];
							if(lado2.totales[0][x+12]==0||(lado2.totales[0][x+12]&0xF0)!=0xE0){
								Surtidor_ClearRxBuffer();
								return 0;
							}							
						}
						for(x=0;x<=4;x++){
							lado2.ppu[0][x]=Surtidor_rxBuffer[x+30];
						}				
						if(nmangueras==2){
							for(x=0;x<=11;x++){
								lado2.totales[1][x]=Surtidor_rxBuffer[x+46];
								if(lado2.totales[1][x]==0||(lado2.totales[1][x]&0xF0)!=0xE0){
									Surtidor_ClearRxBuffer();
									return 0;
								}								
							}
							for(x=0;x<=11;x++){
								lado2.totales[1][x+12]=Surtidor_rxBuffer[x+59];
								if(lado2.totales[1][x+12]==0||(lado2.totales[1][x+12]&0xF0)!=0xE0){
									Surtidor_ClearRxBuffer();
									return 0;
								}								
							}
							for(x=0;x<=4;x++){
								lado2.ppu[1][x]=Surtidor_rxBuffer[x+72];
							}
    						Surtidor_ClearRxBuffer();
    						return 2;                            
						}
						if(nmangueras==3){
							for(x=0;x<=11;x++){
								lado2.totales[1][x]=Surtidor_rxBuffer[x+46];
								if(lado2.totales[1][x]==0||(lado2.totales[1][x]&0xF0)!=0xE0){
									Surtidor_ClearRxBuffer();
									return 0;
								}								
							}
							for(x=0;x<=11;x++){
								lado2.totales[1][x+12]=Surtidor_rxBuffer[x+59];
								if(lado2.totales[1][x+12]==0||(lado2.totales[1][x+12]&0xF0)!=0xE0){
									Surtidor_ClearRxBuffer();
									return 0;
								}								
							}
							for(x=0;x<=4;x++){
								lado2.ppu[1][x]=Surtidor_rxBuffer[x+72];
							}					
							for(x=0;x<=11;x++){
								lado2.totales[2][x]=Surtidor_rxBuffer[x+88];
								if(lado2.totales[2][x]==0||(lado2.totales[2][x]&0xF0)!=0xE0){
									Surtidor_ClearRxBuffer();
									return 0;
								}								
							}
							for(x=0;x<=11;x++){
								lado2.totales[2][x+12]=Surtidor_rxBuffer[x+101];
								if(lado2.totales[2][x+12]==0||(lado2.totales[2][x+12]&0xF0)!=0xE0){
									Surtidor_ClearRxBuffer();
									return 0;
								}								
							}
							for(x=0;x<=4;x++){
								lado2.ppu[2][x]=Surtidor_rxBuffer[x+114];
							}
    						Surtidor_ClearRxBuffer();
    						return 3;                            
						}
						Surtidor_ClearRxBuffer();
						return 1;			
					}
					else{
						Surtidor_ClearRxBuffer();
						return 0;		
					}
				}
				else{
					Surtidor_ClearRxBuffer();
					return 0;		
				}				
			}		
		}
	}
	else{
		return 0;
	}
}

/* [] END OF FILE */
