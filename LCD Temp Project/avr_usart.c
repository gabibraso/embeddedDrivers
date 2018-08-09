/*
* avr_usart.h
*
* Created: 01.01.2009
* Author : Bogdan Tarnauca
*/

#include "avr_usart.h"

/************************************************************************/
/* Time2 interupt for serial timeout control. It provides a 10ms timebase*/
/************************************************************************/
void timer2_init(void)
{
	
#if defined __ATMEGA2560
	TCCR2A = 0x00; //stop
	TCCR2B = 0x00; //stop
	
	//Finterrpt= Fclk/(N*(1+OCRx))
	// N = prescale value
    //pentru CS22,CS21,CS10 = 111 => N = 1024
    //
    //OCRx = Fclk/(Fint*N) - 1
    //OCRx = Fclk*Tint/N - 1

	TCNT2 = 0x00; 
	
	#if (F_CPU == 11059200UL) 
		OCR2A  = 0x6B;	//for Fclck = 11.059.264 Hz
	#elif (F_CPU == 16000000UL) 
		OCR2A  = 0x9B;	//for Fclck = 16.000.000 Hz
	#elif (F_CPU == 7372800UL)
	    OCR2A  = 0x47;	//for Fclck = 7.372.800 Hz
	#else
		#warning !!!! F_CPU was not defined or is not supported by the USART driver. User "#define F_CPU 11059200UL" or "#define F_CPU 16000000UL". Note that a different real CPU frequency will affect the 10ms timebase set by Timer 2 for the timeout mechanims. Timer settings set for a default F_CPU 11059200UL 
		OCR2A  = 0x6B;	//set as for Fclck = 11.059.264 Hz even if a wrong value has been set
	#endif
	
	TCCR2B = 0x07; //CS22,CS21,CS10 = 111 => 1024 Fclk divider
	TCCR2A = 0x02; //WGM22,WGM21,WGM20 = 010 start as CTC. Interrupt when TCNT2 reaches OCR2A	
	TIMSK2 |= 0x02; //Enable CTC COMPARE A (OCR2A)
	
#elif defined __ATMEGA128
	TCCR2 = 0x00; //stop timer
	TCNT2 = 0x00; 
	
	#if (F_CPU == 11059200UL)
	    OCR2  = 0x6B;	//for Fclck = 11.059.264 Hz
	#elif (F_CPU == 16000000UL)
	    OCR2  = 0x9B;	//for Fclck = 16.000.000 Hz
	#elif (F_CPU == 7372800UL)
	    OCR2  = 0x47;	//for Fclck = 7.372.800 Hz    
	#else
	#warning !!!! F_CPU was not defined or is not supported by the USART driver. User "#define F_CPU 11059200UL" or "#define F_CPU 16000000UL". Note that a different real CPU frequency will affect the 10ms timebase set by Timer 2 for the timeout mechanims. Timer settings set for a default F_CPU 11059200UL
	    OCR2  = 0x6B;	//set as for Fclck = 11.059.264 Hz even if a wrong value has been set
	#endif	
	
	TCCR2 = 1<<WGM21 |  1<<CS20  | 1<<CS22;
		//CS22, CS10 = 101 => 1024 Fclk divider
		//WGM21,WGM20 = 10 start as CTC. Interrupt when TCNT2 reaches OCR2A
	
	TIMSK |= 1<<OCIE2;	//Enable Timer2 CTC interrupt
	TIMSK &= ~(1<TOIE2); //Disable Timer2 overflow interrupt

#elif defined __ATMEGA328
	TCCR2A = 0x00; //stop timer
	TCNT2 = 0x00;

	#if (F_CPU == 11059200UL)
	    OCR2A  = 0x6B;	//for Fclck = 11.059.264 Hz
	#elif (F_CPU == 16000000UL)
	    OCR2A  = 0x9B;	//for Fclck = 16.000.000 Hz
	#elif (F_CPU == 7372800UL)
	    OCR2A  = 0x47;	//for Fclck = 7.372.800 Hz        
	#else
	    #warning !!!! F_CPU was not defined or is not supported by the USART driver. User "#define F_CPU 11059200UL" or "#define F_CPU 16000000UL". Note that a different real CPU frequency will affect the 10ms timebase set by Timer 2 for the timeout mechanims. Timer settings set for a default F_CPU 11059200UL
	    OCR2A  = 0x6B;	//set as for Fclck = 11.059.264 Hz even if a wrong value has been set
	#endif

	TCCR2A = 1<<WGM21 |  1<<CS20  | 1<<CS22;
	//CS22, CS10 = 101 => 1024 Fclk divider
	//WGM21,WGM20 = 10 start as CTC. Interrupt when TCNT2 reaches OCR2A

	TIMSK2 |= 1<<OCIE2A;	//Enable Timer2 CTC interrupt
	TIMSK2 &= ~(1<TOIE2); //Disable Timer2 overflow interrupt
		
#endif
}


#ifdef __ICCAVR
#pragma interrupt_handler timer2_compa_isr:TIMER2_COMPA_ISR_VECTOR_NO
void timer2_compa_isr(void)
#else

#if (defined __ATMEGA2560)	
ISR(TIMER2_COMPA_vect)
#elif (defined __ATMEGA128)	
ISR(TIMER2_COMP_vect)
#elif (defined __ATMEGA328)	
ISR(TIMER2_COMPA_vect)
#else
#warning Check interrupt vector name for your device
ISR(TIMER2_COMP_vect)
#endif

#endif
{
	TCNT2 = 0x00;	//RESET THE COUNTER!!!
	//uart_nValoareTimeout - Daca are valoare 0 inseamna ca timeout-ul la nivel de telegrama
	//este dezactivat. Aceasta functionalitate este necesara atunci cand sistemul este
	//un slave si asteapta comenzi de la un sistem master. Momentul la care apar comenzile
	//nu este cunoscut, asadar sistemul poate astepta la nesfarsit fara a semnala time-out
	
	#if (defined __ATMEGA8) || (defined __ATMEGA328) || (defined __ATMEGA128) || (defined __ATMEGA2560)	
	if(uart0_bTimerCounting && uart0_nValoareTimeout != 0)
	{
		uart0_nContorTimeout++;
		if(uart0_nContorTimeout >= uart0_nValoareTimeout)
		{
			uart0_bTimeout = true;
			uart0_bReceptioneaza = false;
			uart0_bStareNeprocesata = true;
			uart0_bTimerCounting = false;
		}
	}

	if(uart0_bTimerCarCounting && uart0_nValoareTimeoutCar != 0)
	{
		uart0_nContorTimeoutCar++;
		if(uart0_nContorTimeoutCar >= uart0_nValoareTimeoutCar)
		{
			uart0_bTimeoutCar = true;
			uart0_bReceptioneaza = false;
			uart0_bStareNeprocesata = true;
			uart0_bTimerCarCounting = false;
		}
	}
	#endif
	
	
	#if (defined __ATMEGA128) || (defined __ATMEGA2560)
	if(uart1_bTimerCounting && uart1_nValoareTimeout != 0)
	{
		uart1_nContorTimeout++;
		if(uart1_nContorTimeout >= uart1_nValoareTimeout)
		{
			uart1_bTimeout = true;
			uart1_bReceptioneaza = false;
			uart1_bStareNeprocesata = true;
			uart1_bTimerCounting = false;
		}
	}

	if(uart1_bTimerCarCounting && uart1_nValoareTimeoutCar != 0)
	{
		uart1_nContorTimeoutCar++;
		if(uart1_nContorTimeoutCar >= uart1_nValoareTimeoutCar)
		{
			uart1_bTimeoutCar = true;
			uart1_bReceptioneaza = false;
			uart1_bStareNeprocesata = true;
			uart1_bTimerCarCounting = false;
		}
	}
	#endif
	
	#if (defined __ATMEGA2560)
	if(uart2_bTimerCounting && uart2_nValoareTimeout != 0)
	{
		uart2_nContorTimeout++;
		if(uart2_nContorTimeout >= uart2_nValoareTimeout)
		{
			uart2_bTimeout = true;
			uart2_bReceptioneaza = false;
			uart2_bStareNeprocesata = true;
			uart2_bTimerCounting = false;
		}
	}

	if(uart2_bTimerCarCounting && uart2_nValoareTimeoutCar != 0)
	{
		uart2_nContorTimeoutCar++;
		if(uart2_nContorTimeoutCar >= uart2_nValoareTimeoutCar)
		{
			uart2_bTimeoutCar = true;
			uart2_bReceptioneaza = false;
			uart2_bStareNeprocesata = true;
			uart2_bTimerCarCounting = false;
		}
	}
	#endif


	#if (defined __ATMEGA2560)
	if(uart3_bTimerCounting && uart3_nValoareTimeout != 0)
	{
		uart3_nContorTimeout++;
		if(uart3_nContorTimeout >= uart3_nValoareTimeout)
		{
			uart3_bTimeout = true;
			uart3_bReceptioneaza = false;
			uart3_bStareNeprocesata = true;
			uart3_bTimerCounting = false;
		}
	}

	if(uart3_bTimerCarCounting && uart3_nValoareTimeoutCar != 0)
	{
		uart3_nContorTimeoutCar++;
		if(uart3_nContorTimeoutCar >= uart3_nValoareTimeoutCar)
		{
			uart3_bTimeoutCar = true;
			uart3_bReceptioneaza = false;
			uart3_bStareNeprocesata = true;
			uart3_bTimerCarCounting = false;
		}
	}
	#endif
}


#if (defined __ATMEGA8) || (defined __ATMEGA328) || (defined __ATMEGA128) || (defined __ATMEGA2560)
//MD+ Acest comentariu nu trebuie inlaturat!!!
void uart0_init(volatile unsigned char *PORT485_DIR,
char Port485DirMask,
bool bRS485Inversat,
char paramUCSR0A,
char paramUCSR0C,
char paramUBRR0L,
char paramUBRR0H,
char paramUCSR0B,
unsigned int  nValoareTimeout,
unsigned int  nValoareTimeoutChar)
{
	uart0_sReceptionat = uart0_sVid;
	uart0_nContorRX = 0;
	uart0_nLungimeRX = 0;
	uart0_nLungMaxBuferRX = 1;

	UCSR0B = 0x00; //disable while setting baud rate
	UCSR0A = paramUCSR0A;
	UCSR0C = paramUCSR0C;
	UBRR0L = paramUBRR0L; //set baud rate lo
	UBRR0H = paramUBRR0H; //set baud rate hi
	UCSR0B = paramUCSR0B;
	uart0_nValoareTimeout = nValoareTimeout;
	uart0_nValoareTimeoutCar = nValoareTimeoutChar;
	
	uart0_bTransmite = false;
	
	UCSR0B ^= BIT(5); //Dezactivez bitul UDRIE
	
	//Daca pointerul catre portul care selecteaza directia de transmisie
	//pe RS485 este NULL se considera ca transmisia se face prin RS232 si ca
	//atare nu mai este necesara selectarea directiei driverului
	if(PORT485_DIR != null)
	{
		uart0_Port485Dir = PORT485_DIR;
		*(uart0_Port485Dir-1) = *(uart0_Port485Dir-1) | Port485DirMask;
		//Setez modul de functionare (IESIRE) pentru linia portului care stabileste
		//directia pe RS485. Se modifica DDRn care se afla la adresa imediat inferioara
		//registrului PORTn
		uart0_Port485DirMask = Port485DirMask;
		uart0_bRS485Inversat = bRS485Inversat;
		uart0_bRS485 = true;
	}
	else
	{
		uart0_bRS485 = false;
	}
	
	timer2_init();
}

/******************************************************************************/

#ifdef __ICCAVR
#pragma interrupt_handler uart0_udre_isr: uart0_UDRE_ISR_VECTOR_NO
void uart0_udre_isr(void)
#else
	#ifdef __ATMEGA328
	ISR(USART_UDRE_vect)
	#else
	ISR(USART0_UDRE_vect)
	#endif
#endif
{
	//character transferred to shift register so UDR is now empty
	
	WDR();
	uart0_nContorTX++;
	
	if( uart0_nContorTX == uart0_nLungimeTX)
	{
		//dezactivez intreruperea UDRE
		UCSR0B ^= BIT(5); //Dezactivez bitul UDRIE
	}
	else
	{
		UDR0 = *(uart0_sTransmis + uart0_nContorTX);
	}

}

/******************************************************************************/
#ifdef __ICCAVR
#pragma interrupt_handler uart0_tx_isr: uart0_TX_ISR_VECTOR_NO
void uart0_tx_isr(void)
#else
	#ifdef __ATMEGA328
	ISR(USART_TX_vect)
	#else
	ISR(USART0_TX_vect)
	#endif
#endif
{
	char nAux;

	WDR();
	//character has been transmitted
	if(uart0_nContorTX == uart0_nLungimeTX)
	{
		uart0_bTxOK = true;
		uart0_bTransmite = false; //Nu mai transmit
		
		//In momentul in care am transmis (prin RS-485) ultimul caracter din buffer
		//trec driverul in regim de ascultare schimband directia acestuia
		//resetez pinul de directie
		//
		// ATENTIE !!!
		//
		// Daca semnalul de selectie a directiei este inversat de catre optocuplor
		//comanda pentru stabilirea directiei este data in mod corespunzator in functie
		//de parametrul stuart_1.bRS485Inversat
		if(uart0_bRS485)
		{
			//salvez valoarea portului prin care stabileste directia, resetand insa bitul care este
			//alocat semnalului de directie. Fac asta pentru a permite operatiile logice de mai jos
			nAux = *uart0_Port485Dir & ~(uart0_Port485DirMask);

			//Setez in 1 pinul care seteaza directia sau il las in zero (vezi comanda de mai sus)
			//daca semnalul de comanda trebuie inversat).
			*uart0_Port485Dir = nAux | (uart0_bRS485Inversat?uart0_Port485DirMask:0x00);
		}

	}
}


