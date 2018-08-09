/*
* avr_usart.h
*
* Created: 01.01.2009
* Author : Bogdan Tarnauca
*/

//MD+ Acest comentariu nu trebuie inlaturat!!!

#ifndef _AVR_USART_H_
	#define _AVR_USART_H_

//Apps must include an "main_config.h" header where libraries related params, overrides, settings should be defined.
//This header file is application specific. Must be provided even if it is empty.
#include "main_config.h"
#include "avr_defines.h"
#include<avr/wdt.h>

/*
/////////////////////////////////////////////////////////////////////////////////////////////////
//
//This driver provides serial communication support for multiple AVR devices for 
//both RS232 and RS485 based drivers.
//These devices provide 1, 2 or 4 UARTs, therefore the driver has to be properly set-up.
//Supported devices are selected by defining one of the following symbols __ATMEGA8, __ATMEGA328, __ATMEGA128, __ATMEGA2560 
//inside the "main_config.h" file (e.g. #define __ATMEGA8 will select ATmega8 as a target). 
//
//"main_config.h" must be created in the project whenever drivers such as this one require it.
//
//IMPORTANT NOTE: This driver relies on Timer2 for timeout generation. When using this driver Timer2 
//must not be used for other purposes.
//
//The driver currently supports two MCU frequencies 11.059264 MHz and 16.000000 MHz for timeout definition.
//The values of the Timer2 registers are set for a 10ms timebase based on the following the F_CPU symbol. 
//Supported values are F_CPU = 11059200UL and F_CPU = 16000000UL. If F_CPU is not defined or is defined to another value, 
//the driver will default to 11059200UL
//
//Bellow is the description of all variables and functions.
//Since the same functionality is provided for each of the UARTs present on a device (when applicable)
//the variables and functions are multiplied accordingly. The description uses the uart<x>_ prefix 
//where x stands for the uart index (0,1,2 or 3 depending on the device).
//
//For historic reasons (limited support for packed structs in older compilers) it was decided to 
//drop the struct based grouping these variables and only add them in a flat manner as bellow.
//
//Also for historical reasons, variables function and parameter names are based in Romanian.
//So are the original comments and descriptions.
//
/////////////////////////////////////////////////////////////////////////////////////////////////


volatile char *uart<x>_Port485Dir;
//Pointer catre portul care selecteaza directia de transmisie pe RS485
//Daca este NULL se considera ca transmisia se face prin RS232 si ca
//atare nu mai este necesara selectarea directiei driverului

volatile char uart<x>_Port485DirMask;
//O masca care specifica care este linia portului care selecteaza
//directia de transmisie pe RS485

volatile char *uart<x>_sReceptionat;
//Pointer la sirul de caractere care a fost receptionat

volatile unsigned int uart<x>_nLungimeRX;
//Lungimea sirului de caractere care trebuie receptionat

volatile unsigned int uart<x>_nLungMaxBuferRX;
//Este un parametru care stabileste lungimea maxima a bufferului de
//intrare pentru functiile de citire. Trebuie sa coincida cu dimensiunea
//zonei de memorie alocata pentru buferul de receptie transmis ca
//parametru si previne suprascrierile in zonele nealocate.

volatile char uart<x>_cTermRX;
//Caracterul terminator pentru sirul de caractere care trebuie
//receptionat
//uart_nLungimeRX si uart_cTermRX se folosesc in regim de sau exclusiv:
//ori  avem receptie cu un numar fix de caractere ori receptie pana
//la intalnirea caracterului terminator

volatile unsigned int uart<x>_nContorRX;
//Contor care indica numarul de caractere receptionate la un moment dat


volatile unsigned int uart<x>_nIndexUltimTestat;
//indexul ultimului caracter testat in rutina mReceptieOK
//Pana la aceasta pozitie s-a verificat daca au fost indeplinite criteriile
//de sfarsit pentru receptia seriala. In acest mod in cazul in care au fost
//receptionate mai multe caractere in buffer fara sa apuc le testez o voi
//face incepand de la acest index si pana la uart_nContorRX

volatile char *uart<x>_sTransmis;
//Pointer la sirul de caractere care trebuie transmis

volatile unsigned int uart<x>_nLungimeTX;
//Lungimea sirului de caractere care trebuie transmis

volatile unsigned int uart<x>_nContorTX;
//Contor care indica numarul de caractere transmise la un moment dat

volatile char uart<x>_nCriteriiTestRX;
//indica numarul criteriilor de test pentru receptie. Daca
//	uart_nCriteriiTestRX = 0 insemna ca a a fost apelata una din rutinele
//uart#_ReceptieTermTX sau uart#_ReceptieLunigime existand un test doar
//pentru caracter terminator de receptie
// uart_nCriteriiTestRX >=1 insemna ca a a fost apelata
//uart#_ReceptieLungSiTermTX, rutina in care testul este multiplu
//(de uart_nCriteriiTestRX ori se testeaza daca caracterul recptionat se
//gaseste in sirul de caractere terminatoare si daca acest caracter
//se gaseste la pozitia dorita.
//Functia este utila atunci cand avem de receptionat mai multe siruri
//de caractere cu lungimi fixe avand caractere terminatoare diferite
//si mai ales atunci cand aceste caractere terminatoare se pot gasi
//si in partea utila a sirului receptionat fara a avea insa semnificatia
//de terminator
//Daca in tabelul cu lungimi avem pozitii in care aceasta valoare este
//0 atunci pentru caracterul corespunzator (de pe aceeasi pozitie) din
//tabelul caracterelor terminatoare receptia se va face pana la prima
//aparitie a caracterului fara a fi testata si pozitia acestuia - Se
//implementeaza practic aceeasi functionalitate ca si in cazul rutinei
//uart#_ReceptieTermTX

volatile unsigned int *uart<x>_sMultiLungimeRX;
//Sirul de caractere terminatroare pentru receptia de tip criteriu
//multiplu

volatile char *uart<x>_sMultiTermRX;
//Sirul de locatii ale caracterelor terminatroare in sirul receptionat
///pentru receptia de tip criteriu multiplu

volatile unsigned int uart<x>_nValoareTimeout;
//memoreaza numarul de perioade date de Timer0 care trebuie sa se scurga
//inainte de a fi generata starea de timeout
//valoarea timpului de garda Timeout este data de relatia
// uart_nValoareTimeout * 1/FTimer0
//unde FTimer0 este frecventa de aparitie a intreruperii
//timer0_ovf_isr(void) - 10ms

//Daca uart_nValoareTimeout are valoare 0 inseamna ca timeout-ul la nivel de
//telegrama este dezactivat. Aceasta functionalitate este necesara atunci
//cand sistemul este slave si asteapta comenzi de la un sistem master.
//Momentul la care apar comenzile nu este cunoscut, asadar sistemul poate
//astepta la nesfarsit fara a semnala time-out


volatile unsigned int uart<x>_nContorTimeout;
//memoreaza de cate ori a fost generata intreruperea Timer0
//Cat timp uart_nContorTimeout < uart_nValoareTimeout inseamna ca nu s-a atins
//starea de timeout

volatile unsigned int uart<x>_nValoareTimeoutCar;
//memoreaza numarul de perioade date de Timer0 care trebuie sa se scurga
//inainte de a fi generata starea de timeout
//valoarea timpului de garda Timeout este data de relatia
// uart_nValoareTimeoutCar * 1/FTimer0
//unde FTimer0 este frecventa de aparitie a intreruperii
//timer0_ovf_isr(void)

volatile unsigned int uart<x>_nContorTimeoutCar;
//memoreaza de cate ori a fost generata intreruperea Timer0
//Cat timp uart_nContorTimeoutCar < uart_nValoareTimeoutCar inseamna ca nu s-a atins
//starea de timeout la receptia unui caracter

volatile char uart<x>_bRS485;
volatile char uart<x>_bRS485Inversat;
volatile char uart<x>_bRecCuTermRX;
volatile char uart<x>_bReceptioneaza;
volatile char uart<x>_bRxOK;
volatile char uart<x>_bTransmite;
volatile char uart<x>_bTxOK;
volatile char uart<x>_bRXBufferLimit;
volatile char uart<x>_bTimeout;
volatile char uart<x>_bTimerCounting;
volatile char uart<x>_bTimeoutCar;
volatile char uart<x>_bTimerCarCounting;
volatile char uart<x>_bStareNeprocesata;
volatile char uart<x>_bSetareParametri;
volatile char uart<x>_bBlocheazaReceptiaLaFinal;

//bRS485 bit care indica daca transmisia se face folosind un driver
//			RS485 care necesita setarea directiei de transmisie sau daca
//			se foloseste un driver RS232
//bRS485Inversat bit care indica daca semnalul care stabileste
//			directia pe RS485 este inversat sau nu
//bRecCuTermRX;
//bRecCuTermRX bit care indica daca receptia unui sir de caractere
//			se face pana la receptia unui caracter terminator
//			(bRecCuTermRX = true) sau pana cand se receptioneaza un
//			numar fix de caractere (bRecCuTermRX = false).
//			Setarea parametrului se face in functiile de receptie
//bReceptioneaza bit care indica daca in momentul testarii valorii
//			acestuia se citeste ceva.
//bRxOK	bit care indica daca citirea fost incheiata cu succes
//bTransmite bit care indica daca in momentul testarii valorii
//			acestuia se scrie ceva
//bTxOK	bit care indica daca transmiterea fost incheiata cu succes
//bRXBufferLimit bit care indica eroare la receptie
//bTimeout bit care indica faptul ca a fost atinsa starea de timeout
//			pe receptie. Se foloseste Timer0 pentru a genereara timeout
//bTimeoutCaracter; bit care indica faptul ca dupa un carcacter receptionat
//			nu a mai fost receptionat un alt caracter in urmatoarele 4ms
//			Folosind acest indiciu se poate determina criteriul de excludere
//			al unor telegrame al caror criteriu de acceptare nu este cunoscut.
//bTimeCounting bit care este setat in momentul in care lansam rutina
//			de receptie si care are ca efect activarea incrementarii
//			contorului de timeout in cadrul rutinei timer-ului.
//bStareNeprocesata; bit care indica faptul ca o schimbare a flagurilor
//			de stare (bRxOK, bRXBufferLimit, bTimeout) nu a fost
//			tratata.
//			Acest flag este util in special in cazul in care in programul
//			principal receptia NU se face prin polling pe bRxOK intr-o bulca
//			imediat dupa apelul unei rutine de recptie ci in bucla infinita
//			globala (cea in care sunt incluse toate instructiunile programului)
//			Imediat dupa ce a fost tratata schimbarea de stare utilizatorul
//			trebuie sa reseteze acest bit.
//			OBSERVATIE: Daca se foloseste polling local (dupa o instructiune
//			de receptie atunci acest bit nu mai este folosit)
//bBlocheazaReceptiaLaFinal bit care indica daca dupa receptia unei telegrame
//			complete se blocheaza receptia pana in momentul in care este comandata
//          o noua receptie. MEcanism utila in cazul in care avem protocoale la care
//			este receptionat mai intai hederul, este prelucrat, iar apoi se
//			receptioneaza si un bloc de date

volatile char uart<x>_sVid[1];
//Acesta este un "sir" de caractere folosit la initializarea
//rutinei seriale pentru a preveni suprascrierea unor zone de memorie
//inainte ca uart_sReceptionat sa fie initializat de catre rutina de receptie
//Adresa la care se scriu datele receptionate este specificata in apelul
//rutinei de receptie. Inainte de un apel insa aceasta adresa nu este initializata
//si exista riscul ca o receptie inainte de initializarea lui uart_sReceptionat sa
//suprascrie zone de memorie si registri.
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////


*** @fn      uart<x>_init(volatile unsigned char *PORT485_DIR, char Port485DirMask, bool bRS485Inversat,
***          char paramUCSR0A, char paramUCSR0C, char paramUBRR0L, char paramUBRR0H, char paramUCSR0B,
***          unsigned int  nValoareTimeout, unsigned int  nValoareTimeoutChar)
*** @brief   This function initializes the uart. 
***
*** @param   PORT485_DIR address of the port whose pin is used for switching the RS485 driver direction. Use null if not applicable.
*** @param   Port485DirMask mask used to identify the RS485 direction switching pin. Use null if not applicable.
*** @param   bRS485Inversat indicates whether the switching pin logic is inverted (e.g. if optocoupled)
*** @param   paramUCSR0A value of the UCSR0A register for uart config. See MCU datasheet
*** @param   paramUCSR0C value of the UCSR0C register for uart config. See MCU datasheet
*** @param   paramUBRR0H value of the UBRR0H register for uart baudrate configuration. See MCU datasheet 
*** @param   paramUCSR0B value of the UCSR0B register for uart baudrate configuration. See MCU datasheet 
*** @param   nValoareTimeout the value (multiples of 10ms) defining the timeout for the data receive commands. 
***		     This is the timeout for receiving the whole message whatever the message received criteria is defined.
*** 		 If 0, no timeout will be generated (the system waits indefinitely for a message, slave mode)
*** @param	 nValoareTimeout the value (multiples of 10ms) defining the timeout for the data receive commands.  
***			 This is a character level timeout generated when two consecutive characters are not received within 
***			 the character timeout value. This functionality is used in rare occasions for some data protocols.
***          If 0, no timeout will be generated between characters
*** @return  none.
void uart<x>_init(volatile unsigned char *PORT485_DIR,
				char Port485DirMask,
				bool bRS485Inversat,
				char paramUCSR0A,
				char paramUCSR0C,
				char paramUBRR0L,
				char paramUBRR0H,
				char paramUCSR0B,
				unsigned int  nValoareTimeout,
				unsigned int  nValoareTimeoutChar);


*** @fn      uart<x>_Transmite(char *sTransmis, unsigned int nLungime, bool bAsteptareTxAnterioara)
***
*** @brief   Commence transmitting data. 
***
*** @param   sTransmis the Tx buffer storing the bytes to be transmitted 
*** @param   nLungime the number of bytes to be transmitted 
*** @param   bAsteptareTxAnterioara if true and a message transmission is already ongoing, the call 
***          will block until the previous message has been transmitted. 
***          If false and a message transmission is already ongoing, the call will reset the Tx
***          parameters, stop the previous transmission and commence this one.
***          Note that this function is otherwise not blocking.
*** @return  none.
void uart<x>_Transmite(char *sTransmis,
                unsigned int nLungime,
                bool bAsteptareTxAnterioara);


*** @fn      void uart<x>_TransmiteExt(char *sTransmis, unsigned int nLungime, bool bAsteptareTxAnterioara,
***          bool bAsteptareTxCurenta);
***
*** @brief   Commence transmitting data.
***
*** @param   sTransmis the Tx buffer storing the bytes to be transmitted
*** @param   nLungime the number of bytes to be transmitted
*** @param   bAsteptareTxAnterioara if true and a message transmission is already ongoing, the call
***          will block until the previous message has been transmitted.
***          If false and a message transmission is already ongoing, the call will reset the Tx
***          parameters, stop the previous transmission and commence this one.
*** @param   bAsteptareTxCurenta it true the function will block until the message is sent otherwise
***          it will exit and the transmission will continue in the "background"
*** @return  none.
void uart<x>_TransmiteExt(char *sTransmis, 
				unsigned int nLungime, 
				bool bAsteptareTxAnterioara, 
				bool bAsteptareTxCurenta);


*** @fn      uart<x>_ReceptieLFix(char *sReceptionat, unsigned int nLungime, unsigned int nLungMaxBuferRX, 
***          bool bReseteazaBuffIntrare, bool bBlocheazaReceptiaLaFinal)
***
*** @brief   Commence receiving data and wait for a fix number of bytes. Typically used for binary based protocols.
***          The function is not blocking. It only initiates the Rx end then exits. Periodic evaluation of 
***          the Rx status has to be performed by calling uart<x>_ReceptieOK
***
*** @param   sReceptionat the Rx buffer storing the received bytes
*** @param   nLungime the number of bytes to be received
*** @param   nLungMaxBuferRX the maximum number of bytes which can be stored into the Rx buffer. 
***          nLungMaxBuferRX >= nLungime. In some cases (messages consisting of a header and datablock)
***          we will wait for a fixed number of bytes (e.g. the header length) signal the receiving 
***          of these bytes through the Rx complete flag, but allow further bytes to be received into 
***          the Rx buffer (e.g. the datablock while the header is being processed)
*** @param   bReseteazaBuffIntrare if true this will reset the Rx buffer index (set it to 0) and commence 
***          receiving and storing data from the beginning (typically when a new message is to be received)
***          If false commence receiving bytes but start storing them form the index next to the last one
***          used in the previous Rx.
*** @param   bBlocheazaReceptiaLaFinal if true, after the specified number of bytes has been received
***          all other bytes received after will be discarded. If false, after the specified number of 
***          bytes has been received, if further bytes received they will not be discarded but stored 
***          as well into the Rx buffer.
***          For example, when implementing the Rx part of a fixed header + variable datablock protocol, 
***          we will first issue an Rx command for a fixed number of bytes with a reset input buffer and 
***          nonblocking mode, then, after the header has been received, we will issue an Rx command 
***          for the number of bytes in the datablock, without the reset of the input buffer (to preserve 
***          the content of the header). This way, while the header is being processed (e.g. determine 
***          the number of bytes for the datablock) we still allow some bytes of the datablock to be 
***          received
*** @return  none.
void uart<x>_ReceptieLFix(char *sReceptionat,
				unsigned int nLungime,
				unsigned int nLungMaxBuferRX,
				bool bReseteazaBuffIntrare,
				bool bBlocheazaReceptiaLaFinal);


*** @fn      void uart<x>_ReceptieTermTX(char *sReceptionat, char cTermRX, unsigned int nLungMaxBuferRX,
***          bool bReseteazaBuffIntrare, bool bBlocheazaReceptiaLaFinal)
***
*** @brief   Commence receiving data and wait for specific terminal char. Typically used for ASCII based protocols.
***
*** @param   sReceptionat the Rx buffer storing the received bytes
*** @param   cTermRX the terminal character which indicates a message has been received.
*** @param   nLungMaxBuferRX the maximum number of bytes which can be stored into the Rx buffer.
*** @param   bReseteazaBuffIntrare if true this will reset the Rx buffer index (set it to 0) and commence
***          receiving and storing data from the beginning (typically when a new message is to be received)
***          If false commence receiving bytes but start storing them form the index next to the last one
***          used in the previous Rx.
*** @param   bBlocheazaReceptiaLaFinal if true, after the specified number of bytes has been received
***          all other bytes received after will be discarded. If false, after the specified number of
***          bytes has been received, if further bytes received they will not be discarded but stored
***          as well into the Rx buffer.
***          For example, when implementing the Rx part of a fixed header + variable datablock protocol,
***          we will first issue an Rx command for a fixed number of bytes with a reset input buffer and
***          nonblocking mode, then, after the header has been received, we will issue an Rx command
***          for the number of bytes in the datablock, without the reset of the input buffer (to preserve
***          the content of the header). This way, while the header is being processed (e.g. determine
***          the number of bytes for the datablock) we still allow some bytes of the datablock to be
***          received
*** @return  none.
void uart<x>_ReceptieTermTX(char *sReceptionat,
				char cTermRX,
				unsigned int nLungMaxBuferRX,
				bool bReseteazaBuffIntrare,
				bool bBlocheazaReceptiaLaFinal);


*** @fn      void uart<x>_ReceptieLungSiTermTX(char *sReceptionat, char *sMultiTermRX, 
***          unsigned int *sMultiLungimeRX, char nCriteriiTestRX, unsigned int nLungMaxBuferRX,
***          bool bReseteazaBuffIntrare, bool bBlocheazaReceptiaLaFinal)
***
*** @brief   Commence receiving data and wait until a specific character is detected on a 
***          specific position. Multiple criteria like this one (a given character in a given 
***          position) can be defined. Whichever condition is met first, the Rx complete 
***          flag will be set.
***          This can be used in protocols with different but known message lengths ending with 
***          different terminal chars.
***
*** @param   sReceptionat the Rx buffer storing the received bytes
*** @param   sMultiTermRX an array containing all the possible terminal characters.
*** @param   sMultiLungimeRX an array containing corresponding positions of the terminal characters 
***          defined in sMultiTermRX 
*** @param   nCriteriiTestRX the number of test criteria defined in the pair sMultiTermRX and sMultiLungimeRX
***          The length of sMultiTermRX and sMultiLungimeRX must be equal with nCriteriiTestRX
*** @param   nLungMaxBuferRX the maximum number of bytes which can be stored into the Rx buffer.
*** @param   bReseteazaBuffIntrare if true this will reset the Rx buffer index (set it to 0) and commence
***          receiving and storing data from the beginning (typically when a new message is to be received)
***          If false commence receiving bytes but start storing them form the index next to the last one
***          used in the previous Rx.
*** @param   bBlocheazaReceptiaLaFinal if true, after the specified number of bytes has been received
***          all other bytes received after will be discarded. If false, after the specified number of
***          bytes has been received, if further bytes received they will not be discarded but stored
***          as well into the Rx buffer.
***          For example, when implementing the Rx part of a fixed header + variable datablock protocol,
***          we will first issue an Rx command for a fixed number of bytes with a reset input buffer and
***          nonblocking mode, then, after the header has been received, we will issue an Rx command
***          for the number of bytes in the datablock, without the reset of the input buffer (to preserve
***          the content of the header). This way, while the header is being processed (e.g. determine
***          the number of bytes for the datablock) we still allow some bytes of the datablock to be
***          received
*** @return  none.
void uart<x>_ReceptieLungSiTermTX(char *sReceptionat,
				char *sMultiTermRX,
				unsigned int *sMultiLungimeRX,
				char nCriteriiTestRX,
				unsigned int nLungMaxBuferRX,
				bool bReseteazaBuffIntrare,
				bool bBlocheazaReceptiaLaFinal);


*** @fn      uart<x>_ReceptieOK()
*** @brief   This checks if the receive criteria have been completed. The receive criteria
***          are defined by the functions initiating the Rx. (e.g. listen for a fixed number o
***          bytes - typically for binary protocols, or listen until a terminal character
***          has been received - typically for ASCII (text) based protocols. See Rx initiating
***          functions for details
***
*** @return  true if an message has been received according to the predefined Rx criteria.
***          false if no message has been received. If a message has been received the 
***          uart<x>_bRxOK and uart<x>_bStareNeprocesata flags will also be set.
bool uart<x>_ReceptieOK();
*/


