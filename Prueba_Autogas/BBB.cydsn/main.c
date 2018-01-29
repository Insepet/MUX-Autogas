/**
* @file main.c
* @Author Insepet LTDA
* @date 28/2/2016
* @brief Archivo principal, maneja librerías y ejecuta el polling de pantallas y dispensador
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <device.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Protocolo.h"
#include "VariablesG.h"
#include "Print.h"
#include "LCD.h"
#include "ibutton.h"
#include "i2c.h"

/*
*********************************************************************************************************
*                                             DECLARACION DE FUNCIONES
*********************************************************************************************************
*/
uint8 letras[26]={0x25,0x42,0x31,0x27,0x1D,0x28,0x29,0x2A,0x22,0x2B,0x2C,0x2D,0x35,0x34,0x23,0x24,0x1B,0x1E,0x26,0x1F,0x21,0x32,0x1C,0x30,0x20,0x2F};
uint8 clave_config[5]="02412";
uint8 test[18]="Test de Impresora";
uint16 num_decimal2;
CY_ISR(animacion);
CY_ISR(animacion2);
CY_ISR(modo_mux);
uint8  idaux[8];
uint8  state_rf;
uint16 timeout_autorizacion;
uint8  estado_tanqueando; 
uint16 timeout_autorizacion2;
uint8  estado_tanqueando2;
uint8  clave_local[4]={'9','6','3','1'};
uint8  v_digitos_1;
uint8  ppuinicial=0;
uint8  Producto;
uint8  Productos[4];/// buffer para guardar los tipos de combustible
uint8  diesel=1,corriente=1,extra=1,supremo_diesel=1;
uint8  status1LP,status2LP,status1st,status2st;
uint8  Side1State=witeSide1,Side2State=witeSide2;
uint8  sinIdentificacionL1,sinIdentificacionL2;
uint8  versionpantalla[13]={'G','R','P','7','0','0','-','3','G','V','2','.','2'};

/**
* @fn init
* @brief Inicialización de periféricos, lectura de variables de eeprom externa
* inicialización de interrupciones
*  
*/ 
void init(void){
    timeout_autorizacion=0;
    timeout_autorizacion2=0;
	uint8 x;
    CyGlobalIntEnable;
    Surtidor_EnableRxInt();
	PC_Start();
    PC_EnableRxInt();
    I2C_1_Start();
    Surtidor_Start();
    Impresora_Start();
    LCD_1_Start();
    LCD_2_Start();
    LCD_1_EnableRxInt();
	LCD_2_EnableRxInt();
    Impresora_EnableRxInt();
    LP_Start();	
    LP_EnableRxInt();    	
	EEPROM_Start();
	//Timer_Modo_Start();
	//isr_5_StartEx(modo_mux);
	/***********Iniciacionde variables***************/
	lado1.grado[0][0]=0;
	lado1.grado[1][0]=0;
	lado1.grado[2][0]=0;
	lado2.grado[0][0]=0;
	lado2.grado[1][0]=0;
	lado2.grado[2][0]=0;
	/*for(y=0;y<=2;y++){
		for(x=0;x<=23;x++){
			lado1.totales[y][x]=0;
		}
	}
	for(y=0;y<=2;y++){
		for(x=0;x<=23;x++){
			lado2.totales[y][x]=0;
		}
	}*/	
    CyDelay(5);
	/******************Lectura de EEPROM*************/
	for(x=0;x<=7;x++){										//Id venta
		id_venta[x]=EEPROM_ReadByte(x);
	}
	if(id_venta[0]!=7){
		id_venta[0]=7;	
		id_venta[1]='8';
		id_venta[2]='6';
		id_venta[3]='3';
		id_venta[4]='6';
		id_venta[5]='0';
		id_venta[6]='2';
		id_venta[7]='9';        
		for(x=0;x<=7;x++){									
			EEPROM_WriteByte(id_venta[x], x);
		}
	}
	num_decimal2=((id_venta[5]&0x0F)*10000)+((id_venta[4]&0x0F)*1000)+((id_venta[3]&0x0F)*100)+((id_venta[2]&0x0F)*10)+((id_venta[1]&0x0F));    
    for(x=8;x<=12;x++){										
		id_estacion[x-8]=EEPROM_ReadByte(x);
	}	
	for(x=13;x<=14;x++){									//Logo
		id_logo[x-13]=EEPROM_ReadByte(x);
	}
	lado1.mangueras=EEPROM_ReadByte(15);					//Numero de grados y codigo de cada uno 1
	lado1.grado[0][0]=EEPROM_ReadByte(16);
	lado1.grado[1][0]=EEPROM_ReadByte(17);
	lado1.grado[2][0]=EEPROM_ReadByte(18);
	
	lado2.mangueras=EEPROM_ReadByte(19);					//Numero de grados y codigo de cada uno 1
	lado2.grado[0][0]=EEPROM_ReadByte(20);
	lado2.grado[1][0]=EEPROM_ReadByte(21);
	lado2.grado[2][0]=EEPROM_ReadByte(22);	
	
	ppux10=EEPROM_ReadByte(23);								//PPU X10

	version[1]=EEPROM_ReadByte(30);							//Version de digitos y decimales
	version[2]=EEPROM_ReadByte(31);
	version[3]=EEPROM_ReadByte(32);
		 
	lado1.dir=EEPROM_ReadByte(26);							//Lado A

	lado2.dir=EEPROM_ReadByte(28);							//Lado B

    lado1.estado=libre;
    lado2.estado=libre;
    estado_tanqueando=0;
    estado_tanqueando2=0;
	lado1.mangueras=3;
	lado2.mangueras=3;  
        
	version[2]=0;
	version[3]=3;
    CyDelay(5);	
    
}


/**
* error_op
* @brief reinicia el dispay indicado en (lcd)
* @param lcd display que se debe reiniciar (1 ó 2)
* 
*/
void error_op(uint8 lcd, uint16 imagen){
	if(lcd==1){
	   set_imagen(1,imagen);
	   flujo_LCD1=100;
	   count_protector=1;    
       lado1.estado=libre; /// con la variable lado1.libre = libre permite que el control vuelva al sistema principal
       LP_ClearRxBuffer();
	   isr_3_StartEx(animacion);  
	   Timer_Animacion_Start();
	}
	else{
	    set_imagen(2,imagen);
	    flujo_LCD2=100;
        lado2.estado=libre;
		count_protector2=1;
	    isr_4_StartEx(animacion2);  
	    Timer_Animacion2_Start();	
	}
}


/**
* init_surt
* @brief Inicia comunicación con el surtidor
* Retorna el númeo de posiciones que contesten la consulta, diseñado para máximo 2 posiciones
* equipos legacy, advantage, encore y prime de 2 posiciones
* solicita datos de venta si el equipo se encuentra en PEOT o FEOT
*  
*/
void init_surt(void){
	uint8 estado, ok=0;
	while(ok!=2){
		ok=0;
		if((lado1.dir==0xFF)||(lado2.dir==0xFF)){
			while(ver_pos()==0){};
		    while(totales(lado1.dir, lado1.mangueras)==0);			
			CyDelay(50);	
			while(totales(lado2.dir, lado2.mangueras)==0);
			CyDelay(50);	
			if(EEPROM_WriteByte(lado1.dir, 26)==CYRET_SUCCESS){
				set_imagen(1,60);								
			}
			else{
				set_imagen(1,85);
			}
			if(EEPROM_WriteByte(lado2.dir, 28)==CYRET_SUCCESS){
				set_imagen(2,60);								
			}
			else{
				set_imagen(2,85);
			}
			flujo_LCD2=0;
			flujo_LCD1=0;			
			ok=2;	
		}
		else{
			estado=get_estado(lado1.dir);										
			switch(estado){
                case 0:                     
					lado1.dir=0xFF;
				break;	
				case 0x06:
					venta(lado1.dir);
					CyDelay(50);	
					totales(lado1.dir, lado1.mangueras);
					CyDelay(50);	
					flujo_LCD1=0;					
					ok++;
				break;
				case 0x07:	
					totales(lado1.dir, lado1.mangueras);
					CyDelay(50);	
					flujo_LCD1=0;					
					ok++;
				break;
				case 0x08:
			     	flujo_LCD1=11;
					ok++;
				break;	
				case 0x09:
			     	flujo_LCD1=11;
					ok++;
				break;
		        case 0x0B:                     //Termino venta
					flujo_LCD1=12;
					ok++;
				break;		
		        case 0x0A:						//Termino venta
					flujo_LCD1=12;
					ok++;
				break;
		        case 0x0C:						//Bloqueado por surtiendo al iniciar
			     	flujo_LCD1=11;
				 	set_imagen(1,8);
					ok++;
				break;						
			}
			estado=get_estado(lado2.dir);										
			switch(estado){
		        case 0:                     
					lado2.dir=0xFF;
				break;	
				case 0x06:
					venta(lado2.dir);
					CyDelay(50);		
					totales(lado2.dir, lado2.mangueras);
					CyDelay(50);	
					flujo_LCD2=0;				
					ok++;
				break;
				case 0x07:	
					totales(lado2.dir, lado2.mangueras);
					CyDelay(50);	
					flujo_LCD2=0;					
					ok++;
				break;
				case 0x08:
			     	flujo_LCD2=11;
				 	//set_imagen(2,8);
					ok++;
				break;
				case 0x09:
			     	flujo_LCD2=11;
				 	//set_imagen(2,8);
					ok++;
				break;
		        case 0x0B:                     //Termino venta
					flujo_LCD2=12;
					ok++;
				break;	
		        case 0x0A:						//Termino venta
					flujo_LCD2=12;
					ok++;
				break;
		        case 0x0C:						//Bloqueado por surtiendo al iniciar
			     	flujo_LCD2=11;
				 	set_imagen(2,8);
					ok++;
				break;										
			}			
		}
	}
}


