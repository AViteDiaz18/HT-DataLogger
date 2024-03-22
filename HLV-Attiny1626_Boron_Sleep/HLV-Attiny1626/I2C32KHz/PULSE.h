/**
 * @file Pulse.h
 * @author H-tech Mexico
 * @brief Pulse managment library 
 * @version 1.0
 * @date 05/07/2023
 */ 


#ifndef PULSE_H_
#define PULSE_H_
#include "TCA.h"
#include <avr/wdt.h>

#define PC0_INTERRUPT				PORTC.INTFLAGS & PIN0_bm /**< Define port C0 to interrupt*/
#define PC0_CLEAR_INTERRUPT_FLAG	PORTC.INTFLAGS &= PIN0_bm /**< Clear interrupt flag from port C0 */

unsigned int count = 0; /**< Pulse counter variable for the Accumulate*/
unsigned int pulsos = 0; /**< Pulse counter variable for the LPS*/
float caudal = 0; /**< Variable who allocates the litter convertion pulse*/

/**
* @brief Interrupt Queue in port C
*/
ISR (PORTC_PORT_vect){
	
	if(PC0_INTERRUPT){
		PC0_CLEAR_INTERRUPT_FLAG;
	}
	else{
		pulsos++;
		TCA0_stop(); /**< Stops the TCA counter */ 
		wdt_reset();
		int Ent = read_EEPROM(5350); /**< Read the register 5350 to convert pulse to flow*/
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
}

#endif /* PULSE_H_ */