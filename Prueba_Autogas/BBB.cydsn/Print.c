/**
* @file Print.c
* @Author Insepet LTDA
* @date 28/2/2016
* @brief Librería para el manejo de la impresora
* no utilizado en esta versión de MUX porque la impresora es controlada
* vía RS-232 por el BeagleBone.<BR>
* La misma librería que se utilizó en el MUX GRP700U
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include <device.h>
#include "VariablesG.h"
#include "I2C.h"
#include "Protocolo.h"
#include "Logos.h"

/*
*********************************************************************************************************
*                                            MENSAJES
*********************************************************************************************************
*/
uint8 msn_eds[20]="ESTACION DE SERVICIO";
uint8 msn_nit[4]="NIT ";
uint8 msn_tel[4]="TEL:";
uint8 msn_fecha[7]="FECHA: ";
uint8 msn_hora[7]="HORA:  ";
uint8 msn_pos[11]="POSICION:  ";
uint8 msn_ppu[15]="PPU:       $/G ";
uint8 msn_vol[13]="VOLUMEN:   G ";
uint8 msn_din[13]="DINERO:    $ ";
uint8 msn_placa[11]="PLACA:     ";
uint8 msn_cuenta[11]="Cuenta:    ";
uint8 msn_km[11]="Km:        ";
uint8 msn_id[11]="Id:        ";
uint8 msn_copia[6]="COPIA ";
uint8 msn_corriente[9]="CORRIENTE";
uint8 msn_diesel[7]="DIESEL";
uint8 msn_extra[5]="EXTRA";
uint8 msn_producto[11]="PRODUCTO:  ";
uint8 msn_fcorte[16]="Fecha de Corte: ";
uint8 msn_hcorte[16]="Hora de Corte:  ";
uint8 msn_ufcorte[24]="Fecha del ultimo Corte: ";
uint8 msn_uhcorte[24]="Hora del ultimo Corte:  ";
uint8 msn_lema1[31]="     GRACIAS POR SU COMPRA     ";
uint8 msn_lema2[31]="         VUELVA PRONTO         ";

