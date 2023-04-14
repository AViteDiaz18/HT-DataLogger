/*
 * Attiny426_V2 Project.cpp
 *
 * Created: 1/11/2023 11:20:51 AM
 * Author : Alejandro Vite Diaz
 */ 

#define USART0_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#define MAX_COMMAND_LEN 8

unsigned int count = 0;
char command[MAX_COMMAND_LEN] = {' '};
uint8_t index1 = 0;
char c;

//******** Section ReportCount **********

unsigned long ReportCount(){
	unsigned long actual = 0;
	unsigned long vuelta = 0;
	unsigned long completo = 65026;
	vuelta = *(uint8_t *)(5375) * completo;
	for(int i = 5120; i <= 5374; i++){
		actual += *(uint8_t *)(i);
	}
	return vuelta+actual;
}

//******** Section Get Float **********
typedef union{
	struct{
		int entero;
		int decimal1;
		int decimal2;
	};
}Flotante;

Flotante get_Float(float a){
	Flotante auxiliar;
	float aux = a;
	auxiliar.entero = (int)a;
	aux = aux-auxiliar.entero;
	aux = aux*10;
	auxiliar.decimal1 = (int)aux;
	aux = aux - auxiliar.decimal1;
	aux = aux*10;
	auxiliar.decimal2 = (int)aux;
	return auxiliar;
}

//*********** Section USART ************* 
static void USART0_sendChar(char c){
	while (!(USART0.STATUS & USART_DREIF_bm)){
		;
	}
	USART0.TXDATAL = c;
}

void USART0_sendString(char *str)
{
	for(size_t i = 0; i < strlen(str); i++)
	{
		USART0_sendChar(str[i]);
	}
}

static int USART0_printChar(char c, FILE *stream){
	USART0_sendChar(c);
	return 0;
}

static FILE USART_stream = FDEV_SETUP_STREAM(USART0_printChar, NULL, _FDEV_SETUP_WRITE);

static void USART0_init(void){
	USART0.BAUD = (uint16_t)USART0_BAUD_RATE(1200);
	USART0.CTRLB = USART_RXEN_bm | USART_TXEN_bm;
	USART0_CTRLA = (1<<USART_RXCIE_bp)|(0<<USART_TXCIE_bp);
	PORTMUX.USARTROUTEA = PORTMUX_USART0_ALT1_gc; 
	stdout = &USART_stream;
}

//************* Section Clock ************** 
static void CLOCK32K_init(){
	CPU_CCP = CCP_IOREG_gc;
	CLKCTRL_MCLKCTRLB = 0 << CLKCTRL_PEN_bp;
	CPU_CCP = CCP_IOREG_gc;
	CLKCTRL_MCLKCTRLA = CLKCTRL_CLKSEL_OSCULP32K_gc;
}

//************ Section Pulse Counter **************
static void Pin_init(){
	//Contador de pulsos
	PORTC.DIR &= ~(1<<3);
	//PORTC.PIN3CTRL |= PORT_PULLUPEN_bm;
	PORTC.PIN3CTRL |= PORT_ISC_RISING_gc;
	//USART
	PORTA.DIR &= ~PIN2_bm;
	PORTA.DIR |= PIN1_bm;
	//Iniciador ADC
	//Presion 1
	PORTB.PIN4CTRL &= ~PORT_ISC_gm;
	PORTB.PIN4CTRL |= PORT_ISC_INPUT_DISABLE_gc;
	PORTB.PIN4CTRL &= ~PORT_PULLUPEN_bm;
	//Presion 2
	PORTA.PIN5CTRL &= ~PORT_ISC_gm;
	PORTA.PIN5CTRL |= PORT_ISC_INPUT_DISABLE_gc;
	PORTA.PIN5CTRL &= ~PORT_PULLUPEN_bm;
	//Bateria
	PORTA.PIN6CTRL &= ~PORT_ISC_gm;
	PORTA.PIN6CTRL |= PORT_ISC_INPUT_DISABLE_gc;
	PORTA.PIN6CTRL &= ~PORT_PULLUPEN_bm;
}

//************** Section ADC ************************
void ADC0_init(){
	ADC0.CTRLB = ADC_PRESC_DIV4_gc;
	ADC0.CTRLC = ADC_REFSEL_2500MV_gc;
}

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

