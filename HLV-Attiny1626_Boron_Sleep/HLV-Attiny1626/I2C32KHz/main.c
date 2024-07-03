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

int main(void)
{
    //int raw_press = 0;
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
	
	_delay_ms(100);
	//printf("Ayuda\r");
	
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
 	TCB0_start();
	
	//printf("Me Despierto\r");
	
	sei();
	
	//write_EEPROM(5120,0x00);
    /* Replace with your application code */
    while (1) 
    {
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
}