/******************************************************************************/
/******************************************************************************/
#if (!defined __ATMEGA128) && (!defined __ATMEGA2560) && (!defined __ATMEGA328)
#error Target device not supported
#endif

#if (defined __ATMEGA8) || (defined __ATMEGA328) || (defined __ATMEGA128) || (defined __ATMEGA2560)
volatile unsigned char *uart0_Port485Dir;
volatile char uart0_Port485DirMask;
volatile char *uart0_sReceptionat;
volatile unsigned int uart0_nLungimeRX;
volatile unsigned int uart0_nLungMaxBuferRX;
volatile char uart0_cTermRX;
volatile unsigned int uart0_nContorRX;
volatile unsigned int uart0_nIndexUltimTestat;
volatile char *uart0_sTransmis;
volatile unsigned int uart0_nLungimeTX;
volatile unsigned int uart0_nContorTX;
volatile char uart0_nCriteriiTestRX;
volatile unsigned int *uart0_sMultiLungimeRX;
volatile char *uart0_sMultiTermRX;
volatile unsigned int uart0_nValoareTimeout;
volatile unsigned int uart0_nContorTimeout;
volatile unsigned int uart0_nValoareTimeoutCar;
volatile unsigned int uart0_nContorTimeoutCar;
volatile char uart0_bRS485;
volatile char uart0_bRS485Inversat;
volatile char uart0_bRecCuTermRX;
volatile char uart0_bReceptioneaza;
volatile char uart0_bRxOK;
volatile char uart0_bTransmite;
volatile char uart0_bTxOK;
volatile char uart0_bRXBufferLimit;
volatile char uart0_bTimeout;
volatile char uart0_bTimerCounting;
volatile char uart0_bTimeoutCar;
volatile char uart0_bTimerCarCounting;
volatile char uart0_bStareNeprocesata;
volatile char uart0_bSetareParametri;
volatile char uart0_bBlocheazaReceptiaLaFinal;
volatile char uart0_sVid[1];
#endif

