/**
 * @file PULSE.h
 * @author Htech Mexico
 * @brief Libreria para el manejo de flujo
 * @version 1.0
 * @date 05-07-2023
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef PULSE_H_
#define PULSE_H_
#include "TCA.h"
#include <avr/wdt.h>

/**
 * @brief Variable que cuenta el numero de pulsos para el acumulado
 *
 */
unsigned int count = 0;
/**
 * @brief Variable que cuenta el numero de pulsos para la conversion de flujo
 *
 */
unsigned long int pulsos = 0;
/**
 * @brief Variable que contiene el dato de flujo ya en conversion
 *
 */
float sumatoria = 0;
float caudal = 0;
float volumen = 0;
float t_actual;
float t_restante;

/**
 * @brief Funcion que habilita el uso de interrupciones externas de vector C
 *
 * @param PORTC_PORT_VECT Vector de interrupciones del puerto C
 *
 */
ISR (PORTC_PORT_vect){
	t_actual = segundos;
	t_restante = (float)(TCA0.SINGLE.CNT);
	pulsos++;
	//TCA0_stop();
	TCA0.SINGLE.CNT = 0;
	segundos = 0;
	wdt_reset();
	//Agregar el manejo de tiempo entre pulsos
	//caudal = count*valor.f/(t_actual + t_restante);
	//printf("Tiempo entre pulsos: %0.2f\r", t_actual + t_restante/32768);
	//printf("Caudal: %0.2f\r", (count*valor.f)/(t_actual + t_restante/32768));
	wdt_reset();
	if(verbose == 1){
		printf("Tiempo%.2f\r", t_actual + t_restante/32768);
	}
	//executeCommand("MS\r");
	//memset(command, 0, MAX_COMMAND_LEN);
	PORTC_INTFLAGS |= PIN3_bm;
	//TCA0_init();
	
	count++;
	//printf("%u\r", count);
}

#endif /* PULSE_H_ */