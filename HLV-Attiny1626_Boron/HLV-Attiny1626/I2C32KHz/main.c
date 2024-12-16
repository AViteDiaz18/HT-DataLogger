/*
 * I2C32KHz.c
 *
 * Created: 24/05/2023 11:16:12 a. m.
 * Author : H-Tech
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

/* Cambios en EEPROM
   5372-5375 D X 
   5368-5371 C X
   5364-5367 B X
   5360-5363 A X
   5359 Modo X Realizar esta parte del codigo
   5358 Sensor 2 X
   5357 Sensor 1 X
   5356 Flujo X
   5355 Cambio de sensores X
   5354	Tipo de cadena X
   5353	Tipo Radio X
   5352 Tipo Nivel X
   5351 Tipo Flujo X
   5350 Activar Acumulado X
   5349 
   5348 Vuelta Acumulado X
*/
void erase_EEPROM(){
	for(int i = 5120; i < 5376; i++){
		*(uint8_t *)(i) = 0x00;
		CPU_CCP = CCP_SPM_gc;
		NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
	}
}

void Sleep_Micro(){
	CPU_CCP = CCP_IOREG_gc;
	WDT.CTRLA = WDT_WINDOW_OFF_gc;
	
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	cli();
	sleep_enable();
	 
	
	sei();
	sleep_cpu();

	sleep_disable();
	
	CPU_CCP = CCP_IOREG_gc;
	WDT.CTRLA = WDT_WINDOW_8KCLK_gc;
}

void config_Sensors(char *S1, char *S2, char *S3){
	if(S3 == 0){
		write_EEPROM(5359,1);
	}
	else{
		write_EEPROM(5359,2);
	}
	*(uint8_t *)(5357) = S1;
	CPU_CCP = CCP_SPM_gc;
	NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
	*(uint8_t *)(5358) = S2;
	CPU_CCP = CCP_SPM_gc;
	NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
	*(uint8_t *)(5356) = S3;
	CPU_CCP = CCP_SPM_gc;
	NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
}

int main(void)
{
    //int raw_press = 0;
	Flotante Pulsos;
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
	
	//Wake up pin
	PORTC.DIR &= ~(1<<0);
	PORTC.PIN0CTRL &= ~PORT_PULLUPEN_bm;
	PORTC.PIN0CTRL |= PORT_ISC_BOTHEDGES_gc;
		
	CPU_CCP = CCP_IOREG_gc;
	WDT.CTRLA = WDT_PERIOD_8KCLK_gc;
	memset(command, 0, MAX_COMMAND_LEN);

	//TCA0_init();
	USART0_init();
	sei();
	if(read_EEPROM(5359) == 0){
		erase_EEPROM();
		//1P I2C rango chico
		config_Sensors('i','A',0);
	}
	else{
		if(read_EEPROM(5359) == 1){
			//Modo dormido
			TCB0_init();
			TCB0_stop();
			//printf("Me Duermo\r");
			//printf("SM\r");
			ADC0.CTRLA = (0 << ADC_ENABLE_bp);
			TCA0.SPLIT.CTRLA = 0;
			TCB0.CTRLA = 0;
			RTC.CTRLA = 0;
			_delay_ms(200);
			Sleep_Micro();
			_delay_ms(200);
			send = 0;
			segundos = 0;
			TCB0_init();
		}
		else{
			if(read_EEPROM(5359) == 2){
				_delay_ms(100);
				printf("Ayuda\n");
			}
		}
	}
    while (1) {
		if(read_EEPROM(5359) == 1){
			if(send == 0){
				TCB0_start();
				executeCommand("MS\r");
				//bateria = get_Voltage(6)*5;
				executeCommand("RT\r");
				memset(command, 0, MAX_COMMAND_LEN);
				send = 1;
			}
			if(seconds >= 30){
				ADC0.CTRLA = (0 << ADC_ENABLE_bp);
				TCA0.SPLIT.CTRLA = 0;
				TCB0.CTRLA = 0;
				RTC.CTRLA = 0;
				wdt_reset();
				if(verbose == 1){
					printf("Me Duermo\r");
				}
				TCB0_stop();
				//printf("SM\r");
				_delay_ms(200);
				Sleep_Micro();
				_delay_ms(200);
				send = 0;
				seconds = 0;
				TCB0_start();
			}
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
			ADC0.CTRLA = (0 << ADC_ENABLE_bp);
			TCA0.SPLIT.CTRLA = 0;
			//TCB0.CTRLA = 0;
			RTC.CTRLA = 0;
			wdt_reset();
		}
		if(read_EEPROM(5359) == 2){
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
			if(read_EEPROM(5356) != 0){
				if(hr >= hora_acumulado){
					wdt_reset();
					int move = read_EEPROM(5351)*4;//*(uint8_t*)(5363)*4;
					int entrada = read_EEPROM(5350);
					int multiplicador = 0;
					switch(entrada){
						case 1:
							multiplicador = 10;
							break;
						case 2:
							multiplicador = 100;
							break;
						case 3:
							multiplicador = 1000;
							break;
						default:
							multiplicador = 1;
						break;
					}
					for(int i=0; i<=3; i++){
						Pulsos.dato[i] = read_EEPROM(i+move+5120);//*(uint8_t *)(i+move+5120);
					}
					//printf("Guardado en %d:",move+5120);
					Pulsos.f += (float)count*multiplicador/1000;
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
		if(read_EEPROM(5359) == 0){
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
		}
	}	
}

