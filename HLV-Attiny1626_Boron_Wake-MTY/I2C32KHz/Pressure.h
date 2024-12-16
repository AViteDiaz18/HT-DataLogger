/**
 * @file Pressure.h
 * @author Htech Mexico
 * @brief Libreria para el manejo rectas de presion
 * @version 1.0
 * @date 05-07-2023
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "ADC.h"
#include "I2C.h"

#ifndef PRESSURE_H_
#define PRESSURE_H_

/**
 * @brief Funcion que realiza los calculos de conversion de voltaje a presion
 *
 *
 * @param vol Voltaje de entrada a convertir
 *
 * @param posicion Posicion fisica en donde se encuentra conectado el sensor ADC
 */
float get_PressureADC(float vol, int posicion){
	Flotante A,B;
	float Pressure = 0;
	float cero = 0.0;
	
	if(posicion == 1){
		for (int i = 0; i <= 3; i++){
			A.dato[i] = read_EEPROM(5360+i);
			B.dato[i] = read_EEPROM(5364+i);
		}	
	}
	else{
		if(posicion == 2){
			for (int i = 0; i <= 3; i++){
				A.dato[i] = read_EEPROM(5368+i);
				B.dato[i] = read_EEPROM(5372+i);
			}
		}
	}
	cero = (B.f/A.f);
	if(vol <= cero){
		Pressure = 0.0;
	}
	else{
		Pressure = (A.f * vol - B.f)*10.2;
	}
	return Pressure;
	
	
}
/**
 * @brief Funcion para obtener la presion mediante corriente en la posicion 2
 *
 * @param vol Voltaje de entrada entregado por el sensor
 *
 */
float get_PresureADC2(float vol){
	Flotante A1;
	Flotante B1;
	float Pressure = 0;
	float cero = 0;
	
	for (int i = 0; i <= 3; i++){
		A1.dato[i] = read_EEPROM(5356+i);
		B1.dato[i] = read_EEPROM(5360+i);
	}
	cero = B1.f/A1.f;
	if(vol <= cero){
		Pressure = 0.0;
	}
	else{
		Pressure = (A1.f * vol - B1.f)*10.2;
	}
	return Pressure;
	
	//***************************************************
}
/**
 * @brief Funcion para obtener la presion mediante corriente en la posicion 1
 *
 * @param vol Voltaje de entrada entregado por el sensor
 *
 */
float get_PresureADC1(float vol){
	Flotante A2;
	Flotante B2;
	float cero = 0.0;
	float Pressure = 0;
	
	for (int i = 0; i <= 3; i++){
		A2.dato[i] = read_EEPROM(5368+i);
		B2.dato[i] = read_EEPROM(5372+i);
	}
	cero = B2.f/A2.f;
	if(vol <=  cero){
		Pressure = 0.0;
	}
	else{
		Pressure = (A2.f * vol - B2.f)*10.2;
	}
	return Pressure;
	
	//***************************************************
}
/**
 * @brief Funcion para obtener la presion por protocolo I2C
 *
 * @param Press Variable entera que contiene el resultado enviado por el sensor de presion
 *
 * @param x Variable que contiene el rango del sensor en BAR
 *
 */
float get_PressureI2C(int Press, int x){
	float P = 0;
	if(x == 1){
		if(Press < 1000){
			P = 0;
		}
		else{
			int a = Press-1000;
			P = (a*10.2)/2000;
			//printf("Presion-1000: %d\r", a);
		}
	}
	if(x == 2){
		if(Press < 1000){
			P = 0;
		}
		else{
			int a = Press-1000;
			P = a/66.66;
			//printf("Presion-1000: %d\r", a);
		}
	}
	return P;
}



#endif /* PRESSURE_H_ */