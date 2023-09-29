/*
 * TCB.h
 *
 * Created: 24/08/2023 10:17:06 a. m.
 *  Author: H-Tech
 */ 


#ifndef TCB_H_
#define TCB_H_

volatile uint16_t seconds = 0;
volatile uint16_t minutes = 0;

void TCB0_init() {
	TCB0.CTRLA = TCB_CLKSEL_DIV1_gc;
	TCB0.CCMP = 32768;
	TCB0.CTRLB = TCB_CNTMODE_INT_gc; // Habilita el modo de interrupción
	TCB0.INTCTRL = TCB_CAPT_bm; // Habilita la interrupción de captura
	//TCB0.CNT = 0;
	//TCB_CLKSEL_CLKDIV2_gc; // Configura el prescaler a 1:2 (F_CPU/2)
	//TCB0.CTRLB = TCB_CNTMODE_INT_gc; // Habilita el modo de interrupción
	//TCB0.INTCTRL = TCB_CAPT_bm; // Habilita la interrupción de captura
}

void TCB0_start() {
	TCB0.CNT = 0; // Reinicia el contador
	TCB0.CTRLA |= TCB_ENABLE_bm; // Inicia el temporizador
}

void TCB0_stop() {
	TCB0.CTRLA &= ~TCB_ENABLE_bm; // Detiene el temporizador
}



ISR(TCB0_INT_vect){
	seconds++;
	TCB0.INTFLAGS = 0x1;
	wdt_reset();
	if(verbose == 1){
		printf("Tiempo: %d\r", seconds);	
	}
}

#endif /* TCB_H_ */