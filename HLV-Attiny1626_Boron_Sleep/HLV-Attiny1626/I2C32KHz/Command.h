/*
 * Command.h
 *
 * Created: 05/07/2023 02:08:49 p. m.
 *  Author: H-Tech
 */ 


#ifndef COMMAND_H_
#define COMMAND_H_

#include <avr/io.h>
#include "USART.h"
#include "Pressure.h"
#include "PULSE.h"
#include "TCB.h"

int send = 0;
float vol1 = 0;
float vol2 = 0;
float presion1 = 0;
float presion2 = 0;
float bateria = 0;
int s1 = 0;
int s2 = 0;
char sensor1,sensor2;

void executeCommand(char *command){
	if(strstr(command,"SM")!= NULL){
		ADC0.CTRLA = (0 << ADC_ENABLE_bp);
		TCA0.SPLIT.CTRLA = 0;
		TCB0.CTRLA = 0;
		RTC.CTRLA = 0;
		wdt_reset();
		TCB0_stop();
		vol1 = 0;
		vol2 = 0;
		presion1 = 0;
		presion2 = 0;
		bateria = 0;
		//printf("SM\r");
		_delay_ms(200);
		Sleep_Micro();
		_delay_ms(200);
		seconds = 0;
		send = 0;
		TCB0_start();	
		
	}
	else{
		if(strstr(command,"EE")!=NULL){
			for(int i = 5120; i < 5352; i++){
				*(uint8_t *)(i) = 0x0;
				CPU_CCP = CCP_SPM_gc;
				NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
			}
			printf("%s",command);
		}
		else{
			if(command[0] == 'W' && command[1] == 'S'){
				write_EEPROM(5348,command[2]);
				write_EEPROM(5349,command[3]);
				printf("%s",command);
			}
			else{
				if(strstr(command,"VS")!= NULL){
					char *token = strtok(command, " ");
					int i = 0;
					if(token != NULL){
						while(token != NULL){
							switch(i){
								case 0:
								break;
								case 1:
								sscanf(token,"%d",&verbose);
								break;
							}
							token = strtok(NULL, " ");
							i++;
						}
					}
					printf("%s\r",command);
				}
				else{
					if(strstr(command,"CN2")!= NULL){
					Flotante Z1,A,B;
					char *token = strtok(command, " ");
					int i = 0;
					if(token != NULL){
						while(token != NULL){
							switch(i){
								case 0:
								break;
								case 1:
								sscanf(token,"%f",&A.f);
								break;
								case 2:
								sscanf(token,"%f",&B.f);
								break;
							}
							token = strtok(NULL, " ");
							i++;
						}
					}
					Z1.f = B.f/A.f;
					wdt_reset();
					for(int i=0; i<=3; i++){
						write_EEPROM(i+5352,Z1.dato[i]);
						write_EEPROM(i+5356,A.dato[i]);
						write_EEPROM(i+5360,B.dato[i]);
						}
					printf("CN\r");
					}
					else{
						if(strstr(command,"CN1")!= NULL){
							Flotante Z2,C,D;
							char *token = strtok(command, " ");
							int i = 0;
							if(token != NULL){
								while(token != NULL){
									switch(i){
										case 0:
											break;
										case 1:
											sscanf(token,"%f",&C.f);
											break;
									case 2:
										sscanf(token,"%f",&D.f);
										break;
									}
								token = strtok(NULL, " ");
								i++;
							}
						}
						Z2.f = D.f/C.f;
						wdt_reset();
						for(int i=0; i<=3; i++){
							write_EEPROM(i+5364,Z2.dato[i]);
							write_EEPROM(i+5368,C.dato[i]);
							write_EEPROM(i+5372,D.dato[i]);
						}
						printf("CN\r");
						}
						else{
							if(strcmp(command,"MS\r") == 0){
								sensor1 = read_EEPROM(5348);
								sensor2 = read_EEPROM(5349);
								PORTB.OUT |= PIN5_bm;
								if(sensor1 == 'P' || sensor2 == 'P'){
									wdt_reset();
									unsigned long int iterations = (5000 * F_CPU) / (10000UL);
									for(unsigned long int i = 0; i < iterations; i++){
										wdt_reset();
									}
								}
								switch(sensor1){
									case 'I':
										PORTA.OUT |= PIN3_bm;
										_delay_ms(200);
										wdt_reset();
										presion1 = get_PressureI2C(get_I2CValue(verbose),2);
										PORTA.OUT &= ~PIN3_bm;
										_delay_ms(200);
										break;
									case 'i':
										//printf("EntraS1\r");
										PORTA.OUT |= PIN7_bm;
										PORTA.OUT |= PIN3_bm;
										_delay_ms(1000);
										wdt_reset();
										presion1 = get_PressureI2C(get_I2CValue(verbose),1);
										PORTA.OUT &= ~PIN7_bm;
										PORTA.OUT &= ~PIN3_bm;
										_delay_ms(1000);
										break;
									case 'P':
										wdt_reset();
										vol1 = get_Voltage(9); //2 segundos
										if(verbose == 1){
											printf("Voltaje Sensor1: %f\r",vol1);
										}
										presion1 = get_PresureADC1(vol1);
										wdt_reset();
										break;
									case 'A':
										s1 = 1;
										break;
									default:
										break;
								}
								switch(sensor2){
									case 'I':
										PORTA.OUT |= PIN3_bm;
										_delay_ms(200);
										wdt_reset();
										presion2 = get_PressureI2C(get_I2CValue(verbose),2);
										PORTA.OUT &= ~PIN3_bm;
										_delay_ms(200);
										break;
									case 'i':
										//printf("EntraS2\r");
										PORTC.OUT |= PIN2_bm;
										PORTA.OUT |= PIN3_bm;
										_delay_ms(1000);
										wdt_reset();
										presion2 = get_PressureI2C(get_I2CValue(verbose),1);
										PORTC.OUT &= ~PIN2_bm;
										PORTA.OUT &= ~PIN3_bm;
										_delay_ms(1000);
										break;
									case 'P':
										wdt_reset();
										vol2 = get_Voltage(5); //2 segundos
										//PORTB.OUT &= ~PIN5_bm;
										if(verbose == 1){
											printf("Voltaje Sensor2: %f\r",vol2);
										}
										//printf("Voltaje2: %f\r",vol2);
										presion2 = get_PresureADC2(vol2);
										wdt_reset();
										break;
									case 'p':
										wdt_reset();
										vol2 = get_Voltage(5); //2 segundos
										//PORTB.OUT &= ~PIN5_bm;
										if(verbose == 1){
											printf("Voltaje Sensor2: %f\r",vol2);
										}
										presion2 = get_PresureADC2(vol2);
										wdt_reset();
										break;
									case 'A':
										s2 = 1;
										break;
									default:
										break;
								}
								PORTB.OUT &= ~PIN5_bm;
								bateria = get_Voltage(6)*5; //2 segundos
								//count = 0;
							}
							else{
								if(strcmp(command,"RT\r") == 0){
									//printf("Bandera1: %d, Bandera2: %d",s1,s2);
									if(s1 == 0 && s2 == 0){
										printf("RH%.2fRQ%.2fRVO%.2f\n",presion1,presion2,bateria);
										TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm;
	 									vol1 = 0;
	 									vol2 = 0;
	 									presion1 = 0;
	 									presion2 = 0;
	 									bateria = 0;
									}
									if(s1 == 0 && s2 != 0){
										printf("RP%.2fRVO%.2f\n",presion1,bateria);
	 									vol1 = 0;
	 									presion1 = 0;
	 									bateria = 0;
									}
									else{
										printf("Incorrect Command\n");
									}
								}
							}
						}
					}
				}
			}	
		}
	}
}



#endif /* COMMAND_H_ */