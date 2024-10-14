/**
 * @file Utils.h
 * @author Htech Mexico
 * @brief Libreria de funciones utilitarias varias
 * @version 1.0
 * @date 05-07-2023
 *
 * @copyright Copyright (c) 2023
 *
 */


#ifndef UTILS_H_
#define UTILS_H_

/**
 * @brief Funcion que realiza la activacion de los pines para las diversas funciones
 *
 */

void PinInit(){
	//Interrupts
	//Activacion del Puerto C3 como entrada de pulsos crecientes y desactiva la resistencia Pull up interna
	PORTC.DIR &= ~(1<<3);
	PORTC.PIN3CTRL &= ~PORT_PULLUPEN_bm;
	PORTC.PIN3CTRL |= PORT_ISC_RISING_gc;
	//USART0
	PORTA.DIR &= ~PIN2_bm;
	PORTA.DIR |= PIN1_bm;
	//VPORTB.DIR = PIN0_bm;
	
	//Valvula R
	PORTB.DIR |= PIN2_bm;
	PORTB.PIN4CTRL &= ~PORT_PULLUPEN_bm;
	PORTB.OUT &= ~PIN2_bm;
	
	//Valvula L
	PORTC.DIR |= PIN1_bm;
	PORTB.PIN4CTRL &= ~PORT_PULLUPEN_bm;
	PORTC.OUT &= ~PIN1_bm;
	
	//Presion 1
	PORTB.PIN4CTRL &= ~PORT_ISC_gm;
	PORTB.PIN4CTRL |= PORT_ISC_INPUT_DISABLE_gc;
	PORTB.PIN4CTRL &= ~PORT_PULLUPEN_bm;
	
	//Presion 2
	PORTA.PIN5CTRL &= ~PORT_ISC_gm;
	PORTA.PIN5CTRL |= PORT_ISC_INPUT_DISABLE_gc;
	PORTA.PIN5CTRL &= ~PORT_PULLUPEN_bm;
	
	//I2C
	PORTB.DIRSET = PIN0_bm|PIN1_bm;
	PORTB.PIN0CTRL &= ~PORT_PULLUPEN_bm;
	PORTB.OUT = PIN0_bm|PIN1_bm;
	PORTB.PIN1CTRL &= ~PORT_PULLUPEN_bm;
	
	//Bateria
	PORTA.PIN6CTRL &= ~PORT_ISC_gm;
	PORTA.PIN6CTRL |= PORT_ISC_INPUT_DISABLE_gc;
	PORTA.PIN6CTRL &= ~PORT_PULLUPEN_bm;
	
	//Activador Fuente 12v
	PORTB.DIR |= PIN5_bm;
	PORTB.PIN5CTRL &= ~PORT_PULLUPEN_bm;
	PORTB.OUT &= ~PIN5_bm;
	
	//Activador 1er I2C
	PORTA.DIR |= PIN3_bm;
	PORTA.PIN3CTRL &= ~PORT_PULLUPEN_bm;
	PORTA.OUT &= ~PIN3_bm;
}

/**
 * @brief Funcion que realiza una desactivacion de los puertos
 *
 */

void PORT_LOWPOWER_Init()
{
	for (uint8_t i = 0; i < 8; i++)
	{
		*((uint8_t *)&PORTA + 0x10 + i) |= 1 << PORT_PULLUPEN_bp;
	}
	for (uint8_t i = 0; i < 6; i++)
	{
		*((uint8_t *)&PORTB + 0x10 + i) |= 1 << PORT_PULLUPEN_bp;
	}
	for (uint8_t i = 0; i < 4; i++)
	{
		*((uint8_t *)&PORTC + 0x10 + i) |= 1 << PORT_PULLUPEN_bp;
	}
	
}

/**
 * @brief Funcion que forza el estado IDLE Sleep en el microcontrolador
 *
 */
/**
 * @brief Funcion que cambia el reloj principal del microcontrolador a 32KHz
 *
 */
void CLOCK32K_init(){
	CPU_CCP = CCP_IOREG_gc;
	CLKCTRL_MCLKCTRLB = 0 << CLKCTRL_PEN_bp;
	CPU_CCP = CCP_IOREG_gc;
	CLKCTRL_MCLKCTRLA = CLKCTRL_CLKSEL_OSCULP32K_gc;
}




#endif /* UTILS_H_ */