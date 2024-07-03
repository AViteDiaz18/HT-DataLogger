/**
 * @file USART.h
 * @author Htech Mexico
 * @brief Libreria para el manejo de comunicacion serial
 * @version 1.0
 * @date 24-05-2023
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef USART_H_
#define USART_H_
/**
 * @brief Constante del calculo para el BAUD RATE del USART
 *
 */
#define USART0_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

#include <avr/io.h>
#include <stdio.h>

/**
 * @brief Constante de tamaño maximo de buffer de comandos
 *
 */

#define MAX_COMMAND_LEN 50

/**
 * @brief Cadena de caracteres para el comando a procesar
 *
 */
char command[MAX_COMMAND_LEN] = {' '};
/**
 * @brief Cadena de caracteres para la primera posicion de la pila de comandos
 *
 */
char command1[MAX_COMMAND_LEN] = {'\0'};
/**
 * @brief Cadena de caracteres para la segunda posicion de la pila de comandos
 *
 */
char command2[MAX_COMMAND_LEN] = {'\0'};
/**
 * @brief Cadena de caracteres para la tercera posicion de la pila de comandos
 *
 */
char command3[MAX_COMMAND_LEN] = {'\0'};
/**
 * @brief Entero que muestra la posicion actual del buffer de comandos
 *
 */
uint8_t index1 = 0;
/**
 * @brief Caracter que aloja la entrada de RXDATAL
 *
 */
char c;
/**
 * @brief Bandera para el procesamiento de la pila
 *
 */
int flag = 0;


/**
* @brief Funcion para el envio de caracteres de manera serial
*
* @param C Caracter que para el envio de datos a TX 
*
*/
void USART0_sendChar(char c){
	while (!(USART0.STATUS & USART_DREIF_bm)){
		;
	}
	USART0.TXDATAL = c;
}

/**
* @brief Funcion para el envio de caracteres de manera serial
*
* @param C Caracter que para el envio de datos a TX
*
* @param stream Archivo para la utilizacion de la funcion printf para enviar
*
*/
int USART0_printChar(char c, FILE *stream){
	USART0_sendChar(c);
	return 0;
}

/**
* @brief Funcion para la recepcion de datos de manera serial
*
*
*/
uint8_t USART0_readChar()
{
	while (!(USART0.STATUS & USART_RXCIF_bm))
	{
		;
	}
	return USART0.RXDATAL;
}

FILE USART_stream = FDEV_SETUP_STREAM(USART0_printChar, NULL, _FDEV_SETUP_WRITE);

/**
* @brief Funcion para la inicializacion de la comunicacion serial
*
*
*/
void USART0_init(){
	USART0.BAUD = (uint16_t)USART0_BAUD_RATE(1200);
	USART0.CTRLB = USART_RXEN_bm | USART_TXEN_bm;
	USART0_CTRLA = (1<<USART_RXCIE_bp)|(0<<USART_TXCIE_bp);
	PORTMUX.USARTROUTEA = PORTMUX_USART0_ALT1_gc;
	stdout = &USART_stream;
}

/**
* @brief Funcion para eliminar el comando por procesar
*
* @param comando Variable a eliminar
*
*/
void eraseCommand(char *comando){
	for(int i = 0; i < MAX_COMMAND_LEN; i++){
		comando[i] = '\0';
	}
}

/**
* @brief Funcion para la activacion de interrupciones del tipo UART
*
* @param USART0_RXC_vect Pila de interrupciones USART0
*
*/
ISR(USART0_RXC_vect) {
	//_delay_ms(50);
	//_delay_us(50);
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
		//
		if(strcmp(command1,"\0") == 0 && strcmp(command2,"\0") == 0 && strcmp(command3,"\0") == 0){
			//memset(command1, 0, MAX_COMMAND_LEN);
			strncpy(command1,command,MAX_COMMAND_LEN);
		}
		else{
			if(strcmp(command1,"\0") != 0 && strcmp(command2,"\0") == 0 && strcmp(command3,"\0") == 0){
				//memset(command2, 0, MAX_COMMAND_LEN);
				strncpy(command2,command,MAX_COMMAND_LEN);
			}
			else{
				if(strcmp(command1,"\0") != 0 && strcmp(command2,"\0") != 0 && strcmp(command3,"\0") == 0){
					//memset(command3, 0, MAX_COMMAND_LEN);
					strncpy(command3,command,MAX_COMMAND_LEN);
				}
			}
		}
		memset(command, 0, MAX_COMMAND_LEN);
		/*printf("Comando1: %s, Comando2 %s, Comando3 %s\r",command1,command2,command3);*/
		//wdt_reset();
		flag = 1;
	}
}


#endif /* USART_H_ */