/******************************************************************************/
#ifdef __ICCAVR
#pragma interrupt_handler uart0_rx_isr: uart0_RX_ISR_VECTOR_NO
void uart0_rx_isr(void)
#else
	#ifdef __ATMEGA328
	ISR(USART_RX_vect)
	#else
	ISR(USART0_RX_vect)
	#endif
#endif
{
	uart0_bTimerCarCounting = true;
	uart0_nContorTimeoutCar = 0;
	//Am receptionat un caracter pornesc timerul ca sa verific daca
	//pauze intre caractere - conform protocolului STOM in cadrul
	//unei telegrame nu pot aparea pauze de transmisie intre doua
	//caractere succesive. O astfel de pauza indica faptul ca
	//o telegrama (indiferent de emitatorul ei) s-a incheiat.
	//Corelata cu numarul de caractere receptionate si cu pozitia
	//acestora se poate obtine un criteriu de rejectie a telegramelor
	//care sunt transmise pe seriala dar care nu sunt considerate
	//ca fiind valide.

	WDR();
	//uart has received a character in UDR
	//if(!uart_bSetareParametri)
	uart0_sVid[0] = UDR0;

	if(uart0_bReceptioneaza)
	{
		*(uart0_sReceptionat + uart0_nContorRX) = uart0_sVid[0];

		if(uart0_nContorRX < uart0_nLungMaxBuferRX)
		{
			uart0_nContorRX++;
			//Incrementez numai daca sunt in domeniul de memorie alocat
			//previn astfel suprascrierile
		}
		else
		{
			uart0_nContorRX = uart0_nLungMaxBuferRX;	//ca sa iau si ultimul caracter
			uart0_bRXBufferLimit = true;
			uart0_bReceptioneaza = false;
			uart0_bStareNeprocesata = true;
			uart0_bTimerCounting = false;
		}
	}
}

/******************************************************************************/
void uart0_TransmiteExt(char *sTransmis, unsigned int nLungime, bool bAsteptareTxAnterioara, bool bAsteptareTxCurenta)
{
	char nAux;
	WDR();
	if(nLungime>0)
	{
		if( (bAsteptareTxAnterioara == true) &&  (uart0_bTransmite == true))
		{
			do	{//Asteapta transmisia anterrioare
				wdt_reset();
			}
			while(uart0_bTransmite);
		}
		uart0_bTransmite = true;
		uart0_sTransmis = sTransmis;
		//Memorez adresa primei locatii din bufferul de transmisie
		uart0_nContorTX = 0;
		//Resetez contorul care numara caracterele transmise
		uart0_bTxOK = false;
		//resetez flagul care indica ca trasmiterea caracterelor din buffer a fost
		//realizata
		uart0_nLungimeTX = nLungime;
		
		//Daca transmitz prin RS-232 trebuie sa setez directia de transmisie.
		//Directia este resetata (ascultam) in momentul in care am transmis ultimul
		//caracter din buffer
		// ATENTIE !!!
		//
		// Daca semnalul de selectie a directiei este inversat de catre optocuplor
		//comanda pentru stabilirea directiei este data in mod corespunzator in functie
		//de parametrul stuart0_1.bRS485Inversat
		
		
		if(uart0_bRS485)
		{
			//salvez valoarea portului prin care stabileste directia, resetand insa bitul care este
			//alocat semnalului de directie. Fac asta pentru a permite operatiile logice de mai jos
			nAux = *uart0_Port485Dir & ~(uart0_Port485DirMask);

			//Setez in 1 pinul care seteaza directia sau il las in zero (vezi comanda de mai sus)
			//daca semnalul de comanda trebuie inversat).
			*uart0_Port485Dir = nAux | (uart0_bRS485Inversat ?0x00:uart0_Port485DirMask);
		}

		UDR0 = *sTransmis;		   //Transmit primul caracter din buffer
		UCSR0B = UCSR0B | BIT(5); //Activez bitul UDRIE
		
		if( (bAsteptareTxCurenta == true) &&  (uart0_bTransmite == true))
		{
			do	{//Blocheaza metoda pana ce se transmite mesajul curent
				wdt_reset();
			}
			while(uart0_bTransmite);
		}
	}
}


void uart0_Transmite(char *sTransmis, unsigned int nLungime, bool bAsteptareTxAnterioara)
{
	uart0_TransmiteExt(sTransmis, nLungime, bAsteptareTxAnterioara, false);
}

/*****************************************************************************/


/*****************************************************************************/



//Rutina care imi verifica daca s-au inteplinit criteriiile de sfarsit de receptie
//Returneaza true daca receptia s-a inceiat false daca inca se mai asteapta caractere pe seriala.
bool uart0_ReceptieOK()
{
	
	char i;
	unsigned int nPozitie, nTamponContorRX;
	WDR();

	nTamponContorRX = uart0_nContorRX;

	for(nPozitie = uart0_nIndexUltimTestat; nPozitie < nTamponContorRX; nPozitie++)
	{
		WDR();
		
		//--------------------------------------
		if(uart0_nCriteriiTestRX != 0)
		{//Receptie cu mai multe criterii de stop ((caracter terminator SI lungime))
			//Corespunde apelului uart0_ReceptieLungSiTermTX
			for(i=0; i<uart0_nCriteriiTestRX; i++)
			{//iterez printere criteriile de test.
				WDR();

				if(*(uart0_sReceptionat + nPozitie) == *(uart0_sMultiTermRX + i))
				{//daca caracterul receptionat se gaseste in lista caracterelor
					//terminatoare testez daca se gaseste pe pozitia
					//corespunzatoare
					if(*(uart0_sMultiLungimeRX + i) == 0)
					{//Daca pozitia  asignata a fost 0 inseamna ca de fapt nu ma
						//intereseaza pozitia in care apare caracterul. si ca atare
						//receptia este OK

						uart0_nLungimeRX = nPozitie+1; //bufer dublu practic
						uart0_bTimerCounting = false;

						//Daca nu blochez receptia la finalul receptiei unei telegrame
						//(situatia intalnita in protocoalele in care se receptioneaza headerul
						//iar apoi se da comanda pentru receptia blocului de date) atunci voi
						//mentine activ mecanismul de timeout la nivel de caracter pana cand
						//o comanda (de obicei cea pentru receptia bloclui de date) care se
						//bloceze receptia la final este executata.
						//Astfel pot sa detectez timeout-ul care apare daca dupa header nu a
						//inceput receptia blocului de date.
						if(uart0_bBlocheazaReceptiaLaFinal == true)
						uart0_bTimerCarCounting = false;
						//Nu merge asa (mai jos) pentru toate cazurile deoarece caompilatorul da
						//urmatoarea eroare: internal error -- out of registers: opsize 2 free 300f0003 local cf0003fc free&local 0 TN type 5 ticks 8880.
						//uart0_bTimerCarCounting = !uart0_bBlocheazaReceptiaLaFinal;

						i = uart0_nCriteriiTestRX; //ies din ciclul de testare
						nPozitie = nTamponContorRX;	//ies din ciclul de testare mare ??
						uart0_bRxOK = true;
						uart0_bStareNeprocesata = true;
						uart0_bReceptioneaza = !uart0_bBlocheazaReceptiaLaFinal;
						uart0_nContorTimeout = 0;
					}
					else
					{//Daca pozitia  asignata a fost diferita de 0 inseamna ca ma
						//intereseaza pozitia in care apare caracterul si ca atare mai fac
						//un test in acest sens
						
						//ATENTIE - asa a fost testat si era stabil
						if(nTamponContorRX == *(uart0_sMultiLungimeRX + i))

						//Asa se pare ca da erori
						//if(nPozitie + 1 == *(uart0_sMultiLungimeRX + i))

						{//caracterul terminatort citit este la pozitisa dorita
							uart0_nLungimeRX = nPozitie+1; //bufer dublu practic
							uart0_bTimerCounting = false;
							
							//Daca nu blochez receptia la finalul receptiei unei telegrame
							//(situatia intalnita in protocoalele in care se receptioneaza headerul
							//iar apoi se da comanda pentru receptia blocului de date) atunci voi
							//mentine activ mecanismul de timeout la nivel de caracter pana cand
							//o comanda (de obicei cea pentru receptia bloclui de date) care se
							//bloceze receptia la final este executata.
							//Astfel pot sa detectez timeout-ul care apare daca dupa header nu a
							//inceput receptia blocului de date.
							if(uart0_bBlocheazaReceptiaLaFinal == true)
							uart0_bTimerCarCounting = false;

							
							i = uart0_nCriteriiTestRX; //ies din ciclul de testare mic
							nPozitie = nTamponContorRX;	//ies din ciclul de testare mare
							uart0_bRxOK = true;
							uart0_bStareNeprocesata = true;
							uart0_bReceptioneaza = !uart0_bBlocheazaReceptiaLaFinal;
							uart0_nContorTimeout = 0;
						}
					}
				} //end if
			} //end for
		} //end if
		else
		{//Receptie cu un singur criteriu de stop (caracter terminator SAU lungime)
			//Corespunde apelului uart0_ReceptieTermTX sau uart0_ReceptieLunigme
			if(uart0_bRecCuTermRX)
			{ //Receptie pana la intalnirea caracterului terminator
				if(*(uart0_sReceptionat + nPozitie) == uart0_cTermRX)
				{
					uart0_nLungimeRX = nPozitie+1;
					uart0_bTimerCounting = false;

					//Daca nu blochez receptia la finalul receptiei unei telegrame
					//(situatia intalnita in protocoalele in care se receptioneaza headerul
					//iar apoi se da comanda pentru receptia blocului de date) atunci voi
					//mentine activ mecanismul de timeout la nivel de caracter pana cand
					//o comanda (de obicei cea pentru receptia bloclui de date) care se
					//bloceze receptia la final este executata.
					//Astfel pot sa detectez timeout-ul care apare daca dupa header nu a
					//inceput receptia blocului de date.
					if(uart0_bBlocheazaReceptiaLaFinal == true)
					uart0_bTimerCarCounting = false;

					
					i = uart0_nCriteriiTestRX; //ies din ciclul de testare
					uart0_bRxOK = true;
					uart0_bStareNeprocesata = true;
					uart0_bReceptioneaza = !uart0_bBlocheazaReceptiaLaFinal;
					uart0_nContorTimeout = 0;
				}
			}
			else
			{ //Receptia unui numar fix de caractere
				if(nTamponContorRX >= uart0_nLungimeRX)
				{
					//uart0_nLungimeRX = nTampoContorRX; NU se aplica deaorece stiu dinainte lungimea
					uart0_bTimerCounting = false;

					//Daca nu blochez receptia la finalul receptiei unei telegrame
					//(situatia intalnita in protocoalele in care se receptioneaza headerul
					//iar apoi se da comanda pentru receptia blocului de date) atunci voi
					//mentine activ mecanismul de timeout la nivel de caracter pana cand
					//o comanda (de obicei cea pentru receptia bloclui de date) care se
					//bloceze receptia la final este executata.
					//Astfel pot sa detectez timeout-ul care apare daca dupa header nu a
					//inceput receptia blocului de date.
					if(uart0_bBlocheazaReceptiaLaFinal == true)
					uart0_bTimerCarCounting = false;


					i = uart0_nCriteriiTestRX; //ies din ciclul de testare
					uart0_bRxOK = true;
					uart0_bStareNeprocesata = true;
					uart0_bReceptioneaza = !uart0_bBlocheazaReceptiaLaFinal;
					uart0_nContorTimeout = 0;
				}
			}
		}
	}
	uart0_nIndexUltimTestat = nTamponContorRX;
	return uart0_bRxOK;
}


/******************************************************************************/
void uart0_ReceptieLFix( char *sReceptionat,
unsigned int nLungime,
unsigned int nLungMaxBuferRX,
bool bReseteazaBuffIntrare,
bool bBlocheazaReceptiaLaFinal)
{
	char nAux;
	WDR();
	if(uart0_bRS485)
	{
		//salvez valoarea portului prin care stabileste directia, resetand insa bitul care este
		//alocat semnalului de directie. Fac asta pentru a permite operatiile logice de mai jos
		nAux = *uart0_Port485Dir & ~(uart0_Port485DirMask);

		//Setez in 1 pinul care seteaza directia sau il las in zero (vezi comanda de mai sus)
		//daca semnalul de comanda trebuie inversat).
		*uart0_Port485Dir = nAux | (uart0_bRS485Inversat?uart0_Port485DirMask:0x00);
	}
	uart0_bSetareParametri = true;
	uart0_sReceptionat = sReceptionat;
	uart0_nLungimeRX = nLungime;
	uart0_nCriteriiTestRX = 0;
	if(bReseteazaBuffIntrare)
	uart0_nContorRX = 0;
	uart0_nLungMaxBuferRX = nLungMaxBuferRX;
	//Setez lungimea maxima a bufferului de receptie. Este utila in special in cazul
	//in care fac receptie cu terminator, dar si in cazul de fata este utila mai
	//ales pentru receptia in buffere multiple cu dimensiuni diferite
	//caracterul terminator nu este receptionat eliminandu-se riscul suprascrierii
	//unei zone de memorire nealocata.
	uart0_bRxOK = false;
	uart0_bStareNeprocesata = false;
	uart0_bRecCuTermRX = false;
	//indic faptul ca receptia NU se face  pana la intalnirea unui caracter
	//terminator ci pana cand e receptionat un anumit numar de caractere
	uart0_bRXBufferLimit = false;
	uart0_nIndexUltimTestat = 0;
	uart0_bTimerCounting = false;
	uart0_bTimeout = false;
	uart0_nContorTimeout = 0;
	uart0_bTimerCounting = true;
	uart0_bReceptioneaza = true;
	uart0_bBlocheazaReceptiaLaFinal = bBlocheazaReceptiaLaFinal;
	uart0_bSetareParametri = false;
}