#if (defined __ATMEGA128) || (defined __ATMEGA2560)
volatile unsigned char *uart1_Port485Dir;
volatile char uart1_Port485DirMask;
volatile char *uart1_sReceptionat;
volatile unsigned int uart1_nLungimeRX;
volatile unsigned int uart1_nLungMaxBuferRX;
volatile char uart1_cTermRX;
volatile unsigned int uart1_nContorRX;
volatile unsigned int uart1_nIndexUltimTestat;
volatile char *uart1_sTransmis;
volatile unsigned int uart1_nLungimeTX;
volatile unsigned int uart1_nContorTX;
volatile char uart1_nCriteriiTestRX;
volatile unsigned int *uart1_sMultiLungimeRX;
volatile char *uart1_sMultiTermRX;
volatile unsigned int uart1_nValoareTimeout;
volatile unsigned int uart1_nContorTimeout;
volatile unsigned int uart1_nValoareTimeoutCar;
volatile unsigned int uart1_nContorTimeoutCar;
volatile char uart1_bRS485;
volatile char uart1_bRS485Inversat;
volatile char uart1_bRecCuTermRX;
volatile char uart1_bReceptioneaza;
volatile char uart1_bRxOK;
volatile char uart1_bTransmite;
volatile char uart1_bTxOK;
volatile char uart1_bRXBufferLimit;
volatile char uart1_bTimeout;
volatile char uart1_bTimerCounting;
volatile char uart1_bTimeoutCar;
volatile char uart1_bTimerCarCounting;
volatile char uart1_bStareNeprocesata;
volatile char uart1_bSetareParametri;
volatile char uart1_bBlocheazaReceptiaLaFinal;
volatile char uart1_sVid[1];
#endif

