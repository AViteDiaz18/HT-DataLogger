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
#include "TCA.h"

float vol1 = 0;
float vol2 = 0;
float presion1 = 0;
float presion2 = 0;
float bateria = 0;
int flujo = 0;
int s1 = 0;
int s2 = 0;
char sensor1,sensor2;

void executeCommand(char *command){
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
						flujo = read_EEPROM(5350);
						PORTB.OUT |= PIN5_bm;
						wdt_reset();
						unsigned long int iterations = (5000 * F_CPU) / (10000UL);
						for(unsigned long int i = 0; i < iterations; i++){
							wdt_reset();
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
								PORTA.OUT |= PIN3_bm;
								_delay_ms(200);
								wdt_reset();
								presion1 = get_PressureI2C(get_I2CValue(verbose),1);
								PORTA.OUT &= ~PIN3_bm;
								_delay_ms(200);
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
							case 'p':
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
								PORTA.OUT |= PIN3_bm;
								_delay_ms(200);
								wdt_reset();
								presion2 = get_PressureI2C(get_I2CValue(verbose),1);
								PORTA.OUT &= ~PIN3_bm;
								_delay_ms(200);
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
							if(s1 == 0 && s2 == 1 && flujo != 0){
								printf("RH%.2fRL%.2fRVO%.2f\r",presion1,caudal,bateria);
								vol1 = 0;
								presion1 = 0;
								bateria = 0;
							}
							if(s1 == 0 && s2 == 0 && flujo != 0){
								printf("RH%.2fRQ%.2fRL%.2fRVO%.2f\r",presion1,presion2,caudal,bateria);
								vol1 = 0;
								vol2 = 0;
								presion1 = 0;
								presion2 = 0;
								bateria = 0;
							}
							if(s1 == 0 && s2 == 0 && flujo == 0){
								printf("RH%.2fRQ%.2fRVO%.2f\r",presion1,presion2,bateria);
								TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm;
								vol1 = 0;
								vol2 = 0;
								presion1 = 0;
								presion2 = 0;
								bateria = 0;
							}
						}
						else{
							if(strcmp(command,"AC\r") == 0){
								if(flujo == 0){
									printf("Incorrect Command\r");
								}
								else{
									Flotante Volumen;
									int desplazo = read_EEPROM(5351)*4;//*(uint8_t*)(5351)*4;
									for(int k=0; k <= 3; k++){
										Volumen.dato[k] = read_EEPROM(k+desplazo+5120);//*(uint8_t *)(k+desplazo+5120);
									}
									printf("AC%.2f\r", Volumen.f);
									for(int k=0; k <= 3; k++){
										write_EEPROM(k+desplazo+5120,0);
									}
									int sumador = read_EEPROM(5351);//*(uint8_t*)(5351);
									sumador++;
									write_EEPROM(5351,sumador);
									int	compara = read_EEPROM(5351);
									if(compara > 56){
										write_EEPROM(5351,0);
									}
								}
							}
							else{
								if(strstr(command,"LT")!= NULL){
									int dato = 0;
									char *token = strtok(command, " ");
									int i = 0;
									if(token != NULL){
										while(token != NULL){
											switch(i){
												case 0:
												break;
												case 1:
													sscanf(token,"%d",&dato);
												break;
											}
											token = strtok(NULL, " ");
											i++;
										}
									}
									if(dato == 10){
										write_EEPROM(5350,1);
										printf("%s %d\r",command, dato);
									}
									else{
										if(dato == 100){
											write_EEPROM(5350,2);
											printf("%s %d\r",command, dato);
										}
										else{
											if(dato == 1000){
												write_EEPROM(5350,3);
												printf("%s %d\r",command, dato);
											}
											else{
												write_EEPROM(5350,0);
												printf("%s %d\r",command, dato);
											}
										}
									}
								}
								else{
									printf("Incorrect Command\r");
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