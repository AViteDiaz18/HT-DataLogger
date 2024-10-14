/**
 * @file main.c
 * @author Htech Mexico
 * @brief Archivo principal del proyecto
 * @version 1.0
 * @date 24-05-2023
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <stdio.h>
#include <string.h>
#include "Utils.h"
#include "Command.h"

#define hora_acumulado 7200 //600

/**
 * @brief Funcion principal para el control del microcontrolador
 *
 */

/*Cambios en EEPROM
 Sensor 1 en 5348
 Sensor 2 en 5349
 Reservado en 5350
 Tipo de nivel 5351
 Cambio de canales 5352
 Vuelta Acumulado 5353
 Activador Acumulado 5354
 Tipo de Flujo 5355
 Dato de Flujo 5356-5359
 Calibracion Sensor 1 5360-5363 5364-5367
 Calibracion Sensor 2 5368-5371 5372-5375
*/

int main(void)
{
	Flotante Pulsos;
	Flotante Flujos;
	CLOCK32K_init();
	PORT_LOWPOWER_Init();
	PinInit();
	ADC0_init();
	
	//Activador 1er I2C
	PORTA.DIR |= PIN7_bm;
	PORTA.PIN7CTRL &= ~PORT_PULLUPEN_bm;
	PORTA.OUT &= ~PIN7_bm;
	
	//Activador 2do I2C
	PORTC.DIR |= PIN2_bm;
	PORTC.PIN2CTRL &= ~PORT_PULLUPEN_bm;
	PORTC.OUT &= ~PIN2_bm;
	
		
	CPU_CCP = CCP_IOREG_gc;
	WDT.CTRLA = WDT_PERIOD_8KCLK_gc;
	memset(command, 0, MAX_COMMAND_LEN);

	TCA0_init();
	USART0_init();
	
	sei();
	
	//write_EEPROM(5120,0x00);
	_delay_ms(100);
	printf("Ayuda\n");
    /* Replace with your application code */
    while (1) 
    {
		if(flag == 1){
			//printf("Comando 1: %s", command1);
			executeCommand(command1);
			if(strcmp(command2,"\0") != 0 && strcmp(command3,"\0") != 0){
				memset(command1, 0, MAX_COMMAND_LEN);
				strncpy(command1,command2,MAX_COMMAND_LEN);
				memset(command2, 0, MAX_COMMAND_LEN);
				strncpy(command2,command3,MAX_COMMAND_LEN);
				memset(command3, 0, MAX_COMMAND_LEN);
				//printf("Comando1: %s, Comando2 %s, Comando3 %s\r",command1,command2,command3);
			}
			else{
				if(strcmp(command2,"\0") != 0 && strcmp(command3,"\0") == 0){
					//executeCommand(command1);
					memset(command1, 0, MAX_COMMAND_LEN);
					strncpy(command1,command2,MAX_COMMAND_LEN);
					memset(command2, 0, MAX_COMMAND_LEN);
					//printf("Comando1: %s, Comando2 %s, Comando3 %s\r",command1,command2,command3); 
				}
				else{
					//executeCommand(command1);
					//eraseCommand(command1);
					memset(command1, 0, MAX_COMMAND_LEN);
					//printf("ComandoE 1: %s", command1);
					flag = 0;
				}
			}
		}
		if(read_EEPROM(5354) == 1){
			if(hr >= hora_acumulado){
				wdt_reset();
				int move = read_EEPROM(5353)*4;//*(uint8_t*)(5363)*4;
				Flujos.dato[0] = read_EEPROM(5356);
				Flujos.dato[1] = read_EEPROM(5357);
				Flujos.dato[2] = read_EEPROM(5358);
				Flujos.dato[3] = read_EEPROM(5359);
				for(int i=0; i<=3; i++){
					Pulsos.dato[i] = read_EEPROM(i+move+5120);//*(uint8_t *)(i+move+5120);
				}
				//printf("Guardado en %d:",move+5120);
				Pulsos.f += (float)count*Flujos.f/1000;
				for(int i=0; i<=3; i++){
					write_EEPROM(i+move+5120,Pulsos.dato[i]);
				}
				hr = 0;
				count = 0;
				wdt_reset();
			}
		}
		ADC0.CTRLA = (0 << ADC_ENABLE_bp);
		TCB0.CTRLA = 0;
		RTC.CTRLA = 0;
		wdt_reset();
    }
}