#if (defined __ATMEGA2560)
volatile unsigned char *uart2_Port485Dir;
volatile char uart2_Port485DirMask;
volatile char *uart2_sReceptionat;
volatile unsigned int uart2_nLungimeRX;
volatile unsigned int uart2_nLungMaxBuferRX;
volatile char uart2_cTermRX;
volatile unsigned int uart2_nContorRX;
volatile unsigned int uart2_nIndexUltimTestat;
volatile char *uart2_sTransmis;
volatile unsigned int uart2_nLungimeTX;
volatile unsigned int uart2_nContorTX;
volatile char uart2_nCriteriiTestRX;
volatile unsigned int *uart2_sMultiLungimeRX;
volatile char *uart2_sMultiTermRX;
volatile unsigned int uart2_nValoareTimeout;
volatile unsigned int uart2_nContorTimeout;
volatile unsigned int uart2_nValoareTimeoutCar;
volatile unsigned int uart2_nContorTimeoutCar;
volatile char uart2_bRS485;
volatile char uart2_bRS485Inversat;
volatile char uart2_bRecCuTermRX;
volatile char uart2_bReceptioneaza;
volatile char uart2_bRxOK;
volatile char uart2_bTransmite;
volatile char uart2_bTxOK;
volatile char uart2_bRXBufferLimit;
volatile char uart2_bTimeout;
volatile char uart2_bTimerCounting;
volatile char uart2_bTimeoutCar;
volatile char uart2_bTimerCarCounting;
volatile char uart2_bStareNeprocesata;
volatile char uart2_bSetareParametri;
volatile char uart2_bBlocheazaReceptiaLaFinal;
volatile char uart2_sVid[1];
#endif