/**
* polling_rf
* @brief función para la comunicación MUX-BBB
* recibe las respuestas del Beagle para enviar datos al dispensador, de preset, bloqueos y solicitudes
*  
*/
void polling_rf(void){
	uint16 status1, status2,size,x,t_preset;
	uint8  precio[5],preset[8];
    
	if(PC_GetRxBufferSize()>=6){
		status1=PC_GetRxBufferSize();
		CyDelay(10);
		status2=PC_GetRxBufferSize();
		if(status1==status2){		
			if((PC_rxBuffer[0]=='B')&&(PC_rxBuffer[1]=='B')&&(PC_rxBuffer[2]=='B')){
				ok_datosRF=0;   
				switch(PC_rxBuffer[3]){
					case cautorizar:
                        if(PC_rxBuffer[4]=='1'){
    						if(PC_rxBuffer[5]=='N'){			//No autorizo el servidor		
    							error_op((PC_rxBuffer[4] & 0x0F),28);
    							break;
    						}
    						if(PC_rxBuffer[10]=='F'){		//Cambia precio
                                precio[0]=lado1.ppu[rventa1.manguera-1][0]&0x0f;	
        						precio[1]=lado1.ppu[rventa1.manguera-1][1]&0x0f;
        						precio[2]=lado1.ppu[rventa1.manguera-1][2]&0x0f;
        						precio[3]=lado1.ppu[rventa1.manguera-1][3]&0x0f;
        						precio[4]=lado1.ppu[rventa1.manguera-1][4]&0x0f;
                            	if(cambiar_precio((lado1.dir & 0x0F),precio,rventa1.manguera)==0){
    								rf_mod[0]='M';
    								rf_mod[1]='U';
    								rf_mod[2]='X';
    								rf_mod[3]=creset;
    								rf_mod[4]=cautorizar;
    								rf_mod[5]=PC_rxBuffer[4];
    								rf_mod[6]='1';
    								rf_mod[7]='*';
    								size=8;
    								ok_datosRF=1;
    								break;
    							}                                                      
      						}
                            else{
    							totales(lado1.dir, lado1.mangueras);
                                if(ppux10==1){
        							precio[0]=PC_rxBuffer[9];	
        							precio[1]=PC_rxBuffer[8];
        							precio[2]=PC_rxBuffer[7];
        							precio[3]=PC_rxBuffer[6];
        							precio[4]=0;			
                                }
                                if(ppux10==0){
        							precio[0]=PC_rxBuffer[10];	
        							precio[1]=PC_rxBuffer[9];
        							precio[2]=PC_rxBuffer[8];
        							precio[3]=PC_rxBuffer[7];
        							precio[4]=PC_rxBuffer[6];                            
                                }
    							if(cambiar_precio((lado1.dir & 0x0F),precio,rventa1.manguera)==0){
    								rf_mod[0]='M';
    								rf_mod[1]='U';
    								rf_mod[2]='X';
    								rf_mod[3]=creset;
    								rf_mod[4]=cautorizar;
    								rf_mod[5]=PC_rxBuffer[4];
    								rf_mod[6]='1';
    								rf_mod[7]='*';
    								size=8;
    								ok_datosRF=1;
    								break;
    							}
                            }
    						preset[1]=PC_rxBuffer[17];				//Preset
    						if(PC_rxBuffer[17]!=','){
    							preset[1]=PC_rxBuffer[17] & 0x0F;
    						}
    						preset[2]=PC_rxBuffer[16];
    						if(PC_rxBuffer[16]!=','){
    							preset[2]=PC_rxBuffer[16] & 0x0F;
    						}
    						preset[3]=PC_rxBuffer[15];
    						if(PC_rxBuffer[15]!=','){
    							preset[3]=PC_rxBuffer[15] & 0x0F;
    						}
    						preset[4]=PC_rxBuffer[14];
    						if(PC_rxBuffer[14]!=','){
    							preset[4]=PC_rxBuffer[14] & 0x0F;
    						}
    						preset[5]=PC_rxBuffer[13];
    						if(PC_rxBuffer[13]!=','){
    							preset[5]=PC_rxBuffer[13] & 0x0F;
    						}
    						preset[6]=PC_rxBuffer[12];
    						if(PC_rxBuffer[12]!=','){
    							preset[6]=PC_rxBuffer[12] & 0x0F;
    						}
    						preset[7]=PC_rxBuffer[11];
    						if(PC_rxBuffer[11]!=','){
    							preset[7]=PC_rxBuffer[11] & 0x0F;
    						}
    						t_preset=PC_rxBuffer[18];
    						if(t_preset=='1'){
    							t_preset=2;
    						}
    						else{
    							t_preset=1;
    						}
    						if(programar((lado1.dir & 0x0F),rventa1.manguera,preset,t_preset)==1){		//Programar
                                if(ppuinicial==0){                          
                                    for(x=0;x<=4;x++){
                                	    precio[x]=lado1.ppu[0][x];	    
                                    }
                                    cambiar_precio(lado1.dir,precio,1);    
                                    for(x=0;x<=4;x++){
                                	    precio[x]=lado1.ppu[1][x];	    
                                    }    
                                    cambiar_precio(lado1.dir,precio,2);
                                    for(x=0;x<=4;x++){
                                	    precio[x]=lado1.ppu[2][x];	    
                                    }
                                    cambiar_precio(lado1.dir,precio,3);       
                                }
                                CyDelay(10);
    							Surtidor_PutChar(0x10|(lado1.dir & 0x0F));							
    							lado1.estado=tanqueo;
    							flujo_LCD1=11;
    							set_imagen(1,8);
    						}
    						else{
    							rf_mod[0]='M';
    							rf_mod[1]='U';
    							rf_mod[2]='X';
    							rf_mod[3]=creset;
    							rf_mod[4]=cautorizar;
    							rf_mod[5]=PC_rxBuffer[4];
    							rf_mod[6]='2';
    							rf_mod[7]='*';
    							size=8;
    							ok_datosRF=1;							
    						}
                        }else{////                        
    						if(PC_rxBuffer[5]=='N'){			//No autorizo el servidor		
    							error_op((PC_rxBuffer[4] & 0x0F),28);
    							break;
    						}
    						if(PC_rxBuffer[10]=='F'){		//Cambia precio
                                precio[0]=lado2.ppu[rventa2.manguera-1][0]&0x0f;	
        						precio[1]=lado2.ppu[rventa2.manguera-1][1]&0x0f;
        						precio[2]=lado2.ppu[rventa2.manguera-1][2]&0x0f;
        						precio[3]=lado2.ppu[rventa2.manguera-1][3]&0x0f;
        						precio[4]=lado2.ppu[rventa2.manguera-1][4]&0x0f;
                            	if(cambiar_precio((lado2.dir & 0x0F),precio,rventa2.manguera)==0){
    								rf_mod[0]='M';
    								rf_mod[1]='U';
    								rf_mod[2]='X';
    								rf_mod[3]=creset;
    								rf_mod[4]=cautorizar;
    								rf_mod[5]=PC_rxBuffer[4];
    								rf_mod[6]='1';
    								rf_mod[7]='*';
    								size=8;
    								ok_datosRF=1;
    								break;
    							}                                                      
      						}
                            else{
    							totales(lado2.dir, lado2.mangueras);
                                if(ppux10==1){
        							precio[0]=PC_rxBuffer[9];	
        							precio[1]=PC_rxBuffer[8];
        							precio[2]=PC_rxBuffer[7];
        							precio[3]=PC_rxBuffer[6];
        							precio[4]=0;			
                                }
                                if(ppux10==0){
        							precio[0]=PC_rxBuffer[10];	
        							precio[1]=PC_rxBuffer[9];
        							precio[2]=PC_rxBuffer[8];
        							precio[3]=PC_rxBuffer[7];
        							precio[4]=PC_rxBuffer[6];                            
                                }
    							if(cambiar_precio((lado2.dir & 0x0F),precio,rventa2.manguera)==0){
    								rf_mod[0]='M';
    								rf_mod[1]='U';
    								rf_mod[2]='X';
    								rf_mod[3]=creset;
    								rf_mod[4]=cautorizar;
    								rf_mod[5]=PC_rxBuffer[4];
    								rf_mod[6]='1';
    								rf_mod[7]='*';
    								size=8;
    								ok_datosRF=1;
    								break;
    							}
                            }
    						preset[1]=PC_rxBuffer[17];				//Preset
    						if(PC_rxBuffer[17]!=','){
    							preset[1]=PC_rxBuffer[17] & 0x0F;
    						}
    						preset[2]=PC_rxBuffer[16];
    						if(PC_rxBuffer[16]!=','){
    							preset[2]=PC_rxBuffer[16] & 0x0F;
    						}
    						preset[3]=PC_rxBuffer[15];
    						if(PC_rxBuffer[15]!=','){
    							preset[3]=PC_rxBuffer[15] & 0x0F;
    						}
    						preset[4]=PC_rxBuffer[14];
    						if(PC_rxBuffer[14]!=','){
    							preset[4]=PC_rxBuffer[14] & 0x0F;
    						}
    						preset[5]=PC_rxBuffer[13];
    						if(PC_rxBuffer[13]!=','){
    							preset[5]=PC_rxBuffer[13] & 0x0F;
    						}
    						preset[6]=PC_rxBuffer[12];
    						if(PC_rxBuffer[12]!=','){
    							preset[6]=PC_rxBuffer[12] & 0x0F;
    						}
    						preset[7]=PC_rxBuffer[11];
    						if(PC_rxBuffer[11]!=','){
    							preset[7]=PC_rxBuffer[11] & 0x0F;
    						}
    						t_preset=PC_rxBuffer[18];
    						if(t_preset=='1'){
    							t_preset=2;
    						}
    						else{
    							t_preset=1;
    						}
    						if(programar((lado2.dir & 0x0F),rventa2.manguera,preset,t_preset)==1){		//Programar
                                if(ppuinicial==0){                          
                                    for(x=0;x<=4;x++){
                                	    precio[x]=lado2.ppu[0][x];	    
                                    }
                                    cambiar_precio(lado2.dir,precio,1);    
                                    for(x=0;x<=4;x++){
                                	    precio[x]=lado2.ppu[1][x];	    
                                    }    
                                    cambiar_precio(lado2.dir,precio,2);
                                    for(x=0;x<=4;x++){
                                	    precio[x]=lado2.ppu[2][x];	    
                                    }
                                    cambiar_precio(lado2.dir,precio,3);       
                                }
                                CyDelay(10);
    							Surtidor_PutChar(0x10|(lado2.dir & 0x0F));							
    							lado2.estado=tanqueo;
    							flujo_LCD2=11;
    							set_imagen(2,8);
    						}
    						else{
    							rf_mod[0]='M';
    							rf_mod[1]='U';
    							rf_mod[2]='X';
    							rf_mod[3]=creset;
    							rf_mod[4]=cautorizar;
    							rf_mod[5]=PC_rxBuffer[4];
    							rf_mod[6]='2';
    							rf_mod[7]='*';
    							size=8;
    							ok_datosRF=1;							
    						}                        
                        }         
					break;
					case cimprimir:
                        if(PC_rxBuffer[4]=='1'){   
    						set_imagen(1,55);					
    						if(lado1.dir==(lado1.dir & 0x0F)){
    							lado1.estado=libre;
    							flujo_LCD1=0;
    						}
    						else if(lado2.dir==(PC_rxBuffer[4] & 0x0F)){
    							lado2.estado=libre;
    							flujo_LCD1=0;
    						}
                        }else{
    						set_imagen(2,55);					
    						if(lado2.dir==(lado2.dir & 0x0F)){
    							lado2.estado=libre;
    							flujo_LCD2=0;
    						}
    						else if(lado1.dir==(PC_rxBuffer[4] & 0x0F)){
    							lado1.estado=libre;
    							flujo_LCD2=0;
    						}                        
                        }
					break;
					case creset:     
                        if(PC_rxBuffer[4]=='1'){                          
                            if(estado_tanqueando==0){
        						switch(PC_rxBuffer[5]){
        							case '1':														
        								lado1.estado=libre;
        								error_op(1,28);					 //Error de id					
        							break;
        								
        							case '2':
        								lado1.estado=libre;
        								error_op(1,12);					//Gracias por su compra
        							break;
        								
        							case '3':
        								lado1.estado=libre;
        								error_op(1,85);					//Error de Operacion
        							break;
        								
        							case '4':
    //    								lado1.estado=libre;//
    //    								error_op(1,12);		//			//Fin Corte
        							break;									
        						}
                            }
                        }else{
                            if(estado_tanqueando2==0){
        						switch(PC_rxBuffer[5]){
        							case '1':														
        								lado2.estado=libre;
        								error_op(2,28);					 //Error de id					
        							break;
        								
        							case '2':
        								lado2.estado=libre;
        								error_op(2,12);					//Gracias por su compra
        							break;
        								
        							case '3':
        								lado2.estado=libre;
        								error_op(2,85);					//Error de Operacion
        							break;
        								
        							case '4':
    //    								lado2.estado=libre;//
    //    								error_op(2,12);		//			//Fin Corte
        							break;									
        						}
                            }                        
                        }
					break;
				}
				if(ok_datosRF==1){
				    for(x=0;x<size;x++){
					   PC_PutChar(rf_mod[x]);
				    }	               
				}
			}
			PC_ClearRxBuffer();
            Surtidor_ClearRxBuffer();
			status1=0;
			status2=0;			
		}		
	}
}

/**
* polling_surt
* @brief consulta el estado de las posiciones del surtidor y cambia el control del dispensador
* le da control al sistema principal si el dispensador se encuentra en estado libre.
*  
*/
void polling_surt(void){

	if((lado1.estado==libre && lado2.estado==libre) ){     
        Pin_1_Write(0);
        Pin_2_Write(0);
        Pin_3_Write(0);
        Pin_4_Write(1);
        Pin_5_Write(1);        
	} 
	else{                          
        Pin_1_Write(1);
        Pin_2_Write(1);
        Pin_3_Write(1);
        Pin_4_Write(0);
        Pin_5_Write(0);         
	}       
}

