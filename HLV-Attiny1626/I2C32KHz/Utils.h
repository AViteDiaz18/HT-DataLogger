/*
 * Utils.h
 *
 * Created: 05/07/2023 03:04:20 p. m.
 *  Author: H-Tech
 */ 


#ifndef UTILS_H_
#define UTILS_H_

void PinInit(){
	//Interrupts
	PORTC.DIR &= ~(1<<3);
	PORTC.PIN3CTRL &= ~PORT_PULLUPEN_bm;
	PORTC.PIN3CTRL |= PORT_ISC_RISING_gc;
	//USART0
	PORTA.DIR &= ~PIN2_bm;
	PORTA.DIR |= PIN1_bm;
	//VPORTB.DIR = PIN0_bm;
	
	//Presion 1
	PORTB.PIN4CTRL &= ~PORT_ISC_gm;
	PORTB.PIN4CTRL |= PORT_ISC_INPUT_DISABLE_gc;
	PORTB.PIN4CTRL &= ~PORT_PULLUPEN_bm;
	
	//Presion 2
	PORTA.PIN5CTRL &= ~PORT_ISC_gm;
	PORTA.PIN5CTRL |= PORT_ISC_INPUT_DISABLE_gc;
	PORTA.PIN5CTRL &= ~PORT_PULLUPEN_bm;
	
	//I2C
	PORTB.PIN0CTRL = PORT_PULLUPEN_bm;
	PORTB.PIN1CTRL = PORT_PULLUPEN_bm;
	PORTB.DIR = PIN0_bm|PIN1_bm;
	
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
	PORTA.PIN2CTRL &= ~PORT_PULLUPEN_bm;
	PORTA.OUT &= ~PIN3_bm;
}

void PORT_LOWPOWER_Init(void)
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

void Sleep_Micro(){
	set_sleep_mode(SLEEP_MODE_IDLE);
	cli();
	sleep_enable();
	
	sei();
	sleep_cpu();
	sleep_disable();
}

void CLOCK32K_init(){
	CPU_CCP = CCP_IOREG_gc;
	CLKCTRL_MCLKCTRLB = 0 << CLKCTRL_PEN_bp;
	CPU_CCP = CCP_IOREG_gc;
	CLKCTRL_MCLKCTRLA = CLKCTRL_CLKSEL_OSCULP32K_gc;
}




#endif /* UTILS_H_ */