#if (defined __ATMEGA2560)
volatile unsigned char *uart3_Port485Dir;
volatile char uart3_Port485DirMask;
volatile char *uart3_sReceptionat;
volatile unsigned int uart3_nLungimeRX;
volatile unsigned int uart3_nLungMaxBuferRX;
volatile char uart3_cTermRX;
volatile unsigned int uart3_nContorRX;
volatile unsigned int uart3_nIndexUltimTestat;
volatile char *uart3_sTransmis;
volatile unsigned int uart3_nLungimeTX;
volatile unsigned int uart3_nContorTX;
volatile char uart3_nCriteriiTestRX;
volatile unsigned int *uart3_sMultiLungimeRX;
volatile char *uart3_sMultiTermRX;
volatile unsigned int uart3_nValoareTimeout;
volatile unsigned int uart3_nContorTimeout;
volatile unsigned int uart3_nValoareTimeoutCar;
volatile unsigned int uart3_nContorTimeoutCar;
volatile char uart3_bRS485;
volatile char uart3_bRS485Inversat;
volatile char uart3_bRecCuTermRX;
volatile char uart3_bReceptioneaza;
volatile char uart3_bRxOK;
volatile char uart3_bTransmite;
volatile char uart3_bTxOK;
volatile char uart3_bRXBufferLimit;
volatile char uart3_bTimeout;
volatile char uart3_bTimerCounting;
volatile char uart3_bTimeoutCar;
volatile char uart3_bTimerCarCounting;
volatile char uart3_bStareNeprocesata;
volatile char uart3_bSetareParametri;
volatile char uart3_bBlocheazaReceptiaLaFinal;
volatile char uart3_sVid[1];
#endif


