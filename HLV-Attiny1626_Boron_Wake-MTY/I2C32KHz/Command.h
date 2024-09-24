/**
 * @file Command.h
 * @author Htech Mexico
 * @brief Libreria para el manejo de comandos de USART
 * @version 1.0
 * @date 05-07-2023
 * DHT11
 * @copyright Copyright (c) 2023
 *
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include <avr/io.h>
#include "USART.h"
#include "Pressure.h"
#include "PULSE.h"

/**
 * @brief Variable para la obtencion de voltaje del puerto uno
 *
 */
float vol1 = 0;
/**
 * @brief Variable para la obtencion de voltaje del puerto dos
 *
 */
float vol2 = 0;
/**
 * @brief Variable para la obtencion de presion del puerto uno
 *
 */
float presion1 = 0;
/**
 * @brief Variable para la obtencion de presion del puerto dos
 *
 */
float presion2 = 0;
/**
 * @brief Variable para la obtencion de voltaje de la bateria del sistema
 *
 */
float bateria = 0;
/**
 * @brief Variable para la obtencion de flujo en el puerto de flujo
 *
 */
int flujo = 0;
/**
 * @brief Variable auxiliar para el sensor 1
 *
 */
int s1 = 0;
/**
 * @brief Variable auxiliar para el sensor 2
 *
 */
int s2 = 0;
/**
 * @brief Variable que almacena el tipo de sensor 1
 *
 */
char sensor1;
/**
 * @brief Variable que almacena el tipo de sensor 2
 *
 */
char sensor2;

/**
 * @brief Funcion que procesa el comando y ejecuta la funcion correspondiente para cada comando
 *
 * @param command Cadena de caracteres procesada por USART y obtenida como un comando
 *
 */