/******************************************************************************/

/*
sReceptionat - receive buffer
cTermRX - terminal character. Reception ends when the terminal characher is received or the buffer length is reached.
nLungMaxBuferRX - max size of the input buffer
bReseteazaBuffIntrare
true = resets the internal counter to 0 when a receive command is issued, typical approach when an input message is expected to be received in one shot
false = does not reset the counter and the buffer is written from the last recorder position + 1. This is useful when receiving messages in multi-stages (e.g. header and body) and when the header requires its own processing.
bBlocheazaReceptiaLaFinal = blocks the method call until the a message is received. Usually not the case, since this would block the main loop. Typically set to false and poling is performed to check the if a message has been received using Receptie_OK()...
*/
void uart0_ReceptieTermTX(
char *sReceptionat,
char cTermRX,
unsigned int nLungMaxBuferRX,
bool bReseteazaBuffIntrare,
bool bBlocheazaReceptiaLaFinal)
{
	WDR();
	uart0_bSetareParametri = true;
	uart0_sReceptionat = sReceptionat;
	uart0_nLungimeRX = 0;
	uart0_cTermRX = cTermRX;
	uart0_nCriteriiTestRX = 0;
	if(bReseteazaBuffIntrare)
	uart0_nContorRX = 0;
	uart0_nLungMaxBuferRX = nLungMaxBuferRX;
	//Setez lungimea maxima a bufferului de receptie. Este utila in cazul in care
	//caracterul terminator nu este receptionat eliminandu-se riscul suprascrierii
	//unei zone de memorire nealocata.
	uart0_bRxOK = false;
	uart0_bStareNeprocesata = false;
	uart0_bRecCuTermRX = true;
	//indic faptul ca receptia NU se face  pana la intalnirea unui caracter
	//terminator ci pana cand e receptionat un anumit numar de caractere
	uart0_bRXBufferLimit = false;
	uart0_nIndexUltimTestat = 0;
	uart0_bTimerCounting = false;
	uart0_bTimeout = false;
	uart0_nContorTimeout = 0;
	uart0_bTimerCounting = true;
	uart0_bReceptioneaza = true;
	uart0_bBlocheazaReceptiaLaFinal = bBlocheazaReceptiaLaFinal;
	uart0_bSetareParametri = false;
}


void uart0_ReceptieLungSiTermTX(
char *sReceptionat,
char *sMultiTermRX,
unsigned int *sMultiLungimeRX,
char nCriteriiTestRX,
unsigned int nLungMaxBuferRX,
bool bReseteazaBuffIntrare,
bool bBlocheazaReceptiaLaFinal)
{
	char nAux;
	
	//Se receptioneaza caractere pana cand unul se gaseste in sirul de caractere
	//terminatoare posibile (stabilite in *sTermRX)iar sirul receptionat are exact
	//lungimea stabilita in *sLungimi la acelasi index ca si caracterul terminator
	//pana cand CReceptionat == sTerm[i] && LungimeSir == sLungimi[i]
	//i=:1,nCriteriiTest  nCriteriiTest(1...255) cand nCriteriiTest=0 receptia
	//se face cu un singur test fie de caracter terminator fie de lungime (rutinele
	//uart#_ReceptieTermTX si uart#_ReceptieLunigme)
	//(de uart_nCriteriiTestRX ori se testeaza daca caracterul recptionat se
	//gaseste in sirul de caractere terminatoare si daca acest caracter
	//se gaseste la pozitia dorita.
	//Functia este utila atunci cand avem de receptionat mai multe siruri
	//de caractere cu lungimi fixe avand caractere terminatoare diferite
	//si mai ales atunci cand aceste caractere terminatoare se pot gasi
	//si in partea utila a sirului receptionat fara a avea insa semnificatia
	//de terminator
	//OBSERVATIE!!!
	//Daca in tabelul cu lungimi avem pozitii in care aceasta valoare este
	//0 atunci pentru caracterul corespunzator (de pe aceeasi pozitie) din
	//tabelul caracterelor terminatoare receptia se va face pana la prima
	//aparitie a caracterului fara a fi testata si pozitia acestuia - Se
	//implementeaza practic aceeasi functionalitate ca si in cazul rutinei
	//uart#_ReceptieTermTX
	
	/*
	Exemplu:
	
	char sTermRX[32]="%&/";
	char sPoz[32]={1,0,7};
	
	uart1_ReceptieLungSiTermTX(sSirA,sTermRX, sPoz, 3, 255);
	
	stuart0_1.bRxOK va fi setat atunci cand fie primul caracter introdus
	este '%' fie cand a fost receptionat '&' indiferent de pozitie fie cand
	al saptelea caracter receptionat a fost '/'
	*/

	WDR();

	if(uart0_bRS485)
	{
		//salvez valoarea portului prin care stabileste directia, resetand insa bitul care este
		//alocat semnalului de directie. Fac asta pentru a permite operatiile logice de mai jos
		nAux = *uart0_Port485Dir & ~(uart0_Port485DirMask);

		//Setez in 1 pinul care seteaza directia sau il las in zero (vezi comanda de mai sus)
		//daca semnalul de comanda trebuie inversat).
		*uart0_Port485Dir = nAux | (uart0_bRS485Inversat?uart0_Port485DirMask:0x00);
	}
	
	uart0_bSetareParametri = true;
	uart0_sReceptionat = sReceptionat;
	uart0_nLungimeRX = 0;
	uart0_nCriteriiTestRX = nCriteriiTestRX;
	uart0_sMultiTermRX = sMultiTermRX;
	uart0_sMultiLungimeRX = sMultiLungimeRX;
	if(bReseteazaBuffIntrare)
	uart0_nContorRX = 0;
	uart0_nLungMaxBuferRX = nLungMaxBuferRX;
	//Setez lungimea maxima a bufferului de receptie. Este utila in cazul in care
	//caracterul terminator nu este receptionat eliminandu-se riscul suprascrierii
	//unei zone de memorire nealocata.
	uart0_bRxOK = false;
	uart0_bStareNeprocesata = false;
	uart0_bRecCuTermRX = true;
	//indic faptul ca receptia NU se face  pana la intalnirea unui caracter
	//terminator ci pana cand e receptionat un anumit numar de caractere
	uart0_bRXBufferLimit = false;
	uart0_nIndexUltimTestat = 0;
	uart0_bTimerCounting = false;
	uart0_bTimeout = false;
	uart0_nContorTimeout = 0;
	uart0_bTimerCounting = true;
	uart0_bReceptioneaza = true;
	uart0_bBlocheazaReceptiaLaFinal = bBlocheazaReceptiaLaFinal;
	uart0_bSetareParametri = false;
}
#endif


#if (defined __ATMEGA128) || (defined __ATMEGA2560)
//MD+ Acest comentariu nu trebuie inlaturat!!!
void uart1_init(volatile unsigned char *PORT485_DIR,
char Port485DirMask,
bool bRS485Inversat,
char paramUCSR0A,
char paramUCSR0C,
char paramUBRR0L,
char paramUBRR0H,
char paramUCSR0B,
unsigned int  nValoareTimeout,
unsigned int  nValoareTimeoutChar)
{
	uart1_sReceptionat = uart1_sVid;
	uart1_nContorRX = 0;
	uart1_nLungimeRX = 0;
	uart1_nLungMaxBuferRX = 1;

	UCSR1B = 0x00; //disable while setting baud rate
	UCSR1A = paramUCSR0A;
	UCSR1C = paramUCSR0C;
	UBRR1L = paramUBRR0L; //set baud rate lo
	UBRR1H = paramUBRR0H; //set baud rate hi
	UCSR1B = paramUCSR0B;
	uart1_nValoareTimeout = nValoareTimeout;
	uart1_nValoareTimeoutCar = nValoareTimeoutChar;
	
	uart1_bTransmite = false;
	
	UCSR1B ^= BIT(5); //Dezactivez bitul UDRIE
	
	//Daca pointerul catre portul care selecteaza directia de transmisie
	//pe RS485 este NULL se considera ca transmisia se face prin RS232 si ca
	//atare nu mai este necesara selectarea directiei driverului
	if(PORT485_DIR != null)
	{
		uart1_Port485Dir = PORT485_DIR;
		*(uart1_Port485Dir-1) = *(uart1_Port485Dir-1) | Port485DirMask;
		//Setez modul de functionare (IESIRE) pentru linia portului care stabileste
		//directia pe RS485. Se modifica DDRn care se afla la adresa imediat inferioara
		//registrului PORTn
		uart1_Port485DirMask = Port485DirMask;
		uart1_bRS485Inversat = bRS485Inversat;
		uart1_bRS485 = true;
	}
	else
	{
		uart1_bRS485 = false;
	}
	
	timer2_init();
}

/******************************************************************************/

#ifdef __ICCAVR
#pragma interrupt_handler uart1_udre_isr: uart1_UDRE_ISR_VECTOR_NO
void uart1_udre_isr(void)
#else
ISR(USART1_UDRE_vect)
#endif
{
	//character transferred to shift register so UDR is now empty
	
	WDR();
	uart1_nContorTX++;
	
	if( uart1_nContorTX == uart1_nLungimeTX)
	{
		//dezactivez intreruperea UDRE
		UCSR1B ^= BIT(5); //Dezactivez bitul UDRIE
	}
	else
	{
		UDR1 = *(uart1_sTransmis + uart1_nContorTX);
	}

}

/******************************************************************************/
#ifdef __ICCAVR
#pragma interrupt_handler uart1_tx_isr: uart1_TX_ISR_VECTOR_NO
void uart1_tx_isr(void)
#else
ISR(USART1_TX_vect)
#endif

{
	char nAux;

	WDR();
	//character has been transmitted
	if(uart1_nContorTX == uart1_nLungimeTX)
	{
		uart1_bTxOK = true;
		uart1_bTransmite = false; //Nu mai transmit
		
		//In momentul in care am transmis (prin RS-485) ultimul caracter din buffer
		//trec driverul in regim de ascultare schimband directia acestuia
		//resetez pinul de directie
		//
		// ATENTIE !!!
		//
		// Daca semnalul de selectie a directiei este inversat de catre optocuplor
		//comanda pentru stabilirea directiei este data in mod corespunzator in functie
		//de parametrul stuart_1.bRS485Inversat
		if(uart1_bRS485)
		{
			//salvez valoarea portului prin care stabileste directia, resetand insa bitul care este
			//alocat semnalului de directie. Fac asta pentru a permite operatiile logice de mai jos
			nAux = *uart1_Port485Dir & ~(uart1_Port485DirMask);

			//Setez in 1 pinul care seteaza directia sau il las in zero (vezi comanda de mai sus)
			//daca semnalul de comanda trebuie inversat).
			*uart1_Port485Dir = nAux | (uart1_bRS485Inversat?uart1_Port485DirMask:0x00);
		}

	}
}


/******************************************************************************/
#ifdef __ICCAVR
#pragma interrupt_handler uart1_rx_isr: uart1_RX_ISR_VECTOR_NO
void uart1_rx_isr(void)
#else
ISR(USART1_RX_vect)
#endif
{
	uart1_bTimerCarCounting = true;
	uart1_nContorTimeoutCar = 0;
	//Am receptionat un caracter pornesc timerul ca sa verific daca
	//pauze intre caractere - conform protocolului STOM in cadrul
	//unei telegrame nu pot aparea pauze de transmisie intre doua
	//caractere succesive. O astfel de pauza indica faptul ca
	//o telegrama (indiferent de emitatorul ei) s-a incheiat.
	//Corelata cu numarul de caractere receptionate si cu pozitia
	//acestora se poate obtine un criteriu de rejectie a telegramelor
	//care sunt transmise pe seriala dar care nu sunt considerate
	//ca fiind valide.

	WDR();
	//uart has received a character in UDR
	//if(!uart_bSetareParametri)
	uart1_sVid[0] = UDR1;

	if(uart1_bReceptioneaza)
	{
		*(uart1_sReceptionat + uart1_nContorRX) = uart1_sVid[0];

		if(uart1_nContorRX < uart1_nLungMaxBuferRX)
		{
			uart1_nContorRX++;
			//Incrementez numai daca sunt in domeniul de memorie alocat
			//previn astfel suprascrierile
		}
		else
		{
			uart1_nContorRX = uart1_nLungMaxBuferRX;	//ca sa iau si ultimul caracter
			uart1_bRXBufferLimit = true;
			uart1_bReceptioneaza = false;
			uart1_bStareNeprocesata = true;
			uart1_bTimerCounting = false;
		}
	}
}