/*
*********************************************************************************************************
*                                         void imprimir(uint8 val)
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

void imprimir(uint8 pos, uint8 print, uint8 prod, uint8 *ppu, uint8 *volumen, uint8 *dinero, uint8 *placa, uint8 *km_p){
	uint8 x, digito;
	for(x=0;x<=((31-20)/2);x++){
		write_psoc1(print,0x20);			
	}	
	for(x=0;x<=19;x++){										//ESTACION DE SERVICIO
		write_psoc1(print,msn_eds[x]);
	}
    write_psoc1(print,10);
	for(x=0;x<=((31-nombre[0])/2);x++){						//NOMBRE
		write_psoc1(print,0x20);
	}
	for(x=1;x<=nombre[0];x++){
		write_psoc1(print,nombre[x]);			
	}
    write_psoc1(print,10);
	if(nit[0]>=1){
		for(x=0;x<=((31-(nit[0]+4))/2);x++){				//NIT
			write_psoc1(print,0x20);			
		}		
		for(x=0;x<=3;x++){									
			write_psoc1(print,msn_nit[x]);
		}
		for(x=1;x<=nit[0];x++){
			write_psoc1(print,nit[x]);			
		}
		write_psoc1(print,10);
	}
	if(telefono[0]>=1){
		for(x=0;x<=((31-(telefono[0]+4))/2);x++){	//TELEFONO
			write_psoc1(print,0x20);			
		}		
		for(x=0;x<=3;x++){									
			write_psoc1(print,msn_tel[x]);
		}
		for(x=1;x<=telefono[0];x++){
			write_psoc1(print,telefono[x]);			
		}			
	    write_psoc1(print,10);
	    write_psoc1(print,10);
	}
	for(x=0;x<=((31-direccion[0])/2);x++){			//DIRECCION
		write_psoc1(print,0x20);			
	}
	for(x=1;x<=direccion[0];x++){
		write_psoc1(print,direccion[x]);			
	}
    write_psoc1(print,10);
    write_psoc1(print,10);
    write_psoc1(print,10);
	for(x=0;x<=6;x++){										//FECHA								
		write_psoc1(print,msn_fecha[x]);
	}
	if(leer_fecha()==1){
		write_psoc1(print,(((fecha[0]&0x30)>>4)+48));
		write_psoc1(print,((fecha[0]&0x0F)+48));
		write_psoc1(print,'/');
		write_psoc1(print,(((fecha[1]&0x10)>>4)+48));
		write_psoc1(print,((fecha[1]&0x0F)+48));	
		write_psoc1(print,'/');
		write_psoc1(print,(((fecha[2]&0xF0)>>4)+48));
		write_psoc1(print,((fecha[2]&0x0F)+48));			
	}
    write_psoc1(print,10);
	for(x=0;x<=6;x++){									
		write_psoc1(print,msn_hora[x]);
	}
	if(leer_hora()==1){										//HORA
		write_psoc1(print,(((hora[1]&0x10)>>4)+48));
		write_psoc1(print,((hora[1]&0x0F)+48));
		write_psoc1(print,':');
		write_psoc1(print,(((hora[0]&0xF0)>>4)+48));
		write_psoc1(print,((hora[0]&0x0F)+48));
		if((hora[1]&0x20)==0x20){
			write_psoc1(print,'p');
		}
		else{
			write_psoc1(print,'a');						
		}			
		write_psoc1(print,'m');
	}
    write_psoc1(print,10);
    write_psoc1(print,10);
	for(x=0;x<=10;x++){										//POSICION								
		write_psoc1(print,msn_pos[x]);
	}
    write_psoc1(print,((pos/10)+48));
	write_psoc1(print,((pos%10)+48));
    write_psoc1(print,10);
	for(x=0;x<=10;x++){										//PRODUCTO								
		write_psoc1(print,msn_producto[x]);
	}
	if(prod==1){
		for(x=1;x<=nombre_pro1[0];x++){
			write_psoc1(print,nombre_pro1[x]);			
		}		
	}
	else if(prod==2){
		for(x=1;x<=nombre_pro2[0];x++){
			write_psoc1(print,nombre_pro2[x]);			
		}	
	}
	else if(prod==3){
		for(x=1;x<=nombre_pro3[0];x++){
			write_psoc1(print,nombre_pro3[x]);			
		}	
	}	
	write_psoc1(print,10);
	for(x=0;x<=14;x++){											//PPU								
		write_psoc1(print,msn_ppu[x]);
	}
	digito=0;
	for(x=4;x>=1;x--){
        if((ppu[x]==0)&&(digito==0)){
            
        }
        else{
			if(x==(version[2]-1)){
				write_psoc1(print,',');
			}			
            digito=1;
            write_psoc1(print,ppu[x]+48);
        }	
	}
	write_psoc1(print,ppu[0]+48);
	if(ppux10==1){
		write_psoc1(print,48);	
	}
	write_psoc1(print,10);
	for(x=0;x<=12;x++){											//VOLUMEN							
		write_psoc1(print,msn_vol[x]);
	}
	digito=0;
	for(x=6;x>=1;x--){
        if((volumen[x]==0)&&(digito==0)){
			if(x==3){
				write_psoc1(print,'0');
				digito=1;
			}      	    
        }
        else{
			if(x==(version[3]-1)){
				write_psoc1(print,',');
			}
            digito=1;
            write_psoc1(print,volumen[x]+48);
        }	
	}
	write_psoc1(print,volumen[0]+48);
    write_psoc1(print,10);
	for(x=0;x<=12;x++){											//DINERO							
		write_psoc1(print,msn_din[x]);
	} 
    digito=0;	
	for(x=6;x>=1;x--){
        if((dinero[x]==0)&&(digito==0)){
            
        }
        else{
			if(x==(version[2]-1)){
				write_psoc1(print,',');
			}
            digito=1;
            write_psoc1(print,dinero[x]+48);
        }	
	}
	if(version[1]==6){
		write_psoc1(print,dinero[0]+48);
	}
	if(placa[1]!=0){										//PLACA
		write_psoc1(print,10);
		write_psoc1(print,10);
		for(x=0;x<=10;x++){																		
			write_psoc1(print,msn_placa[x]);
		} 
		for(x=1;x<=placa[0];x++){
			write_psoc1(print,placa[x]);	
		}	
	}
    write_psoc1(print,10);
    write_psoc1(print,10);
	if(km[1]==1){											//KM
		for(x=0;x<=10;x++){																		
			write_psoc1(print,msn_km[x]);
		}		
		digito=0;
		for(x=9;x>=1;x--){
	        if((km_p[x]==0)&&(digito==0)){
	            
	        }
	        else{
	            digito=1;
	            write_psoc1(print,km_p[x]+48);
	        }	
		}
	}
    write_psoc1(print,10);
    write_psoc1(print,10);
	write_psoc1(print,10);
	for(x=0;x<=30;x++){										//LEMA1																	
		write_psoc1(print,msn_lema1[x]);
	} 
	write_psoc1(print,10); 	
	for(x=0;x<=30;x++){										//LEMA2																	
		write_psoc1(print,msn_lema2[x]);
	} 	
    write_psoc1(print,10); 
    write_psoc1(print,10); 
    write_psoc1(print,10);
    write_psoc1(print,10);
	write_psoc1(print,10);	
	
}

/*
*********************************************************************************************************
*                          void print_logo(uint8 logo, uint16 ini, uint16 fin)
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
void print_logo(uint8 logo, uint16 ini, uint16 fin, uint8 puerto){
	uint16 i;
	switch(logo){
		case 'B':
			for(i=ini;i<=fin;i++){
        		write_psoc1(puerto,biomax[i]);			
			}
		break;

		case 'b':
			for(i=ini;i<=fin;i++){
        		write_psoc1(puerto,brio[i]);			
			}
		break;			

		case 'C':
			for(i=ini;i<=fin;i++){
        		write_psoc1(puerto,cencosud[i]);			
			}
		break;	
			
		case 'e':
			for(i=ini;i<=fin;i++){
        		write_psoc1(puerto,ecospetrol[i]);			
			}
		break;

		case 'X':
			for(i=ini;i<=fin;i++){
        		write_psoc1(puerto,esso[i]);			
			}
		break;
			
		case 'E':
			for(i=ini;i<=fin;i++){
        		write_psoc1(puerto,exito[i]);			
			}
		break;
			
		case 'G':
			for(i=ini;i<=fin;i++){
        		write_psoc1(puerto,gulf[i]);			
			}
		break;	
			
		case 'm':
			for(i=ini;i<=fin;i++){
        		write_psoc1(puerto,mineroil[i]);			
			}
		break;	
			
		case 'M':
			for(i=ini;i<=fin;i++){
        		write_psoc1(puerto,mobil[i]);			
			}
		break;	
			
		case 'P':
			for(i=ini;i<=fin;i++){
        		write_psoc1(puerto,petrobras[i]);			
			}
		break;
			
		case 'p':
			for(i=ini;i<=fin;i++){
        		write_psoc1(puerto,plus[i]);			
			}
		break;	
			
		case 'T':
			for(i=ini;i<=fin;i++){
        		write_psoc1(puerto,terpel[i]);			
			}
		break;
			
		case 't':
			for(i=ini;i<=fin;i++){
        		write_psoc1(puerto,petromil[i]);			
			}
		break;			
			
		case 'H':
			for(i=ini;i<=fin;i++){
        		write_psoc1(puerto,texaco[i]);			
			}
		break;	
			
		case 'Z':
			for(i=ini;i<=fin;i++){
        		write_psoc1(puerto,zeus[i]);			
			}
		break;
			
							
	}	
}

/* [] END OF FILE */
