/**
 * @file HLV-Attiny1626_Configuracion_Inicial.c
 * @author Htech Mexico
 * @brief Configuracion de registros de la EEPROM del ATTINY1626
 * @version 1.0
 * @date 29-06-2023
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <avr/io.h>
#include <stdio.h>

/**
 * @brief Funcion que habilita el reloj de 32KHz en Attiny1626
 *
 */
static void CLOCK32K_init(){
	CPU_CCP = CCP_IOREG_gc;
	CLKCTRL_MCLKCTRLB = 0 << CLKCTRL_PEN_bp;
	CPU_CCP = CCP_IOREG_gc;
	CLKCTRL_MCLKCTRLA = CLKCTRL_CLKSEL_OSCULP32K_gc;
}

/**
 * @brief Declaracion de un tipo de dato union para la separacion de flotantes en Bytes
 *
 */
typedef union Float_Byte{
	float f;
	unsigned char dato[4];
}Flotante;	

/**
* @brief Borra todos los registros de la data EEPROM
*
*/
void erase_EEPROM(){
	for(int i = 5120; i < 5376; i++){
		*(uint8_t *)(i) = 0x00;
		CPU_CCP = CCP_SPM_gc;
		NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
	}
}

/**
* @brief Configura los posibles sensores de la HLV-Attiny1626
*
* @param S1 Caracter de configuracion del sensor 1
* 
*Para el sensor 1 existen las posibilidades:
*	
*		P: Sensor 4.20mA
*
*		I: Sensor I2C 20bar
*
*		i: Sensor I2C 7bar
*
*		N: Sensor 4.20mA de sumergimiento
*
* @param S2 Caracter de configuracion del sensor 2
* 
*Para el sensor 2 existen las posibilidades:
*	
*		P: Sensor 4.20mA
*
*		I: Sensor I2C 20bar
*
*		i: Sensor I2C 7bar
*
*		N: Sensor 4.20mA de sumergimiento
*
* @param S3 Caracter de configuracion del sensor 3
* 
*Para el sensor 3 existen las posibilidades de flujo:
*	
*		0 = 1LPS
*
*		1 = 10LPS
*
*		2 = 100LPS
*
*		3 = 1000LPS
*
*		4 = 10000LPS
*	
*/
void config_Sensors(char *S1, char *S2, float flujo, char *t_caudal){
	Flotante flow;
	flow.f = flujo;
	*(uint8_t *)(5348) = S1;
	CPU_CCP = CCP_SPM_gc;
	NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
	*(uint8_t *)(5349) = S2;
	CPU_CCP = CCP_SPM_gc;
	NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
	*(uint8_t *)(5356) = flow.dato[0];
	CPU_CCP = CCP_SPM_gc;
	NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
	*(uint8_t *)(5357) = flow.dato[1];
	CPU_CCP = CCP_SPM_gc;
	NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
	*(uint8_t *)(5358) = flow.dato[2];
	CPU_CCP = CCP_SPM_gc;
	NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
	*(uint8_t *)(5359) = flow.dato[3];
	CPU_CCP = CCP_SPM_gc;
	NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
	if(strstr(t_caudal,"Volumen")!= NULL){
		*(uint8_t *)(5355) = 0;
		CPU_CCP = CCP_SPM_gc;
		NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
	}
	else{
		if (strstr(t_caudal,"Caudal")!= NULL){
			*(uint8_t *)(5355) = 1;
			CPU_CCP = CCP_SPM_gc;
			NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
		}
	}
}

/**
* @brief Configura las posibles rectas de calibracion para los sensores 4.20ma
*
* @param a Dato del tipo flotante que corresponde al parametro a en la ecuacion ax+b
*
* @param b Dato del tipo flotante que corresponde al parametro en la ecuacion ax+b
*
* @param port Dato del tipo entero que indica la posicion del sensor a calibrar
*/

void calibrate_sensor(float a, float b, int port){
	Flotante A,B;
	A.f = a;
	B.f = b; 
	if(port == 2){
		for(int i=0; i<=3; i++){
			CPU_CCP = CCP_SPM_gc;
			NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
			*(uint8_t *)(i+5368) = A.dato[i];
			CPU_CCP = CCP_SPM_gc;
			NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
			*(uint8_t *)(i+5372) = B.dato[i];
			CPU_CCP = CCP_SPM_gc;
			NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
		}
	}
	if(port == 1){
		for(int i=0; i<=3; i++){
			CPU_CCP = CCP_SPM_gc;
			NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
			*(uint8_t *)(i+5360) = A.dato[i];
			CPU_CCP = CCP_SPM_gc;
			NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
			*(uint8_t *)(i+5364) = B.dato[i];
			CPU_CCP = CCP_SPM_gc;
			NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
			
		}
	}
}

/**
 * @brief Funcion principal para el manejo de registros
 *
 */
int main()
{
	CLOCK32K_init();
	erase_EEPROM();
	config_Sensors('i','i',1000,"Caudal");
/*<<<<<<< Updated upstream
	config_Sensors('A','A',100,"Caudal");
=======
	config_Sensors('i','A',0);
	set_reinicios();
>>>>>>> Stashed changes*/
	//calibrate_sensor(9.172461,4.777018,1);
	//calibrate_sensor(9.073957,4.540608,2);
	
    while (1) 
    {
    }
}