/******************************************************************************/
void uart1_TransmiteExt(char *sTransmis, unsigned int nLungime, bool bAsteptareTxAnterioara, bool bAsteptareTxCurenta)
{
	char nAux;
	WDR();
	if(nLungime>0)
	{
		if( (bAsteptareTxAnterioara == true) &&  (uart1_bTransmite == true))
		{
			do	{//Asteapta transmisia anterrioare
				wdt_reset();
			}
			while(uart1_bTransmite);
		}
		uart1_bTransmite = true;
		uart1_sTransmis = sTransmis;
		//Memorez adresa primei locatii din bufferul de transmisie
		uart1_nContorTX = 0;
		//Resetez contorul care numara caracterele transmise
		uart1_bTxOK = false;
		//resetez flagul care indica ca trasmiterea caracterelor din buffer a fost
		//realizata
		uart1_nLungimeTX = nLungime;
		
		//Daca transmitz prin RS-232 trebuie sa setez directia de transmisie.
		//Directia este resetata (ascultam) in momentul in care am transmis ultimul
		//caracter din buffer
		// ATENTIE !!!
		//
		// Daca semnalul de selectie a directiei este inversat de catre optocuplor
		//comanda pentru stabilirea directiei este data in mod corespunzator in functie
		//de parametrul stuart1_1.bRS485Inversat
		
		
		if(uart1_bRS485)
		{
			//salvez valoarea portului prin care stabileste directia, resetand insa bitul care este
			//alocat semnalului de directie. Fac asta pentru a permite operatiile logice de mai jos
			nAux = *uart1_Port485Dir & ~(uart1_Port485DirMask);

			//Setez in 1 pinul care seteaza directia sau il las in zero (vezi comanda de mai sus)
			//daca semnalul de comanda trebuie inversat).
			*uart1_Port485Dir = nAux | (uart1_bRS485Inversat ?0x00:uart1_Port485DirMask);
		}

		UDR1 = *sTransmis;		   //Transmit primul caracter din buffer
		UCSR1B = UCSR1B | BIT(5); //Activez bitul UDRIE
		
		if( (bAsteptareTxCurenta == true) &&  (uart1_bTransmite == true))
		{
			do	{//Blocheaza metoda pana ce se transmite mesajul curent
				wdt_reset();
			}
			while(uart1_bTransmite);
		}
	}
}

void uart1_Transmite(char *sTransmis, unsigned int nLungime, bool bAsteptareTxAnterioara)
{
	uart1_TransmiteExt(sTransmis, nLungime, bAsteptareTxAnterioara, false);
}

/*****************************************************************************/


/*****************************************************************************/



//Rutina care imi verifica daca s-au inteplinit criteriiile de sfarsit de receptie
//Returneaza true daca receptia s-a inceiat false daca inca se mai asteapta caractere pe seriala.
bool uart1_ReceptieOK()
{
	
	char i;
	unsigned int nPozitie, nTamponContorRX;
	WDR();

	nTamponContorRX = uart1_nContorRX;

	for(nPozitie = uart1_nIndexUltimTestat; nPozitie < nTamponContorRX; nPozitie++)
	{
		WDR();
		
		//--------------------------------------
		if(uart1_nCriteriiTestRX != 0)
		{//Receptie cu mai multe criterii de stop ((caracter terminator SI lungime))
			//Corespunde apelului uart1_ReceptieLungSiTermTX
			for(i=0; i<uart1_nCriteriiTestRX; i++)
			{//iterez printere criteriile de test.
				WDR();

				if(*(uart1_sReceptionat + nPozitie) == *(uart1_sMultiTermRX + i))
				{//daca caracterul receptionat se gaseste in lista caracterelor
					//terminatoare testez daca se gaseste pe pozitia
					//corespunzatoare
					if(*(uart1_sMultiLungimeRX + i) == 0)
					{//Daca pozitia  asignata a fost 0 inseamna ca de fapt nu ma
						//intereseaza pozitia in care apare caracterul. si ca atare
						//receptia este OK

						uart1_nLungimeRX = nPozitie+1; //bufer dublu practic
						uart1_bTimerCounting = false;

						//Daca nu blochez receptia la finalul receptiei unei telegrame
						//(situatia intalnita in protocoalele in care se receptioneaza headerul
						//iar apoi se da comanda pentru receptia blocului de date) atunci voi
						//mentine activ mecanismul de timeout la nivel de caracter pana cand
						//o comanda (de obicei cea pentru receptia bloclui de date) care se
						//bloceze receptia la final este executata.
						//Astfel pot sa detectez timeout-ul care apare daca dupa header nu a
						//inceput receptia blocului de date.
						if(uart1_bBlocheazaReceptiaLaFinal == true)
						uart1_bTimerCarCounting = false;
						//Nu merge asa (mai jos) pentru toate cazurile deoarece caompilatorul da
						//urmatoarea eroare: internal error -- out of registers: opsize 2 free 300f0003 local cf0003fc free&local 0 TN type 5 ticks 8880.
						//uart1_bTimerCarCounting = !uart1_bBlocheazaReceptiaLaFinal;

						i = uart1_nCriteriiTestRX; //ies din ciclul de testare
						nPozitie = nTamponContorRX;	//ies din ciclul de testare mare ??
						uart1_bRxOK = true;
						uart1_bStareNeprocesata = true;
						uart1_bReceptioneaza = !uart1_bBlocheazaReceptiaLaFinal;
						uart1_nContorTimeout = 0;
					}
					else
					{//Daca pozitia  asignata a fost diferita de 0 inseamna ca ma
						//intereseaza pozitia in care apare caracterul si ca atare mai fac
						//un test in acest sens
						
						//ATENTIE - asa a fost testat si era stabil
						if(nTamponContorRX == *(uart1_sMultiLungimeRX + i))

						//Asa se pare ca da erori
						//if(nPozitie + 1 == *(uart1_sMultiLungimeRX + i))

						{//caracterul terminatort citit este la pozitisa dorita
							uart1_nLungimeRX = nPozitie+1; //bufer dublu practic
							uart1_bTimerCounting = false;
							
							//Daca nu blochez receptia la finalul receptiei unei telegrame
							//(situatia intalnita in protocoalele in care se receptioneaza headerul
							//iar apoi se da comanda pentru receptia blocului de date) atunci voi
							//mentine activ mecanismul de timeout la nivel de caracter pana cand
							//o comanda (de obicei cea pentru receptia bloclui de date) care se
							//bloceze receptia la final este executata.
							//Astfel pot sa detectez timeout-ul care apare daca dupa header nu a
							//inceput receptia blocului de date.
							if(uart1_bBlocheazaReceptiaLaFinal == true)
							uart1_bTimerCarCounting = false;

							
							i = uart1_nCriteriiTestRX; //ies din ciclul de testare mic
							nPozitie = nTamponContorRX;	//ies din ciclul de testare mare
							uart1_bRxOK = true;
							uart1_bStareNeprocesata = true;
							uart1_bReceptioneaza = !uart1_bBlocheazaReceptiaLaFinal;
							uart1_nContorTimeout = 0;
						}
					}
				} //end if
			} //end for
		} //end if
		else
		{//Receptie cu un singur criteriu de stop (caracter terminator SAU lungime)
			//Corespunde apelului uart1_ReceptieTermTX sau uart1_ReceptieLunigme
			if(uart1_bRecCuTermRX)
			{ //Receptie pana la intalnirea caracterului terminator
				if(*(uart1_sReceptionat + nPozitie) == uart1_cTermRX)
				{
					uart1_nLungimeRX = nPozitie+1;
					uart1_bTimerCounting = false;

					//Daca nu blochez receptia la finalul receptiei unei telegrame
					//(situatia intalnita in protocoalele in care se receptioneaza headerul
					//iar apoi se da comanda pentru receptia blocului de date) atunci voi
					//mentine activ mecanismul de timeout la nivel de caracter pana cand
					//o comanda (de obicei cea pentru receptia bloclui de date) care se
					//bloceze receptia la final este executata.
					//Astfel pot sa detectez timeout-ul care apare daca dupa header nu a
					//inceput receptia blocului de date.
					if(uart1_bBlocheazaReceptiaLaFinal == true)
					uart1_bTimerCarCounting = false;

					
					i = uart1_nCriteriiTestRX; //ies din ciclul de testare
					uart1_bRxOK = true;
					uart1_bStareNeprocesata = true;
					uart1_bReceptioneaza = !uart1_bBlocheazaReceptiaLaFinal;
					uart1_nContorTimeout = 0;
				}
			}
			else
			{ //Receptia unui numar fix de caractere
				if(nTamponContorRX >= uart1_nLungimeRX)
				{
					//uart1_nLungimeRX = nTampoContorRX; NU se aplica deaorece stiu dinainte lungimea
					uart1_bTimerCounting = false;

					//Daca nu blochez receptia la finalul receptiei unei telegrame
					//(situatia intalnita in protocoalele in care se receptioneaza headerul
					//iar apoi se da comanda pentru receptia blocului de date) atunci voi
					//mentine activ mecanismul de timeout la nivel de caracter pana cand
					//o comanda (de obicei cea pentru receptia bloclui de date) care se
					//bloceze receptia la final este executata.
					//Astfel pot sa detectez timeout-ul care apare daca dupa header nu a
					//inceput receptia blocului de date.
					if(uart1_bBlocheazaReceptiaLaFinal == true)
					uart1_bTimerCarCounting = false;


					i = uart1_nCriteriiTestRX; //ies din ciclul de testare
					uart1_bRxOK = true;
					uart1_bStareNeprocesata = true;
					uart1_bReceptioneaza = !uart1_bBlocheazaReceptiaLaFinal;
					uart1_nContorTimeout = 0;
				}
			}
		}
	}
	uart1_nIndexUltimTestat = nTamponContorRX;
	return uart1_bRxOK;
}


/******************************************************************************/
void uart1_ReceptieLFix( char *sReceptionat,
unsigned int nLungime,
unsigned int nLungMaxBuferRX,
bool bReseteazaBuffIntrare,
bool bBlocheazaReceptiaLaFinal)
{
	char nAux;
	WDR();
	if(uart1_bRS485)
	{
		//salvez valoarea portului prin care stabileste directia, resetand insa bitul care este
		//alocat semnalului de directie. Fac asta pentru a permite operatiile logice de mai jos
		nAux = *uart1_Port485Dir & ~(uart1_Port485DirMask);

		//Setez in 1 pinul care seteaza directia sau il las in zero (vezi comanda de mai sus)
		//daca semnalul de comanda trebuie inversat).
		*uart1_Port485Dir = nAux | (uart1_bRS485Inversat?uart1_Port485DirMask:0x00);
	}
	uart1_bSetareParametri = true;
	uart1_sReceptionat = sReceptionat;
	uart1_nLungimeRX = nLungime;
	uart1_nCriteriiTestRX = 0;
	if(bReseteazaBuffIntrare)
	uart1_nContorRX = 0;
	uart1_nLungMaxBuferRX = nLungMaxBuferRX;
	//Setez lungimea maxima a bufferului de receptie. Este utila in special in cazul
	//in care fac receptie cu terminator, dar si in cazul de fata este utila mai
	//ales pentru receptia in buffere multiple cu dimensiuni diferite
	//caracterul terminator nu este receptionat eliminandu-se riscul suprascrierii
	//unei zone de memorire nealocata.
	uart1_bRxOK = false;
	uart1_bStareNeprocesata = false;
	uart1_bRecCuTermRX = false;
	//indic faptul ca receptia NU se face  pana la intalnirea unui caracter
	//terminator ci pana cand e receptionat un anumit numar de caractere
	uart1_bRXBufferLimit = false;
	uart1_nIndexUltimTestat = 0;
	uart1_bTimerCounting = false;
	uart1_bTimeout = false;
	uart1_nContorTimeout = 0;
	uart1_bTimerCounting = true;
	uart1_bReceptioneaza = true;
	uart1_bBlocheazaReceptiaLaFinal = bBlocheazaReceptiaLaFinal;
	uart1_bSetareParametri = false;
}

/******************************************************************************/

/*
sReceptionat - receive buffer
cTermRX - terminal character. Reception ends when the terminal characher is received or the buffer length is reached.
nLungMaxBuferRX - max size of the input buffer
bReseteazaBuffIntrare
true = resets the internal counter to 0 when a receive command is issued, typical approach when an input message is expected to be received in one shot
false = does not reset the counter and the buffer is written from the last recorder position + 1. This is useful when receiving messages in multi-stages (e.g. header and body) and when the header requires its own processing.
bBlocheazaReceptiaLaFinal = blocks the method call until the a message is received. Usually not the case, since this would block the main loop. Typically set to false and poling is performed to check the if a message has been received using Receptie_OK()...
*/
void uart1_ReceptieTermTX(
char *sReceptionat,
char cTermRX,
unsigned int nLungMaxBuferRX,
bool bReseteazaBuffIntrare,
bool bBlocheazaReceptiaLaFinal)
{
	WDR();
	uart1_bSetareParametri = true;
	uart1_sReceptionat = sReceptionat;
	uart1_nLungimeRX = 0;
	uart1_cTermRX = cTermRX;
	uart1_nCriteriiTestRX = 0;
	if(bReseteazaBuffIntrare)
	uart1_nContorRX = 0;
	uart1_nLungMaxBuferRX = nLungMaxBuferRX;
	//Setez lungimea maxima a bufferului de receptie. Este utila in cazul in care
	//caracterul terminator nu este receptionat eliminandu-se riscul suprascrierii
	//unei zone de memorire nealocata.
	uart1_bRxOK = false;
	uart1_bStareNeprocesata = false;
	uart1_bRecCuTermRX = true;
	//indic faptul ca receptia NU se face  pana la intalnirea unui caracter
	//terminator ci pana cand e receptionat un anumit numar de caractere
	uart1_bRXBufferLimit = false;
	uart1_nIndexUltimTestat = 0;
	uart1_bTimerCounting = false;
	uart1_bTimeout = false;
	uart1_nContorTimeout = 0;
	uart1_bTimerCounting = true;
	uart1_bReceptioneaza = true;
	uart1_bBlocheazaReceptiaLaFinal = bBlocheazaReceptiaLaFinal;
	uart1_bSetareParametri = false;
}


