/*
* avr_timere_sw.h
*
* Created: 01.01.2009
* Author : Bogdan Tarnauca
*/

#include "avr_timere_sw.h"

volatile unsigned char nTimereCreate = 0;
volatile unsigned char bTimereHWInterupt = 0;

void timer0_init(void)
{
#if defined __ATMEGA2560
	TCCR0A = 0x00; //stop
	TCCR0B = 0x00; //stop
	
	//Finterrpt= Fclk/(N*(1+OCRx))
	// N = prescale value
	//pentru CS22,CS21,CS10 = 111 => N = 1024
	//
	//OCRx = Fclk/(Fint*N) - 1
	//OCRx = Fclk*Tint/N - 1


	TCNT0 = 0x00; 
  
	#if (F_CPU == 11059200UL)
	    OCR0A  = 0x6B;	//for Fclck = 11.059.264 Hz
	#elif (F_CPU == 16000000UL)
	    OCR0A  = 0x9B;	//for Fclck = 16.000.000 Hz
	#elif (F_CPU == 7372800UL)
	    OCR0A  = 0x47;	//for Fclck = 7.372.800 Hz        
	#else
		#warning !!!! F_CPU was not defined or is not supported by the USART driver. User "#define F_CPU 11059000UL" or "#define F_CPU 16000000UL". Note that a different real CPU frequency will affect the 10ms timebase set by Timer 2 for the timeout mechanims. Timer settings set for a default F_CPU 11059200UL 
		OCR0A  = 0x6B;	//set as for Fclck = 11.059.264 Hz even if a wrong value has been set
	#endif
	
	
	TCCR0B = 1<<CS00 | 1<<CS02; //CS22,CS21,CS10 = 111 => 1024 Fclk divider
	TCCR0A = 1<<WGM01; //WGM22,WGM21,WGM20 = 010 start as CTC. Interrupt when TCNT2 reaches OCR2A		
	TIMSK0 |= 1<<OCIE0A; //
	
#elif defined __ATMEGA128
	TCCR0 = 0x00; //stop timer
	TCNT0 = 0x00; 
	
	#if (F_CPU == 11059200UL)
	    OCR0  = 0x6B;	//for Fclck = 11.059.264 Hz
	#elif (F_CPU == 16000000UL)
	    OCR0  = 0x9B;	//for Fclck = 16.000.000 Hz
	#elif (F_CPU == 7372800UL)
	    OCR0  = 0x47;	//for Fclck = 7.372.800 Hz
	#else
	#warning !!!! F_CPU was not defined or is not supported by the USART driver. User "#define F_CPU 11059000UL" or "#define F_CPU 16000000UL". Note that a different real CPU frequency will affect the 10ms timebase set by Timer 2 for the timeout mechanims. Timer settings set for a default F_CPU 11059200UL
	    OCR0  = 0x6B;	//set as for Fclck = 11.059.264 Hz even if a wrong value has been set
	#endif
	
	TCCR0 = 1<<WGM01 |  1<<CS00  |  1<<CS01  | 1<<CS02;
		//CS02, CS00 = 101 => 1024 Fclk divider
		//WGM01 = 10 start as CTC. Interrupt when TCNT2 reaches OCR2A
	
	TIMSK |= 1<<OCIE0;	//Enable Timer2 CTC interrupt
	
#elif defined __ATMEGA328
	TCCR0A = 0x00; //stop
	TCCR0B = 0x00; //stop

	//Finterrpt= Fclk/(2*N*(1+OCRx)
	// N = prescale value

	TCNT0 = 0x00;

	#if (F_CPU == 11059200UL)
	    OCR0A  = 0x6B;	//for Fclck = 11.059.264 Hz
	#elif (F_CPU == 16000000UL)
	    OCR0A  = 0x9B;	//for Fclck = 16.000.000 Hz
	#elif (F_CPU == 7372800UL)
	    OCR0A  = 0x47;	//for Fclck = 7.372.800 Hz
	#else
	#warning !!!! F_CPU was not defined or is not supported by the USART driver. User "#define F_CPU 11059000UL" or "#define F_CPU 16000000UL". Note that a different real CPU frequency will affect the 10ms timebase set by Timer 2 for the timeout mechanims. Timer settings set for a default F_CPU 11059200UL
	    OCR0A  = 0x6B;	//set as for Fclck = 11.059.264 Hz even if a wrong value has been set
	#endif


	TCCR0B = 1<<CS00 | 1<<CS02; //CS22,CS21,CS10 = 111 => 1024 Fclk divider
	TCCR0A = 1<<WGM01; //WGM22,WGM21,WGM20 = 010 start as CTC. Interrupt when TCNT2 reaches OCR2A
	TIMSK0 |= 1<<OCIE0A; //
	
#endif
	
	
	/*
#warning - SW Timers timer initialization need refactoring to properly support/test multiple targets.
 #ifdef __ATMEGA128
     TCCR0 = 0x00; //stop
     ASSR = 0x00; //set async mode
     TCNT0 = 0x95; //setup
     OCR0  = 0x6B;
     TCCR0 = 0x07; //start
 #else
     TCCR0B = 0x00; //stop
     TCNT0 = 0x95; //set count
     TCCR0A = 0x00; 
     TCCR0B = 0x05; //start timer
 #endif*/
}