/**
* polling_LCD1
* @brief consulta el estado de la variable flujo_LCD la cual cambia su valor según el proceso actual
* con cada touch se dispara un proceso, la variable lleva el registro del proceso que se está ejecutando
* así que cada vez que se realice lectura de pantalla realiza la tarea correspondiente al caso
*
*/
void polling_LCD1(void){
	uint8 x,y,aux[10],precio[5],x_1;
    uint8 h_preset;
    uint8 preset_local[8];
	char numero[8];
	double num_decimal;
	switch(flujo_LCD1){
        case 0:
         isr_3_StartEx(animacion); 
         Timer_Animacion_Start();
         count_protector=0;
         flujo_LCD1=1;	
         Side1State = witeSide1; 
        break;
		
        case 1:    
         if(LCD_1_GetRxBufferSize()==8){ 
            totales(lado1.dir, lado1.mangueras);//
            CyDelay(50);    // 
        	rf_mod[0]='M';
    		rf_mod[1]='U';
    		rf_mod[2]='X';
    		rf_mod[3]='H';  
        	rf_mod[4]='o';
    		rf_mod[5]='l';
    		rf_mod[6]='a';
    		rf_mod[7]='*'; 
			PC_ClearRxBuffer();
		    for(x=0;x<=7;x++){
			   PC_PutChar(rf_mod[x]);
		    }				
			CyDelay(80);            
            for(x=0;x<=11;x++){     //volumen_pro1 
                if(lado1.totales[0][x]!=0 || lado1.totales[1][x]!=0 || lado1.totales[2][x]!=0){
                    isr_3_Stop(); 
                    Timer_Animacion_Stop(); 
        	        flujo_LCD1=13;
        	        set_imagen(1,99);				
                    LCD_1_ClearRxBuffer();
                }                                
            }             
         }
        break;
		
        case 2: 
         if(LCD_1_GetRxBufferSize()==8){
            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
				count_protector=0;
                if(count_teclas1<teclas1){									
                    if(LCD_1_rxBuffer[3]<=9){								//Numero de 1-9
                        count_teclas1++;
                        Buffer_LCD1[count_teclas1]=LCD_1_rxBuffer[3];
                        write_LCD(1,(LCD_1_rxBuffer[3]+0x30), posy1, ((count_teclas1*(sizeletra1+1))+posx1), sizeletra1);
                    }
                    if(LCD_1_rxBuffer[3]==0x0A){            				//Comando de 0
						if((id_teclado1==1)&&(count_teclas1==1)&&(Buffer_LCD1[1]==0)){
						}
						else{
	                        count_teclas1++;
	                        Buffer_LCD1[count_teclas1]=0;
							write_LCD(1,0x30, posy1, ((count_teclas1*(sizeletra1+1))+posx1), sizeletra1);
						}
                    }  
                    if(LCD_1_rxBuffer[3]==0x51){            	 			//Comando de Coma
                        if(count_teclas1>=1 && comas1==0){
                            count_teclas1++;
                            Buffer_LCD1[count_teclas1]=id_coma1;
							write_LCD(1,id_coma1, posy1, ((count_teclas1*(sizeletra1+1))+posx1), sizeletra1);
                            comas1=1;
                        }
                    }                    
                }
                if(LCD_1_rxBuffer[3]==0x0B){								//Cancel
                    if(count_teclas1==0){
						switch(id_teclado1){
							case 1:
							set_imagen(1,5);
							flujo_LCD1=4;
							break;
							case 2:
	                        flujo_LCD1=0;
							lado1.estado=libre;                            
							break;
							case 3:
							set_imagen(1,0);
							flujo_LCD1=0;
							lado1.estado=libre;
							break;
							case 4:
							set_imagen(1,0);
							flujo_LCD1=0;
							lado1.estado=libre;
							break;
                            case 5:
							set_imagen(1,0);
							flujo_LCD1=0;
							lado1.estado=libre;                            
                            break;
                            case 6:
                            flujo_LCD1=16;
                            set_imagen(1,95);                                                        
                            break;  
                            case 8:
                            flujo_LCD1=16;
                            set_imagen(1,95);                                                        
                            break;                             
						}
                    }
                    else{
						write_LCD(1,0x20, posy1, ((count_teclas1*(sizeletra1+1))+posx1), sizeletra1);
                        if(Buffer_LCD1[count_teclas1]==id_coma1){
                            comas1=0;
                        }
						Buffer_LCD1[count_teclas1]=0;
                        count_teclas1--;
                    }
                }
                if(LCD_1_rxBuffer[3]==0x0C){								//Enter
					switch(id_teclado1){
						case 1:
							for(x=0;x<=7;x++){
								numero[x]=0;
							}						
							if((count_teclas1>=1)&&(Buffer_LCD1[count_teclas1]!=',')){
								for(x=1;x<=count_teclas1;x++){
									if(Buffer_LCD1[x]!=','){
										numero[x-1]=Buffer_LCD1[x]+48;
									}
									else{
										numero[x-1]=Buffer_LCD1[x];
									}
								}
								num_decimal=atof(numero);
								if(((rventa1.preset[0]==1)&&(num_decimal>=800))||((rventa1.preset[0]==2)&&(num_decimal<=900)&&(num_decimal>0))){
									for(x=count_teclas1;x>=1;x--){
										rventa1.preset[x]=Buffer_LCD1[(count_teclas1-x)+1];
									}
									flujo_LCD1=5;								
                         			lado1.estado=espera;
						            set_imagen(1,7);   
								}
							}
						break;
						case 2:	
                            for(x=0;x<=10;x++){
                              rventa1.km[x]=0;
                            }
							for(x=count_teclas1;x>=1;x--){
								rventa1.km[x]=Buffer_LCD1[(count_teclas1-x)+1];
							}
	                        flujo_LCD1=4;	
	                        set_imagen(1,5);
						break;
						case 3:
							for(x=count_teclas1;x>=1;x--){
								rventa1.cedula[x]=Buffer_LCD1[(count_teclas1-x)+1]+48;
							}
                        	flujo_LCD1=2;	
                        	set_imagen(1,5);
							count_teclas1=0;
							id_teclado1=4;
							teclas1=10;
							posx1=2;
							posy1=3;
							sizeletra1=1;								
						break;
						case 4:
							for(x=count_teclas1;x>=1;x--){
								rventa1.password[x]=Buffer_LCD1[(count_teclas1-x)+1]+48;
							}
                        	flujo_LCD1=102;	
                        	set_imagen(1,57);
							lado1.estado=16;
						break;	
                        case 5:
							for(x=count_teclas1;x>=1;x--){
								rventa1.password_local[x]=Buffer_LCD1[(count_teclas1-x)+1]+48;
							}
                            if(clave_local[0]==rventa1.password_local[4] && clave_local[1]==rventa1.password_local[3] && clave_local[2]==rventa1.password_local[2] && clave_local[3]==rventa1.password_local[1]){
                        	    flujo_LCD1=16;	
                        	    set_imagen(1,38); 
                                CyDelay(200);
							    set_imagen(1,95);                                
                            }else{
                        	    flujo_LCD1=0;	
                        	    set_imagen(1,39); 
							    lado1.estado=libre;                                 
                                CyDelay(200);
							    set_imagen(1,0);                                 
                            }
                        break; 
                        case 6:                        
						    EEPROM_WriteByte(Buffer_LCD1[count_teclas1],30);
                            flujo_LCD1=16;
                            set_imagen(1,95);                               
                            CyDelay(200);                                                   
                        break;   
                        case 7:
                            flujo_LCD1=18;
                            set_imagen(1,115);
                            switch(Producto){
                                case 1:
                                    Productos[0]=Buffer_LCD1[1]+48;
                                    switch(Productos[0]){
                                        case '1':
                                        lado1.grado[0][0]=1;                                        
                                        break;
                                        case '2':
                                        lado1.grado[1][0]=1;                                        
                                        break;
                                        case '3':
                                        lado1.grado[2][0]=1;                                        
                                        break;                    
                                        default:
                                        diesel=0;
                                        break;
                                    }                                   
                                break;
                                case 2:
                                    Productos[1]=Buffer_LCD1[1]+48;
                                    switch(Productos[1]){
                                        case '1':
                                        lado1.grado[0][0]=2;                                        
                                        break;
                                        case '2':
                                        lado1.grado[1][0]=2;                                        
                                        break;
                                        case '3':
                                        lado1.grado[2][0]=2;                                        
                                        break; 
                                        default:
                                        corriente=0;
                                        break;                                        
                                    }                                      
                                break;
                                case 3:
                                    Productos[2]=Buffer_LCD1[1]+48;
                                    switch(Productos[2]){
                                        case '1':
                                        lado1.grado[0][0]=3;                                        
                                        break;
                                        case '2':
                                        lado1.grado[1][0]=3;                                        
                                        break;
                                        case '3':
                                        lado1.grado[2][0]=3;                                        
                                        break; 
                                        default:
                                        extra=0;
                                        break;                                        
                                    }                                      
                                break;
                                case 4:
                                    Productos[3]=Buffer_LCD1[1]+48;
                                    switch(Productos[3]){
                                        case '1':
                                        lado1.grado[0][0]=4;                                        
                                        break;
                                        case '2':
                                        lado1.grado[1][0]=4;                                        
                                        break;
                                        case '3':
                                        lado1.grado[2][0]=4;                                        
                                        break;
                                        default:
                                        supremo_diesel=0;
                                        break;                                      
                                    }                                      
                                break;                                    
                            }
                            write_LCD(1,Productos[0], 11, 44, 1);
                            write_LCD(1,Productos[1], 16, 44, 1); 
                            write_LCD(1,Productos[2], 20, 44, 1);
                            write_LCD(1,Productos[3], 25, 44, 1);
							lado1.estado=libre;                                 
                            CyDelay(200);                             
                        break;          
                        case 8:
                            for(x=1;x<=4;x++){
                              id_estacion[x]=0;
                            }
                            id_estacion[0]=4;
							for(x=count_teclas1;x>=1;x--){
								id_estacion[x]=Buffer_LCD1[(count_teclas1-x)+1]+0X30;
							}                            
                            if(id_estacion[1]>=0x30 && id_estacion[2]<0x30 && id_estacion[3]<0x30 && id_estacion[4]<0x30){
                                id_estacion[2]='0';
                                id_estacion[3]='0';
                                id_estacion[4]='0';                                
                            }
                            if(id_estacion[1]>=0x30 && id_estacion[2]>=0x30 && id_estacion[3]<0x30 && id_estacion[4]<0x30){
                                id_estacion[3]='0';
                                id_estacion[4]='0';                             
                            }  
                            if(id_estacion[1]>=0x30 && id_estacion[2]>=0x30 && id_estacion[3]>=0x30 && id_estacion[4]<0x30){
                                id_estacion[4]='0';                              
                            }
                    		for(x=8;x<=12;x++){	
                    			EEPROM_WriteByte(id_estacion[x-8], x);
                    		}   
                            flujo_LCD1=16;
                            set_imagen(1,95);                               
                            CyDelay(200);                                    
                        break;
					}					
                }
            }          
            LCD_1_ClearRxBuffer();
         }
		 if((count_protector>=30)&&(rventa1.tipo_venta==0)&&(id_teclado1==2)){
			count_teclas1=0;
			teclas1=7;
			posx1=4;
			posy1=3;
			sizeletra1=1;				
			set_imagen(1,10);
			flujo_LCD1=13;
			count_protector=0;
		 }		
        break;		
		
		case 3:
         if(touch_present(1)==1){
             if(touch_write(1,0x33)){
	             for(x=0;x<=7;x++){
	                 rventa1.id[x]=touch_read_byte(1);
	             }
				 crc_total=0;
				 for(x=0;x<7;x++){
				 	crc_total=crc_check(crc_total,rventa1.id[x]);
				 }					
				 if(crc_total==rventa1.id[7]){
		             set_imagen(1,19);
		             count_protector=0;              
		             isr_3_StartEx(animacion); 
		             Timer_Animacion_Start(); 
					 flujo_LCD1=101;
					 prox_caso[0][0]=2;
					 prox_caso[0][1]=14;
					 count_teclas1=0;							//Inicia el contador de teclas	
					 id_teclado1=2;
					 teclas1=10;
					 posx1=2;
					 posy1=3;
					 sizeletra1=1;	
				 }
             }
         }
         if(LCD_1_GetRxBufferSize()==8){
            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
                switch(LCD_1_rxBuffer[3]){
                    case 0x3B:
                     flujo_LCD1=0; 
					 lado1.estado=libre;                     
                    break; 
                }
            }
            //CyDelay(100);            
            LCD_1_ClearRxBuffer();
         }		
		break;		
		
        case 4:
         if(LCD_1_GetRxBufferSize()==8){
            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
                switch(LCD_1_rxBuffer[3]){
                    case 0x0F:                               	//$
					  for(x=0;x<=7;x++){
					 	rventa1.preset[x]=0;
					  }					
                      flujo_LCD1=2; 
					  count_teclas1=0;							//Inicia el contador de teclas	
					  id_teclado1=1;
					  id_coma1=',';	
                      teclas1=version[1]; 						//cantidad de teclas q puede digitar
					  rventa1.preset[0]=1;
                      comas1=0;									
                      set_imagen(1,6); 
					  posx1=4;
					  posy1=3;
					  sizeletra1=1;	
					  write_LCD(1,'$', 3, 4, 1);
                    break;
                    case 0x10: 									//G 
					  for(x=0;x<=7;x++){
					 	rventa1.preset[x]=0;
					  }						
                      flujo_LCD1=2; 
					  count_teclas1=0;							//Inicia el contador de teclas	
					  id_teclado1=1;
					  id_coma1=',';	
                      teclas1=version[1]; 
					  rventa1.preset[0]=2;
                      comas1=0;									
                      set_imagen(1,13); 
					  posx1=4;
					  posy1=3;
					  sizeletra1=1;					
					  write_LCD(1,'G', 3, 4, 1);
                    break;
                    case 0x43:  								//Full 
					  for(x=0;x<=7;x++){
					 	rventa1.preset[x]=0;
					  }
                      rventa1.preset[0]=3;					
					  for(x=5;x<=7;x++){
						rventa1.preset[x]=9;		
					  }						
		              flujo_LCD1=7;                         
		              set_imagen(1,7);
					  lado1.estado=espera;
                    break;
                    case 0x3B:                                //Cancel                     
                      flujo_LCD1=0;
                      sinIdentificacionL1=0;
					  lado1.estado=libre;                     
                    break;                    
                }
            }          
            LCD_1_ClearRxBuffer();
         } 
        break;		
		case 5:
		 CyDelay(50);
         if(LCD_1_GetRxBufferSize()==8){
            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
                if(LCD_1_rxBuffer[3]==0x7E){					//Cancel
					flujo_LCD1=0;
                }												
			}
            //CyDelay(100);            
            LCD_1_ClearRxBuffer();
			break;
		 }
         x=get_estado(lado1.dir);
         if(x==7){								//Espera a que este en listo el equipo	
			flujo_LCD1=7;
            CyDelay(50);
         }
         else if(x==0){
			CyDelay(50);
			programar(lado1.dir,1,aux,2);
         }
         else if(x==0x0C){
            error_op(1,85);
         }
		 else if(x==0x0B){                   //Termino venta
			flujo_LCD1=12;
         }						
		 else if(x==0x0A){					//Termino venta
			flujo_LCD1=12;
		 }        
        break;
		case 7:
		 CyDelay(10);
         if(LCD_1_GetRxBufferSize()==8){
            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
                if(LCD_1_rxBuffer[3]==0x7E){					//Cancel
					flujo_LCD1=0;
    				lado1.estado=libre;   
                    sinIdentificacionL1=0;                    
                }												
			}            
            LCD_1_ClearRxBuffer();
			break;
		 }        
		 rventa1.manguera=estado_ex(lado1.dir);
		 if(rventa1.manguera!=0){
			flujo_LCD1=8; 
            CyDelay(10);		
		 }
		break;
		
		case 8:
         CyDelay(10);
		 if(get_estado(lado1.dir)==7){     
			set_imagen(1,57);
            isr_3_Stop(); 
            Timer_Animacion_Stop(); 	            
			flujo_LCD1=9;
            timeout_autorizacion=0; 
            lado1.estado=listo;     
            
            count_protector=0;
	        isr_3_StartEx(animacion);  
	        Timer_Animacion_Start();            
            
		 }
		break;
		case 9:
        ///case 9 envía los datos al Beagle para autorización
            if(sinIdentificacionL1!=1){
    			rf_mod[0]='M';
    			rf_mod[1]='U';
    			rf_mod[2]='X';
    			rf_mod[3]='0';
    			for(x=0;x<=15;x++){						//Serial				
    				y=(rventa1.id[x/2]>>4)&0x0F;
    				if((x%2)==0){
    					y=rventa1.id[x/2]&0x0F;
    				}
    				rf_mod[x+4] = y+48;
    				if(y>9){
    					rf_mod[x+4] = y+55;
    				}
    			}
    			rf_mod[20]=(lado1.grado[rventa1.manguera-1][0])+48;			//Id Producto	
    			//rf_mod[20]=2+48;												//Id Producto	
    			rf_mod[21]=id_estacion[4];										//Id Estación
    			rf_mod[22]=id_estacion[3];
    			rf_mod[23]=id_estacion[2];
    			rf_mod[24]=id_estacion[1];//4			
                if(ppux10==0){
        			rf_mod[25]=(lado1.ppu[rventa1.manguera-1][4]&0x0F)+0x30;		//PPU
        			rf_mod[26]=(lado1.ppu[rventa1.manguera-1][3]&0x0F)+0x30;
        			rf_mod[27]=(lado1.ppu[rventa1.manguera-1][2]&0x0F)+0x30;
        			rf_mod[28]=(lado1.ppu[rventa1.manguera-1][1]&0x0F)+0x30;
        			rf_mod[29]=(lado1.ppu[rventa1.manguera-1][0]&0x0F)+0x30;
                }
                if(ppux10==1){
        			rf_mod[25]=(lado1.ppu[rventa1.manguera-1][3]&0x0F)+0x30;		//PPUx10
        			rf_mod[26]=(lado1.ppu[rventa1.manguera-1][2]&0x0F)+0x30;
        			rf_mod[27]=(lado1.ppu[rventa1.manguera-1][1]&0x0F)+0x30;
        			rf_mod[28]=(lado1.ppu[rventa1.manguera-1][0]&0x0F)+0x30;
        			rf_mod[29]=0x30;   
                }
    			rf_mod[30]=(rventa1.preset[0]+48);								//Tipo de Preset
    			rf_mod[31]=rventa1.preset[7];									//Preset
    			if(rventa1.preset[7]<10){				
    				rf_mod[31]=rventa1.preset[7]+0x30;							
    			}
    			rf_mod[32]=rventa1.preset[6];
    			if(rventa1.preset[6]<10){				
    				rf_mod[32]=rventa1.preset[6]+0x30;
    			}
    			rf_mod[33]=rventa1.preset[5];
    			if(rventa1.preset[5]<10){				
    				rf_mod[33]=rventa1.preset[5]+0x30;
    			}
    			rf_mod[34]=rventa1.preset[4];
    			if(rventa1.preset[4]<10){				
    				rf_mod[34]=rventa1.preset[4]+0x30;
    			}
    			rf_mod[35]=rventa1.preset[3];
    			if(rventa1.preset[3]<10){				
    				rf_mod[35]=rventa1.preset[3]+0x30;
    			}
    			rf_mod[36]=rventa1.preset[2];
    			if(rventa1.preset[2]<10){				
    				rf_mod[36]=rventa1.preset[2]+0x30;
    			}
    			rf_mod[37]=rventa1.preset[1];
    			if(rventa1.preset[1]<10){				
    				rf_mod[37]=rventa1.preset[1]+0x30;
    			}
    			for(x=0;x<=6;x++){
    				rf_mod[38+x]=rventa1.km[x+1] + 48;
    			}
    			rf_mod[45]=0x31;
    			rf_mod[46]='*';
    			PC_ClearRxBuffer();
    		    for(x=0;x<=46;x++){
    			   PC_PutChar(rf_mod[x]);
    		    }				
    			CyDelay(250);
    		    if(PC_GetRxBufferSize()==2){
    				if((PC_rxBuffer[0]=='O') && (PC_rxBuffer[1]=='K')){
                        timeout_autorizacion=2;
    					lado1.estado=espera;
    					flujo_LCD1=10;
                        isr_3_Stop(); 
                        Timer_Animacion_Stop(); 
                        count_protector=0;
    					PC_ClearRxBuffer();
                        break;
    				}	
    			} 
                if(count_protector>15){ 
                   isr_3_Stop(); 
                   Timer_Animacion_Stop(); 			
        		   count_protector=0;
    			   flujo_LCD1=0;
                   lado1.estado=libre;               
                }
    	        if(LCD_1_GetRxBufferSize()==8){
    	            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){                                       
                        switch(LCD_1_rxBuffer[3]){                                                                              
    	                    break;					
    	                    case 0x7E:									
    						   set_imagen(1,0);	
                               isr_3_Stop(); 
                               Timer_Animacion_Stop(); 			
                    		   count_protector=0;
                			   flujo_LCD1=0;
                               lado1.estado=libre;                         
                            
    	                    break;					
    	                }
    	            }
    	            CyDelay(100);
    	            LCD_1_ClearRxBuffer();
    	        }                
            }else{
                switch(rventa1.preset[0]){
                    case 1:
                        h_preset=2;
                    break;
                    case 2:
                        h_preset=1;
                    break; 
                    case 3:
                        h_preset=2;
    			        if(version[1]==7){ 
                            rventa1.preset[0]=7;                        
                            rventa1.preset[1]=0;
                            rventa1.preset[2]=0;
                            rventa1.preset[3]=0;
                            rventa1.preset[4]=9;
                            rventa1.preset[5]=9;
                            rventa1.preset[6]=9;
                            rventa1.preset[7]=9; 
                        }else{
                            rventa1.preset[0]=6;                        
                            rventa1.preset[1]=0;
                            rventa1.preset[2]=0;
                            rventa1.preset[3]=0;
                            rventa1.preset[4]=9;
                            rventa1.preset[5]=9;
                            rventa1.preset[6]=9;                         
                        }
                    break;                      
                }         
                for(x=1;x<=8;x++){
					if(rventa1.preset[x]!=','){
						rventa1.preset[x]=rventa1.preset[x] & 0x0F;
					}                    
                }                            
				if(programar((lado1.dir & 0x0F),rventa1.manguera,rventa1.preset,h_preset)==1){
                    CyDelay(50);
					Surtidor_PutChar(0x10|(lado1.dir & 0x0F));							
					lado1.estado=tanqueo;
					flujo_LCD1=11;
					set_imagen(1,8);
				}    
                isr_3_Stop(); 
                Timer_Animacion_Stop();
                count_protector=0;
    		    PC_ClearRxBuffer();                
            }
		break;
		case 10:   
		    if(timeout_autorizacion>=2){
                timeout_autorizacion++;
                CyDelay(1);
                if(timeout_autorizacion>=20000){
                    timeout_autorizacion=0;
                    flujo_LCD1=9;
                }
            }	    
		break;	
		
		case 11:
         estado_tanqueando=1;
		 CyDelay(50);
		 switch(get_estado(lado1.dir)){
	         case 0x0B:                     //Termino venta
				CyDelay(100);
				for(x=0;x<=8;x++){
					rventa1.dinero[x]=0;
					rventa1.volumen[x]=0;
				}					
				if(venta(lado1.dir)==1){
					flujo_LCD1=12;
					set_imagen(1,57);                
				}
                totales(lado1.dir, lado1.mangueras);               
			 break;	
	         case 0x0A:						//Termino venta
                CyDelay(100);
				for(x=0;x<=8;x++){
					rventa1.dinero[x]=0;
					rventa1.volumen[x]=0;
				}				
				if(venta(lado1.dir)==1){
					flujo_LCD1=12;
					set_imagen(1,57);                    
				}	
                totales(lado1.dir, lado1.mangueras);
			 break;
	         case 0x06:                     //No hizo venta
				for(x=0;x<=8;x++){
					rventa1.dinero[x]=0;
					rventa1.volumen[x]=0;
				}			
				flujo_LCD1=12;
				set_imagen(1,57); 
                totales(lado1.dir, lado1.mangueras);
			 break;				 	
         }		 	
		break;
		
		case 12:
        ///case 12 envía el dato de la venta al Beagle para registro e impresión
			rf_mod[0]='M';
			rf_mod[1]='U';
			rf_mod[2]='X';
			rf_mod[3]='1';
			rf_mod[4]=1+ 48;								//Cara
			rf_mod[5]=(lado1.grado[rventa1.producto-1][0] + 48);	//Id Producto	
			rf_mod[6]=rventa1.volumen[5] + 48;						//Volumen
			rf_mod[7]=rventa1.volumen[4] + 48;
			rf_mod[8]=rventa1.volumen[3] + 48;
			rf_mod[9]=',';
			rf_mod[10]=rventa1.volumen[2] + 48;
			rf_mod[11]=rventa1.volumen[1] + 48;
			rf_mod[12]=rventa1.volumen[0] + 48;
			if(version[1]==7){
				rf_mod[13]=rventa1.dinero[7] + 48;	//Dinero 7 digitos
				rf_mod[14]=rventa1.dinero[6] + 48;
				rf_mod[15]=rventa1.dinero[5] + 48;
				rf_mod[16]=rventa1.dinero[4] + 48;
				rf_mod[17]=rventa1.dinero[3] + 48;
				rf_mod[18]=rventa1.dinero[2] + 48;
				rf_mod[19]=rventa1.dinero[1] + 48;					
			}
			else{
				rf_mod[13]=rventa1.dinero[6] + 48;	//Dinero 6 digitos 
				rf_mod[14]=rventa1.dinero[5] + 48;
				rf_mod[15]=rventa1.dinero[4] + 48;
				rf_mod[16]=rventa1.dinero[3] + 48;
				rf_mod[17]=rventa1.dinero[2] + 48;
				rf_mod[18]=rventa1.dinero[1] + 48;					
				rf_mod[19]=rventa1.dinero[0] + 48;
			}
            if(ppux10==0){
    			rf_mod[20]=rventa1.ppu[4] + 48;			//PPU
    			rf_mod[21]=rventa1.ppu[3] + 48;
    			rf_mod[22]=rventa1.ppu[2] + 48;
    			rf_mod[23]=rventa1.ppu[1] + 48;
    			rf_mod[24]=rventa1.ppu[0] + 48;
            }
            if(ppux10==1){
    			rf_mod[20]=rventa1.ppu[3] + 48;			//PPU x10
    			rf_mod[21]=rventa1.ppu[2] + 48;
    			rf_mod[22]=rventa1.ppu[1] + 48;
    			rf_mod[23]=rventa1.ppu[0] + 48;
    			rf_mod[24]=48;
            }
			rf_mod[25]=id_estacion[4];				//Id Estacion	
			rf_mod[26]=id_estacion[3];
			rf_mod[27]=id_estacion[2];
			rf_mod[28]=id_estacion[1];
			for(x=0;x<=15;x++){						//Serial				
				y=(rventa1.id[x/2]>>4)&0x0F;
				if((x%2)==0){
					y=rventa1.id[x/2]&0x0F;
				}
				rf_mod[x+29] = y+48;
				if(y>9){
					rf_mod[x+29] = y+55;
				}
			}
			num_decimal2++;
			id_venta[5]=(num_decimal2/10000)+48; 
			id_venta[4]=(num_decimal2%10000/1000)+48;
			id_venta[3]=((num_decimal2%10000)%1000)/100+48;
			id_venta[2]=((((num_decimal2%10000)%1000)%100)/10)+48;
			id_venta[1]=((((num_decimal2%10000)%1000)%100)%10)+48;//0x3a             
			for(x=1;x<=7;x++){						//Id de Venta
				rf_mod[44+x]=id_venta[x];
			}          
			for(x=1;x<=5;x++){						//Guarda Id venta
				EEPROM_WriteByte(id_venta[x],x);
			}        
			for(x=0;x<=6;x++){						//Km
				rf_mod[52+x]=rventa1.km[x+1] + 48;
			}				
			rf_mod[59]='*';
			PC_ClearRxBuffer();
		    for(x=0;x<=59;x++){
			   PC_PutChar(rf_mod[x]);
		    }		
    		CyDelay(2000);             
			lado1.estado=espera;
			flujo_LCD1=102;	
            estado_tanqueando=0;
    
			lado1.estado=libre;            
		    set_imagen(1,0);	
            flujo_LCD1=0;              
			PC_ClearRxBuffer();
			precio[0]=lado1.ppu[rventa1.producto-1][0];
			precio[1]=lado1.ppu[rventa1.producto-1][1];
			precio[2]=lado1.ppu[rventa1.producto-1][2];
			precio[3]=lado1.ppu[rventa1.producto-1][3];
			precio[4]=lado1.ppu[rventa1.producto-1][4];                
		break;
			
		case 13:  
            y=24;    
            for(x=0;x<=12;x++){
                write_LCD(1,versionpantalla[x],1,y,1);
                y=y+2;                
            } 
            y=0;
	        if(LCD_1_GetRxBufferSize()==8){
	            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){                    
                    Side1State = witeSide1;                    
                    switch(LCD_1_rxBuffer[3]){
	                    case 0x5C:                                  //Venta
                        	lado1.estado=espera;
                            sinIdentificacionL1=0;
                            rf_mod[0]='M';
    						rf_mod[1]='U';
    						rf_mod[2]='X';
    						rf_mod[3]='5'; 
                            x_1=4;                           
                            for(x=0;x<=11;x++){     
                                rf_mod[x_1]=(lado1.totales[0][x]&0X0F)+48;
                                x_1++;
                            }
                            for(x=12;x<=23;x++){    
                                rf_mod[x_1]=(lado1.totales[0][x]&0X0F)+48;
                                x_1++;
                            }         
                            for(x=0;x<=11;x++){          
                                rf_mod[x_1]=(lado1.totales[1][x]&0X0F)+48;
                                x_1++;
                            }
                            for(x=12;x<=23;x++){       
                                rf_mod[x_1]=(lado1.totales[1][x]&0X0F)+48;
                                x_1++;
                            }           
                            for(x=0;x<=11;x++){       
                                rf_mod[x_1]=(lado1.totales[2][x]&0X0F)+48;
                                x_1++;
                            }
                            for(x=12;x<=23;x++){      
                                rf_mod[x_1]=(lado1.totales[2][x]&0X0F)+48;
                                x_1++;
                            }
                            rf_mod[75]=1 + 48;  
                            rf_mod[76]='*';                                     
                            PC_ClearRxBuffer();
						    for(x=0;x<=76;x++){
							   PC_PutChar(rf_mod[x]);
						    }          
							CyDelay(250);                             
							flujo_LCD1=19;                            
                            set_imagen(1,73);
                            Side1State = witeSide1;                            
	                    break;                
	                    case 0x45:								    //Otras opciones
                            set_imagen(1,114);
                            flujo_LCD1=15;
	                    break; 
	                    case 0x59:									//Corte
							set_imagen(1,57);
							rf_mod[0]='M';
							rf_mod[1]='U';
							rf_mod[2]='X';
							rf_mod[3]='2';
                			totales(lado1.dir, lado1.mangueras);
                			CyDelay(50);  
                            x_1=4;
                            for(x=0;x<=11;x++){    
                                rf_mod[x_1]=(lado1.totales[0][x]&0X0F)+48;
                                x_1++;
                            }
                            for(x=12;x<=23;x++){   
                                rf_mod[x_1]=(lado1.totales[0][x]&0X0F)+48;
                                x_1++;
                            }         
                            for(x=0;x<=11;x++){       
                                rf_mod[x_1]=(lado1.totales[1][x]&0X0F)+48;
                                x_1++;
                            }
                            for(x=12;x<=23;x++){      
                                rf_mod[x_1]=(lado1.totales[1][x]&0X0F)+48;
                                x_1++;
                            }           
                            for(x=0;x<=11;x++){     
                                rf_mod[x_1]=(lado1.totales[2][x]&0X0F)+48;
                                x_1++;
                            }
                            for(x=12;x<=23;x++){       
                                rf_mod[x_1]=(lado1.totales[2][x]&0X0F)+48;
                                x_1++;
                            }
                            rf_mod[75]=1 + 48;
                            rf_mod[76]='*';                             
                            PC_ClearRxBuffer();
						    for(x=0;x<=76;x++){
							   PC_PutChar(rf_mod[x]);
						    }                                 
							CyDelay(250);                
					        set_imagen(1,0);
					        flujo_LCD1=0;
					        lado1.estado=libre;                                               
							PC_ClearRxBuffer();                                                                              
	                    break;					
	                    case 0x7E:									//ir a menu
						  set_imagen(1,0);	
	                      flujo_LCD1=0;     
	                    break;					
	                }
	            }
	            CyDelay(100);
	            LCD_1_ClearRxBuffer();
	        }				
		break;
			
		case 14:
	        if(LCD_1_GetRxBufferSize()==8){
	            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
	                if(count_teclas1<teclas1){
	                    if(LCD_1_rxBuffer[3]<=9){
	                        rventa1.password[count_teclas1+1]=LCD_1_rxBuffer[3]+0x30;
	                        write_LCD(1,'*', posy1, ((count_teclas1*(sizeletra1+1))+posx1), sizeletra1);
	                        count_teclas1++;                        
	                    }
	                    if(LCD_1_rxBuffer[3]==0x0A){            //Comando de 0
	                        rventa1.password[count_teclas1+1]=0x30;
	                        write_LCD(1,'*', posy1, ((count_teclas1*(sizeletra1+1))+posx1), sizeletra1);
	                        count_teclas1++;                        
	                    }                    
	                }
	                if(LCD_1_rxBuffer[3]==0x0B){                //Cancelar      
	                    if(teclas1==0){
	                        flujo_LCD1=0;
	                    }
	                    else{
	                        count_teclas1--;                                        
	                        write_LCD(1,0x20, posy1, ((count_teclas1*(sizeletra1+1))+posx1), sizeletra1);
	                    }
	                }
	                if(LCD_1_rxBuffer[3]==0x0C){                //Enter
	                    if(count_teclas1>=1){
	                        rventa1.password[0]=count_teclas1;
							y=0;
							if(rventa1.password[0]==4){
								for(x=1;x<=4;x++){
									if(rventa1.password[x]==clave_config[x]){
										y++;
									}
								}
								if(y==4){
			                        flujo_LCD1=15;
			                        set_imagen(1,93);								
								}
								else{
			                        flujo_LCD1=0;							
								}
							}
							else{
		                        flujo_LCD1=0;						
							}
	                    }
	                }
	            }           
	            LCD_1_ClearRxBuffer();
	        }         			
		break;
            
        case 15:
            ///case 15 envía solicitud al Beagle de reimpresión
	        if(LCD_1_GetRxBufferSize()==8){
	            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
	                switch(LCD_1_rxBuffer[3]){
                        case 0xb9:                  //Reimpresion
                        	rf_mod[0]='M';
			                rf_mod[1]='U';
			                rf_mod[2]='X';
			                rf_mod[3]='3';
			                rf_mod[4]='1';                            
                            rf_mod[5]='*';
							PC_ClearRxBuffer();				
						    for(x=0;x<=5;x++){
							   PC_PutChar(rf_mod[x]);
						    }
                            CyDelay(150);  
							set_imagen(1,0);
							flujo_LCD1=0;
							lado1.estado=libre;                                                                                                           
                        break;
                        case 0xba:                  //Configuracion inicial 
                            flujo_LCD1=2;
                            set_imagen(1,37);
                            id_teclado1=5;
					        count_teclas1=0;							   
					        id_coma1=',';	
                            teclas1=4;              						                 							
					        posx1=4;
					        posy1=3;
					        sizeletra1=1;	                               
                        break;     
	                    case 0x7E:					
						  set_imagen(1,0);	
	                      flujo_LCD1=0;     
	                    break;		                            
                    }
                }
            }
        break;
              
        case 16:
	        if(LCD_1_GetRxBufferSize()==8){
	            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
	                switch(LCD_1_rxBuffer[3]){
                        case 0x7F:                                       
                            flujo_LCD1=18;
                            set_imagen(1,115); 
                        break;
                        case 0x80:                                         
                            flujo_LCD1=2;
                            set_imagen(1,100);
                            id_teclado1=6;
					        count_teclas1=0;							   
					        id_coma1=',';	
                            teclas1=1;              														
					        posx1=4;
					        posy1=3;
					        sizeletra1=1;                        
                        break;
                        case 0x81:     
                            flujo_LCD1=17;
                            set_imagen(1,109);                        
                        break;
                        case 0xC2:      
                            flujo_LCD1=2;
                            set_imagen(1,132);    
                            id_teclado1=8;
					        count_teclas1=0;							    //Inicia el contador de teclas	
					        id_coma1=',';	
                            teclas1=4;              						//cantidad de teclas q puede digitar 								
					        posx1=4;
					        posy1=3;
					        sizeletra1=1;                                
                        break;                             
	                    case 0x7E:		//ir a menu
						  set_imagen(1,0);	
	                      flujo_LCD1=0;     
	                    break;                                   
                    }
                }
            }            
        break;    
            
        case 17:
	        if(LCD_1_GetRxBufferSize()==8){
	            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
	                switch(LCD_1_rxBuffer[3]){
                        case 0x8f:  //X
						    EEPROM_WriteByte(0x00,23);
                        	set_imagen(1,60);
                            CyDelay(200);
                            flujo_LCD1=16;
                            set_imagen(1,95);                                                        
                        break;
                        case 0x90:  //X
						    EEPROM_WriteByte(0x01,23);
                        	set_imagen(1,60);
                            CyDelay(200);
                            flujo_LCD1=16;
                            set_imagen(1,95);                                                         
                        break;
	                    case 0x7E:	
                            flujo_LCD1=16;
                            set_imagen(1,95);                             
	                    break;                         
                    }
                }
            }              
        break;    
          
        case 18:
	        if(LCD_1_GetRxBufferSize()==8){
	            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){
	                switch(LCD_1_rxBuffer[3]){
                        case 0xbb:
                            Producto=1;                        
                            flujo_LCD1=2;
                            set_imagen(1,132);  
                            id_teclado1=7;
					        count_teclas1=0;							    //Inicia el contador de teclas	
					        id_coma1=',';	
                            teclas1=1;              						//cantidad de teclas q puede digitar                      comas1=0;									
					        posx1=4;
					        posy1=3;
					        sizeletra1=1;        
                        break;
                        case 0xbc: 
                            Producto=2;                        
                            flujo_LCD1=2;
                            set_imagen(1,132);  
                            id_teclado1=7;
					        count_teclas1=0;							    	
					        id_coma1=',';	
                            teclas1=1;              															
					        posx1=4;
					        posy1=3;
					        sizeletra1=1;        
                        break;                            
                        case 0xbd:  
                            Producto=3;                        
                            flujo_LCD1=2;
                            set_imagen(1,132);  
                            id_teclado1=7;
					        count_teclas1=0;							    
					        id_coma1=',';	
                            teclas1=1;              													
					        posx1=4;
					        posy1=3;
					        sizeletra1=1;                               
                        break;                            
                        case 0xbe:  
                            Producto=4;                        
                            flujo_LCD1=2;
                            set_imagen(1,132);  
                            id_teclado1=7;
					        count_teclas1=0;							    	
					        id_coma1=',';	
                            teclas1=1;              														
					        posx1=4;
					        posy1=3;
					        sizeletra1=1;        
                        break;                     
	                    case 0x7E:	  
                        	rf_mod[0]='M';
			                rf_mod[1]='U';
			                rf_mod[2]='X';
			                rf_mod[3]='4'; 
                            y=4;
                            for(x=0;x<=2;x++){
                                switch(lado1.grado[x][0]){
                                	case 1:
                                		rf_mod[y]='D';
                                	break;
                                	case 2:
                                		rf_mod[y]='C';
                                	break;
                                	case 3:
                                		rf_mod[y]='E';
                                	break;
                                	case 4:
                                		rf_mod[y]='S';
                                	break;
                                    default:
                                        rf_mod[y]='0';
                                    break;
                                }                                                 
                                y++;                                
                            }  
                            if(diesel==0){
                                y=0;                                    
                                for(x=4;x<=6;x++){
                                    if(rf_mod[x]=='D'){
                                       rf_mod[x]='0';     
                                        lado1.grado[y][0]=0;                                        
                                    }
                                    y++;
                                }
                            }        
                            if(corriente==0){
                                y=0;                                    
                                for(x=4;x<=6;x++){
                                    if(rf_mod[x]=='C'){
                                       rf_mod[x]='0';     
                                        lado1.grado[y][0]=0;                                        
                                    }
                                    y++;
                                }
                            }     
                            if(extra==0){
                                y=0;                                    
                                for(x=4;x<=6;x++){
                                    if(rf_mod[x]=='E'){
                                       rf_mod[x]='0';     
                                        lado1.grado[y][0]=0;                                        
                                    }
                                    y++;
                                }
                            }
                            if(supremo_diesel==0){
                                y=0;                                    
                                for(x=4;x<=6;x++){
                                    if(rf_mod[x]=='S'){
                                       rf_mod[x]='0';     
                                        lado1.grado[y][0]=0;                                        
                                    }
                                    y++;
                                }
                            }      							                            
                            rf_mod[7]=1 + 48;
                            rf_mod[8]='*';                            
							PC_ClearRxBuffer();				
						    for(x=0;x<=8;x++){
							   PC_PutChar(rf_mod[x]);
						    }                               
                            diesel=1;
                            extra=1;
                            corriente=1;
                            supremo_diesel=1;                                
                            EEPROM_WriteByte(lado1.grado[0][0],16);
                            EEPROM_WriteByte(lado1.grado[1][0],17);
                            EEPROM_WriteByte(lado1.grado[2][0],18);  
                            flujo_LCD1=16;
                            set_imagen(1,95);                              
	                    break;                         
                    }
                }
            }         
        break;
           
        case 19:
	        if(LCD_1_GetRxBufferSize()==8){
	            if((LCD_1_rxBuffer[0]==0xAA) && (LCD_1_rxBuffer[6]==0xC3) && (LCD_1_rxBuffer[7]==0x3C)){                    
                    switch(LCD_1_rxBuffer[3]){
	                    case 0x5e:                                  //Con identificacion
                            Side1State = creditSide1;       
                            switch(Side2State){
                                case witeSide2:
							        flujo_LCD1=3;
							        set_imagen(1,18);                                   
                                break;
                                case cashSide2:
							        set_imagen(1,125);
                                    CyDelay(2000);
	                                flujo_LCD1=0;                                
						            set_imagen(1,0);	                                    
                                break;
                                case creditSide2:
							        set_imagen(1,125);
                                    CyDelay(2000);
	                                flujo_LCD1=0;                                
						            set_imagen(1,0);                                    
                                break;
                                default:
                                break;
                            }        
	                    break;                
	                    case 0x5f:								    //Sin identificacion
                            Side1State = cashSide1;                         
                            for(x=0;x<=15;x++){
                                rventa1.id[x]=0;
                            }
                            for(x=0;x<=10;x++){
                              rventa1.km[x]=0;
                            }
                            switch(Side2State){
                                case witeSide2:
							        set_imagen(1,124);
                                    CyDelay(2000);
	                                flujo_LCD1=0;                                
						            set_imagen(1,0);
					                lado1.estado=libre;                                       
                                break;
                                case cashSide2: 
							        set_imagen(1,125);
                                    CyDelay(2000);
	                                flujo_LCD1=0;                                
						            set_imagen(1,0);
					                lado1.estado=libre;                                       
                                break;
                                case creditSide2:
	                                flujo_LCD1=4;	
	                                set_imagen(1,5); 
                                    sinIdentificacionL1=1;
                                break;                                
                                default:
                                break;
                            }                            
	                    break;  
	                    case 0x7E:									//ir a menu
					      lado1.estado=libre;                             
	                      flujo_LCD1=0;                                
						  set_imagen(1,0);
	                    break;	
                        case 0x94:									//ir a menu
					      lado1.estado=libre;                         
	                      flujo_LCD1=0;                                
						  set_imagen(1,0);	 
	                    break;	
	                }
	            }
	            CyDelay(100);
	            LCD_1_ClearRxBuffer();
	        }				            
        break;    
            
		case 102:
            
            
        break;
        case 101:  
          if(count_protector>=2){              
            isr_3_Stop(); 
            Timer_Animacion_Stop(); 			
            flujo_LCD1=prox_caso[0][0];
			set_imagen(1,prox_caso[0][1]);
			count_protector=0;
          }            
        break;			
		
        case 100:  
         if(count_protector>=4){
            flujo_LCD1=0;
            isr_3_Stop(); 
            Timer_Animacion_Stop(); 
            count_protector=0;
         }            
        break;			
		
			
	}	
	
}