void uart1_ReceptieLungSiTermTX(
char *sReceptionat,
char *sMultiTermRX,
unsigned int *sMultiLungimeRX,
char nCriteriiTestRX,
unsigned int nLungMaxBuferRX,
bool bReseteazaBuffIntrare,
bool bBlocheazaReceptiaLaFinal)
{
	char nAux;
	
	//Se receptioneaza caractere pana cand unul se gaseste in sirul de caractere
	//terminatoare posibile (stabilite in *sTermRX)iar sirul receptionat are exact
	//lungimea stabilita in *sLungimi la acelasi index ca si caracterul terminator
	//pana cand CReceptionat == sTerm[i] && LungimeSir == sLungimi[i]
	//i=:1,nCriteriiTest  nCriteriiTest(1...255) cand nCriteriiTest=0 receptia
	//se face cu un singur test fie de caracter terminator fie de lungime (rutinele
	//uart#_ReceptieTermTX si uart#_ReceptieLunigme)
	//(de uart_nCriteriiTestRX ori se testeaza daca caracterul recptionat se
	//gaseste in sirul de caractere terminatoare si daca acest caracter
	//se gaseste la pozitia dorita.
	//Functia este utila atunci cand avem de receptionat mai multe siruri
	//de caractere cu lungimi fixe avand caractere terminatoare diferite
	//si mai ales atunci cand aceste caractere terminatoare se pot gasi
	//si in partea utila a sirului receptionat fara a avea insa semnificatia
	//de terminator
	//OBSERVATIE!!!
	//Daca in tabelul cu lungimi avem pozitii in care aceasta valoare este
	//0 atunci pentru caracterul corespunzator (de pe aceeasi pozitie) din
	//tabelul caracterelor terminatoare receptia se va face pana la prima
	//aparitie a caracterului fara a fi testata si pozitia acestuia - Se
	//implementeaza practic aceeasi functionalitate ca si in cazul rutinei
	//uart#_ReceptieTermTX
	
	/*
	Exemplu:
	
	char sTermRX[32]="%&/";
	char sPoz[32]={1,0,7};
	
	uart1_ReceptieLungSiTermTX(sSirA,sTermRX, sPoz, 3, 255);
	
	stuart1_1.bRxOK va fi setat atunci cand fie primul caracter introdus
	este '%' fie cand a fost receptionat '&' indiferent de pozitie fie cand
	al saptelea caracter receptionat a fost '/'
	*/

	WDR();

	if(uart1_bRS485)
	{
		//salvez valoarea portului prin care stabileste directia, resetand insa bitul care este
		//alocat semnalului de directie. Fac asta pentru a permite operatiile logice de mai jos
		nAux = *uart1_Port485Dir & ~(uart1_Port485DirMask);

		//Setez in 1 pinul care seteaza directia sau il las in zero (vezi comanda de mai sus)
		//daca semnalul de comanda trebuie inversat).
		*uart1_Port485Dir = nAux | (uart1_bRS485Inversat?uart1_Port485DirMask:0x00);
	}
	
	uart1_bSetareParametri = true;
	uart1_sReceptionat = sReceptionat;
	uart1_nLungimeRX = 0;
	uart1_nCriteriiTestRX = nCriteriiTestRX;
	uart1_sMultiTermRX = sMultiTermRX;
	uart1_sMultiLungimeRX = sMultiLungimeRX;
	if(bReseteazaBuffIntrare)
	uart1_nContorRX = 0;
	uart1_nLungMaxBuferRX = nLungMaxBuferRX;
	//Setez lungimea maxima a bufferului de receptie. Este utila in cazul in care
	//caracterul terminator nu este receptionat eliminandu-se riscul suprascrierii
	//unei zone de memorire nealocata.
	uart1_bRxOK = false;
	uart1_bStareNeprocesata = false;
	uart1_bRecCuTermRX = true;
	//indic faptul ca receptia NU se face  pana la intalnirea unui caracter
	//terminator ci pana cand e receptionat un anumit numar de caractere
	uart1_bRXBufferLimit = false;
	uart1_nIndexUltimTestat = 0;
	uart1_bTimerCounting = false;
	uart1_bTimeout = false;
	uart1_nContorTimeout = 0;
	uart1_bTimerCounting = true;
	uart1_bReceptioneaza = true;
	uart1_bBlocheazaReceptiaLaFinal = bBlocheazaReceptiaLaFinal;
	uart1_bSetareParametri = false;
}
#endif


#if (defined __ATMEGA2560)
//MD+ Acest comentariu nu trebuie inlaturat!!!
void uart2_init(volatile unsigned char *PORT485_DIR,
char Port485DirMask,
bool bRS485Inversat,
char paramUCSR0A,
char paramUCSR0C,
char paramUBRR0L,
char paramUBRR0H,
char paramUCSR0B,
unsigned int  nValoareTimeout,
unsigned int  nValoareTimeoutChar)
{
	uart2_sReceptionat = uart2_sVid;
	uart2_nContorRX = 0;
	uart2_nLungimeRX = 0;
	uart2_nLungMaxBuferRX = 1;

	UCSR2B = 0x00; //disable while setting baud rate
	UCSR2A = paramUCSR0A;
	UCSR2C = paramUCSR0C;
	UBRR2L = paramUBRR0L; //set baud rate lo
	UBRR2H = paramUBRR0H; //set baud rate hi
	UCSR2B = paramUCSR0B;
	uart2_nValoareTimeout = nValoareTimeout;
	uart2_nValoareTimeoutCar = nValoareTimeoutChar;
	
	uart2_bTransmite = false;
	
	UCSR2B ^= BIT(5); //Dezactivez bitul UDRIE
	
	//Daca pointerul catre portul care selecteaza directia de transmisie
	//pe RS485 este NULL se considera ca transmisia se face prin RS232 si ca
	//atare nu mai este necesara selectarea directiei driverului
	if(PORT485_DIR != NULL)
	{
		uart2_Port485Dir = PORT485_DIR;
		*(uart2_Port485Dir-1) = *(uart2_Port485Dir-1) | Port485DirMask;
		//Setez modul de functionare (IESIRE) pentru linia portului care stabileste
		//directia pe RS485. Se modifica DDRn care se afla la adresa imediat inferioara
		//registrului PORTn
		uart2_Port485DirMask = Port485DirMask;
		uart2_bRS485Inversat = bRS485Inversat;
		uart2_bRS485 = true;
	}
	else
	{
		uart2_bRS485 = false;
	}
	
	timer2_init();
}

/******************************************************************************/

#ifdef __ICCAVR
#pragma interrupt_handler uart2_udre_isr: uart2_UDRE_ISR_VECTOR_NO
void uart2_udre_isr(void)
#else
ISR(USART2_UDRE_vect)
#endif
{
	//character transferred to shift register so UDR is now empty
	
	WDR();
	uart2_nContorTX++;
	
	if( uart2_nContorTX == uart2_nLungimeTX)
	{
		//dezactivez intreruperea UDRE
		UCSR2B ^= BIT(5); //Dezactivez bitul UDRIE
	}
	else
	{
		UDR2 = *(uart2_sTransmis + uart2_nContorTX);
	}

}

/******************************************************************************/
#ifdef __ICCAVR
#pragma interrupt_handler uart2_tx_isr: uart2_TX_ISR_VECTOR_NO
void uart2_tx_isr(void)
#else
ISR(USART2_TX_vect)
#endif

{
	char nAux;

	WDR();
	//character has been transmitted
	if(uart2_nContorTX == uart2_nLungimeTX)
	{
		uart2_bTxOK = true;
		uart2_bTransmite = false; //Nu mai transmit
		
		//In momentul in care am transmis (prin RS-485) ultimul caracter din buffer
		//trec driverul in regim de ascultare schimband directia acestuia
		//resetez pinul de directie
		//
		// ATENTIE !!!
		//
		// Daca semnalul de selectie a directiei este inversat de catre optocuplor
		//comanda pentru stabilirea directiei este data in mod corespunzator in functie
		//de parametrul stuart_1.bRS485Inversat
		if(uart2_bRS485)
		{
			//salvez valoarea portului prin care stabileste directia, resetand insa bitul care este
			//alocat semnalului de directie. Fac asta pentru a permite operatiile logice de mai jos
			nAux = *uart2_Port485Dir & ~(uart2_Port485DirMask);

			//Setez in 1 pinul care seteaza directia sau il las in zero (vezi comanda de mai sus)
			//daca semnalul de comanda trebuie inversat).
			*uart2_Port485Dir = nAux | (uart2_bRS485Inversat?uart2_Port485DirMask:0x00);
		}

	}
}


/******************************************************************************/
#ifdef __ICCAVR
#pragma interrupt_handler uart2_rx_isr: uart2_RX_ISR_VECTOR_NO
void uart2_rx_isr(void)
#else
ISR(USART2_RX_vect)
#endif
{
	uart2_bTimerCarCounting = true;
	uart2_nContorTimeoutCar = 0;
	//Am receptionat un caracter pornesc timerul ca sa verific daca
	//pauze intre caractere - conform protocolului STOM in cadrul
	//unei telegrame nu pot aparea pauze de transmisie intre doua
	//caractere succesive. O astfel de pauza indica faptul ca
	//o telegrama (indiferent de emitatorul ei) s-a incheiat.
	//Corelata cu numarul de caractere receptionate si cu pozitia
	//acestora se poate obtine un criteriu de rejectie a telegramelor
	//care sunt transmise pe seriala dar care nu sunt considerate
	//ca fiind valide.

	WDR();
	//uart has received a character in UDR
	//if(!uart_bSetareParametri)
	uart2_sVid[0] = UDR2;

	if(uart2_bReceptioneaza)
	{
		*(uart2_sReceptionat + uart2_nContorRX) = uart2_sVid[0];

		if(uart2_nContorRX < uart2_nLungMaxBuferRX)
		{
			uart2_nContorRX++;
			//Incrementez numai daca sunt in domeniul de memorie alocat
			//previn astfel suprascrierile
		}
		else
		{
			uart2_nContorRX = uart2_nLungMaxBuferRX;	//ca sa iau si ultimul caracter
			uart2_bRXBufferLimit = true;
			uart2_bReceptioneaza = false;
			uart2_bStareNeprocesata = true;
			uart2_bTimerCounting = false;
		}
	}
}

/******************************************************************************/
void uart2_TransmiteExt(char *sTransmis, unsigned int nLungime, bool bAsteptareTxAnterioara, bool bAsteptareTxCurenta)
{
	char nAux;
	WDR();
	if(nLungime>0)
	{
		if( (bAsteptareTxAnterioara == true) &&  (uart2_bTransmite == true))
		{
			do	{//Asteapta transmisia anterrioare
				wdt_reset();
			}
			while(uart2_bTransmite);
		}
		uart2_bTransmite = true;
		uart2_sTransmis = sTransmis;
		//Memorez adresa primei locatii din bufferul de transmisie
		uart2_nContorTX = 0;
		//Resetez contorul care numara caracterele transmise
		uart2_bTxOK = false;
		//resetez flagul care indica ca trasmiterea caracterelor din buffer a fost
		//realizata
		uart2_nLungimeTX = nLungime;
		
		//Daca transmitz prin RS-232 trebuie sa setez directia de transmisie.
		//Directia este resetata (ascultam) in momentul in care am transmis ultimul
		//caracter din buffer
		// ATENTIE !!!
		//
		// Daca semnalul de selectie a directiei este inversat de catre optocuplor
		//comanda pentru stabilirea directiei este data in mod corespunzator in functie
		//de parametrul stuart2_1.bRS485Inversat
		
		
		if(uart2_bRS485)
		{
			//salvez valoarea portului prin care stabileste directia, resetand insa bitul care este
			//alocat semnalului de directie. Fac asta pentru a permite operatiile logice de mai jos
			nAux = *uart2_Port485Dir & ~(uart2_Port485DirMask);

			//Setez in 1 pinul care seteaza directia sau il las in zero (vezi comanda de mai sus)
			//daca semnalul de comanda trebuie inversat).
			*uart2_Port485Dir = nAux | (uart2_bRS485Inversat ?0x00:uart2_Port485DirMask);
		}

		UDR2 = *sTransmis;		   //Transmit primul caracter din buffer
		UCSR2B = UCSR2B | BIT(5); //Activez bitul UDRIE
		
		if( (bAsteptareTxCurenta == true) &&  (uart2_bTransmite == true))
		{
			do	{//Blocheaza metoda pana ce se transmite mesajul curent
				wdt_reset();
			}
			while(uart2_bTransmite);
		}
	}
}

void uart2_Transmite(char *sTransmis, unsigned int nLungime, bool bAsteptareTxAnterioara)
{
	uart2_TransmiteExt(sTransmis, nLungime, bAsteptareTxAnterioara, false);
}

/*****************************************************************************/


/*****************************************************************************/