void executeCommand(char *command){
	if(strstr(command,"RST")!= NULL){
		_delay_ms(1000);
		CPU_CCP = CCP_IOREG_gc;
		RSTCTRL_SWRR = RSTCTRL_SWRE_bm;
		
		RSTCTRL_SWRR = RSTCTRL_SWRF_bm;
	}
	if (strstr(command,"GWS")!= NULL){
		sensor1 = read_EEPROM(5348);
		sensor2 = read_EEPROM(5349);
		printf("S1:%cS2:%c\n",sensor1,sensor2);
	}
	else{
		if(strstr(command,"GLT")!= NULL){
			int litros;
			flujo = read_EEPROM(5350);
			switch(flujo){
				case 0:
					litros = 0;
				break;
				case 1:
					litros = 10;
				break;
				case 2:
					litros = 100;
				break;
				case 3:
					litros = 1000;
				break;
				case 4:
					litros = 5000;
				break;
				default:
					litros = 1;	
				break;
			}
			printf("LT:%d\n",litros);
		}
		else{
			if(strstr(command,"GCN1")!= NULL){
				Flotante A1,B1;
				for (int i = 0; i <= 3; i++){
					A1.dato[i] = read_EEPROM(5368+i);
					B1.dato[i] = read_EEPROM(5372+i);
				}
				printf("CN1:(%.6fx-%.6f)*10.2\n",A1.f,B1.f);
			}
			else{
				if (strstr(command,"GCN2")!= NULL){
					Flotante A2,B2;
					for (int i = 0; i <= 3; i++){
						A2.dato[i] = read_EEPROM(5356+i);
						B2.dato[i] = read_EEPROM(5360+i);
					}
					printf("CN2:(%.6fx-%.6f)*10.2\n",A2.f,B2.f);	
				}
				else{
					if(command[0] == 'W' && command[1] == 'S'){
						write_EEPROM(5348,command[2]);
						write_EEPROM(5349,command[3]);
						printf("WS\n");
					}
					else{
						if(strstr(command,"VS")!= NULL){
							char *token = strtok(command, ",");
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
									token = strtok(NULL, ",");
									i++;
								}
							}
							printf("%s\n",command);
						}
						else{
							if(strstr(command,"CN2")!= NULL){
							Flotante Z1,A,B;
							char *token = strtok(command, ",");
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
									token = strtok(NULL, ",");
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
							printf("CN\n");
							}
							else{
								if(strstr(command,"CN1")!= NULL){
									Flotante Z2,C,D;
									char *token = strtok(command, ",");
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
										token = strtok(NULL, ",");
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
								printf("CN\n");
								}
								else{
									if(strcmp(command,"MS\r") == 0){
										sensor1 = read_EEPROM(5348);
										sensor2 = read_EEPROM(5349);
										flujo = read_EEPROM(5350);
										if(sensor1 == 'P' || sensor2 == 'P'){
											PORTB.OUT |= PIN5_bm;
											wdt_reset();
											unsigned long int iterations = (5000 * F_CPU) / (10000UL);
											for(unsigned long int i = 0; i < iterations; i++){
												wdt_reset();
											}
										}
										switch(sensor1){
											case 'I':
												PORTA.OUT |= PIN7_bm;
												PORTA.OUT |= PIN3_bm;
												_delay_ms(1000);
												wdt_reset();
												presion1 = get_PressureI2C(get_I2CValue(verbose,0),2);
												PORTA.OUT &= ~PIN7_bm;
												PORTA.OUT &= ~PIN3_bm;
												_delay_ms(1000);
												break;
											case 'i':
												//printf("EntraS1\r");
												PORTA.OUT |= PIN7_bm;
												PORTA.OUT |= PIN3_bm;
												_delay_ms(1000);
												wdt_reset();
												presion1 = 0;
												presion1 = get_PressureI2C(get_I2CValue(verbose,0),1);
												PORTA.OUT &= ~PIN7_bm;
												PORTA.OUT &= ~PIN3_bm;
												_delay_ms(1000);
												break;
											case 'P':
												wdt_reset();
												vol1 = get_Voltage(9); //2 segundos
												if(verbose == 1){
													printf("Voltaje Sensor1: %f\n",vol1);
												}
												presion1 = get_PresureADC1(vol1);
												wdt_reset();
												break;
											case 'p':
												wdt_reset();
												vol1 = get_Voltage(9); //2 segundos
												if(verbose == 1){
													printf("Voltaje Sensor1: %f\n",vol1);
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
												PORTC.OUT |= PIN2_bm;
												PORTA.OUT |= PIN3_bm;
												_delay_ms(1000);
												wdt_reset();
												presion2 = get_PressureI2C(get_I2CValue(verbose,0),2);
												PORTC.OUT &= ~PIN2_bm;
												PORTA.OUT &= ~PIN3_bm;
												break;
											case 'i':
												//printf("EntraS2\r");
												PORTC.OUT |= PIN2_bm;
												PORTA.OUT |= PIN3_bm;
												_delay_ms(1000);
												wdt_reset();
												presion2 = 0;
												presion2 = get_PressureI2C(get_I2CValue(verbose,0),1);
												PORTC.OUT &= ~PIN2_bm;
												PORTA.OUT &= ~PIN3_bm;
												_delay_ms(200);
												break;
											case 'P':
												wdt_reset();
												vol2 = get_Voltage(5); //2 segundos
												//PORTB.OUT &= ~PIN5_bm;
												if(verbose == 1){
													printf("Voltaje Sensor2: %f\n",vol2);
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
													printf("Voltaje Sensor2: %f\n",vol2);
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
												printf("RH%.2fRL%.2fRVO%.2f\n",presion1,caudal,bateria);
												vol1 = 0;
												caudal = 0;
												presion1 = 0;
												bateria = 0;
											}
											if(s1 == 0 && s2 == 0 && flujo != 0){
												//CSV
												printf("RH%.2fRL%.2fRQ%.2fRVO%.2f\n",presion1,caudal,presion2,bateria);
												//MQTT - FTP
												//printf("RH%.2fRQ%.2fRL%.2fRVO%.2f\n",presion1,presion2,caudal,bateria);
												vol1 = 0;
												vol2 = 0;
												caudal = 0;
												pulsos = 0;
												presion1 = 0;
												presion2 = 0;
												bateria = 0;
											}
											if(s1 != 0 && s2 != 0 && flujo != 0){
												printf("RF%.2fRVO%.2f\n",caudal,bateria);
												caudal = 0;
												bateria = 0;
											}
										}
										else{
											if(strcmp(command,"AC\r") == 0){
												if(flujo == 0){
													printf("Incorrect Command\n");
												}
												else{
													Flotante Volumen;
													int desplazo = read_EEPROM(5351)*4;//*(uint8_t*)(5351)*4;
													for(int k=0; k <= 3; k++){
														Volumen.dato[k] = read_EEPROM(k+desplazo+5120);//*(uint8_t *)(k+desplazo+5120);
													}
													printf("AC%.2f\n", Volumen.f);
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
													char *token = strtok(command,",");
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
															token = strtok(NULL, ",");
															i++;
														}
													}
													if(dato == 10){
														write_EEPROM(5350,1);
														printf("LT\n");
													}
													else{
														if(dato == 100){
															write_EEPROM(5350,2);
															printf("LT\n");
														}
														else{
															if(dato == 1000){
																write_EEPROM(5350,3);
																printf("LT\n");
															}
															else{
																if(dato == 5000){
																	write_EEPROM(5350,4);
																	printf("LT\n");
																}
																else
																{
																	write_EEPROM(5350,0);
																	printf("LT\n");	
																}
															}
														}
													}
												}
												else{
													if (strcmp(command,"VR\r") == 0){
														printf("Right Move\r");
														PORTB.OUT |= PIN2_bm;
														_delay_ms(100);
														PORTB.OUT &= ~PIN2_bm;
													}
													else{
														if (strcmp(command,"VL\r") == 0){
															printf("Left Move\r");
															PORTC.OUT |= PIN1_bm;
															_delay_ms(100);
															PORTC.OUT &= ~PIN1_bm;
														}
														else{
															if(strcmp(command,"WDR\r")== 0){
																wdt_reset();
																printf("WDR,1\r");
																_delay_ms(1000000);
																wdt_reset();
															}
															else{
																//printf("Incorrect Command\n");	
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
					}
				}
			}
		}
	}
}

#endif /* COMMAND_H_ */