/**
* polling_LCD2
* @brief consulta el estado de la variable flujo_LCD2 la cual cambia su valor según el proceso actual
* con cada touch se dispara un proceso, la variable lleva el registro del proceso que se está ejecutando
* así que cada vez que se realice lectura de pantalla realiza la tarea correspondiente al caso
*
*/
void polling_LCD2(void){ 
    
	uint8 x,y,aux[10],precio[5],x_1;
    uint8 h_preset2;    
	char numero[8];
	double num_decimal;
	switch(flujo_LCD2){
        case 0:
         isr_4_StartEx(animacion2); 
         Timer_Animacion2_Start();
         count_protector2=0;
         flujo_LCD2=1;	
         Side2State = witeSide2; 
        break;
		
        case 1:
         if(LCD_2_GetRxBufferSize()==8){ 
            totales(lado2.dir, lado2.mangueras);
            CyDelay(50);      
        	rf_mod[0]='M';
    		rf_mod[1]='U';
    		rf_mod[2]='X';
    		rf_mod[3]='H';  
        	rf_mod[4]='o';
    		rf_mod[5]='l';
    		rf_mod[6]='a';
    		rf_mod[7]='*'; 
			PC_ClearRxBuffer();
		    for(x=0;x<=7;x++){
			   PC_PutChar(rf_mod[x]);
		    }				
			CyDelay(80);                   
            for(x=0;x<=11;x++){    
                if(lado2.totales[0][x]!=0 || lado2.totales[1][x]!=0 || lado2.totales[2][x]!=0){
                    isr_4_Stop(); 
                    Timer_Animacion2_Stop(); 
        	        flujo_LCD2=13;
        	        set_imagen(2,99);				
                    LCD_2_ClearRxBuffer();	
                }                                
            }             
         }
        break;

        case 2: 
         if(LCD_2_GetRxBufferSize()==8){
            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
				count_protector2=0;
                if(count_teclas2<teclas2){									
                    if(LCD_2_rxBuffer[3]<=9){								//Numero de 1-9
                        count_teclas2++;
                        Buffer_LCD2[count_teclas2]=LCD_2_rxBuffer[3];
                        write_LCD(2,(LCD_2_rxBuffer[3]+0x30), posy2, ((count_teclas2*(sizeletra2+1))+posx2), sizeletra2);
                    }
                    if(LCD_2_rxBuffer[3]==0x0A){            				//Comando de 0
						if((id_teclado2==1)&&(count_teclas2==1)&&(Buffer_LCD2[1]==0)){
						}
						else{
	                        count_teclas2++;
	                        Buffer_LCD2[count_teclas2]=0;
							write_LCD(2,0x30, posy2, ((count_teclas2*(sizeletra2+1))+posx2), sizeletra2);
						}
                    }  
                    if(LCD_2_rxBuffer[3]==0x51){            	 			//Comando de Coma
                        if(count_teclas2>=1 && comas2==0){
                            count_teclas2++;
                            Buffer_LCD2[count_teclas2]=id_coma2;
							write_LCD(2,id_coma2, posy2, ((count_teclas2*(sizeletra2+1))+posx2), sizeletra2);
                            comas2=1;
                        }
                    }                    
                }
                if(LCD_2_rxBuffer[3]==0x0B){								//Cancel
                    if(count_teclas2==0){
						switch(id_teclado2){
							case 1:
							set_imagen(2,5);
							flujo_LCD2=4;
							break;
							case 2:
	                        flujo_LCD2=0;
							lado2.estado=libre;                            
							break;
							case 3:
							set_imagen(2,0);
							flujo_LCD2=0;
							lado2.estado=libre;
							break;
							case 4:
							set_imagen(2,0);
							flujo_LCD2=0;
							lado2.estado=libre;
							break;
                            case 5:
							set_imagen(2,0);
							flujo_LCD2=0;
							lado2.estado=libre;                            
                            break;
                            case 6:
                            flujo_LCD2=16;
                            set_imagen(2,95);                                                      
                            break;  
                            case 8:
                            flujo_LCD2=16;
                            set_imagen(2,95);                                                       
                            break;                             
						}
                    }
                    else{
						write_LCD(2,0x20, posy2, ((count_teclas2*(sizeletra2+1))+posx2), sizeletra2);
                        if(Buffer_LCD2[count_teclas2]==id_coma2){
                            comas2=0;
                        }
						Buffer_LCD2[count_teclas2]=0;
                        count_teclas2--;
                    }
                }
                if(LCD_2_rxBuffer[3]==0x0C){								//Enter
					switch(id_teclado2){
						case 1:
							for(x=0;x<=7;x++){
								numero[x]=0;
							}						
							if((count_teclas2>=1)&&(Buffer_LCD2[count_teclas2]!=',')){
								for(x=1;x<=count_teclas2;x++){
									if(Buffer_LCD2[x]!=','){
										numero[x-1]=Buffer_LCD2[x]+48;
									}
									else{
										numero[x-1]=Buffer_LCD2[x];
									}
								}
								num_decimal=atof(numero);                                
								if(((rventa2.preset[0]==1)&&(num_decimal>=800))||((rventa2.preset[0]==2)&&(num_decimal<=900)&&(num_decimal>0))) {                                
									for(x=count_teclas2;x>=1;x--){
										rventa2.preset[x]=Buffer_LCD2[(count_teclas2-x)+1];
									}
									flujo_LCD2=5;								
                         			lado2.estado=espera;
						            set_imagen(2,7);
								}
							}
						break;
						case 2:	
                            for(x=0;x<=10;x++){
                              rventa2.km[x]=0;
                            }
							for(x=count_teclas2;x>=1;x--){
								rventa2.km[x]=Buffer_LCD2[(count_teclas2-x)+1];
							}
	                        flujo_LCD2=4;	
	                        set_imagen(2,5);
						break;
						case 3:
							for(x=count_teclas2;x>=1;x--){
								rventa2.cedula[x]=Buffer_LCD2[(count_teclas2-x)+1]+48;
							}
                        	flujo_LCD2=2;	
                        	set_imagen(2,5);
							count_teclas2=0;
							id_teclado2=4;
							teclas2=10;
							posx2=2;
							posy2=3;
							sizeletra2=1;								
						break;
						case 4:
							for(x=count_teclas2;x>=1;x--){
								rventa2.password[x]=Buffer_LCD2[(count_teclas2-x)+1]+48;
							}
                        	flujo_LCD2=102;	
                        	set_imagen(2,57);
							lado2.estado=16;
						break;	
                        case 5:
							for(x=count_teclas2;x>=1;x--){
								rventa2.password_local[x]=Buffer_LCD2[(count_teclas2-x)+1]+48;
							}
                            if(clave_local[0]==rventa2.password_local[4] && clave_local[1]==rventa2.password_local[3] && clave_local[2]==rventa2.password_local[2] && clave_local[3]==rventa2.password_local[1]){
                        	    flujo_LCD2=16;	
                        	    set_imagen(2,38); 
                                CyDelay(200);
							    set_imagen(2,95);                                
                            }else{
                        	    flujo_LCD2=0;	
                        	    set_imagen(2,39); 
							    lado2.estado=libre;                                 
                                CyDelay(200);
							    set_imagen(2,0);                                 
                            }
                        break; 
                        case 6:                        
						    EEPROM_WriteByte(Buffer_LCD2[count_teclas2],30);
                        	set_imagen(2,60);
                            CyDelay(200);
                            flujo_LCD2=16;
                            set_imagen(2,95);                                                                                 
                        break;   
                        case 7:
                            flujo_LCD2=18;
                            set_imagen(2,115);
                            switch(Producto){
                                case 1:
                                    Productos[0]=Buffer_LCD2[1]+48;
                                    switch(Productos[0]){
                                        case '1':
                                        lado2.grado[0][0]=1;                                        
                                        break;
                                        case '2':
                                        lado2.grado[1][0]=1;                                        
                                        break;
                                        case '3':
                                        lado2.grado[2][0]=1;                                        
                                        break;                    
                                        default:
                                        diesel=0;
                                        break;
                                    }                                   
                                break;
                                case 2:
                                    Productos[1]=Buffer_LCD2[1]+48;
                                    switch(Productos[1]){
                                        case '1':
                                        lado2.grado[0][0]=2;                                        
                                        break;
                                        case '2':
                                        lado2.grado[1][0]=2;                                        
                                        break;
                                        case '3':
                                        lado2.grado[2][0]=2;                                        
                                        break; 
                                        default:
                                        corriente=0;
                                        break;                                        
                                    }                                      
                                break;
                                case 3:
                                    Productos[2]=Buffer_LCD2[1]+48;
                                    switch(Productos[2]){
                                        case '1':
                                        lado2.grado[0][0]=3;                                        
                                        break;
                                        case '2':
                                        lado2.grado[1][0]=3;                                        
                                        break;
                                        case '3':
                                        lado2.grado[2][0]=3;                                        
                                        break; 
                                        default:
                                        extra=0;
                                        break;                                        
                                    }                                      
                                break;
                                case 4:
                                    Productos[3]=Buffer_LCD2[1]+48;
                                    switch(Productos[3]){
                                        case '1':
                                        lado2.grado[0][0]=4;                                        
                                        break;
                                        case '2':
                                        lado2.grado[1][0]=4;                                        
                                        break;
                                        case '3':
                                        lado2.grado[2][0]=4;                                        
                                        break;
                                        default:
                                        supremo_diesel=0;
                                        break;                                      
                                    }                                      
                                break;                                    
                            }
                            write_LCD(2,Productos[0], 11, 44, 1);
                            write_LCD(2,Productos[1], 16, 44, 1); 
                            write_LCD(2,Productos[2], 20, 44, 1);
                            write_LCD(2,Productos[3], 25, 44, 1); 
							lado2.estado=libre;                                 
                            CyDelay(200);                             
                        break;          
                        case 8:
                            for(x=1;x<=4;x++){
                              id_estacion[x]=0;
                            }
                            id_estacion[0]=4;
							for(x=count_teclas2;x>=1;x--){
								id_estacion[x]=Buffer_LCD2[(count_teclas2-x)+1]+0X30;
							}                            
                            if(id_estacion[1]>=0x30 && id_estacion[2]<0x30 && id_estacion[3]<0x30 && id_estacion[4]<0x30){
                                id_estacion[2]='0';
                                id_estacion[3]='0';
                                id_estacion[4]='0';                                
                            }
                            if(id_estacion[1]>=0x30 && id_estacion[2]>=0x30 && id_estacion[3]<0x30 && id_estacion[4]<0x30){
                                id_estacion[3]='0';
                                id_estacion[4]='0';                             
                            }  
                            if(id_estacion[1]>=0x30 && id_estacion[2]>=0x30 && id_estacion[3]>=0x30 && id_estacion[4]<0x30){
                                id_estacion[4]='0';                              
                            }
                    		for(x=8;x<=12;x++){	
                    			EEPROM_WriteByte(id_estacion[x-8], x);
                    		}   
                        	set_imagen(2,60);
                            CyDelay(200);
                            flujo_LCD2=16;
                            set_imagen(2,95);                                                                  
                        break;
					}					
                }
            }        
            LCD_2_ClearRxBuffer();
         }
		 if((count_protector2>=30)&&(rventa2.tipo_venta==0)&&(id_teclado2==2)){
			count_teclas2=0;
			teclas2=7;
			posx2=4;
			posy2=3;
			sizeletra2=1;				
			set_imagen(2,10);
			flujo_LCD2=13;
			count_protector2=0;
		 }		
        break;		        

		case 3:
         if(touch_present(2)==1){
             if(touch_write(2,0x33)){
	             for(x=0;x<=7;x++){
	                 rventa2.id[x]=touch_read_byte(2);
	             }
				 crc_total=0;
				 for(x=0;x<7;x++){
				 	crc_total=crc_check(crc_total,rventa2.id[x]);
				 }					
				 if(crc_total==rventa2.id[7]){
		             set_imagen(2,19);
		             count_protector2=0;              
		             isr_4_StartEx(animacion2); 
		             Timer_Animacion2_Start(); 
					 flujo_LCD2=101;
					 prox_caso[0][0]=2;
					 prox_caso[0][1]=14;
					 count_teclas2=0;							//Inicia el contador de teclas	
					 id_teclado2=2;
					 teclas2=10;
					 posx2=2;
					 posy2=3;
					 sizeletra2=1;	
				 }
             }
         }
         if(LCD_2_GetRxBufferSize()==8){
            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
                switch(LCD_2_rxBuffer[3]){
                    case 0x3B:
                     flujo_LCD2=0; 
					 lado2.estado=libre;                     
                    break; 
                }
            }         
            LCD_2_ClearRxBuffer();
         }		
		break;		
		
        case 4:
         if(LCD_2_GetRxBufferSize()==8){
            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
                switch(LCD_2_rxBuffer[3]){
                    case 0x0F:                               	//$
					  for(x=0;x<=7;x++){
					 	rventa2.preset[x]=0;
					  }					
                      flujo_LCD2=2; 
					  count_teclas2=0;							//Inicia el contador de teclas	
					  id_teclado2=1;
					  id_coma2=',';	
                      teclas2=version[1]; 						//cantidad de teclas q puede digitar
					  rventa2.preset[0]=1;
                      comas2=0;									
                      set_imagen(2,6); 
					  posx2=4;
					  posy2=3;
					  sizeletra2=1;	
					  write_LCD(2,'$', 3, 4, 1);
                    break;
                    
                    case 0x10: 									//G 
					  for(x=0;x<=7;x++){
					 	rventa2.preset[x]=0;
					  }						
                      flujo_LCD2=2; 
					  count_teclas2=0;							//Inicia el contador de teclas	
					  id_teclado2=1;
					  id_coma2=',';	
                      teclas2=version[1]; 
					  rventa2.preset[0]=2;
                      comas2=0;									
                      set_imagen(2,13); 
					  posx2=4;
					  posy2=3;
					  sizeletra2=1;					
					  write_LCD(2,'G', 3, 4, 1);
                    break;
                    
                    case 0x43:  								//Full 
					  for(x=0;x<=7;x++){
					 	rventa2.preset[x]=0;
					  }
                      rventa2.preset[0]=3;					
					  for(x=5;x<=7;x++){
						rventa2.preset[x]=9;		
					  }						
		              flujo_LCD2=7;                         
		              set_imagen(2,7);
					  lado2.estado=espera;
                    break;
                    
                    case 0x3B:                                //Cancel                     
                      flujo_LCD2=0;
                      sinIdentificacionL2=0;  
					  lado2.estado=libre;                     
                    break;                    
                }
            }          
            LCD_2_ClearRxBuffer();
         } 
        break;
		
		case 5:
		 CyDelay(50);
         if(LCD_2_GetRxBufferSize()==8){
            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
                if(LCD_2_rxBuffer[3]==0x7E){					//Cancel
					flujo_LCD2=0;
                }												
			}           
            LCD_2_ClearRxBuffer();
			break;
		 }
         x=get_estado(lado2.dir);
         if(x==7){								//Espera a que este en listo el equipo	
			flujo_LCD2=7;
            CyDelay(50);
         }
         else if(x==0){
			CyDelay(50);
			programar(lado2.dir,1,aux,2);
         }
         else if(x==0x0C){
            error_op(2,85);
         }
		 else if(x==0x0B){                   //Termino venta
			flujo_LCD2=12;
         }						
		 else if(x==0x0A){					//Termino venta
			flujo_LCD2=12;
		 }        
        break;
		
		case 7:
		 CyDelay(10);
         if(LCD_2_GetRxBufferSize()==8){
            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
                if(LCD_2_rxBuffer[3]==0x7E){					//Cancel
					flujo_LCD2=0;
    				lado2.estado=libre; 
                    sinIdentificacionL2=0;                     
                }												
			}          
            LCD_2_ClearRxBuffer();
			break;
		 }            
		 rventa2.manguera=estado_ex(lado2.dir);
		 if(rventa2.manguera!=0){
			flujo_LCD2=8; 
            CyDelay(10);		
		 }
		break;        
        
		case 8:
         CyDelay(10);
		 if(get_estado(lado2.dir)==7){     
			set_imagen(2,57);
            isr_4_Stop(); 
            Timer_Animacion2_Stop(); 	            
			flujo_LCD2=9;
            timeout_autorizacion2=0; 
            lado2.estado=listo;     
            
            count_protector2=0;
	        isr_4_StartEx(animacion2);  
	        Timer_Animacion2_Start();            
		 }
		break;
        	
    	case 9:
            if(sinIdentificacionL2!=1){
    			rf_mod[0]='M';
    			rf_mod[1]='U';
    			rf_mod[2]='X';
    			rf_mod[3]='0';
    			for(x=0;x<=15;x++){										
    				y=(rventa2.id[x/2]>>4)&0x0F;
    				if((x%2)==0){
    					y=rventa2.id[x/2]&0x0F;
    				}
    				rf_mod[x+4] = y+48;
    				if(y>9){
    					rf_mod[x+4] = y+55;
    				}
    			}
    			rf_mod[20]=(lado2.grado[rventa2.manguera-1][0])+48;			//Id Producto	
    			//rf_mod[20]=2+48;												//Id Producto	
    			rf_mod[21]=id_estacion[4];										//Id Estación
    			rf_mod[22]=id_estacion[3];
    			rf_mod[23]=id_estacion[2];
    			rf_mod[24]=id_estacion[1];//4			
                if(ppux10==0){
        			rf_mod[25]=(lado2.ppu[rventa2.manguera-1][4]&0x0F)+0x30;		//PPU
        			rf_mod[26]=(lado2.ppu[rventa2.manguera-1][3]&0x0F)+0x30;
        			rf_mod[27]=(lado2.ppu[rventa2.manguera-1][2]&0x0F)+0x30;
        			rf_mod[28]=(lado2.ppu[rventa2.manguera-1][1]&0x0F)+0x30;
        			rf_mod[29]=(lado2.ppu[rventa2.manguera-1][0]&0x0F)+0x30;
                }
                if(ppux10==1){
        			rf_mod[25]=(lado2.ppu[rventa2.manguera-1][3]&0x0F)+0x30;		//PPUx10
        			rf_mod[26]=(lado2.ppu[rventa2.manguera-1][2]&0x0F)+0x30;
        			rf_mod[27]=(lado2.ppu[rventa2.manguera-1][1]&0x0F)+0x30;
        			rf_mod[28]=(lado2.ppu[rventa2.manguera-1][0]&0x0F)+0x30;
        			rf_mod[29]=0x30;   
                }
    			rf_mod[30]=(rventa2.preset[0]+48);								//Tipo de Preset
    			rf_mod[31]=rventa2.preset[7];									//Preset
    			if(rventa2.preset[7]<10){				
    				rf_mod[31]=rventa2.preset[7]+0x30;							
    			}
    			rf_mod[32]=rventa2.preset[6];
    			if(rventa2.preset[6]<10){				
    				rf_mod[32]=rventa2.preset[6]+0x30;
    			}
    			rf_mod[33]=rventa2.preset[5];
    			if(rventa2.preset[5]<10){				
    				rf_mod[33]=rventa2.preset[5]+0x30;
    			}
    			rf_mod[34]=rventa2.preset[4];
    			if(rventa2.preset[4]<10){				
    				rf_mod[34]=rventa2.preset[4]+0x30;
    			}
    			rf_mod[35]=rventa2.preset[3];
    			if(rventa2.preset[3]<10){				
    				rf_mod[35]=rventa2.preset[3]+0x30;
    			}
    			rf_mod[36]=rventa2.preset[2];
    			if(rventa2.preset[2]<10){				
    				rf_mod[36]=rventa2.preset[2]+0x30;
    			}
    			rf_mod[37]=rventa2.preset[1];
    			if(rventa2.preset[1]<10){				
    				rf_mod[37]=rventa2.preset[1]+0x30;
    			}
    			for(x=0;x<=6;x++){
    				rf_mod[38+x]=rventa2.km[x+1] + 48;
    			}
    			rf_mod[45]=0x32;
    			rf_mod[46]='*';
    			PC_ClearRxBuffer();				
    		    for(x=0;x<=46;x++){
    			   PC_PutChar(rf_mod[x]);
    		    }				
    			CyDelay(250);
    		    if(PC_GetRxBufferSize()==2){
    				if((PC_rxBuffer[0]=='O') && (PC_rxBuffer[1]=='K')){
                        timeout_autorizacion2=2;
    					lado2.estado=espera;
    					flujo_LCD2=10;
                        isr_4_Stop(); 
                        Timer_Animacion2_Stop();
                        count_protector2=0;
    					PC_ClearRxBuffer();
                        break;
    				}	
    			}     
                if(count_protector2>15){ 
                   isr_4_Stop(); 
                   Timer_Animacion2_Stop(); 			
        		   count_protector2=0;
    			   flujo_LCD2=0;
                   lado2.estado=libre;            
                }  
    	        if(LCD_2_GetRxBufferSize()==8){
    	            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){                                      
                        switch(LCD_2_rxBuffer[3]){				
    	                    case 0x7E:									//ir a menu
    						   set_imagen(2,0);	
                               isr_4_Stop(); 
                               Timer_Animacion2_Stop(); 			
                    		   count_protector2=0;
                			   flujo_LCD2=0;
                               lado2.estado=libre;                             
    	                    break;					
    	                }
    	            }
    	            CyDelay(100);
    	            LCD_2_ClearRxBuffer();
    	        }	                
            }else{
                switch(rventa2.preset[0]){
                    case 1:
                        h_preset2=2;
                    break;
                    case 2:
                        h_preset2=1;
                    break; 
                    case 3:
                        h_preset2=2;
    			        if(version[1]==7){ 
                            rventa2.preset[0]=7;                        
                            rventa2.preset[1]=0;
                            rventa2.preset[2]=0;
                            rventa2.preset[3]=0;
                            rventa2.preset[4]=9;
                            rventa2.preset[5]=9;
                            rventa2.preset[6]=9;
                            rventa2.preset[7]=9; 
                        }else{
                            rventa2.preset[0]=6;                        
                            rventa2.preset[1]=0;
                            rventa2.preset[2]=0;
                            rventa2.preset[3]=0;
                            rventa2.preset[4]=9;
                            rventa2.preset[5]=9;
                            rventa2.preset[6]=9;                         
                        }
                    break;                      
                }         
                for(x=1;x<=8;x++){
					if(rventa2.preset[x]!=','){
						rventa2.preset[x]=rventa2.preset[x] & 0x0F;
					}                    
                }                            
				if(programar((lado2.dir & 0x0F),rventa2.manguera,rventa2.preset,h_preset2)==1){
                    CyDelay(50);
					Surtidor_PutChar(0x10|(lado2.dir & 0x0F));							
					lado2.estado=tanqueo;
					flujo_LCD2=11;
					set_imagen(2,8);
				}  
                isr_4_Stop(); 
                Timer_Animacion2_Stop();
                count_protector2=0;
    		    PC_ClearRxBuffer();                                         
            }
		break;

		case 10:   
		    if(timeout_autorizacion2>=2){
                timeout_autorizacion2++;
                CyDelay(1);
                if(timeout_autorizacion2>=20000){
                    timeout_autorizacion2=0;
                    flujo_LCD2=9;
                }
            }	    
		break;	
		
		case 11:
         estado_tanqueando2=1;
		 CyDelay(50);
		 switch(get_estado(lado2.dir)){
	         case 0x0B:                     //Termino venta
				CyDelay(100);
				for(x=0;x<=8;x++){
					rventa2.dinero[x]=0;
					rventa2.volumen[x]=0;
				}					
				if(venta(lado2.dir)==1){
					flujo_LCD2=12;
					set_imagen(2,57);                        
				}	
                totales(lado2.dir, lado2.mangueras);
			 break;	
				
	         case 0x0A:						//Termino venta
                CyDelay(100);
				for(x=0;x<=8;x++){
					rventa2.dinero[x]=0;
					rventa2.volumen[x]=0;
				}				
				if(venta(lado2.dir)==1){
					flujo_LCD2=12;
					set_imagen(2,57);                         
				}	
                totales(lado2.dir, lado2.mangueras);
			 break;

	         case 0x06:                     //No hizo venta
				for(x=0;x<=8;x++){
					rventa2.dinero[x]=0;
					rventa2.volumen[x]=0;
				}			
				flujo_LCD2=12;
				set_imagen(2,57);  
                totales(lado2.dir, lado2.mangueras);
			 break;				 	
         }		 	
		break;
		
		case 12:      
			rf_mod[0]='M';
			rf_mod[1]='U';
			rf_mod[2]='X';
			rf_mod[3]='1';
			rf_mod[4]=2+ 48;								//Cara
			rf_mod[5]=(lado2.grado[rventa2.producto-1][0] + 48);	//Id Producto	
			rf_mod[6]=rventa2.volumen[5] + 48;						//Volumen
			rf_mod[7]=rventa2.volumen[4] + 48;
			rf_mod[8]=rventa2.volumen[3] + 48;
			rf_mod[9]=',';
			rf_mod[10]=rventa2.volumen[2] + 48;
			rf_mod[11]=rventa2.volumen[1] + 48;
			rf_mod[12]=rventa2.volumen[0] + 48;
			if(version[1]==7){
				rf_mod[13]=rventa2.dinero[7] + 48;	//Dinero 7 digitos
				rf_mod[14]=rventa2.dinero[6] + 48;
				rf_mod[15]=rventa2.dinero[5] + 48;
				rf_mod[16]=rventa2.dinero[4] + 48;
				rf_mod[17]=rventa2.dinero[3] + 48;
				rf_mod[18]=rventa2.dinero[2] + 48;
				rf_mod[19]=rventa2.dinero[1] + 48;					
			}
			else{
				rf_mod[13]=rventa2.dinero[6] + 48;	//Dinero 6 digitos 
				rf_mod[14]=rventa2.dinero[5] + 48;
				rf_mod[15]=rventa2.dinero[4] + 48;
				rf_mod[16]=rventa2.dinero[3] + 48;
				rf_mod[17]=rventa2.dinero[2] + 48;
				rf_mod[18]=rventa2.dinero[1] + 48;					
				rf_mod[19]=rventa2.dinero[0] + 48;
			}
            if(ppux10==0){
    			rf_mod[20]=rventa2.ppu[4] + 48;			//PPU
    			rf_mod[21]=rventa2.ppu[3] + 48;
    			rf_mod[22]=rventa2.ppu[2] + 48;
    			rf_mod[23]=rventa2.ppu[1] + 48;
    			rf_mod[24]=rventa2.ppu[0] + 48;
            }
            if(ppux10==1){
    			rf_mod[20]=rventa2.ppu[3] + 48;			//PPU x10
    			rf_mod[21]=rventa2.ppu[2] + 48;
    			rf_mod[22]=rventa2.ppu[1] + 48;
    			rf_mod[23]=rventa2.ppu[0] + 48;
    			rf_mod[24]=48;
            }
			rf_mod[25]=id_estacion[4];				//Id Estacion	
			rf_mod[26]=id_estacion[3];
			rf_mod[27]=id_estacion[2];
			rf_mod[28]=id_estacion[1];
			for(x=0;x<=15;x++){						//Serial				
				y=(rventa2.id[x/2]>>4)&0x0F;
				if((x%2)==0){
					y=rventa2.id[x/2]&0x0F;
				}
				rf_mod[x+29] = y+48;
				if(y>9){
					rf_mod[x+29] = y+55;
				}
			}
			num_decimal2++;
			id_venta[5]=(num_decimal2/10000)+48; 
			id_venta[4]=(num_decimal2%10000/1000)+48;
			id_venta[3]=((num_decimal2%10000)%1000)/100+48;
			id_venta[2]=((((num_decimal2%10000)%1000)%100)/10)+48;
			id_venta[1]=((((num_decimal2%10000)%1000)%100)%10)+48;//0x3a                 
			for(x=1;x<=7;x++){						//Id de Venta
				rf_mod[44+x]=id_venta[x];
			}
			for(x=1;x<=5;x++){						//Guarda Id venta            
				EEPROM_WriteByte(id_venta[x],x);
			}
			for(x=0;x<=6;x++){						//Km
				rf_mod[52+x]=rventa2.km[x+1] + 48;
			}				
			rf_mod[59]='*';
			PC_ClearRxBuffer();	
		    for(x=0;x<=59;x++){
			   PC_PutChar(rf_mod[x]);
		    }	
    		CyDelay(2000);              
			lado2.estado=espera;
			flujo_LCD2=102;	
            estado_tanqueando2=0;
    
			lado2.estado=libre;            
		    set_imagen(2,0);	
            flujo_LCD2=0;                 
			PC_ClearRxBuffer();
			precio[0]=lado2.ppu[rventa2.producto-1][0];
			precio[1]=lado2.ppu[rventa2.producto-1][1];
			precio[2]=lado2.ppu[rventa2.producto-1][2];
			precio[3]=lado2.ppu[rventa2.producto-1][3];
			precio[4]=lado2.ppu[rventa2.producto-1][4];
			cambiar_precio(lado2.dir,precio,rventa2.producto);
		break;
			
		case 13:
            y=24;    
            for(x=0;x<=12;x++){
                write_LCD(2,versionpantalla[x],1,y,1);
                y=y+2;                
            } 
            y=0;                
	        if(LCD_2_GetRxBufferSize()==8){
	            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){                    
                    Side2State = witeSide2;                    
                    switch(LCD_2_rxBuffer[3]){
	                    case 0x5C:                                  //Venta
					        lado2.estado=espera;  
                            sinIdentificacionL2=0;                        
                            rf_mod[0]='M';
							rf_mod[1]='U';
							rf_mod[2]='X';
							rf_mod[3]='5'; 
                            x_1=4;
                            for(x=0;x<=11;x++){     
                                rf_mod[x_1]=(lado2.totales[0][x]&0X0F)+48;
                                x_1++;
                            }
                            for(x=12;x<=23;x++){    
                                rf_mod[x_1]=(lado2.totales[0][x]&0X0F)+48;
                                x_1++;
                            }         
                            for(x=0;x<=11;x++){          
                                rf_mod[x_1]=(lado2.totales[1][x]&0X0F)+48;
                                x_1++;
                            }
                            for(x=12;x<=23;x++){       
                                rf_mod[x_1]=(lado2.totales[1][x]&0X0F)+48;
                                x_1++;
                            }           
                            for(x=0;x<=11;x++){        
                                rf_mod[x_1]=(lado2.totales[2][x]&0X0F)+48;
                                x_1++;
                            }
                            for(x=12;x<=23;x++){       
                                rf_mod[x_1]=(lado2.totales[2][x]&0X0F)+48;
                                x_1++;
                            }
                            rf_mod[75]=2 + 48;    
                            rf_mod[76]='*';                                     
                            PC_ClearRxBuffer();
						    for(x=0;x<=76;x++){
							   PC_PutChar(rf_mod[x]);
						    }                                 
							CyDelay(250);                                                  
							flujo_LCD2=19;                            
                            set_imagen(2,73);                           
	                    break;                
	                    case 0x45:								    //Otras opciones
                            set_imagen(2,114);
                            flujo_LCD2=15;
	                    break; 
	                    case 0x59:									//Corte
							set_imagen(2,57);
							rf_mod[0]='M';
							rf_mod[1]='U';
							rf_mod[2]='X';
							rf_mod[3]='2';
                			totales(lado2.dir, lado2.mangueras);
                			CyDelay(50);  
                            x_1=4;
                            for(x=0;x<=11;x++){     
                                rf_mod[x_1]=(lado2.totales[0][x]&0X0F)+48;
                                x_1++;
                            }
                            for(x=12;x<=23;x++){   
                                rf_mod[x_1]=(lado2.totales[0][x]&0X0F)+48;
                                x_1++;
                            }         
                            for(x=0;x<=11;x++){       
                                rf_mod[x_1]=(lado2.totales[1][x]&0X0F)+48;
                                x_1++;
                            }
                            for(x=12;x<=23;x++){       
                                rf_mod[x_1]=(lado2.totales[1][x]&0X0F)+48;
                                x_1++;
                            }           
                            for(x=0;x<=11;x++){      
                                rf_mod[x_1]=(lado2.totales[2][x]&0X0F)+48;
                                x_1++;
                            }
                            for(x=12;x<=23;x++){       
                                rf_mod[x_1]=(lado2.totales[2][x]&0X0F)+48;
                                x_1++;
                            }
                            rf_mod[75]=2 + 48;  
                            rf_mod[76]='*';                             
                            PC_ClearRxBuffer();
						    for(x=0;x<=76;x++){
							   PC_PutChar(rf_mod[x]);
						    }                                 
							CyDelay(250);                
					        set_imagen(2,0);
					        flujo_LCD2=0;
					        lado2.estado=libre;                                               
							PC_ClearRxBuffer();                                                                              
	                    break;					
	                    case 0x7E:									//ir a menu
						  set_imagen(2,0);	
	                      flujo_LCD2=0;     
	                    break;					
	                }
	            }
	            CyDelay(100);
	            LCD_2_ClearRxBuffer();
	        }				
		break;        
        
		case 14:
	        if(LCD_2_GetRxBufferSize()==8){
	            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
	                if(count_teclas2<teclas2){
	                    if(LCD_2_rxBuffer[3]<=9){
	                        rventa2.password[count_teclas2+1]=LCD_2_rxBuffer[3]+0x30;
	                        write_LCD(2,'*', posy2, ((count_teclas2*(sizeletra2+1))+posx2), sizeletra2);
	                        count_teclas2++;                        
	                    }
	                    if(LCD_2_rxBuffer[3]==0x0A){            //Comando de 0
	                        rventa2.password[count_teclas2+1]=0x30;
	                        write_LCD(2,'*', posy2, ((count_teclas2*(sizeletra2+1))+posx2), sizeletra2);
	                        count_teclas2++;                        
	                    }                    
	                }
	                if(LCD_2_rxBuffer[3]==0x0B){                //Cancelar      
	                    if(teclas2==0){
	                        flujo_LCD2=0;
	                    }
	                    else{
	                        count_teclas2--;                                        
	                        write_LCD(2,0x20, posy2, ((count_teclas2*(sizeletra2+1))+posx2), sizeletra2);
	                    }
	                }
	                if(LCD_2_rxBuffer[3]==0x0C){                //Enter
	                    if(count_teclas2>=1){
	                        rventa2.password[0]=count_teclas2;
							y=0;
							if(rventa2.password[0]==4){
								for(x=1;x<=4;x++){
									if(rventa2.password[x]==clave_config[x]){
										y++;
									}
								}
								if(y==4){
			                        flujo_LCD2=15;
			                        set_imagen(2,93);								
								}
								else{
			                        flujo_LCD2=0;							
								}
							}
							else{
		                        flujo_LCD2=0;						
							}
	                    }
	                }
	            }          
	            LCD_2_ClearRxBuffer();
	        }         			
		break;            
            
        case 15:
	        if(LCD_2_GetRxBufferSize()==8){
	            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
	                switch(LCD_2_rxBuffer[3]){
                        case 0xb9:                  //Reimpresion
                        	rf_mod[0]='M';
			                rf_mod[1]='U';
			                rf_mod[2]='X';
			                rf_mod[3]='3';
			                rf_mod[4]='2';                            
                            rf_mod[5]='*';
							PC_ClearRxBuffer();				
						    for(x=0;x<=5;x++){
							   PC_PutChar(rf_mod[x]);
						    }
                            CyDelay(150);  
							set_imagen(2,0);
							flujo_LCD2=0;
							lado2.estado=libre;                                                                                                             
                        break;
                        case 0xba:                  //Configuracion inicial 
                            flujo_LCD2=2;
                            set_imagen(2,37);
                            id_teclado2=5;
					        count_teclas2=0;							    //Inicia el contador de teclas	
					        id_coma2=',';	
                            teclas2=4;              						//cantidad de teclas q puede digitar                      comas1=0;									
					        posx2=4;
					        posy2=3;
					        sizeletra2=1;	                               
                        break;      
	                    case 0x7E:					//ir a menu
						  set_imagen(2,0);	
	                      flujo_LCD2=0;     
	                    break;		                            
                    }
                }
            }
        break;       
            
        case 16:
	        if(LCD_2_GetRxBufferSize()==8){
	            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
	                switch(LCD_2_rxBuffer[3]){
                        case 0x7F:      
                            flujo_LCD2=18;
                            set_imagen(2,115); 
                        break;
                        case 0x80:       
                            flujo_LCD2=2;
                            set_imagen(2,100);
                            id_teclado2=6;
					        count_teclas2=0;							    //Inicia el contador de teclas	
					        id_coma2=',';	
                            teclas2=1;              						//cantidad de teclas q puede digitar                      comas1=0;									
					        posx2=4;
					        posy2=3;
					        sizeletra2=1;                        
                        break;
                        case 0x81:      
                            flujo_LCD2=17;
                            set_imagen(2,109);                        
                        break;
                        case 0xC2:    
                            flujo_LCD2=2;
                            set_imagen(2,132);    
                            id_teclado2=8;
					        count_teclas2=0;							    //Inicia el contador de teclas	
					        id_coma2=',';	
                            teclas2=4;              						//cantidad de teclas q puede digitar                      comas1=0;									
					        posx2=4;
					        posy2=3;
					        sizeletra2=1;                                
                        break;                             
	                    case 0x7E:		
						  set_imagen(2,0);	
	                      flujo_LCD2=0;     
	                    break;                                   
                    }
                }
            }            
        break;   
                           
        case 17:
	        if(LCD_2_GetRxBufferSize()==8){
	            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
	                switch(LCD_2_rxBuffer[3]){
                        case 0x8f:  //X
						    EEPROM_WriteByte(0x00,23);
                        	set_imagen(2,60);
                            CyDelay(200);
                            flujo_LCD2=16;
                            set_imagen(2,95);                                                       
                        break;
                        case 0x90:  //X
						    EEPROM_WriteByte(0x01,23);	
                        	set_imagen(2,60);
                            CyDelay(200);
                            flujo_LCD2=16;
                            set_imagen(2,95);                                                        
                        break;
	                    case 0x7E:	
                            flujo_LCD2=16;
                            set_imagen(2,95);                             
	                    break;                         
                    }
                }
            }              
        break;    
          
        case 18:
	        if(LCD_2_GetRxBufferSize()==8){
	            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){
	                switch(LCD_2_rxBuffer[3]){
                        case 0xbb:  
                            Producto=1;                        
                            flujo_LCD2=2;
                            set_imagen(2,132);  
                            id_teclado2=7;
					        count_teclas2=0;							    //Inicia el contador de teclas	
					        id_coma2=',';	
                            teclas2=1;              						//cantidad de teclas q puede digitar                      comas1=0;									
					        posx2=4;
					        posy2=3;
					        sizeletra2=1;        
                        break;
                        case 0xbc:  
                            Producto=2;                        
                            flujo_LCD2=2;
                            set_imagen(2,132);  
                            id_teclado2=7;
					        count_teclas2=0;							    	
					        id_coma2=',';	
                            teclas2=1;              															
					        posx2=4;
					        posy2=3;
					        sizeletra2=1;        
                        break;                            
                        case 0xbd:   
                            Producto=3;                        
                            flujo_LCD2=2;
                            set_imagen(2,132);  
                            id_teclado2=7;
					        count_teclas2=0;							    
					        id_coma2=',';	
                            teclas2=1;              													
					        posx2=4;
					        posy2=3;
					        sizeletra2=1;                               
                        break;                            
                        case 0xbe:  
                            Producto=4;                        
                            flujo_LCD2=2;
                            set_imagen(2,132);  
                            id_teclado2=7;
					        count_teclas2=0;							    	
					        id_coma2=',';	
                            teclas2=1;              														
					        posx2=4;
					        posy2=3;
					        sizeletra2=1;        
                        break;                     
	                    case 0x7E:	  
                        	rf_mod[0]='M';
			                rf_mod[1]='U';
			                rf_mod[2]='X';
			                rf_mod[3]='4'; 
                            y=4;
                            for(x=0;x<=2;x++){
                                switch(lado2.grado[x][0]){
                                	case 1:
                                		rf_mod[y]='D';
                                	break;
                                	case 2:
                                		rf_mod[y]='C';
                                	break;
                                	case 3:
                                		rf_mod[y]='E';
                                	break;
                                	case 4:
                                		rf_mod[y]='S';
                                	break;
                                    default:
                                        rf_mod[y]='0';
                                    break;
                                }                                                 
                                y++;                                
                            }  
                            if(diesel==0){
                                y=0;                                    
                                for(x=4;x<=6;x++){
                                    if(rf_mod[x]=='D'){
                                       rf_mod[x]='0';     
                                        lado2.grado[y][0]=0;                                        
                                    }
                                    y++;
                                }
                            }        
                            if(corriente==0){
                                y=0;                                    
                                for(x=4;x<=6;x++){
                                    if(rf_mod[x]=='C'){
                                       rf_mod[x]='0';     
                                        lado2.grado[y][0]=0;                                        
                                    }
                                    y++;
                                }
                            }     
                            if(extra==0){
                                y=0;                                    
                                for(x=4;x<=6;x++){
                                    if(rf_mod[x]=='E'){
                                       rf_mod[x]='0';     
                                        lado2.grado[y][0]=0;                                        
                                    }
                                    y++;
                                }
                            }
                            if(supremo_diesel==0){
                                y=0;                                    
                                for(x=4;x<=6;x++){
                                    if(rf_mod[x]=='S'){
                                       rf_mod[x]='0';     
                                        lado2.grado[y][0]=0;                                        
                                    }
                                    y++;
                                }
                            }                                       
                            rf_mod[7]=2 + 48;                               
                            rf_mod[8]='*';
							PC_ClearRxBuffer();				
						    for(x=0;x<=8;x++){
							   PC_PutChar(rf_mod[x]);
						    }                                
                            diesel=1;
                            extra=1;
                            corriente=1;
                            supremo_diesel=1;                                
                            EEPROM_WriteByte(lado2.grado[0][0],20);
                            EEPROM_WriteByte(lado2.grado[1][0],21);
                            EEPROM_WriteByte(lado2.grado[2][0],22);  
                            flujo_LCD2=16;
                            set_imagen(2,95);                             
	                    break;                         
                    }
                }
            }         
        break;
           
        case 19:
	        if(LCD_2_GetRxBufferSize()==8){
	            if((LCD_2_rxBuffer[0]==0xAA) && (LCD_2_rxBuffer[6]==0xC3) && (LCD_2_rxBuffer[7]==0x3C)){                    
                    switch(LCD_2_rxBuffer[3]){
	                    case 0x5e:                                  
                            Side2State = creditSide2;                        
                            switch(Side1State){
                                case witeSide1: 
							        flujo_LCD2=3;
							        set_imagen(2,18);                                     
                                break;
                                case cashSide1:
							        set_imagen(2,125);
                                    CyDelay(2000);
	                                flujo_LCD2=0;                                
						            set_imagen(2,0);                                    
                                break;
                                case creditSide1:
							        set_imagen(2,125);
                                    CyDelay(2000);
	                                flujo_LCD2=0;                                
						            set_imagen(2,0);                                       
                                break;
                                default:
                                break;
                            }    
	                    break;                
	                    case 0x5f:								   
                            Side2State = cashSide2;                              
                            for(x=0;x<=15;x++){
                                rventa2.id[x]=0;
                            }
                            for(x=0;x<=10;x++){
                              rventa2.km[x]=0;
                            } 
                            switch(Side1State){
                                case witeSide1:
							        set_imagen(2,124);
                                    CyDelay(2000);
	                                flujo_LCD2=0;                                
						            set_imagen(2,0);
					                lado2.estado=libre;                                     
                                break;
                                case cashSide1:
							        set_imagen(2,125);
                                    CyDelay(2000);
	                                flujo_LCD2=0;                                
						            set_imagen(2,0);
					                lado2.estado=libre;                                     
                                break;
                                case creditSide1:
	                                flujo_LCD2=4;	
	                                set_imagen(2,5);  
                                    sinIdentificacionL2=1;                                    
                                break;                                
                                default:
                                break;
                            }   
	                    break;  
	                    case 0x7E:									//ir a menu
					      lado2.estado=libre;                            
	                      flujo_LCD2=0;                                
						  set_imagen(2,0);	 
	                    break;	
                        case 0x94:									//ir a menu
					      lado2.estado=libre;                         
	                      flujo_LCD2=0;                                
						  set_imagen(2,0);	 
	                    break;	
	                }
	            }
	            CyDelay(100);
	            LCD_2_ClearRxBuffer();
	        }				            
        break;    
            
		case 102:

        break;
        case 101:  
          if(count_protector2>=2){              
            isr_4_Stop(); 
            Timer_Animacion2_Stop(); 			
            flujo_LCD2=prox_caso[0][0];
			set_imagen(2,prox_caso[0][1]);
			count_protector2=0;
          }            
        break;			
		
        case 100:  
         if(count_protector2>=4){
            flujo_LCD2=0;
            isr_4_Stop(); 
            Timer_Animacion2_Stop(); 
            count_protector2=0;
         }            
        break;			      
    }
    
}


