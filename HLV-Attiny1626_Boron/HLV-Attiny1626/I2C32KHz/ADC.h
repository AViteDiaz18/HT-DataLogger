/**
 * @file ADC.h
 * @author H-tech Mexico
 * @brief ADC manage library
 * @version 1.0
 * @date 05/07/2023
 */ 


#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>
#include "EEPROM.h"


/**
* @brief ByteUnion for EEPROM Data
* @param f Complete data in float format
* @param dato Separate data in byte format
*/
typedef union Float_Byte{
	float f;
	unsigned char dato[4];
}Flotante;

/**
* @brief ADC_init Starts and enable ports and process for ADC sampling
*/
void ADC0_init(){
	ADC0.CTRLB = ADC_PRESC_DIV4_gc; /**< Selects a 1/4 prescaler*/
	ADC0.CTRLC = ADC_REFSEL_2500MV_gc; /**< Selects a 2.5V reference voltage*/
}

/**
* @brief ADC0_read Sample the ADC ports
* @return ADC Sample in Amperage Format
*/
uint16_t ADC0_read(void)
{
	ADC0.COMMAND = ADC_START_IMMEDIATE_gc | ADC_MODE_SINGLE_12BIT_gc; /**< Selects ADC Single mode on 12Bits */
	while ( !(ADC0.INTFLAGS & ADC_RESRDY_bm) )
	{
		;
	}
	ADC0.INTFLAGS = ADC_RESRDY_bm;
	return ADC0.RESULT;
}

/**
* @brief get_Voltage Get the Voltage Data from Amperage by Omh law
* @param pin Selects the pin port to sample
* @return ADC sample in Voltage Format
*/
float get_Voltage(int pin){
	float lectura = 0;
	float Voltage = 0;
	int a = 0;
	switch (pin){
		case 9:
		ADC0.CTRLA = ADC_ENABLE_bm; /** < Enable ADC sampling */
		ADC0.MUXPOS = ADC_MUXPOS_AIN9_gc; /**< Select port 9*/
		break;
		case 5:
		ADC0.CTRLA = ADC_ENABLE_bm; /**< Enable ADC sampling */
		ADC0.MUXPOS = ADC_MUXPOS_AIN5_gc; /**< Select port 5*/
		break;
		case 6:
		ADC0.CTRLA = ADC_ENABLE_bm; /** < Enable ADC sampling */
		ADC0.MUXPOS = ADC_MUXPOS_AIN6_gc; /**< Select port 6*/
		break;
	}
	while(a < 20){
		lectura = ((ADC0_read()*2.5)/4096); /** Do 20 Samples */
		Voltage = (0.5*lectura) + ((1.0 - 0.5)*Voltage); /** Apply a tendential average*/
		a++;
		_delay_ms(100);
	}
	ADC0.CTRLA &= ~ADC_ENABLE_bm; /** Unable the ADC sampling */
	return Voltage;
}




#endif /* ADC_H_ */