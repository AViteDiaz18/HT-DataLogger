/*
 * Pressure.h
 *
 * Created: 06/07/2023 10:33:14 a. m.
 *  Author: H-Tech
 */ 


#include "ADC.h"
#include "I2C.h"

#ifndef PRESSURE_H_
#define PRESSURE_H_

float get_PressureADC(float vol, int posicion){
	Flotante Zero,A,B;
	float Pressure = 0;
	
	if(posicion == 1){
		for (int i = 0; i <= 3; i++){
			Zero.dato[i] = read_EEPROM(5364+i);
			A.dato[i] = read_EEPROM(5368+i);
			B.dato[i] = read_EEPROM(5372+i);
		}	
	}
	else{
		if(posicion == 2){
			for (int i = 0; i <= 3; i++){
				Zero.dato[i] = read_EEPROM(5352+i);
				A.dato[i] = read_EEPROM(5356+i);
				B.dato[i] = read_EEPROM(5360+i);
			}
		}
	}
	
	if(vol <=  Zero.f){
		Pressure = 0.0;
	}
	else{
		Pressure = (A.f * vol - B.f)*10.2;
	}
	return Pressure;
	
	
}

float get_PresureADC2(float vol){
	Flotante Zero1;
	Flotante A1;
	Flotante B1;
	float Pressure = 0;
	
	for (int i = 0; i <= 3; i++){
		Zero1.dato[i] = read_EEPROM(5352+i);
		A1.dato[i] = read_EEPROM(5356+i);
		B1.dato[i] = read_EEPROM(5360+i);
	}
	if(vol <=  Zero1.f){
		Pressure = 0.0;
	}
	else{
		Pressure = (A1.f * vol - B1.f)*10.2;
	}
	return Pressure;
	
	//***************************************************
}

float get_PresureADC1(float vol){
	Flotante Zero2;
	Flotante A2;
	Flotante B2;
	float Pressure = 0;
	
	for (int i = 0; i <= 3; i++){
		Zero2.dato[i] = read_EEPROM(5364+i);
		A2.dato[i] = read_EEPROM(5368+i);
		B2.dato[i] = read_EEPROM(5372+i);
	}
	if(vol <=  Zero2.f){
		Pressure = 0.0;
	}
	else{
		Pressure = (A2.f * vol - B2.f)*10.2;
	}
	return Pressure;
	
	//***************************************************
}

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