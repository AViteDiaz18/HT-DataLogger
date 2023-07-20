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

int main(void)
{
	Flotante Pulsos;
    //int raw_press = 0;
	CLOCK32K_init();
	PORT_LOWPOWER_Init();
	PinInit();
	ADC0_init();
	
	
	CPU_CCP = CCP_IOREG_gc;
	WDT.CTRLA = WDT_PERIOD_8KCLK_gc;
	memset(command, 0, MAX_COMMAND_LEN);

	TCA0_init();
	USART0_init();
	
	sei();
	
	//write_EEPROM(5120,0x00);
	_delay_ms(100);
	printf("Ayuda\r");
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
		if(flujo != 0){
			if(minutos == 60){
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
				minutos = 0;
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