//Rutina care imi verifica daca s-au inteplinit criteriiile de sfarsit de receptie
//Returneaza true daca receptia s-a inceiat false daca inca se mai asteapta caractere pe seriala.
bool uart2_ReceptieOK()
{
	
	char i;
	unsigned int nPozitie, nTamponContorRX;
	WDR();

	nTamponContorRX = uart2_nContorRX;

	for(nPozitie = uart2_nIndexUltimTestat; nPozitie < nTamponContorRX; nPozitie++)
	{
		WDR();
		
		//--------------------------------------
		if(uart2_nCriteriiTestRX != 0)
		{//Receptie cu mai multe criterii de stop ((caracter terminator SI lungime))
			//Corespunde apelului uart2_ReceptieLungSiTermTX
			for(i=0; i<uart2_nCriteriiTestRX; i++)
			{//iterez printere criteriile de test.
				WDR();

				if(*(uart2_sReceptionat + nPozitie) == *(uart2_sMultiTermRX + i))
				{//daca caracterul receptionat se gaseste in lista caracterelor
					//terminatoare testez daca se gaseste pe pozitia
					//corespunzatoare
					if(*(uart2_sMultiLungimeRX + i) == 0)
					{//Daca pozitia  asignata a fost 0 inseamna ca de fapt nu ma
						//intereseaza pozitia in care apare caracterul. si ca atare
						//receptia este OK

						uart2_nLungimeRX = nPozitie+1; //bufer dublu practic
						uart2_bTimerCounting = false;

						//Daca nu blochez receptia la finalul receptiei unei telegrame
						//(situatia intalnita in protocoalele in care se receptioneaza headerul
						//iar apoi se da comanda pentru receptia blocului de date) atunci voi
						//mentine activ mecanismul de timeout la nivel de caracter pana cand
						//o comanda (de obicei cea pentru receptia bloclui de date) care se
						//bloceze receptia la final este executata.
						//Astfel pot sa detectez timeout-ul care apare daca dupa header nu a
						//inceput receptia blocului de date.
						if(uart2_bBlocheazaReceptiaLaFinal == true)
						uart2_bTimerCarCounting = false;
						//Nu merge asa (mai jos) pentru toate cazurile deoarece caompilatorul da
						//urmatoarea eroare: internal error -- out of registers: opsize 2 free 300f0003 local cf0003fc free&local 0 TN type 5 ticks 8880.
						//uart2_bTimerCarCounting = !uart2_bBlocheazaReceptiaLaFinal;

						i = uart2_nCriteriiTestRX; //ies din ciclul de testare
						nPozitie = nTamponContorRX;	//ies din ciclul de testare mare ??
						uart2_bRxOK = true;
						uart2_bStareNeprocesata = true;
						uart2_bReceptioneaza = !uart2_bBlocheazaReceptiaLaFinal;
						uart2_nContorTimeout = 0;
					}
					else
					{//Daca pozitia  asignata a fost diferita de 0 inseamna ca ma
						//intereseaza pozitia in care apare caracterul si ca atare mai fac
						//un test in acest sens
						
						//ATENTIE - asa a fost testat si era stabil
						if(nTamponContorRX == *(uart2_sMultiLungimeRX + i))

						//Asa se pare ca da erori
						//if(nPozitie + 1 == *(uart2_sMultiLungimeRX + i))

						{//caracterul terminatort citit este la pozitisa dorita
							uart2_nLungimeRX = nPozitie+1; //bufer dublu practic
							uart2_bTimerCounting = false;
							
							//Daca nu blochez receptia la finalul receptiei unei telegrame
							//(situatia intalnita in protocoalele in care se receptioneaza headerul
							//iar apoi se da comanda pentru receptia blocului de date) atunci voi
							//mentine activ mecanismul de timeout la nivel de caracter pana cand
							//o comanda (de obicei cea pentru receptia bloclui de date) care se
							//bloceze receptia la final este executata.
							//Astfel pot sa detectez timeout-ul care apare daca dupa header nu a
							//inceput receptia blocului de date.
							if(uart2_bBlocheazaReceptiaLaFinal == true)
							uart2_bTimerCarCounting = false;

							
							i = uart2_nCriteriiTestRX; //ies din ciclul de testare mic
							nPozitie = nTamponContorRX;	//ies din ciclul de testare mare
							uart2_bRxOK = true;
							uart2_bStareNeprocesata = true;
							uart2_bReceptioneaza = !uart2_bBlocheazaReceptiaLaFinal;
							uart2_nContorTimeout = 0;
						}
					}
				} //end if
			} //end for
		} //end if
		else
		{//Receptie cu un singur criteriu de stop (caracter terminator SAU lungime)
			//Corespunde apelului uart2_ReceptieTermTX sau uart2_ReceptieLunigme
			if(uart2_bRecCuTermRX)
			{ //Receptie pana la intalnirea caracterului terminator
				if(*(uart2_sReceptionat + nPozitie) == uart2_cTermRX)
				{
					uart2_nLungimeRX = nPozitie+1;
					uart2_bTimerCounting = false;

					//Daca nu blochez receptia la finalul receptiei unei telegrame
					//(situatia intalnita in protocoalele in care se receptioneaza headerul
					//iar apoi se da comanda pentru receptia blocului de date) atunci voi
					//mentine activ mecanismul de timeout la nivel de caracter pana cand
					//o comanda (de obicei cea pentru receptia bloclui de date) care se
					//bloceze receptia la final este executata.
					//Astfel pot sa detectez timeout-ul care apare daca dupa header nu a
					//inceput receptia blocului de date.
					if(uart2_bBlocheazaReceptiaLaFinal == true)
					uart2_bTimerCarCounting = false;

					
					i = uart2_nCriteriiTestRX; //ies din ciclul de testare
					uart2_bRxOK = true;
					uart2_bStareNeprocesata = true;
					uart2_bReceptioneaza = !uart2_bBlocheazaReceptiaLaFinal;
					uart2_nContorTimeout = 0;
				}
			}
			else
			{ //Receptia unui numar fix de caractere
				if(nTamponContorRX >= uart2_nLungimeRX)
				{
					//uart2_nLungimeRX = nTampoContorRX; NU se aplica deaorece stiu dinainte lungimea
					uart2_bTimerCounting = false;

					//Daca nu blochez receptia la finalul receptiei unei telegrame
					//(situatia intalnita in protocoalele in care se receptioneaza headerul
					//iar apoi se da comanda pentru receptia blocului de date) atunci voi
					//mentine activ mecanismul de timeout la nivel de caracter pana cand
					//o comanda (de obicei cea pentru receptia bloclui de date) care se
					//bloceze receptia la final este executata.
					//Astfel pot sa detectez timeout-ul care apare daca dupa header nu a
					//inceput receptia blocului de date.
					if(uart2_bBlocheazaReceptiaLaFinal == true)
					uart2_bTimerCarCounting = false;


					i = uart2_nCriteriiTestRX; //ies din ciclul de testare
					uart2_bRxOK = true;
					uart2_bStareNeprocesata = true;
					uart2_bReceptioneaza = !uart2_bBlocheazaReceptiaLaFinal;
					uart2_nContorTimeout = 0;
				}
			}
		}
	}
	uart2_nIndexUltimTestat = nTamponContorRX;
	return uart2_bRxOK;
}


/******************************************************************************/
void uart2_ReceptieLFix( char *sReceptionat,
unsigned int nLungime,
unsigned int nLungMaxBuferRX,
bool bReseteazaBuffIntrare,
bool bBlocheazaReceptiaLaFinal)
{
	char nAux;
	WDR();
	if(uart2_bRS485)
	{
		//salvez valoarea portului prin care stabileste directia, resetand insa bitul care este
		//alocat semnalului de directie. Fac asta pentru a permite operatiile logice de mai jos
		nAux = *uart2_Port485Dir & ~(uart2_Port485DirMask);

		//Setez in 1 pinul care seteaza directia sau il las in zero (vezi comanda de mai sus)
		//daca semnalul de comanda trebuie inversat).
		*uart2_Port485Dir = nAux | (uart2_bRS485Inversat?uart2_Port485DirMask:0x00);
	}
	uart2_bSetareParametri = true;
	uart2_sReceptionat = sReceptionat;
	uart2_nLungimeRX = nLungime;
	uart2_nCriteriiTestRX = 0;
	if(bReseteazaBuffIntrare)
	uart2_nContorRX = 0;
	uart2_nLungMaxBuferRX = nLungMaxBuferRX;
	//Setez lungimea maxima a bufferului de receptie. Este utila in special in cazul
	//in care fac receptie cu terminator, dar si in cazul de fata este utila mai
	//ales pentru receptia in buffere multiple cu dimensiuni diferite
	//caracterul terminator nu este receptionat eliminandu-se riscul suprascrierii
	//unei zone de memorire nealocata.
	uart2_bRxOK = false;
	uart2_bStareNeprocesata = false;
	uart2_bRecCuTermRX = false;
	//indic faptul ca receptia NU se face  pana la intalnirea unui caracter
	//terminator ci pana cand e receptionat un anumit numar de caractere
	uart2_bRXBufferLimit = false;
	uart2_nIndexUltimTestat = 0;
	uart2_bTimerCounting = false;
	uart2_bTimeout = false;
	uart2_nContorTimeout = 0;
	uart2_bTimerCounting = true;
	uart2_bReceptioneaza = true;
	uart2_bBlocheazaReceptiaLaFinal = bBlocheazaReceptiaLaFinal;
	uart2_bSetareParametri = false;
}

/******************************************************************************/

/*
sReceptionat - receive buffer
cTermRX - terminal character. Reception ends when the terminal characher is received or the buffer length is reached.
nLungMaxBuferRX - max size of the input buffer
bReseteazaBuffIntrare
true = resets the internal counter to 0 when a receive command is issued, typical approach when an input message is expected to be received in one shot
false = does not reset the counter and the buffer is written from the last recorder position + 1. This is useful when receiving messages in multi-stages (e.g. header and body) and when the header requires its own processing.
bBlocheazaReceptiaLaFinal = blocks the method call until the a message is received. Usually not the case, since this would block the main loop. Typically set to false and poling is performed to check the if a message has been received using Receptie_OK()...
*/
void uart2_ReceptieTermTX(
char *sReceptionat,
char cTermRX,
unsigned int nLungMaxBuferRX,
bool bReseteazaBuffIntrare,
bool bBlocheazaReceptiaLaFinal)
{
	WDR();
	uart2_bSetareParametri = true;
	uart2_sReceptionat = sReceptionat;
	uart2_nLungimeRX = 0;
	uart2_cTermRX = cTermRX;
	uart2_nCriteriiTestRX = 0;
	if(bReseteazaBuffIntrare)
	uart2_nContorRX = 0;
	uart2_nLungMaxBuferRX = nLungMaxBuferRX;
	//Setez lungimea maxima a bufferului de receptie. Este utila in cazul in care
	//caracterul terminator nu este receptionat eliminandu-se riscul suprascrierii
	//unei zone de memorire nealocata.
	uart2_bRxOK = false;
	uart2_bStareNeprocesata = false;
	uart2_bRecCuTermRX = true;
	//indic faptul ca receptia NU se face  pana la intalnirea unui caracter
	//terminator ci pana cand e receptionat un anumit numar de caractere
	uart2_bRXBufferLimit = false;
	uart2_nIndexUltimTestat = 0;
	uart2_bTimerCounting = false;
	uart2_bTimeout = false;
	uart2_nContorTimeout = 0;
	uart2_bTimerCounting = true;
	uart2_bReceptioneaza = true;
	uart2_bBlocheazaReceptiaLaFinal = bBlocheazaReceptiaLaFinal;
	uart2_bSetareParametri = false;
}