/**************************************************************************/
/* Time2 interrupt for serial timeout control. It provides a 10ms timebase*/
/**************************************************************************/
void timer2_init(void);

#ifdef __ICCAVR
#pragma interrupt_handler timer2_compa_isr:TIMER2_COMPA_ISR_VECTOR_NO
void timer2_compa_isr(void);
#else

#if (defined __ATMEGA2560)	
ISR(TIMER2_COMPA_vect);
#elif (defined __ATMEGA128)	
ISR(TIMER2_COMP_vect);
#elif (defined __ATMEGA328)	
ISR(TIMER2_COMPA_vect);
#else
#warning Check interrupt vector name for your device
ISR(TIMER2_COMP_vect);
#endif
#endif

/******************************************************************************/
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
                unsigned int  nValoareTimeoutChar);

/******************************************************************************/
#ifdef __ICCAVR
#pragma interrupt_handler uart0_udre_isr: uart0_UDRE_ISR_VECTOR_NO
void uart0_udre_isr(void);
#else
	#ifdef __ATMEGA328
	ISR(USART_UDRE_vect);
	#else
	ISR(USART0_UDRE_vect);
	#endif
#endif

/******************************************************************************/
#ifdef __ICCAVR
#pragma interrupt_handler uart0_tx_isr: uart0_TX_ISR_VECTOR_NO
void uart0_tx_isr(void);
#else
	#ifdef __ATMEGA328
	ISR(USART_TX_vect);
	#else
	ISR(USART0_TX_vect);
	#endif
