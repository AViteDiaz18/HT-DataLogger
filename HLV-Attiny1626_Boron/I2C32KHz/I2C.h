/**
 * @file I2C.h
 * @author Htech Mexico
 * @brief Libreria para el manejo del protocolo I2C
 * @version 1.0
 * @date 05-07-2023
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef I2C_H_
#define I2C_H_
/**
 * @brief Constante del calculo para el reloj de I2C
 *
 */
#define TWI_BAUD ((((float) F_CPU / (float) 20000) - 10)/2)

#include <avr/io.h>

/**
 * @brief Definicion del tipo status de I2C
 *
 */
typedef enum {
	TWI_INIT = 0,
	TWI_READY,
	TWI_ERROR
} TWI_Status;

/**
 * @brief Funcion para la obtencion del Status de I2C
 *
 */
TWI_Status TWI_GetStatus(void)
{
	TWI_Status state = TWI_INIT;
	do
	{
		if (TWI0.MSTATUS & (TWI_WIF_bm | TWI_RIF_bm))
		{
			state = TWI_READY;
		}
		else if (TWI0.MSTATUS & (TWI_BUSERR_bm | TWI_ARBLOST_bm))
		{
			/* get here only in case of bus error or arbitration lost */
			state = TWI_ERROR;
		}
	} while (!state);

	return state;
}

/**
 * @brief Funcion que espera el ACK en el protocolo I2C
 *
 */
uint8_t RX_acked(void)
{
	// Actual status of the line O means ACK - 1 means NACK. Therefore ! MSTATUS bit
	return (!(TWI0.MSTATUS & TWI_RXACK_bm));
}

/**
 * @brief Funcion que obtiene los datos de sensado de presion para los sensores MSP300 y MS32
 *
 */
int get_I2CValue(int verbose){
	_delay_ms(500);
	int P_high = 0;
	int P_low = 0;
	int Press;
	
	
	//printf("**************************\r");
	
	TWI0.MBAUD = TWI_BAUD;
	
	TWI0.MCTRLA = (1 << TWI_ENABLE_bp) | (1 << TWI_SMEN_bp);
	//TWI0.MCTRLA = TWI_ENABLE_bm | TWI_SMEN_bm;
	
	//TWI0.MSTATUS = 0x00;
	TWI0.MADDR = 0x00;
	
	TWI0.MDATA = 0x00;
	TWI0.MCTRLB = TWI_FLUSH_bm;
	TWI0.MSTATUS |= (TWI_RIF_bm | TWI_WIF_bm);
	TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;
	
	TWI0.MADDR = 0x51;
	while(!(TWI0.MSTATUS & TWI_RXACK_bm) != 1);
	TWI0.MCTRLB = TWI_MCMD_STOP_gc;
	if (verbose == 1){
		printf("Inicia no Add: %x, Status %d\n", TWI0_MDATA, TWI0_MSTATUS);
	}
	else{
		_delay_ms(20);
	}	
	
	//printf("Inicia no Add: %x, Status %d\r", TWI0_MDATA, TWI0_MSTATUS);
	//_delay_ms(50);
	TWI0.MCTRLB = TWI_FLUSH_bm;
	TWI0.MSTATUS |= (TWI_RIF_bm | TWI_WIF_bm);
	if (verbose == 1){
		//_delay_ms(30);
		//printf("Inicia no Add: %x, Status %d\r", TWI0_MDATA, TWI0_MSTATUS);
	}
	else{
		_delay_ms(50);
	}
	TWI0.MADDR = 0x51;
	
	//printf("Inicia no Add: %x, Status %d\r", TWI0_MDATA, TWI0_MSTATUS);
	//_delay_ms(210);
	
	
	if(TWI_GetStatus() == TWI_READY){//&& RX_acked()){
		
		P_high = TWI0.MDATA;
		
		if (verbose == 1){
			printf("Phigh: %x, Status %d\n", P_high, TWI0_MSTATUS);
		}
		else{
			_delay_ms(50);
		}
		
		TWI0.MCTRLB = (0 << TWI_ACKACT_bp) | TWI_MCMD_STOP_gc;
		P_low = TWI0.MDATA;
		
		if (verbose == 1){
			printf("Plow: %x, Status %d\n", P_low, TWI0_MSTATUS);
		}
		else{
			_delay_ms(50);
		}
	}
	
	TWI0.MCTRLA &= ~TWI_ENABLE_bm;
	
	
	//sensor chico
	Press = (P_high << 8) | P_low ;
	
	if (verbose == 1){
		printf("Presion en RAW: %d\n", Press);
	}
	
	return Press;
}

#endif /* I2C_H_ */