void uart2_ReceptieLungSiTermTX(
char *sReceptionat,
char *sMultiTermRX,
unsigned int *sMultiLungimeRX,
char nCriteriiTestRX,
unsigned int nLungMaxBuferRX,
bool bReseteazaBuffIntrare,
bool bBlocheazaReceptiaLaFinal)
{
	char nAux;
	
	//Se receptioneaza caractere pana cand unul se gaseste in sirul de caractere
	//terminatoare posibile (stabilite in *sTermRX)iar sirul receptionat are exact
	//lungimea stabilita in *sLungimi la acelasi index ca si caracterul terminator
	//pana cand CReceptionat == sTerm[i] && LungimeSir == sLungimi[i]
	//i=:1,nCriteriiTest  nCriteriiTest(1...255) cand nCriteriiTest=0 receptia
	//se face cu un singur test fie de caracter terminator fie de lungime (rutinele
	//uart#_ReceptieTermTX si uart#_ReceptieLunigme)
	//(de uart_nCriteriiTestRX ori se testeaza daca caracterul recptionat se
	//gaseste in sirul de caractere terminatoare si daca acest caracter
	//se gaseste la pozitia dorita.
	//Functia este utila atunci cand avem de receptionat mai multe siruri
	//de caractere cu lungimi fixe avand caractere terminatoare diferite
	//si mai ales atunci cand aceste caractere terminatoare se pot gasi
	//si in partea utila a sirului receptionat fara a avea insa semnificatia
	//de terminator
	//OBSERVATIE!!!
	//Daca in tabelul cu lungimi avem pozitii in care aceasta valoare este
	//0 atunci pentru caracterul corespunzator (de pe aceeasi pozitie) din
	//tabelul caracterelor terminatoare receptia se va face pana la prima
	//aparitie a caracterului fara a fi testata si pozitia acestuia - Se
	//implementeaza practic aceeasi functionalitate ca si in cazul rutinei
	//uart#_ReceptieTermTX
	
	/*
	Exemplu:
	
	char sTermRX[32]="%&/";
	char sPoz[32]={1,0,7};
	
	uart2_ReceptieLungSiTermTX(sSirA,sTermRX, sPoz, 3, 255);
	
	stuart2_1.bRxOK va fi setat atunci cand fie primul caracter introdus
	este '%' fie cand a fost receptionat '&' indiferent de pozitie fie cand
	al saptelea caracter receptionat a fost '/'
	*/

	WDR();

	if(uart2_bRS485)
	{
		//salvez valoarea portului prin care stabileste directia, resetand insa bitul care este
		//alocat semnalului de directie. Fac asta pentru a permite operatiile logice de mai jos
		nAux = *uart2_Port485Dir & ~(uart2_Port485DirMask);

		//Setez in 1 pinul care seteaza directia sau il las in zero (vezi comanda de mai sus)
		//daca semnalul de comanda trebuie inversat).
		*uart2_Port485Dir = nAux | (uart2_bRS485Inversat?uart2_Port485DirMask:0x00);
	}
	
	uart2_bSetareParametri = true;
	uart2_sReceptionat = sReceptionat;
	uart2_nLungimeRX = 0;
	uart2_nCriteriiTestRX = nCriteriiTestRX;
	uart2_sMultiTermRX = sMultiTermRX;
	uart2_sMultiLungimeRX = sMultiLungimeRX;
	if(bReseteazaBuffIntrare)
	uart2_nContorRX = 0;
	uart2_nLungMaxBuferRX = nLungMaxBuferRX;
	//Setez lungimea maxima a bufferului de receptie. Este utila in cazul in care
	//caracterul terminator nu este receptionat eliminandu-se riscul suprascrierii
	//unei zone de memorire nealocata.
	uart2_bRxOK = false;
	uart2_bStareNeprocesata = false;
	uart2_bRecCuTermRX = true;
	//indic faptul ca receptia NU se face  pana la intalnirea unui caracter
	//terminator ci pana cand e receptionat un anumit numar de caractere
	uart2_bRXBufferLimit = false;
	uart2_nIndexUltimTestat = 0;
	uart2_bTimerCounting = false;
	uart2_bTimeout = false;
	uart2_nContorTimeout = 0;
	uart2_bTimerCounting = true;
	uart2_bReceptioneaza = true;
	uart2_bBlocheazaReceptiaLaFinal = bBlocheazaReceptiaLaFinal;
	uart2_bSetareParametri = false;
}
#endif


#if (defined __ATMEGA2560)
//MD+ Acest comentariu nu trebuie inlaturat!!!
void uart3_init(volatile unsigned char *PORT485_DIR,
char Port485DirMask,
bool bRS485Inversat,
char paramUCSR0A,
char paramUCSR0C,
char paramUBRR0L,
char paramUBRR0H,
char paramUCSR0B,
unsigned int  nValoareTimeout,
unsigned int  nValoareTimeoutChar)
{
	uart3_sReceptionat = uart3_sVid;
	uart3_nContorRX = 0;
	uart3_nLungimeRX = 0;
	uart3_nLungMaxBuferRX = 1;

	UCSR3B = 0x00; //disable while setting baud rate
	UCSR3A = paramUCSR0A;
	UCSR3C = paramUCSR0C;
	UBRR3L = paramUBRR0L; //set baud rate lo
	UBRR3H = paramUBRR0H; //set baud rate hi
	UCSR3B = paramUCSR0B;
	uart3_nValoareTimeout = nValoareTimeout;
	uart3_nValoareTimeoutCar = nValoareTimeoutChar;
	
	uart3_bTransmite = false;
	
	UCSR3B ^= BIT(5); //Dezactivez bitul UDRIE
	
	//Daca pointerul catre portul care selecteaza directia de transmisie
	//pe RS485 este NULL se considera ca transmisia se face prin RS232 si ca
	//atare nu mai este necesara selectarea directiei driverului
	if(PORT485_DIR != NULL)
	{
		uart3_Port485Dir = PORT485_DIR;
		*(uart3_Port485Dir-1) = *(uart3_Port485Dir-1) | Port485DirMask;
		//Setez modul de functionare (IESIRE) pentru linia portului care stabileste
		//directia pe RS485. Se modifica DDRn care se afla la adresa imediat inferioara
		//registrului PORTn
		uart3_Port485DirMask = Port485DirMask;
		uart3_bRS485Inversat = bRS485Inversat;
		uart3_bRS485 = true;
	}
	else
	{
		uart3_bRS485 = false;
	}
	
	timer2_init();
}

/******************************************************************************/

#ifdef __ICCAVR
#pragma interrupt_handler uart3_udre_isr: uart3_UDRE_ISR_VECTOR_NO
void uart3_udre_isr(void)
#else
ISR(USART3_UDRE_vect)
#endif
{
	//character transferred to shift register so UDR is now empty
	
	WDR();
	uart3_nContorTX++;
	
	if( uart3_nContorTX == uart3_nLungimeTX)
	{
		//dezactivez intreruperea UDRE
		UCSR3B ^= BIT(5); //Dezactivez bitul UDRIE
	}
	else
	{
		UDR0 = *(uart3_sTransmis + uart3_nContorTX);
	}

}

/******************************************************************************/
#ifdef __ICCAVR
#pragma interrupt_handler uart3_tx_isr: uart3_TX_ISR_VECTOR_NO
void uart3_tx_isr(void)
#else
ISR(USART3_TX_vect)
#endif

{
	char nAux;

	WDR();
	//character has been transmitted
	if(uart3_nContorTX == uart3_nLungimeTX)
	{
		uart3_bTxOK = true;
		uart3_bTransmite = false; //Nu mai transmit
		
		//In momentul in care am transmis (prin RS-485) ultimul caracter din buffer
		//trec driverul in regim de ascultare schimband directia acestuia
		//resetez pinul de directie
		//
		// ATENTIE !!!
		//
		// Daca semnalul de selectie a directiei este inversat de catre optocuplor
		//comanda pentru stabilirea directiei este data in mod corespunzator in functie
		//de parametrul stuart_1.bRS485Inversat
		if(uart3_bRS485)
		{
			//salvez valoarea portului prin care stabileste directia, resetand insa bitul care este
			//alocat semnalului de directie. Fac asta pentru a permite operatiile logice de mai jos
			nAux = *uart3_Port485Dir & ~(uart3_Port485DirMask);

			//Setez in 1 pinul care seteaza directia sau il las in zero (vezi comanda de mai sus)
			//daca semnalul de comanda trebuie inversat).
			*uart3_Port485Dir = nAux | (uart3_bRS485Inversat?uart3_Port485DirMask:0x00);
		}

	}
}


/******************************************************************************/
#ifdef __ICCAVR
#pragma interrupt_handler uart3_rx_isr: uart3_RX_ISR_VECTOR_NO
void uart3_rx_isr(void)
#else
ISR(USART3_RX_vect)
#endif
{
	uart3_bTimerCarCounting = true;
	uart3_nContorTimeoutCar = 0;
	//Am receptionat un caracter pornesc timerul ca sa verific daca
	//pauze intre caractere - conform protocolului STOM in cadrul
	//unei telegrame nu pot aparea pauze de transmisie intre doua
	//caractere succesive. O astfel de pauza indica faptul ca
	//o telegrama (indiferent de emitatorul ei) s-a incheiat.
	//Corelata cu numarul de caractere receptionate si cu pozitia
	//acestora se poate obtine un criteriu de rejectie a telegramelor
	//care sunt transmise pe seriala dar care nu sunt considerate
	//ca fiind valide.

	WDR();
	//uart has received a character in UDR
	//if(!uart_bSetareParametri)
	uart3_sVid[0] = UDR3;

	if(uart3_bReceptioneaza)
	{
		*(uart3_sReceptionat + uart3_nContorRX) = uart3_sVid[0];

		if(uart3_nContorRX < uart3_nLungMaxBuferRX)
		{
			uart3_nContorRX++;
			//Incrementez numai daca sunt in domeniul de memorie alocat
			//previn astfel suprascrierile
		}
		else
		{
			uart3_nContorRX = uart3_nLungMaxBuferRX;	//ca sa iau si ultimul caracter
			uart3_bRXBufferLimit = true;
			uart3_bReceptioneaza = false;
			uart3_bStareNeprocesata = true;
			uart3_bTimerCounting = false;
		}
	}
}

/******************************************************************************/
void uart3_TransmiteExt(char *sTransmis, unsigned int nLungime, bool bAsteptareTxAnterioara, bool bAsteptareTxCurenta)
{
	char nAux;
	WDR();
	if(nLungime>0)
	{
		if( (bAsteptareTxAnterioara == true) &&  (uart3_bTransmite == true))
		{
			do	{//Asteapta transmisia anterrioare
				wdt_reset();
			}
			while(uart3_bTransmite);
		}
		uart3_bTransmite = true;
		uart3_sTransmis = sTransmis;
		//Memorez adresa primei locatii din bufferul de transmisie
		uart3_nContorTX = 0;
		//Resetez contorul care numara caracterele transmise
		uart3_bTxOK = false;
		//resetez flagul care indica ca trasmiterea caracterelor din buffer a fost
		//realizata
		uart3_nLungimeTX = nLungime;
		
		//Daca transmitz prin RS-232 trebuie sa setez directia de transmisie.
		//Directia este resetata (ascultam) in momentul in care am transmis ultimul
		//caracter din buffer
		// ATENTIE !!!
		//
		// Daca semnalul de selectie a directiei este inversat de catre optocuplor
		//comanda pentru stabilirea directiei este data in mod corespunzator in functie
		//de parametrul stuart3_1.bRS485Inversat
		
		
		if(uart3_bRS485)
		{
			//salvez valoarea portului prin care stabileste directia, resetand insa bitul care este
			//alocat semnalului de directie. Fac asta pentru a permite operatiile logice de mai jos
			nAux = *uart3_Port485Dir & ~(uart3_Port485DirMask);

			//Setez in 1 pinul care seteaza directia sau il las in zero (vezi comanda de mai sus)
			//daca semnalul de comanda trebuie inversat).
			*uart3_Port485Dir = nAux | (uart3_bRS485Inversat ?0x00:uart3_Port485DirMask);
		}

		UDR3 = *sTransmis;		   //Transmit primul caracter din buffer
		UCSR3B = UCSR3B | BIT(5); //Activez bitul UDRIE
		
		if( (bAsteptareTxCurenta == true) &&  (uart3_bTransmite == true))
		{
			do	{//Blocheaza metoda pana ce se transmite mesajul curent
				wdt_reset();
			}
			while(uart3_bTransmite);
		}
	}
}

void uart3_Transmite(char *sTransmis, unsigned int nLungime, bool bAsteptareTxAnterioara)
{
	uart3_TransmiteExt(sTransmis, nLungime, bAsteptareTxAnterioara, false);
}


/*****************************************************************************/


/*****************************************************************************/



