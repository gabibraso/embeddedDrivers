/*
* main.c
*
* Created: 22.05.2018
* Author : Bogdan Tarnauca
*/

//Applications must include an "main_config.h" header where libraries related params, overrides, settings should be defined.
//This includes F_CPU, the target, compiler, libs.
//This header file is application specific. Must be provided even if it is empty, all ABT AVR libs include it
#include "main_config.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include<avr/wdt.h>
#include<avr/interrupt.h>

#include "main_defines.h"
//Application related defines and variables

#include "avr_usart.h"
//USART driver from the libabr_uart library

#include "main_func.h"
//Application functions

#include "main_MCU_init.h"
//MCU initialization (ports, UART, etc.)

int count = 0;
char str[3];
bool flag = true;

int main(void)
{
	mainCycleCount = 0;

	//Initialize MCU
	init_devices();

	//Print reset source.
	if(MCUCSR & 0x01)
	uart1_Transmite("\n\rPOWER ON!",11, true);
	WDR();
	if(MCUCSR & 0x02)
	uart1_Transmite("\n\rEXTERNAL!",11, true);
	WDR();
	if(MCUCSR & 0x04)
	uart1_Transmite("\n\rBROUN OUT!",12, true);
	WDR();
	if(MCUCSR & 0x08)
	uart1_Transmite("\n\rWATCH DOG!",12, true);
	WDR();
	if(MCUCSR & 0x10)
	uart1_Transmite("\n\rJTAG!",7, true);
	WDR();
	uart1_Transmite("\n\r",2, true);
	MCUCSR = 0;

	uart1_bReceptioneaza = false;

	// Initializare porturi LED-uri
	DDRB  = 0xFF;
	PORTB = 0x00;
	// Initializare porturi Butoane
	DDRE  = (0<<DDRE4) | (0<<DDRE5) | (0<<DDRE6);
	PORTE = (1<<PORTE4) | (1<<PORTE5) | (1<<PORTE6);
	// Setare pin intrerupere
	EICRB = (1<<ISC41) | (0<<ISC40) | (1<<ISC51) | (0<<ISC50) | (1<<ISC61) | (0<<ISC60);
	EIMSK = (1<<INT4) | (1<<INT5) |(1<<INT6);
	

	
	TCCR1A = (0<<WGM11) | (0<<WGM10); //set mod CTC
	TCCR1B = (0<<WGM13) | (1<<WGM12) | (0<<CS12) | (0<<CS11) | (0<<CS10); // 256
	TCNT1 = 0;				
	OCR1A = 28800;			//1s
	TIMSK = (1<<OCIE1A);
	
	TCCR0 = (1<<WGM01) | (0<<WGM00)| (0<<CS02) | (0<<CS01) | (0<<CS00);
	TCNT0 = 0;
	OCR0 = 255;
	TIMSK |= (1<<OCIE0);
	
	sei();
	

	
	do
	{
		//Reset watchdog to prevent a reset
		wdt_reset();
		
		//If there is no pending UART operation start listening
		if(!uart1_bReceptioneaza && !uart1_bTransmite && !uart1_bStareNeprocesata)
		{
			//Listen for characters until the terminal character is received
			//(carriage return in this case) or the maximum number of chars
			//is received (to prevent buffer over flow)
			//Received data is stored in the snTelegramaRx input buffer.
			//The input buffer is reset (index set to 0) before Rx is initiated
			//Once the terminal char is received, receiving further characters
			//is blocked. Any other chars will be discarded
			uart1_ReceptieTermTX(snTelegramaRx, RX_TERM, L_BUF_RX, true, true);
		}

		
		//Periodically evaluate the status of a message Rx
		//Checking the Rx complete condition in a non-ISR function is meant to keep
		//the ISR as short as possible.
		uart1_ReceptieOK();
		wdt_reset();

		//Check if the UART unprocessed flag has been set
		if(uart1_bStareNeprocesata)
		{
			//An TxOK, RxOK, Timeout of BufferLimit condition has been detected
			//We don't test for timeout as we have configured the UART in slave mode

			if(uart1_bRXBufferLimit)
			{
				//ToDo: Do some error processing for buffer limit.
				//Reset the uart flags
				uart1_bReceptioneaza = false;
				uart1_bRXBufferLimit = false;
				uart1_nContorRX = 0;
				uart1_bStareNeprocesata = false;
				uart1_bRxOK = false;
				wdt_reset();
			}
			
			if(uart1_bRxOK)
			{
				//Copy the received data from the Rx buffer into the Tx buffer
				//and send the echo.
				memcpy(snTelegramaTx, snTelegramaRx, uart1_nLungimeRX);
				//add new line and carriage return
				memcpy(snTelegramaTx + uart1_nLungimeRX, sNewLine, 3);
				
				//Commence the transmission
				//If a transmission is already ongoing block here until that is finished
				//and then set the current transmission.
				//Note that the transmission function is otherwise not blocking.
				uart1_TransmiteExt(snTelegramaTx , strlen(snTelegramaTx), true, false);
			}
			
			//reset the flag to prevent re-processing of the same conditions
			uart1_bStareNeprocesata = false;
		}
		
		itoa(count,str,10);
		uart1_Transmite("\n\rCounter: ",11, true);
		uart1_Transmite(str,4, true);
	}
	
	while(1);
}



ISR(INT4_vect)
{
	count++;
}

ISR(INT5_vect)
{
	TCNT1 = 0;
	TCCR1B =  (0<<WGM13) | (1<<WGM12) | (1<<CS12) | (0<<CS11) | (0<<CS10);
}



ISR(TIMER1_COMPA_vect)
{
	if((PINE & (1<<PINE5)) == 0)
	{
		count++;
	}
	
	TCCR1B = (0<<WGM13) | (1<<WGM12) | (0<<CS12) | (0<<CS11) | (0<<CS10);
}

