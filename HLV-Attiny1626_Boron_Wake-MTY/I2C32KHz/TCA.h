/**
 * @file TCA.h
 * @author Htech Mexico
 * @brief Libreria para el manejo del contador TCA
 * @version 1.0
 * @date 05-07-2023
 *
 * @copyright Copyright (c) 2023
 *
 */



#ifndef TCA_H_
#define TCA_H_

#include <avr/io.h>
#include <avr/wdt.h>
//#include "PULSE.h"
/**
 * @brief Variable para la activacion del parametro verbose
 *
 */
int verbose = 0;
/**
 * @brief Variable para el manejo de medio segundo
 *
 */
volatile int ms500 = 0;
/**
 * @brief Variable para el conteo de una hora
 *
 */
volatile int hr = 0;
/**
 * @brief Variable para el manejo de segundos en el contador de 16bits
 *
 */
volatile uint16_t segundos = 0;
/**
 * @brief Variable auxiliar para el manejo de segundos en el contador
 *
 */
volatile uint16_t tiempo = 0;
/**
 * @brief Variable auxiliar para el conteo de minutos
 *
 */
volatile uint16_t minutos = 0;


/**
 * @brief Funcion para la inicializacion de TCA
 *
 */
void TCA0_init() {
	TCA0.SINGLE.PER = 16384; // Establece el valor de comparación para aproximadamente 1 segundo
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc | TCA_SINGLE_ENABLE_bm; // Configura el prescaler a 1:1 y habilita el temporizador
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm; // Habilita la interrupción por desbordamiento
}

/**
 * @brief Funcion que detiene el contador TCA
 *
 */
void TCA0_stop(){
	TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm;
}

/**
 * @brief Funcion que activa el uso de interrupciones de TCA
 *
 * @param TCA0_OVF_vect vector de interrupciones de TCA
 *
 */
ISR(TCA0_OVF_vect){
	hr++;
	ms500++;
	TCA0.SINGLE.INTFLAGS = 0x01;
	if(verbose == 1){
		//printf("Ms500 = %d\r", ms500);
	}
	/*segundos = 60;
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
	wdt_reset();*/
};


#endif /* TCA_H_ */