#ifdef __ICCAVR
#pragma interrupt_handler timer0_compa_isr:TIMER0_COMPA_ISR_VECTOR_NO
void timer0_compa_isr(void)
#else
#if (defined __ATMEGA2560)	
ISR(TIMER0_COMPA_vect)
#elif (defined __ATMEGA128)	
ISR(TIMER0_COMP_vect)
#elif (defined __ATMEGA328)	
ISR(TIMER0_COMPA_vect)
#else
#warning Check interrupt vector name for your device
ISR(TIMER0_COMP_vect)
#endif

#endif
{
	unsigned char i;
	TCNT0 = 0x00; //reload counter value 10,000 ms

	bTimereHWInterupt = true;

	//Daca am definit un handler aditionat pentru intreruperea acestui timer HW
	//pe langa functionalitatea timerelor SW, atunci codul va fi apelat aici
	//!!! Atentie: de folosit doar pentru operatii minore neconsumatoare de timp
	//si cu grija :-) in cazul in care doresc tratarea chiar cand intrerupere HW
	//a fost detectata
	if(TimerInterruptExtendedVector != null)
	{
		TimerInterruptExtendedVector();
	}
	//Decrementarea contoarelor timerelor SW
	for(i = 0; i<TIMER_SW_NUMAR_TIMERE; i++)
	{
		//decrementez si testez daca timerul a expirat
		if(sstrTimere[i].bTimerUtilizat && sstrTimere[i].nStareTimer == timerPornit)
		{
			if(sstrTimere[i].nContorCurent > 0)
			{
				sstrTimere[i].nContorCurent--;			
			}
			
			if(sstrTimere[i].nContorCurent == 0)
			{
				//indica expirarea timerului
				sstrTimere[i].nStareTimer = timerExpirat;
			}
		}
	}
}


void timerSW_InitTimerHW()
{
	//-- poate TIMER1_OVF_vect transmis ca parametru astfel incat sa nu fim dependenti
	//in headfer de timer.
	///eventual cu remarca ca setariele temporizare a timerului sa se faca 
	//extern headerului. am elimina orice problema legata de utilizarea unui timer
	//indisponibil pentru o anumita aplicatie.
	timer0_init();
}


//functia returneaza un handler la timerul creat sau un indicator de eroare
unsigned int timerSW_Creaza(void (*pfFunctieCallback)(void), unsigned int nValoareInitialaContor, TenumStareTimer nStareInitialaTimer, unsigned char bAutoRestart)
{
	unsigned int i;
	if(nTimereCreate < TIMER_SW_NUMAR_TIMERE)
	{
		for(i = 0; i<TIMER_SW_NUMAR_TIMERE; i++)
		{
			if(!sstrTimere[i].bTimerUtilizat)
			{
				sstrTimere[i].timer_callback = pfFunctieCallback;
				sstrTimere[i].nContorInitial = nValoareInitialaContor /TIMER_SW_PERIOADA_TICK;
				sstrTimere[i].nContorCurent = nValoareInitialaContor /TIMER_SW_PERIOADA_TICK;
				sstrTimere[i].bAutoRestart = bAutoRestart;
				sstrTimere[i].nStareTimer = nStareInitialaTimer;
				sstrTimere[i].bTimerUtilizat = true;
				nTimereCreate++;
				return i;
			}
		}
	}
		return EROARE_TIMERE_INDISPONIBILE;

}

