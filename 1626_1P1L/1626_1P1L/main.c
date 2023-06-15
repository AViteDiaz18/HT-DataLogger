/*
 * 1626_1P1L.c
 *
 * Created: 24/05/2023 11:16:12 a. m.
 * Author : H-Tech
 */ 

#define USART0_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <stdio.h>

#define MAX_COMMAND_LEN 50

unsigned int count = 0;
char command[MAX_COMMAND_LEN] = {' '};
uint8_t index1 = 0;
char c;

//*********** Section USART ************* 
static void USART0_sendChar(char c){
	while (!(USART0.STATUS & USART_DREIF_bm)){
		;
	}
	USART0.TXDATAL = c;
}

static int USART0_printChar(char c, FILE *stream){
	USART0_sendChar(c);
	return 0;
}

uint8_t USART0_readChar()
{
	while (!(USART0.STATUS & USART_RXCIF_bm))
	{
		;
	}
	return USART0.RXDATAL;
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
//************* Section Low Power **************
static void Sleep_Micro(){
	set_sleep_mode(SLEEP_MODE_IDLE);
	cli();
	sleep_enable();
	
	sei();
	sleep_cpu();
	sleep_disable();
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

//***************** Section ADC *****************

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
	return Voltage;
}

typedef union Float_Byte{
	float f;
	unsigned char dato[4];
}Flotante;

float get_Presure2(float vol){
	
	//       HLV004
	Flotante Zero2;
	Zero2.dato[0] = *(uint8_t*)(5364);
	Zero2.dato[1] = *(uint8_t*)(5365);
	Zero2.dato[2] = *(uint8_t*)(5366);
	Zero2.dato[3] = *(uint8_t*)(5367);
	Flotante A2;
	A2.dato[0] = *(uint8_t*)(5368);
	A2.dato[1] = *(uint8_t*)(5369);
	A2.dato[2] = *(uint8_t*)(5370);
	A2.dato[3] = *(uint8_t*)(5371);
	Flotante B2;
	B2.dato[0] = *(uint8_t*)(5372);
	B2.dato[1] = *(uint8_t*)(5373);
	B2.dato[2] = *(uint8_t*)(5374);
	B2.dato[3] = *(uint8_t*)(5375);

	float Pressure = 0;
	if(vol <=  Zero2.f){
		
		//vol = 0.0;
		Pressure = 0.0;
	}
	else{
		Pressure = (A2.f * vol - B2.f)*10.2;
	}
	//return vol;
	return Pressure;
	
	//***************************************************
}

static void executeCommand(char *command){
	if(strstr(command,"CN")!= NULL){
		Flotante Z1,Z2,A,B,C,D;
		char *token = strtok(command, " ");
		int i = 0;
		if(token != NULL){
			while(token != NULL){
				switch(i){
					case 0:
					break;
					case 1:
					sscanf(token,"%f",&C.f);
					break;
					case 2:
					sscanf(token,"%f",&D.f);
					break;
				}
				token = strtok(NULL, " ");
				i++;
			}
		}
		Z2.f = D.f/C.f;
		wdt_reset();
		for(int i=0; i<=3; i++){
			*(uint8_t *)(i+5364) = Z2.dato[i];
			CPU_CCP = CCP_SPM_gc;
			NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
			*(uint8_t *)(i+5368) = C.dato[i];
			CPU_CCP = CCP_SPM_gc;
			NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
			*(uint8_t *)(i+5372) = D.dato[i];
			CPU_CCP = CCP_SPM_gc;
			NVMCTRL.CTRLA = NVMCTRL_CMD_PAGEERASEWRITE_gc;
		}
		printf("CN\r");
	}
	else{
		if(strcmp(command,"MS\r") == 0){
			float vol1;
			float vol2;
			float presion1;
			float presion2;
			float bateria;
			
			//unsigned long cont = 0;
			
			//PORTB.DIR |= PIN5_bm;
			//PORTB.PIN5CTRL &= ~PIN5_bm;
			PORTB.OUT |= PIN5_bm;
			_delay_ms(5000);//5 segundos
			wdt_reset();
			vol2 = get_Voltage(10); //2 segundos
			wdt_reset();
			//printf("Voltaje2: %f\r",vol2);
			//presion1 = get_Presure1(vol1);
			presion2 = get_Presure2(vol2);
			
			PORTB.OUT &= ~PIN5_bm;
			bateria = get_Voltage(6)*5; //2 segundos
			wdt_reset();
			
			printf("RH%.2fRL%dRVO%.2f\r",presion2,count,bateria);
			count = 0;
		}
		else{
			printf("Incorrect Command\r");
		}
	}
}

int main(void)
{
    int raw_press = 0;
	CLOCK32K_init();
	PORT_LOWPOWER_Init();
	ADC0_init();
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
	
	//Bateria
	PORTA.PIN6CTRL &= ~PORT_ISC_gm;
	PORTA.PIN6CTRL |= PORT_ISC_INPUT_DISABLE_gc;
	PORTA.PIN6CTRL &= ~PORT_PULLUPEN_bm;
	
	//Activador Fuente 12v
	PORTB.DIR |= PIN5_bm;
	PORTB.PIN5CTRL &= ~PORT_PULLUPEN_bm;
	PORTB.OUT &= ~PIN5_bm;
	
	CPU_CCP = CCP_IOREG_gc;
	WDT.CTRLA = WDT_PERIOD_8KCLK_gc;
	memset(command, 0, MAX_COMMAND_LEN);	
	
	USART0_init();
	
	sei();
	
	
	//printf("Ayuda\r");
    /* Replace with your application code */
    while (1) 
    {
		ADC0.CTRLA = (0 << ADC_ENABLE_bp);
		//USART0.CTRLB = (0 << USART_TXEN_bp);
		TCB0.CTRLA = 0;
		TCA0.SPLIT.CTRLA = 0;
		RTC.CTRLA = 0;
		//PORTB.OUT &= ~PIN5_bm;
		Sleep_Micro();
		wdt_reset();
    }
}

ISR(USART0_RXC_vect) {
	//_delay_ms(50);
	c = USART0_readChar();
	//	printf("C: %c\r", c);
	if(c != '\0' && c != '\r')
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
		//printf("Command %s \r", command);
		command[index1] = '\r';
		index1 = 0;
		wdt_reset();
		executeCommand(command);
		memset(command, 0, MAX_COMMAND_LEN);
	}
}

ISR (PORTC_PORT_vect){
	count++;
	//executeCommand("MS\r");
	//memset(command, 0, MAX_COMMAND_LEN);
	PORTC_INTFLAGS |= PIN3_bm;
	
	//count++;
	//printf("%u\r", count);
}