float get_Voltage(int pin){
	float lectura = 0;
	float Voltage = 0;
	int a = 0;
	PORTB.DIR |= PIN5_bm;
	PORTB.OUT |= PIN5_bm;
	_delay_ms(700);
	switch (pin){
		case 10:
			ADC0.CTRLA = ADC_ENABLE_bm;
			ADC0.MUXPOS = ADC_MUXPOS_AIN9_gc;
			break;
		case 11:
			ADC0.CTRLA = ADC_ENABLE_bm;
			ADC0.MUXPOS = ADC_MUXPOS_AIN5_gc;
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
	PORTB.OUT &= ~PIN5_bm;
	return Voltage;
}

float get_Presure(float vol){
	float Pressure;
	if(vol <=  0.495){
		//vol = 0.0;
		Pressure = 0.0;
	}
	else{
		//vol = (8.9775485 * vol - 4.4092)*10.2;
		Pressure = (8.9775485 * vol - 4.4092)*10.2;
	}
	//return vol;
	return Pressure;	
}

static void executeCommand(char *command)
{	
	Flotante presion1;
	Flotante presion2;
	Flotante bateria;
	
	unsigned long cont = 0;
	/*float P1;
	float P2;
	float B1;*/
	
	/*B1 = get_Voltage(6)*5;
	P1 = get_Presure(get_Voltage(10));
	P2 = get_Presure(get_Voltage(11));*/
	
	presion1 = get_Float(get_Presure(get_Voltage(10)));
	presion2 = get_Float(get_Presure(get_Voltage(11)));
	bateria = get_Float(get_Voltage(6)*5);
		
	/*presion1 = get_Float(P1);
	presion2 = get_Float(P2);
	bateria = get_Float(B1);*/
	cont = ReportCount();
	
	if(strcmp(command,"MS\r") == 0){
		printf("RP1%d.%d%dRP2%d.%d%dRQ1%lu.0RV1%d.%d%d\r",presion1.entero,presion1.decimal1,presion1.decimal2,presion2.entero,presion2.decimal1,presion2.decimal2,cont,bateria.entero,bateria.decimal1,bateria.decimal2);
	}
	else{
		USART0_sendString("Incorrect Command\r");
	}
}

int main(void){
	CLOCK32K_init();
	ADC0_init();
	Pin_init();
	USART0_init();
	/**(uint8_t *)(5375) = 0;
	CPU_CCP = CCP_SPM_gc;
	NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
	for(int i = 5120; i <= 5373; i++){
		*(uint8_t *)(i) = 0x00;
		CPU_CCP = CCP_SPM_gc;
		NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
	}
	*(uint8_t *)(5374) = 0x00;
	CPU_CCP = CCP_SPM_gc;
	NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;*/
	sei();
	while (1){
		;
	}
}

//************** Section Interruptions **************

ISR(USART0_RXC_vect) { 
	_delay_ms(50);
	c = USART0_RXDATAL;
	if(c != 0 && c != '\r')
	{
		command[index1] = c;
		index1++;
		if(index1 > MAX_COMMAND_LEN)
		{
			index1 = 0;
		}
	}
	if(c == '\r')
	{
		command[index1] = '\r';
		index1 = 0;
		
		executeCommand(command);
	}
}

ISR (PORTC_PORT_vect){
	//Capacitor 20ms ReadSwitch
	//Normal Seametrics
	//_delay_ms(20);
	PORTC_INTFLAGS |= PIN3_bm;
	count++;
	printf("%u\r", count);
	uint8_t aux = 0;
	uint8_t ant = 0;
	if(*(uint8_t*)(5375) == 0xFF){
		;
	}
	else{
		for(int i = 5120; i <= 5374; i++){
			ant = aux;
			aux = *(uint8_t*)(i);
			if(aux == 0 && i == 5120){
				*(uint8_t*)(5120) = aux+1;
				CPU_CCP = CCP_SPM_gc;
				NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
				break;
			}
			if(aux < ant){
				*(uint8_t*)(i) = aux+1;
				CPU_CCP = CCP_SPM_gc;
				NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
				break;
			}
			if(i == 5374 && aux == 0xFF){
				*(uint8_t*)(5375) = *(uint8_t*)(5375)+1;
				CPU_CCP = CCP_SPM_gc;
				NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
				for(int i=5120; i<=5374; i++){
					*(uint8_t *)(i) = 0x00;
					CPU_CCP = CCP_SPM_gc;
					NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
				}
			}
			if(i == 5374 && ant == aux){
				*(uint8_t*)(5120) = aux+1;
				CPU_CCP = CCP_SPM_gc;
				NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
			}
		}
	}
	// ********** EEPROM COUNTER *************
	/*if(index == 5247){
		index = 5120;
		int help = *(uint8_t *)(index);
		help = help + 1;
		*(uint8_t *)(index) = help;
		CPU_CCP = CCP_SPM_gc;
		NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
		index++;
	}
	else{
		int help = *(uint8_t *)(index);
		help = help + 1;
		*(uint8_t *)(index) = help;
		CPU_CCP = CCP_SPM_gc;
		NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
		index++;	
	}
	if(*(uint8_t *)(5246) == 0xFF){
		for(int i=5120; i<=5246; i++){
			*(uint8_t *)(i) = 0;
			CPU_CCP = CCP_SPM_gc;
			NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
		}
		int help2 = *(uint8_t *)(5247);
		help2 = help2 + 1;
		*(uint8_t *)(5247) = help2;
		CPU_CCP = CCP_SPM_gc;
		NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
	}*/
}
