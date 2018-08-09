/*
* avr_utilitare.h
*
* Created: 01.01.2009
* Author : Bogdan Tarnauca
*/

#ifndef _AVR_UTILITARE_H_
	#define _AVR_UTILITARE_H_

//Apps must include an "main_config.h" header where libraries related params, overrides, settings should be defined.
//This header file is application specific. Must be provided even it is empty.
#include "main_config.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "avr_defines.h"

void mReseteazaBufer(char *sBufer, int nLungime, char cCarDeReset);

//---------------------------------------------------------------------------------------

//Convertestre sirul sSirSursa in hexa reprezentat in ASCII cu un un spatiu
//intre doua reprezentari succesive
void mStrToHex(char *sSirSursa, unsigned int nLSursa,char *sRezultat, unsigned int *nLRez);
//void sleepCC(long nClockCycles);
void impartire(
			   unsigned int deimpartit,
			   unsigned int impartitor,
			   unsigned int *cat_intreg,
			   unsigned int *cat_zecimal,
			   char nNumarDigiti);
void impartire_long(
			   unsigned long deimpartit,
			   unsigned long impartitor,
			   unsigned long *cat_intreg,
			   unsigned long *cat_zecimal,
			   char nNumarDigiti);
//---------------------------------------------------------------------------------------
//atentie nu poate copia mai mult de 255 de caractere.
void string_copy(
			  char *sDestinatie,
			  char nPozDestinatie,
			  char *sSursa,
			  char nPozSursa,
			  char nNrCaractere);
//---------------------------------------------------------------------------------------
char string_right_justify(
			char *sDestinatie,		//sirul in care facem justificarea la dreapta
			char *sSursa, //sirul pe care dorim sa il trcem la dreapta
			char cBlankChar);
char string_left_justify(
			char *sDestinatie,		//sirul in care facem justificarea la stanga
			char *sSursa, //sirul pe care dorim sa il trcem la dreapta
			char cBlankChar);
void string_reset(char *sSirDeResetat, char cResetChar, char cTerminatorChar, unsigned int nLungimeFaraTerm);

//---------------------------------------------------------------------------------------
//Adauga la un string atatea caractere 'cExpandCar' pana cand strindul ajunge la lungimea 'nLungime'
//Returneaza lungimea stringului expandat sau -1 daca lungimea originala este mai mare decat 'nLungime'
//ATENTIE Cand se acolo spatiu pentru string trebuie sa fie suficient astfel incat apelurile
//functiei sa nu scrie alte locatii
signed int string_expand(char *sSirDeExpandat, char cExpandCar, unsigned int nLungime);

long abs_long(long nValoare);
void pin_Toggle(volatile unsigned char *pPORT, char nPin);
void pin_On(volatile unsigned char *pPORT, char nPin);
void pin_Off(volatile unsigned char *pPORT, char nPin);
char getCharBit(char nValoare, char nPozitie);
char getIntBit(unsigned int nValoare, char nPozitie);
char setCharBit(char *nValoare, char nPozitie, bool bBitValue);
char setIntBit(unsigned int *nValoare, char nPozitie, bool bBitValue);

#endif /* _AVR_UTILITARE_H_ */