#endif

/******************************************************************************/
#ifdef __ICCAVR
#pragma interrupt_handler uart0_rx_isr: uart0_RX_ISR_VECTOR_NO
void uart0_rx_isr(void);
#else
	#ifdef __ATMEGA328
	ISR(USART_RX_vect);
	#else
	ISR(USART0_RX_vect);
	#endif
#endif

/******************************************************************************/
void uart0_TransmiteExt(char *sTransmis, 
                unsigned int nLungime, 
                bool bAsteptareTxAnterioara, 
                bool bAsteptareTxCurenta);

/******************************************************************************/
void uart0_Transmite(char *sTransmis, 
                unsigned int nLungime, 
                bool bAsteptareTxAnterioara);

/******************************************************************************/
bool uart0_ReceptieOK();

/******************************************************************************/
void uart0_ReceptieLFix(char *sReceptionat,
                unsigned int nLungime,
                unsigned int nLungMaxBuferRX,
                bool bReseteazaBuffIntrare,
                bool bBlocheazaReceptiaLaFinal);

/******************************************************************************/
void uart0_ReceptieTermTX(char *sReceptionat,
                char cTermRX,
                unsigned int nLungMaxBuferRX,
                bool bReseteazaBuffIntrare,
                bool bBlocheazaReceptiaLaFinal);

/*****************************************************************************/
void uart0_ReceptieLungSiTermTX(char *sReceptionat,
                char *sMultiTermRX,
                unsigned int *sMultiLungimeRX,
                char nCriteriiTestRX,
                unsigned int nLungMaxBuferRX,
                bool bReseteazaBuffIntrare,
                bool bBlocheazaReceptiaLaFinal);
#endif

/******************************************************************************/
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
                unsigned int  nValoareTimeoutChar);

/******************************************************************************/
#ifdef __ICCAVR
#pragma interrupt_handler uart1_udre_isr: uart1_UDRE_ISR_VECTOR_NO
void uart1_udre_isr(void);
#else
ISR(USART1_UDRE_vect);
#endif

/******************************************************************************/
#ifdef __ICCAVR
#pragma interrupt_handler uart1_tx_isr: uart1_TX_ISR_VECTOR_NO
void uart1_tx_isr(void);
#else
ISR(USART1_TX_vect);
#endif

/******************************************************************************/
#ifdef __ICCAVR
#pragma interrupt_handler uart1_rx_isr: uart1_RX_ISR_VECTOR_NO
void uart1_rx_isr(void);
#else
ISR(USART1_RX_vect);
#endif

/******************************************************************************/
void uart1_TransmiteExt(char *sTransmis, 
                unsigned int nLungime, 
                bool bAsteptareTxAnterioara, 
                bool bAsteptareTxCurenta);

/******************************************************************************/
void uart1_Transmite(char *sTransmis, 
                unsigned int nLungime, 
                bool bAsteptareTxAnterioara);

/******************************************************************************/
bool uart1_ReceptieOK();

/******************************************************************************/
void uart1_ReceptieLFix(char *sReceptionat,
                unsigned int nLungime,
                unsigned int nLungMaxBuferRX,
                bool bReseteazaBuffIntrare,
                bool bBlocheazaReceptiaLaFinal);

