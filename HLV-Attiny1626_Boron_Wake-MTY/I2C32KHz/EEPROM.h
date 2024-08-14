/**
 * @file EEPROM.h
 * @author Htech Mexico
 * @brief Libreria para el manejo de registros de EEPROM
 * @version 1.0
 * @date 05-07-2023
 *
 * @copyright Copyright (c) 2023
 *
 */


#ifndef EEPROM_H_
#define EEPROM_H_

/**
 * @brief Funcion para la escritura de un registro de EEPROM
 *
 * @param posicion Variable que contiene la posicion de registro a escribir
 *
 * @param dato Variable que contiene el dato a escribir en el registro
 *
 */
void write_EEPROM(int posicion, unsigned char dato){
	*(uint8_t *)(posicion) = dato;
	CPU_CCP = CCP_SPM_gc;
	NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
}

/**
 * @brief Funcion para la lectura de un registro en EEPROM
 *
 * @param posicion Variable que contiene la posicion del registro de lectura
 *
 */
unsigned char read_EEPROM(int posicion){
	return *(uint8_t*)(posicion);
}

#endif /* EEPROM_H_ */