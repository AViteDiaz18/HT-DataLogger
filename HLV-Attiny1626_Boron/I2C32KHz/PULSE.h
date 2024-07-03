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
float caudal = 0;

/**
 * @brief Funcion que habilita el uso de interrupciones externas de vector C
 *
 * @param PORTC_PORT_VECT Vector de interrucpiones del puerto C
 *
 */
ISR (PORTC_PORT_vect){
	//_delay_ms(100);
	pulsos++;
	TCA0_stop();
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
	if(ms500 < 1 || ms500 > 20000){
		caudal = 0;
	}
	else{
		caudal = pulsos*mul/((float)ms500*0.5);
		ms500 = 0;
		pulsos = 0;	
	}
	wdt_reset();
	count++;
	if(verbose == 1){
		printf("Caudal%.2f\r", caudal);
	}
	//executeCommand("MS\r");
	//memset(command, 0, MAX_COMMAND_LEN);
	PORTC_INTFLAGS |= PIN3_bm;
	TCA0_init();
	
	//count++;
	//printf("%u\r", count);
	
}

#endif /* PULSE_H_ */