/******************************************************************************/
void uart1_ReceptieTermTX(char *sReceptionat,
                char cTermRX,
                unsigned int nLungMaxBuferRX,
                bool bReseteazaBuffIntrare,
                bool bBlocheazaReceptiaLaFinal);

/******************************************************************************/
void uart1_ReceptieLungSiTermTX(char *sReceptionat,
                char *sMultiTermRX,
                unsigned int *sMultiLungimeRX,
                char nCriteriiTestRX,
                unsigned int nLungMaxBuferRX,
                bool bReseteazaBuffIntrare,
                bool bBlocheazaReceptiaLaFinal);
#endif

/******************************************************************************/
/******************************************************************************/
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
                unsigned int  nValoareTimeoutChar);

/******************************************************************************/
#ifdef __ICCAVR
#pragma interrupt_handler uart2_udre_isr: uart2_UDRE_ISR_VECTOR_NO
void uart2_udre_isr(void);
#else
ISR(USART2_UDRE_vect);
#endif

/******************************************************************************/
#ifdef __ICCAVR
#pragma interrupt_handler uart2_tx_isr: uart2_TX_ISR_VECTOR_NO
void uart2_tx_isr(void);
#else
ISR(USART2_TX_vect);
#endif

/******************************************************************************/
#ifdef __ICCAVR
#pragma interrupt_handler uart2_rx_isr: uart2_RX_ISR_VECTOR_NO
void uart2_rx_isr(void);
#else
ISR(USART2_RX_vect);
#endif

/******************************************************************************/
void uart2_TransmiteExt(char *sTransmis, 
                unsigned int nLungime, 
                bool bAsteptareTxAnterioara, 
                bool bAsteptareTxCurenta);

/******************************************************************************/
void uart2_Transmite(char *sTransmis, 
                unsigned int nLungime, 
                bool bAsteptareTxAnterioara);

/******************************************************************************/
bool uart2_ReceptieOK();

/******************************************************************************/
void uart2_ReceptieLFix(char *sReceptionat,
                unsigned int nLungime,
                unsigned int nLungMaxBuferRX,
                bool bReseteazaBuffIntrare,
                bool bBlocheazaReceptiaLaFinal);

/******************************************************************************/
void uart2_ReceptieTermTX(char *sReceptionat,
                char cTermRX,
                unsigned int nLungMaxBuferRX,
                bool bReseteazaBuffIntrare,
                bool bBlocheazaReceptiaLaFinal);

/******************************************************************************/
void uart2_ReceptieLungSiTermTX(char *sReceptionat,
                char *sMultiTermRX,
                unsigned int *sMultiLungimeRX,
                char nCriteriiTestRX,
                unsigned int nLungMaxBuferRX,
                bool bReseteazaBuffIntrare,
                bool bBlocheazaReceptiaLaFinal);

#endif

/******************************************************************************/
/******************************************************************************/
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
                unsigned int  nValoareTimeoutChar);

/******************************************************************************/
#ifdef __ICCAVR
#pragma interrupt_handler uart3_udre_isr: uart3_UDRE_ISR_VECTOR_NO
void uart3_udre_isr(void);
#else
ISR(USART3_UDRE_vect);
#endif

/******************************************************************************/
#ifdef __ICCAVR
#pragma interrupt_handler uart3_tx_isr: uart3_TX_ISR_VECTOR_NO
void uart3_tx_isr(void);
#else
ISR(USART3_TX_vect);
#endif

/******************************************************************************/
#ifdef __ICCAVR
#pragma interrupt_handler uart3_rx_isr: uart3_RX_ISR_VECTOR_NO
void uart3_rx_isr(void);
#else
ISR(USART3_RX_vect);
#endif

/******************************************************************************/
void uart3_TransmiteExt(char *sTransmis, 
                unsigned int nLungime, 
                bool bAsteptareTxAnterioara, 
                bool bAsteptareTxCurenta);

/******************************************************************************/
void uart3_Transmite(char *sTransmis, 
                unsigned int nLungime, 
                bool bAsteptareTxAnterioara);

/******************************************************************************/
bool uart3_ReceptieOK();

/******************************************************************************/
void uart3_ReceptieLFix(char *sReceptionat,
                unsigned int nLungime,
                unsigned int nLungMaxBuferRX,
                bool bReseteazaBuffIntrare,
                bool bBlocheazaReceptiaLaFinal);

/******************************************************************************/
void uart3_ReceptieTermTX(char *sReceptionat,
                char cTermRX,
                unsigned int nLungMaxBuferRX,
                bool bReseteazaBuffIntrare,
                bool bBlocheazaReceptiaLaFinal);

/******************************************************************************/
void uart3_ReceptieLungSiTermTX(char *sReceptionat,
                char *sMultiTermRX,
                unsigned int *sMultiLungimeRX,
                char nCriteriiTestRX,
                unsigned int nLungMaxBuferRX,
                bool bReseteazaBuffIntrare,
                bool bBlocheazaReceptiaLaFinal);
#endif

#endif /* _AVR_USART_H_ */