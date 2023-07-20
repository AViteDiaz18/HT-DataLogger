/*
 * EEPROM.h
 *
 * Created: 05/07/2023 02:16:18 p. m.
 *  Author: H-Tech
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

void write_EEPROM(int posicion, unsigned char dato){
	*(uint8_t *)(posicion) = dato;
	CPU_CCP = CCP_SPM_gc;
	NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
}

unsigned char read_EEPROM(int posicion){
	return *(uint8_t*)(posicion);
}

#endif /* EEPROM_H_ */