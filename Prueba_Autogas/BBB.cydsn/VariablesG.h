/**
* @file VariablesG.h
* @Author Insepet LTDA
* @date 28/2/2016
* @brief Contiene las variables globales que se usan en el programa y las principales estructuras
*/

#ifndef VARIABLESG_H
#define VARIABLESG_H
#include <device.h>	
/**
* Imagenes para las animaciones de espera
*    
*/	
#define iprotector1     0
#define iprotector2     1
#define iprotector3     2
#define iprotector4     3
#define iprotector5     20
#define iprotector6     21
#define iprotector7     22
#define iprotector8     23
#define iprotector9     24
#define iprotector10    25
#define iprotector11    26
#define iprotector12    27
/**
* Estados del Modulo MUX, indica las tareas que está haciendo
* para el flujo de control con el Beagle
*/
///No tiene ninguna peticion ni espera ninguna orden    
#define libre 		1
///Espera comando del BBB
#define espera 		2	
///Ocupado esperando respuesta del BBB    
#define ocupado		3
///Listo para enviar autorizacion
#define listo		4
///Autorizado 
#define autoriza_ok	5				
///surtiendo combustible
#define tanqueo     6	
///Listo para enviar una venta    
#define venta_ok	7	
///Imprimiendo    
#define imprimiendo 8					
	
/**
* Comandos Modulo Beagle a MUX para la solicitud de respuestas
*/
#define	cautorizar	        0x30
#define cimprimir	        0x31
#define cbluetooth	        0x32	
#define creset		        0x45
#define totales_elec        0x46
//#define ok_totales          0x47     
#define	witeSide1       10
#define cashSide1	    11
#define creditSide1 	12	
#define	witeSide2       20
#define cashSide2	    21
#define creditSide2	    22    

/****************************************************************************************************************
									Variables para manejo de LCD
****************************************************************************************************************/	
volatile uint8 	count_teclas1, comas1, comas2, id_coma1, id_teclado1, count_teclas2, id_coma2, id_teclado2;															
volatile uint16 posx1,posy1,sizeletra1,posx2,posy2,sizeletra2;	
volatile uint8 	teclas1,teclas2;
volatile uint8 	flujo_LCD1,flujo_LCD2;
volatile uint8 	Buffer_LCD1[30];
volatile uint8 	Buffer_LCD2[30];
volatile uint8 	count_protector,count_protector2;
volatile uint8  count_esperaBBB,count_esperaBBB2;
volatile uint8 	password_lcd[9];
volatile uint16 imagen_grado;										//Imagen que a mostrar para elegir grado
volatile uint16 prox_caso[2][2];									//0 flujo 1 imagen	
volatile uint8 	bloqueo_LCD[3];
volatile uint8	id_estacion[5];
volatile uint8	id_venta[8];
volatile uint8	trama_totales[200];
/****************************************************************************************************************
						Variables para manejo de datos del surtidor
****************************************************************************************************************/	
volatile uint8 	ppux10;
volatile uint8 	nombre_pro1[17],nombre_pro2[17],nombre_pro3[17];	//Nombre de cada producto
volatile uint8 	version[4];											//1 version - 2 Decimal Dinero - 3 Decimal Volumen

/**
 * @struct surtidor
 * @brief Separación de caras del equipo
 */
struct surtidor{
    volatile uint8 dir;
    volatile uint8 estado;
    volatile uint8 totales[3][24];   	
	volatile uint8 ppu[3][5];
	volatile uint8 grado[3][1];			
	volatile uint8 mangueras;
};
struct surtidor lado1;
struct surtidor lado2;
/****************************************************************************************************************
								Variables para manejo de recibo
****************************************************************************************************************/
volatile uint8 nombre[33];
volatile uint8 nit[15];
volatile uint8 telefono[20];
volatile uint8 direccion[33];
volatile uint8 lema1[32];
volatile uint8 lema2[32];
volatile uint8 id_logo[2];
volatile uint8 km[2];
uint8 fecha[3];
uint8 hora[2];
/**
 * @struct recibo
 * @brief Variables del recibo, la estructura permite separar todos los datos de cada venta para su impresión
 */
struct recibo{					//Datos venta
    uint8 posicion;
    uint8 grado;
    uint8 ppu[5];
    uint8 dinero[9];	
    uint8 volumen[9];
	uint8 producto;
	uint8 manguera;
    uint8 preset[8]; 			
	uint8 id[16];
	uint8 km[11];
	uint8 placa[9];
	uint8 imprimir;			
	uint8 tipo_venta;		// 0 Contado 1 credito 2 autoservicio
	uint8 cedula[11];
	uint8 password[11];
	uint8 turno;
	uint8 autorizado;		//Autorizado 0 venta mux local - 0x30 turno cerrado - 0x31 venta con nsx y turno abierto - 0x32 venta CDG   	
    uint8 password_local[5];
};
///datos de la venta actual lado 1
struct recibo rventa1; 
///datos de la venta actual lado 2
struct recibo rventa2;   	

///Bandera para indicar que hay comunicacion RF
uint8 	ok_RF;      		 
///Hay datos para enviar al Beagle
uint8 	ok_datosRF;			 	
///Datos que van al Beagle
uint8  	rf_mod[200];		 

/**
								Buffer I2C de lectura
*/
uint8 	buffer_i2c[64];												

/**
								Variables para manejo de Ibutton
*/
uint8	crc_total;													//Checksum Ibutton
/**
								Variables para manejo de Impresora
*/
uint8	print1[2];													//Puerto de la impresora
uint8	print2[2];



#endif

//[] END OF FILE