//Rutina care imi verifica daca s-au inteplinit criteriiile de sfarsit de receptie
//Returneaza true daca receptia s-a inceiat false daca inca se mai asteapta caractere pe seriala.
bool uart3_ReceptieOK()
{
	
	char i;
	unsigned int nPozitie, nTamponContorRX;
	WDR();

	nTamponContorRX = uart3_nContorRX;

	for(nPozitie = uart3_nIndexUltimTestat; nPozitie < nTamponContorRX; nPozitie++)
	{
		WDR();
		
		//--------------------------------------
		if(uart3_nCriteriiTestRX != 0)
		{//Receptie cu mai multe criterii de stop ((caracter terminator SI lungime))
			//Corespunde apelului uart3_ReceptieLungSiTermTX
			for(i=0; i<uart3_nCriteriiTestRX; i++)
			{//iterez printere criteriile de test.
				WDR();

				if(*(uart3_sReceptionat + nPozitie) == *(uart3_sMultiTermRX + i))
				{//daca caracterul receptionat se gaseste in lista caracterelor
					//terminatoare testez daca se gaseste pe pozitia
					//corespunzatoare
					if(*(uart3_sMultiLungimeRX + i) == 0)
					{//Daca pozitia  asignata a fost 0 inseamna ca de fapt nu ma
						//intereseaza pozitia in care apare caracterul. si ca atare
						//receptia este OK

						uart3_nLungimeRX = nPozitie+1; //bufer dublu practic
						uart3_bTimerCounting = false;

						//Daca nu blochez receptia la finalul receptiei unei telegrame
						//(situatia intalnita in protocoalele in care se receptioneaza headerul
						//iar apoi se da comanda pentru receptia blocului de date) atunci voi
						//mentine activ mecanismul de timeout la nivel de caracter pana cand
						//o comanda (de obicei cea pentru receptia bloclui de date) care se
						//bloceze receptia la final este executata.
						//Astfel pot sa detectez timeout-ul care apare daca dupa header nu a
						//inceput receptia blocului de date.
						if(uart3_bBlocheazaReceptiaLaFinal == true)
						uart3_bTimerCarCounting = false;
						//Nu merge asa (mai jos) pentru toate cazurile deoarece caompilatorul da
						//urmatoarea eroare: internal error -- out of registers: opsize 2 free 300f0003 local cf0003fc free&local 0 TN type 5 ticks 8880.
						//uart3_bTimerCarCounting = !uart3_bBlocheazaReceptiaLaFinal;

						i = uart3_nCriteriiTestRX; //ies din ciclul de testare
						nPozitie = nTamponContorRX;	//ies din ciclul de testare mare ??
						uart3_bRxOK = true;
						uart3_bStareNeprocesata = true;
						uart3_bReceptioneaza = !uart3_bBlocheazaReceptiaLaFinal;
						uart3_nContorTimeout = 0;
					}
					else
					{//Daca pozitia  asignata a fost diferita de 0 inseamna ca ma
						//intereseaza pozitia in care apare caracterul si ca atare mai fac
						//un test in acest sens
						
						//ATENTIE - asa a fost testat si era stabil
						if(nTamponContorRX == *(uart3_sMultiLungimeRX + i))

						//Asa se pare ca da erori
						//if(nPozitie + 1 == *(uart3_sMultiLungimeRX + i))

						{//caracterul terminatort citit este la pozitisa dorita
							uart3_nLungimeRX = nPozitie+1; //bufer dublu practic
							uart3_bTimerCounting = false;
							
							//Daca nu blochez receptia la finalul receptiei unei telegrame
							//(situatia intalnita in protocoalele in care se receptioneaza headerul
							//iar apoi se da comanda pentru receptia blocului de date) atunci voi
							//mentine activ mecanismul de timeout la nivel de caracter pana cand
							//o comanda (de obicei cea pentru receptia bloclui de date) care se
							//bloceze receptia la final este executata.
							//Astfel pot sa detectez timeout-ul care apare daca dupa header nu a
							//inceput receptia blocului de date.
							if(uart3_bBlocheazaReceptiaLaFinal == true)
							uart3_bTimerCarCounting = false;

							
							i = uart3_nCriteriiTestRX; //ies din ciclul de testare mic
							nPozitie = nTamponContorRX;	//ies din ciclul de testare mare
							uart3_bRxOK = true;
							uart3_bStareNeprocesata = true;
							uart3_bReceptioneaza = !uart3_bBlocheazaReceptiaLaFinal;
							uart3_nContorTimeout = 0;
						}
					}
				} //end if
			} //end for
		} //end if
		else
		{//Receptie cu un singur criteriu de stop (caracter terminator SAU lungime)
			//Corespunde apelului uart3_ReceptieTermTX sau uart3_ReceptieLunigme
			if(uart3_bRecCuTermRX)
			{ //Receptie pana la intalnirea caracterului terminator
				if(*(uart3_sReceptionat + nPozitie) == uart3_cTermRX)
				{
					uart3_nLungimeRX = nPozitie+1;
					uart3_bTimerCounting = false;

					//Daca nu blochez receptia la finalul receptiei unei telegrame
					//(situatia intalnita in protocoalele in care se receptioneaza headerul
					//iar apoi se da comanda pentru receptia blocului de date) atunci voi
					//mentine activ mecanismul de timeout la nivel de caracter pana cand
					//o comanda (de obicei cea pentru receptia bloclui de date) care se
					//bloceze receptia la final este executata.
					//Astfel pot sa detectez timeout-ul care apare daca dupa header nu a
					//inceput receptia blocului de date.
					if(uart3_bBlocheazaReceptiaLaFinal == true)
					uart3_bTimerCarCounting = false;

					
					i = uart3_nCriteriiTestRX; //ies din ciclul de testare
					uart3_bRxOK = true;
					uart3_bStareNeprocesata = true;
					uart3_bReceptioneaza = !uart3_bBlocheazaReceptiaLaFinal;
					uart3_nContorTimeout = 0;
				}
			}
			else
			{ //Receptia unui numar fix de caractere
				if(nTamponContorRX >= uart3_nLungimeRX)
				{
					//uart3_nLungimeRX = nTampoContorRX; NU se aplica deaorece stiu dinainte lungimea
					uart3_bTimerCounting = false;

					//Daca nu blochez receptia la finalul receptiei unei telegrame
					//(situatia intalnita in protocoalele in care se receptioneaza headerul
					//iar apoi se da comanda pentru receptia blocului de date) atunci voi
					//mentine activ mecanismul de timeout la nivel de caracter pana cand
					//o comanda (de obicei cea pentru receptia bloclui de date) care se
					//bloceze receptia la final este executata.
					//Astfel pot sa detectez timeout-ul care apare daca dupa header nu a
					//inceput receptia blocului de date.
					if(uart3_bBlocheazaReceptiaLaFinal == true)
					uart3_bTimerCarCounting = false;


					i = uart3_nCriteriiTestRX; //ies din ciclul de testare
					uart3_bRxOK = true;
					uart3_bStareNeprocesata = true;
					uart3_bReceptioneaza = !uart3_bBlocheazaReceptiaLaFinal;
					uart3_nContorTimeout = 0;
				}
			}
		}
	}
	uart3_nIndexUltimTestat = nTamponContorRX;
	return uart3_bRxOK;
}


/******************************************************************************/
void uart3_ReceptieLFix( char *sReceptionat,
unsigned int nLungime,
unsigned int nLungMaxBuferRX,
bool bReseteazaBuffIntrare,
bool bBlocheazaReceptiaLaFinal)
{
	char nAux;
	WDR();
	if(uart3_bRS485)
	{
		//salvez valoarea portului prin care stabileste directia, resetand insa bitul care este
		//alocat semnalului de directie. Fac asta pentru a permite operatiile logice de mai jos
		nAux = *uart3_Port485Dir & ~(uart3_Port485DirMask);

		//Setez in 1 pinul care seteaza directia sau il las in zero (vezi comanda de mai sus)
		//daca semnalul de comanda trebuie inversat).
		*uart3_Port485Dir = nAux | (uart3_bRS485Inversat?uart3_Port485DirMask:0x00);
	}
	uart3_bSetareParametri = true;
	uart3_sReceptionat = sReceptionat;
	uart3_nLungimeRX = nLungime;
	uart3_nCriteriiTestRX = 0;
	if(bReseteazaBuffIntrare)
	uart3_nContorRX = 0;
	uart3_nLungMaxBuferRX = nLungMaxBuferRX;
	//Setez lungimea maxima a bufferului de receptie. Este utila in special in cazul
	//in care fac receptie cu terminator, dar si in cazul de fata este utila mai
	//ales pentru receptia in buffere multiple cu dimensiuni diferite
	//caracterul terminator nu este receptionat eliminandu-se riscul suprascrierii
	//unei zone de memorire nealocata.
	uart3_bRxOK = false;
	uart3_bStareNeprocesata = false;
	uart3_bRecCuTermRX = false;
	//indic faptul ca receptia NU se face  pana la intalnirea unui caracter
	//terminator ci pana cand e receptionat un anumit numar de caractere
	uart3_bRXBufferLimit = false;
	uart3_nIndexUltimTestat = 0;
	uart3_bTimerCounting = false;
	uart3_bTimeout = false;
	uart3_nContorTimeout = 0;
	uart3_bTimerCounting = true;
	uart3_bReceptioneaza = true;
	uart3_bBlocheazaReceptiaLaFinal = bBlocheazaReceptiaLaFinal;
	uart3_bSetareParametri = false;
}

/******************************************************************************/

/*
sReceptionat - receive buffer
cTermRX - terminal character. Reception ends when the terminal characher is received or the buffer length is reached.
nLungMaxBuferRX - max size of the input buffer
bReseteazaBuffIntrare
true = resets the internal counter to 0 when a receive command is issued, typical approach when an input message is expected to be received in one shot
false = does not reset the counter and the buffer is written from the last recorder position + 1. This is useful when receiving messages in multi-stages (e.g. header and body) and when the header requires its own processing.
bBlocheazaReceptiaLaFinal = blocks the method call until the a message is received. Usually not the case, since this would block the main loop. Typically set to false and poling is performed to check the if a message has been received using Receptie_OK()...
*/
void uart3_ReceptieTermTX(
char *sReceptionat,
char cTermRX,
unsigned int nLungMaxBuferRX,
bool bReseteazaBuffIntrare,
bool bBlocheazaReceptiaLaFinal)
{
	WDR();
	uart3_bSetareParametri = true;
	uart3_sReceptionat = sReceptionat;
	uart3_nLungimeRX = 0;
	uart3_cTermRX = cTermRX;
	uart3_nCriteriiTestRX = 0;
	if(bReseteazaBuffIntrare)
	uart3_nContorRX = 0;
	uart3_nLungMaxBuferRX = nLungMaxBuferRX;
	//Setez lungimea maxima a bufferului de receptie. Este utila in cazul in care
	//caracterul terminator nu este receptionat eliminandu-se riscul suprascrierii
	//unei zone de memorire nealocata.
	uart3_bRxOK = false;
	uart3_bStareNeprocesata = false;
	uart3_bRecCuTermRX = true;
	//indic faptul ca receptia NU se face  pana la intalnirea unui caracter
	//terminator ci pana cand e receptionat un anumit numar de caractere
	uart3_bRXBufferLimit = false;
	uart3_nIndexUltimTestat = 0;
	uart3_bTimerCounting = false;
	uart3_bTimeout = false;
	uart3_nContorTimeout = 0;
	uart3_bTimerCounting = true;
	uart3_bReceptioneaza = true;
	uart3_bBlocheazaReceptiaLaFinal = bBlocheazaReceptiaLaFinal;
	uart3_bSetareParametri = false;
}


void uart3_ReceptieLungSiTermTX(
char *sReceptionat,
char *sMultiTermRX,
unsigned int *sMultiLungimeRX,
char nCriteriiTestRX,
unsigned int nLungMaxBuferRX,
bool bReseteazaBuffIntrare,
bool bBlocheazaReceptiaLaFinal)
{
	char nAux;
	
	//Se receptioneaza caractere pana cand unul se gaseste in sirul de caractere
	//terminatoare posibile (stabilite in *sTermRX)iar sirul receptionat are exact
	//lungimea stabilita in *sLungimi la acelasi index ca si caracterul terminator
	//pana cand CReceptionat == sTerm[i] && LungimeSir == sLungimi[i]
	//i=:1,nCriteriiTest  nCriteriiTest(1...255) cand nCriteriiTest=0 receptia
	//se face cu un singur test fie de caracter terminator fie de lungime (rutinele
	//uart#_ReceptieTermTX si uart#_ReceptieLunigme)
	//(de uart_nCriteriiTestRX ori se testeaza daca caracterul recptionat se
	//gaseste in sirul de caractere terminatoare si daca acest caracter
	//se gaseste la pozitia dorita.
	//Functia este utila atunci cand avem de receptionat mai multe siruri
	//de caractere cu lungimi fixe avand caractere terminatoare diferite
	//si mai ales atunci cand aceste caractere terminatoare se pot gasi
	//si in partea utila a sirului receptionat fara a avea insa semnificatia
	//de terminator
	//OBSERVATIE!!!
	//Daca in tabelul cu lungimi avem pozitii in care aceasta valoare este
	//0 atunci pentru caracterul corespunzator (de pe aceeasi pozitie) din
	//tabelul caracterelor terminatoare receptia se va face pana la prima
	//aparitie a caracterului fara a fi testata si pozitia acestuia - Se
	//implementeaza practic aceeasi functionalitate ca si in cazul rutinei
	//uart#_ReceptieTermTX
	
	/*
	Exemplu:
	
	char sTermRX[32]="%&/";
	char sPoz[32]={1,0,7};
	
	uart3_ReceptieLungSiTermTX(sSirA,sTermRX, sPoz, 3, 255);
	
	stuart3_1.bRxOK va fi setat atunci cand fie primul caracter introdus
	este '%' fie cand a fost receptionat '&' indiferent de pozitie fie cand
	al saptelea caracter receptionat a fost '/'
	*/

	WDR();

	if(uart3_bRS485)
	{
		//salvez valoarea portului prin care stabileste directia, resetand insa bitul care este
		//alocat semnalului de directie. Fac asta pentru a permite operatiile logice de mai jos
		nAux = *uart3_Port485Dir & ~(uart3_Port485DirMask);

		//Setez in 1 pinul care seteaza directia sau il las in zero (vezi comanda de mai sus)
		//daca semnalul de comanda trebuie inversat).
		*uart3_Port485Dir = nAux | (uart3_bRS485Inversat?uart3_Port485DirMask:0x00);
	}
	
	uart3_bSetareParametri = true;
	uart3_sReceptionat = sReceptionat;
	uart3_nLungimeRX = 0;
	uart3_nCriteriiTestRX = nCriteriiTestRX;
	uart3_sMultiTermRX = sMultiTermRX;
	uart3_sMultiLungimeRX = sMultiLungimeRX;
	if(bReseteazaBuffIntrare)
	uart3_nContorRX = 0;
	uart3_nLungMaxBuferRX = nLungMaxBuferRX;
	//Setez lungimea maxima a bufferului de receptie. Este utila in cazul in care
	//caracterul terminator nu este receptionat eliminandu-se riscul suprascrierii
	//unei zone de memorire nealocata.
	uart3_bRxOK = false;
	uart3_bStareNeprocesata = false;
	uart3_bRecCuTermRX = true;
	//indic faptul ca receptia NU se face  pana la intalnirea unui caracter
	//terminator ci pana cand e receptionat un anumit numar de caractere
	uart3_bRXBufferLimit = false;
	uart3_nIndexUltimTestat = 0;
	uart3_bTimerCounting = false;
	uart3_bTimeout = false;
	uart3_nContorTimeout = 0;
	uart3_bTimerCounting = true;
	uart3_bReceptioneaza = true;
	uart3_bBlocheazaReceptiaLaFinal = bBlocheazaReceptiaLaFinal;
	uart3_bSetareParametri = false;
}
#endif
