/**
 * @file ADC.h
 * @author Htech Mexico
 * @brief Libreria para el manejo de los puertos ADC
 * @version 1.0
 * @date 24-05-2023
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>
#include "EEPROM.h"

/**
 * @brief Definicion de una union para el manejo de flotantes en Bytes
 *
 */
typedef union Float_Byte{
	float f;
	unsigned char dato[4];
}Flotante;

/**
 * @brief Funcion para la inicializacion del ADC
 *
 */
void ADC0_init(){
	
	ADC0.CTRLB = ADC_PRESC_DIV4_gc;
	ADC0.CTRLC = ADC_REFSEL_2500MV_gc;
}

/**
 * @brief Funcion para la lectura de corriente en los puertos ADC
 *
 */
uint16_t ADC0_read(void)
{
	ADC0.COMMAND = ADC_START_IMMEDIATE_gc | ADC_MODE_SINGLE_12BIT_gc;
	while ( !(ADC0.INTFLAGS & ADC_RESRDY_bm) )
	{
		;
	}
	ADC0.INTFLAGS = ADC_RESRDY_bm;
	return ADC0.RESULT;
}

/**
 * @brief Funcion para la conversion de corriente a voltaje
 *
 * @param pin Variable que almacena el puerto en el que se realiza el sensado y la conversion
 */
float get_Voltage(int pin){
	float lectura = 0;
	float Voltage = 0;
	int a = 0;
	switch (pin){
		case 9:
		ADC0.CTRLA = ADC_ENABLE_bm;
		ADC0.MUXPOS = ADC_MUXPOS_AIN9_gc;
		break;
		case 5:
		ADC0.CTRLA = ADC_ENABLE_bm;
		ADC0.MUXPOS = ADC_MUXPOS_AIN5_gc;
		break;
		case 4:
		ADC0.CTRLA = ADC_ENABLE_bm;
		ADC0.MUXPOS = ADC_MUXPOS_AIN4_gc;
		break;
		case 6:
		ADC0.CTRLA = ADC_ENABLE_bm;
		ADC0.MUXPOS = ADC_MUXPOS_AIN6_gc;
		break;
	}
	while(a < 20){
		lectura = ((ADC0_read()*2.5)/4096);
		Voltage = (0.5*lectura) + ((1.0 - 0.5)*Voltage);
		a++;
		_delay_ms(100);
	}
	ADC0.CTRLA &= ~ADC_ENABLE_bm;
	return Voltage;
}




#endif /* ADC_H_ */