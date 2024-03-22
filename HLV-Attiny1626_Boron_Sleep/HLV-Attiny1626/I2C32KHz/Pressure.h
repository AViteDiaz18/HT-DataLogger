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

float get_LevelADC(float vol, int posicion){
	Flotante Zero,A,B,CL;
	float Level = 0;
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
	if(vol < 0.911833){
		Level = 0;
	}
	else{
		Level = ((A.f * vol - B.f) * 0.703089) - ((A.f * 0.911833 - B.f) * 0.703089);
	}
	
	CL.dato[0] = read_EEPROM(5124);
	CL.dato[1] = read_EEPROM(5125);
	CL.dato[2] = read_EEPROM(5126);
	CL.dato[3] = read_EEPROM(5127);
	
	if(CL.f != 0 && (Level < CL.f)){
		Level = (float)(CL.f - Level);
	}
	
	return Level;
}


#endif /* PRESSURE_H_ */