/**
* CY_ISR 
* @brief función de interrupción
* muestra la animación de inicio
* de la variable flujo_LDC1 = 1 o flujo_LCD2=1
* @param animacion
* @param animacion2
* 
*/
CY_ISR(animacion){
    Timer_Animacion_ReadStatusRegister();
    if(flujo_LCD1==1){
        if(count_protector<=6){
            count_protector++;
            set_imagen(1,(iprotector5+count_protector));  
        }
        else{
           count_protector=0; 
           set_imagen(1,(iprotector5+count_protector));  
        }
    }
    else{
        count_protector++;
        count_esperaBBB++;
    }
}


CY_ISR(animacion2){
    Timer_Animacion2_ReadStatusRegister();
    if(flujo_LCD2==1){
        if(count_protector2<=6){
            count_protector2++;
            set_imagen(2,(iprotector5+count_protector2));  
        }
        else{
           count_protector2=0; 
           set_imagen(2,(iprotector5+count_protector2));  
        }
    }
    else{
        count_protector2++;
        count_esperaBBB2++;
    }
}

CY_ISR(modo_mux){
	Timer_Modo_ReadStatusRegister();
	state_rf++;
	if(state_rf==15){
		if((rventa1.autorizado!=100)&&(rventa2.autorizado!=100)){
			rventa1.autorizado=0;
			rventa2.autorizado=0;
		}	
		state_rf=0;
	}
}

/**
* main 
* @brief función principal
* los procesos del microcontrolador inician su ejecución en esta función
* se hace la inicialización de periféricos
* en el ciclo infinito se consultan pantallas y puerto serial del Beagle
*/
int main(){ 
    
    init();
    init_surt();    
    CyWdtStart(CYWDT_1024_TICKS,CYWDT_LPMODE_NOCHANGE);    
    for(;;){
        polling_surt(); 
        polling_LCD1(); 
    	CyWdtClear();             
        polling_rf();            
        polling_LCD2();            
    	CyWdtClear();          
    }

}





