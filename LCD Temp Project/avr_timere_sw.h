/*
* avr_timere_sw.h
*
* Created: 01.01.2009
* Author : Bogdan Tarnauca
*/

#ifndef _AVR_TIMERE_SW_H_
	#define _AVR_TIMERE_SW_H_


//Apps must include an "main_config.h" header where libraries related params, overrides, settings should be defined.
//This header file is application specific. Must be provided even it is empty.
#include "main_config.h"

#include "avr_defines.h"
#include "avr_utilitare.h"

#define TIMER_SW_NUMAR_TIMERE 20
#define EROARE_TIMERE_INDISPONIBILE -1
#define EROARE_TIMER_NECREAT -2
#define EROARE_ID_TIMER_INVALID -3
#define null 0

#define TIMER_SW_PERIOADA_TICK 10
            //perioda timererului in ms

void (*TimerInterruptExtendedVector)(void);


volatile unsigned char nTimereCreate;
volatile unsigned char bTimereHWInterupt;



typedef enum
{
	timerPornit,
	timerOprit,
	timerExpirat
} TenumStareTimer;

typedef struct
{
	TenumStareTimer nStareTimer;

	unsigned int nContorInitial;
	unsigned char bTimerUtilizat;
	unsigned int nContorCurent;
	unsigned char bAutoRestart;
	void (*timer_callback)(void);
} TstructTimerControl;


TstructTimerControl sstrTimere[TIMER_SW_NUMAR_TIMERE];


//TIMER2 initialize - prescale:1024
// WGM: Normal
// desired value: 10mSec
// actual value:  9,908mSec (0,9%)
//Settings for a 11.05920 MHz Crystal

void timer0_init(void);

#ifdef __ICCAVR
#pragma interrupt_handler timer0_compa_isr:TIMER0_COMPA_ISR_VECTOR_NO
void timer0_compa_isr(void);
#else
#if (defined __ATMEGA2560)
ISR(TIMER0_COMPA_vect);
#elif (defined __ATMEGA128)
ISR(TIMER0_COMP_vect);
#elif (defined __ATMEGA328)
ISR(TIMER0_COMPA_vect);
#else
#warning Check interrupt vector name for your device
ISR(TIMER0_COMP_vect);
#endif

#endif



void timerSW_InitTimerHW();


//functia returneaza un handler la timerul creat sau un indicator de eroare
unsigned int timerSW_Creaza(void (*pfFunctieCallback)(void), unsigned int nValoareInitialaContor, TenumStareTimer nStareInitialaTimer, unsigned char bAutoRestart);

unsigned int timerSW_Distruge(unsigned int nIDTimer);



unsigned int timerSW_Evalueaza();

int timerSW_Restarteaza(unsigned int nIDTimer);

int timerSW_Opreste(unsigned int nIDTimer);

int timerSW_Forteaza(unsigned int nIDTimer);

int timerSW_Continua(unsigned int nIDTimer);

#endif /* _AVR_TIMERE_SW_H_ */
