/*
 * TCA.h
 *
 * Created: 05/07/2023 01:47:32 p. m.
 *  Author: H-Tech
 */ 


#ifndef TCA_H_
#define TCA_H_

#include <avr/io.h>
#include <avr/wdt.h>
#include "PULSE.h"

volatile uint16_t segundos = 0;
volatile uint16_t tiempo = 0;
volatile uint16_t minutos = 0;
float caudal = 0;


void TCA0_init() {
	TCA0.SINGLE.PER = 30720; // Establece el valor de comparación para aproximadamente 1 segundo
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV64_gc | TCA_SINGLE_ENABLE_bm; // Configura el prescaler a 1:1 y habilita el temporizador
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm; // Habilita la interrupción por desbordamiento
}


ISR(TCA0_OVF_vect){
	segundos = 60;
	TCA0.SINGLE.INTFLAGS = 0x01;
	wdt_reset();
	int Ent = read_EEPROM(5350);
	int mul = 0;
	switch(Ent){
		case 1:
		mul = 10;
		break;
		case 2:
		mul = 100;
		break;
		case 3:
		mul = 1000;
		break;
		default:
		mul = 1;
		break;
	}
	caudal = (float)pulsos*mul/segundos;
	if(verbose == 1){
		printf("Tiempo = %d\r", segundos);
		printf("Pulsos: %d\r", pulsos);
		printf("Caudal: %.2f\r", caudal);	
	}
	pulsos = 0;
	minutos++;
	wdt_reset();
};


#endif /* TCA_H_ */