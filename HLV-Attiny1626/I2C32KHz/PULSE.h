/*
 * PULSE.h
 *
 * Created: 05/07/2023 01:52:17 p. m.
 *  Author: H-Tech
 */ 


#ifndef PULSE_H_
#define PULSE_H_
#include <avr/wdt.h>

unsigned int count = 0;
unsigned int pulsos = 0;
int verbose = 0;

ISR (PORTC_PORT_vect){
	//_delay_ms(100);
	wdt_reset();
	count++;
	pulsos++;
	if(verbose == 1){
		printf("%u\r", pulsos);
	}
	//executeCommand("MS\r");
	//memset(command, 0, MAX_COMMAND_LEN);
	PORTC_INTFLAGS |= PIN3_bm;
	
	//count++;
	//printf("%u\r", count);
	
}

#endif /* PULSE_H_ */