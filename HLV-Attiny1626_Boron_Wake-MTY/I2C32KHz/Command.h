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
 * @brief Variable para la obtencion de voltaje de la bateria de valvulas
 *
 */
float bateria2 = 0;
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

char sensor3;

Flotante flow;

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
			flow.dato[0] = read_EEPROM(5356);
			flow.dato[1] = read_EEPROM(5357);
			flow.dato[2] = read_EEPROM(5358);
			flow.dato[3] = read_EEPROM(5359);
			printf("GLT%.2f\n",flow.f);
		}
		else{
			if(strstr(command,"GC1")!= NULL){
				Flotante A1,B1;
				for (int i = 0; i <= 3; i++){
					A1.dato[i] = read_EEPROM(5360+i);
					B1.dato[i] = read_EEPROM(5364+i);
				}
				printf("GC1%.2f_%.2f\n",A1.f,B1.f);
			}
			else{
				if (strstr(command,"GC2")!= NULL){
					Flotante A2,B2;
					for (int i = 0; i <= 3; i++){
						A2.dato[i] = read_EEPROM(5368+i);
						B2.dato[i] = read_EEPROM(5372+i);
					}
					printf("GC2%.2f_%.2f\n",A2.f,B2.f);	
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
							Flotante A,B;
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
							wdt_reset();
							for(int i=0; i<=3; i++){
								write_EEPROM(i+5368,A.dato[i]);
								write_EEPROM(i+5372,B.dato[i]);
								}
							printf("CN2\n");
							}
							else{
								if(strstr(command,"CN1")!= NULL){
									Flotante C,D;
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
									wdt_reset();
									for(int i=0; i<=3; i++){
										write_EEPROM(i+5360,C.dato[i]);
										write_EEPROM(i+5364,D.dato[i]);
									}
									printf("CN1\n");
								}
								else{
									if(strcmp(command,"MS\r") == 0){
										sensor1 = read_EEPROM(5348);
										sensor2 = read_EEPROM(5349);
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
										bateria2 = get_Voltage(4)*5;
										bateria = get_Voltage(6)*5; //2 segundos
										//count = 0;
									}
									else{
										if(strcmp(command,"RT\r") == 0){
											Flotante valor;
											valor.dato[0] = read_EEPROM(5356);
											valor.dato[1] = read_EEPROM(5357);
											valor.dato[2] = read_EEPROM(5358);
											valor.dato[3] = read_EEPROM(5359);
											if ((t_actual + t_restante) == 0)
												caudal = 0;
											else
												caudal = (valor.f)/(t_actual + t_restante/32768);
											volumen = pulsos*valor.f;
											t_actual = 0;
											t_restante = 0;
											//1P1F
											if(s1 == 0 && s2 == 1){
												if(read_EEPROM(5355) == 0){
													//Volumetrico
													printf("RH%.2fRL%.0fRVO%.2fRVP%.2f\n",presion1,volumen,bateria,bateria2);
													caudal = 0;
													volumen = 0;
													pulsos = 0;
													presion1 = 0;
												}
												else{
													//Caudal
													printf("RH%.2fRF%.2fRVO%.2fRVP%.2f\n",presion1,caudal,bateria,bateria2);
													caudal = 0;
													presion1 = 0;
												}
												vol1 = 0;
											}
											//2P1F
											if(s1 == 0 && s2 == 0){
												//CSV
												if(read_EEPROM(5355) == 0){
													//Volumetrico
													if(read_EEPROM(5352) == 0){
														printf("RH%.2fRL%.0fRQ%.2fRVO%.2fRVP%.2f\n",presion1,volumen,presion2,bateria,bateria2);
													}
													else{
														printf("RH%.2fRL%.0fRQ%.2fRVO%.2fRVP%.2f\n",presion2,volumen,presion1,bateria,bateria2);	
													}
													vol1 = 0;
													vol2 = 0;
													volumen = 0;
													pulsos = 0;
													presion1 = 0;
													presion2 = 0;
												}
												else{
													//Caudal
													if(read_EEPROM(5352) == 0){
														printf("RH%.2fRF%.2fRQ%.2fRVO%.2fRVP%.2f\n",presion1,caudal,presion2,bateria,bateria2);
													}
													else{
														printf("RH%.2fRF%.2fRQ%.2fRVO%.2fRVP%.2f\n",presion2,caudal,presion1,bateria,bateria2);
													}
													vol1 = 0;
													vol2 = 0;
													caudal = 0;
													presion1 = 0;
													presion2 = 0;
												}
											}
											//1F
											if(s1 != 0 && s2 != 0){
												//Volumetrico
												if(read_EEPROM(5355) == 0){
													printf("RL%.0fRVO%.2fRVP%.2f\n",volumen,bateria,bateria2);
													volumen = 0;
													pulsos = 0;
												}
												else{
													//Caudal
													printf("RF%.2fRVO%.2fRVP%.2f\n",caudal,bateria,bateria2);
													caudal = 0;
													
												}
											}
											bateria = 0;
											bateria2 = 0;
										}
										else{
											if(strcmp(command,"AC\r") == 0){
												if(read_EEPROM(5354) == 0){
													if (verbose == 1){
														printf("No acumulado\r");
													}
												}
												else{
													Flotante Volumen;
													Flotante valor;
													valor.dato[0] = read_EEPROM(5356);
													valor.dato[1] = read_EEPROM(5357);
													valor.dato[2] = read_EEPROM(5358);
													valor.dato[3] = read_EEPROM(5359);
													int desplazo = read_EEPROM(5353)*4;//*(uint8_t*)(5351)*4;
													for(int k=0; k <= 3; k++){
														Volumen.dato[k] = read_EEPROM(k+desplazo+5120);//*(uint8_t *)(k+desplazo+5120);
													}
													Volumen.f += (float)count*valor.f/1000;
													printf("AC%.2f\n", Volumen.f);
													count = 0;
													for(int k=0; k <= 3; k++){
														write_EEPROM(k+desplazo+5120,0);
													}
													int sumador = read_EEPROM(5353);//*(uint8_t*)(5351);
													sumador++;
													write_EEPROM(53513,sumador);
													int	compara = read_EEPROM(5353);
													if(compara > 56){
														write_EEPROM(5353,0);
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
													flow.f = dato;
													write_EEPROM(5356,flow.dato[0]);
													write_EEPROM(5357,flow.dato[1]);
													write_EEPROM(5358,flow.dato[2]);
													write_EEPROM(5359,flow.dato[3]);
													flow.dato[0] = read_EEPROM(5356);
													flow.dato[1] = read_EEPROM(5357);
													flow.dato[2] = read_EEPROM(5358);
													flow.dato[3] = read_EEPROM(5359);
													printf("LT,%.2f\n", flow.f);
													
												}
												else{
													if (strcmp(command,"VR\r") == 0){
														//printf("Right Move\n");
														PORTB.OUT |= PIN2_bm;
														_delay_ms(100);
														PORTB.OUT &= ~PIN2_bm;
														printf("VR\n");
													}
													else{
														if (strcmp(command,"VL\r") == 0){
															//printf("Left Move\n");
															PORTC.OUT |= PIN1_bm;
															_delay_ms(100);
															PORTC.OUT &= ~PIN1_bm;
															printf("VL\n");
														}
														else{
															//Switch de activacion de acumulado
															//0 desactivado - 1 activado
															if(command[0] == 'S' && command[1] == 'A' && command[2] == 'C'){
																//int actual = read_EEPROM(5354);
																if(command[3] == '0'){
																	write_EEPROM(5354,0x00);
																	printf("%s\n",command);
																}
																else if(command[3] == '1'){
																	write_EEPROM(5354,0x01);
																	printf("%s\n",command);
																}
															}
															else{
																//Switch de cambio de sensores
																//0 original - 1 invertido
																//if(strcmp(command,"SSW\r") == 0){
																if(command[0] == 'S' && command[1] == 'S' && command[2] == 'W'){
																	//int actual = read_EEPROM(5352);
																	if(command[3] == '0'){
																		write_EEPROM(5352,0x00);
																		printf("%s\n",command);
																	}
																	else if(command[3] == '1'){
																		write_EEPROM(5352,0x01);
																		printf("%s\n",command);
																	}
																}
																else{
																	//Switch de tipo de flujo
																	//0 volumen - 1 caudal
																	//if(strstr(command,"SFT")!= NULL){
																	if(command[0] == 'S' && command[1] == 'F' && command[2] == 'T'){
																		//int actual = read_EEPROM(5355);
																		if(command[3] == '0'){
																			caudal = 0;
																			pulsos = 0;
																			volumen = 0;
																			count = 0;
																			TCA0_stop();
																			write_EEPROM(5355,0x00);
																			printf("%s\n",command);
																		}
																		else if(command[3] == '1'){
																			caudal = 0;
																			pulsos = 0;
																			volumen = 0;
																			count = 0;
																			write_EEPROM(5355,0x01);
																			printf("%s\n",command);
																		}																	
																	}
																	else{
																		if(command[0] == 'G' && command[1] == 'A' && command[2] == 'C'){
																			printf("GAC%d\n",read_EEPROM(5354));
																		}
																		else{
																			if(command[0] == 'G' && command[1] == 'S' && command[2] == 'W'){
																				printf("GSW%d\n",read_EEPROM(5352));
																			}
																			else{
																				if(command[0] == 'G' && command[1] == 'S' && command[2] == '1'){
																					printf("GS1%c\n",read_EEPROM(5348));
																				}
																				else{
																					if(command[0] == 'G' && command[1] == 'S' && command[2] == '2'){
																						printf("GS2%c\n",read_EEPROM(5349));
																					}
																					else{
																						if(command[0] == 'G' && command[1] == 'F' && command[2] == 'T'){
																							printf("GFT%d\n",read_EEPROM(5355));
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
						}
					}
				}
			}
		}
	}
}



#endif /* COMMAND_H_ */