unsigned int timerSW_Distruge(unsigned int nIDTimer)
{
	if(nIDTimer >= TIMER_SW_NUMAR_TIMERE)
	{
		return EROARE_ID_TIMER_INVALID;
	} 

	if(sstrTimere[nIDTimer].bTimerUtilizat)
	{	
		sstrTimere[nIDTimer].nStareTimer = timerOprit;
		sstrTimere[nIDTimer].bTimerUtilizat = false;	
		nTimereCreate--;
		return nTimereCreate; 
	}
	else
	{
		return EROARE_TIMER_NECREAT;
	}
}

unsigned int timerSW_Evalueaza()
{
	unsigned int i;
	unsigned char nTimereActive = 0; //numarul timerelor care nu sunt create si sunt timer_Pornit
	
	if(bTimereHWInterupt)
	{
		for(i = 0; i<TIMER_SW_NUMAR_TIMERE; i++)
		{
			WDR();
			if(sstrTimere[i].bTimerUtilizat)
			{
				//fac o contorizare a timerelor care inca mai ruleaza si nu au expirat inca
				if(sstrTimere[i].nStareTimer == timerPornit)
				{
					nTimereActive++;	//Timerele expirate sau cele fara AutoRestart sunt considerate inactive
				}
				
				//Daca a expirat, apelez callback-ul asociat
				if(sstrTimere[i].nStareTimer == timerExpirat)
				{
					sstrTimere[i].timer_callback();				
					if(sstrTimere[i].bAutoRestart)
					{
						sstrTimere[i].nContorCurent = sstrTimere[i].nContorInitial;
						sstrTimere[i].nStareTimer = timerPornit;
						nTimereActive++;		//Un timer cu autorestart este considerat activ
					}
					else
					{
						sstrTimere[i].nStareTimer = timerOprit;
					}
				}
				
			}
		}
	}
	bTimereHWInterupt = false;
	return nTimereActive;	
}

int timerSW_Restarteaza(unsigned int nIDTimer)
{
	if(nIDTimer >= TIMER_SW_NUMAR_TIMERE)
	{
		return EROARE_ID_TIMER_INVALID;
	} 

	if(sstrTimere[nIDTimer].bTimerUtilizat)
	{	
		sstrTimere[nIDTimer].nStareTimer = timerPornit;
		sstrTimere[nIDTimer].nContorCurent = sstrTimere[nIDTimer].nContorInitial;
		return 1; 
	}
	else
	{
		return EROARE_TIMER_NECREAT;
	}	
}

int timerSW_Opreste(unsigned int nIDTimer)
{
	if(nIDTimer >= TIMER_SW_NUMAR_TIMERE)
	{
		return EROARE_ID_TIMER_INVALID;
	} 

	//Daca timerul a expirat dar nu a fost tratat inca, aceasta functie va anula 
	//tratarea expirarii
	if(sstrTimere[nIDTimer].bTimerUtilizat)
	{	
		sstrTimere[nIDTimer].nStareTimer = timerOprit;
		return 1; 
	}
	else
	{
		return EROARE_TIMER_NECREAT;
	}	
}

int timerSW_Forteaza(unsigned int nIDTimer)
{
	if(nIDTimer >= TIMER_SW_NUMAR_TIMERE)
	{
		return EROARE_ID_TIMER_INVALID;
	} 

	if(sstrTimere[nIDTimer].bTimerUtilizat)
	{	
		sstrTimere[nIDTimer].nContorCurent = 0;
		sstrTimere[nIDTimer].nStareTimer = timerExpirat;
		return 1; 
	}
	else
	{
		return EROARE_TIMER_NECREAT;
	}	
}

int timerSW_Continua(unsigned int nIDTimer)
{
	if(nIDTimer >= TIMER_SW_NUMAR_TIMERE)
	{
		return EROARE_ID_TIMER_INVALID;
	} 
	
	if(sstrTimere[nIDTimer].bTimerUtilizat && (sstrTimere[nIDTimer].nStareTimer == timerOprit) )
	{	
		sstrTimere[nIDTimer].nStareTimer = timerPornit;
		return 1; 
	}
	else
	{
		return EROARE_TIMER_NECREAT;
	}	
}
