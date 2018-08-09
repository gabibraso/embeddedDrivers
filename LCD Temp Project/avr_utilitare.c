/*
* avr_utilitare.c
*
* Created: 01.01.2009
* Author : Bogdan Tarnauca
*/

#include "avr_utilitare.h"

void mReseteazaBufer(char *sBufer, int nLungime, char cCarDeReset)
{
	do
	{
		*(sBufer + nLungime--) = cCarDeReset;
	}
	while(nLungime >=0);
}

//---------------------------------------------------------------------------------------
//Convertestre sirul sSirSursa in hexa reprezentat in ASCII cu un un spatiu
//intre doua reprezentari succesive
void mStrToHex(char *sSirSursa, unsigned int nLSursa,char *sRezultat, unsigned int *nLRez)
{
	char sTmp[10];
	unsigned int i;

	for(i = 0; i<nLSursa; i++)
	{
		WDR(); 

		#ifdef __WIN_AVR
			itoa( (unsigned int)(*(sSirSursa+i)), sTmp, 16); //WIN_AVR
		#else
			itoa( sTmp, (unsigned int)(*(sSirSursa+i)), 16); //ICC_AVR
		#endif
		



		if(*(sSirSursa+i) < 0x10)
		{	//pun un 0 in fata cifrelor 0-F
			*(sTmp + 1) = *sTmp;
			*sTmp = '0';
		}
		
		*(sRezultat + i*3 + 0) = toupper(*sTmp);
		*(sRezultat + i*3 + 1) = toupper(*(sTmp + 1));
		*(sRezultat + i*3 + 2) = ' ';
	}
	*nLRez = nLSursa*3;
}

/*
void sleep(int nMilisecunde)
{
	float nValoareInteratie; //!!!!ATENTIE float manaca memorie de rupe!
	unsigned long nContor = 0;
	
	nValoareInteratie = 64. / FRECVENTA_CUART;
	nContor = (unsigned long)(nMilisecunde * 1000./nValoareInteratie);
	do{	
	WDR(); 
	}
	while(nContor--);
}
*/

// ---------------------------------------------------------------------------------------
// void sleepCC(long nClockCycles)
// {
// 	# warning ABT: Lungimea intarzierilor introduse de functia SleepCC(long nClockCycles) sunt 	dependente de compilator, de optiunile de otimizare si ca atare functia ar trebui sa fie inlocuita.
// 
// 	do{	
// 	WDR();
// 	#ifdef __WIN_AVR 
// 	_delay_us(5);
// 	#endif
// 	}
// 	while(nClockCycles--);
// }


//---------------------------------------------------------------------------------------
void impartire(
			   unsigned int deimpartit, 
			   unsigned int impartitor, 
			   unsigned int *cat_intreg, 
			   unsigned int *cat_zecimal,
			   char nNumarDigiti)
{
	//ATENTIE: Numarul de digiti trebuie sa fie mai mic sau egal decat 4
	//deoarece pentru 5 as putea depasi 65535 cat pot reprezenta pe un
	//integer. Pentru precizie mai mare parametrii metodei de impartire 
	//trebuie sa fie de tip long.

	unsigned int rest;
	char i;

	*cat_intreg = deimpartit / impartitor;
	rest = deimpartit % impartitor;

	//ATENTIE: Deoarece prin reprezentarea partii zecimale ca un numar intreg (de ex: 0.134 => 134)
	//nu pot reprezenta direct numere de genul 0.013 care ar deveni 13 insa echivalent cu 0.13...
	//voi pune intotdeauna in fata numarului meu cifra 1
	//REZULTAT: 0.434 => 1434
	//          0.043 => 1043
	*cat_zecimal = ((rest * 10) / impartitor) + 10;

    for(i = 1; i<nNumarDigiti; i++)
	{
		rest = (rest*10) % impartitor;
		*cat_zecimal = (*cat_zecimal * 10) + ((rest * 10) / impartitor); 
	}
	/*
	123 / 24 = 5,125 r 3 
	30 / 24 = 1 r 6
	60 / 24 = 2 r 12
	120 / 24 = 5 

	void itoa(char *buf, int value, int base)
		converts an integer value to an ASCII string, using base as the radix.
	void ltoa(char *buf, long value, int base)
		converts a long value to an ASCII string, using base as the radix.
	*/
}


//---------------------------------------------------------------------------------------
void impartire_long(
			   unsigned long deimpartit, 
			   unsigned long impartitor, 
			   unsigned long *cat_intreg, 
			   unsigned long *cat_zecimal,
			   char nNumarDigiti)
{


	unsigned long rest;
	char i;

	*cat_intreg = deimpartit / impartitor;
	rest = deimpartit % impartitor;

	//ATENTIE: Deoarece prin reprezentarea partii zecimale ca un numar intreg (de ex: 0.134 => 134)
	//nu pot reprezenta direct numere de genul 0.013 care ar deveni 13 insa echivalent cu 0.13...
	//voi pune intotdeauna in fata numarului meu cifra 1
	//REZULTAT: 0.434 => 1434
	//          0.043 => 1043
	*cat_zecimal = ((rest * 10) / impartitor) + 10;

    for(i = 1; i<nNumarDigiti; i++)
	{
		rest = (rest*10) % impartitor;
		*cat_zecimal = (*cat_zecimal * 10) + ((rest * 10) / impartitor); 
	}
	/*
	123 / 24 = 5,125 r 3 
	30 / 24 = 1 r 6
	60 / 24 = 2 r 12
	120 / 24 = 5 

	void itoa(char *buf, int value, int base)
		converts an integer value to an ASCII string, using base as the radix.
	void ltoa(char *buf, long value, int base)
		converts a long value to an ASCII string, using base as the radix.
	*/
}


//---------------------------------------------------------------------------------------
//atentie nu poate copia mai mult de 255 de caractere.
void string_copy(
			  char *sDestinatie, 
			  char nPozDestinatie, 
			  char *sSursa, 
			  char nPozSursa, 
			  char nNrCaractere)
{
	unsigned int i;
	for(i = 0; i<nNrCaractere; i++)
	{
		WDR();
		*(sDestinatie + nPozDestinatie + i) = *(sSursa + nPozSursa + i);
	}
	*(sDestinatie + nPozDestinatie + i) = 0x00;
}


//---------------------------------------------------------------------------------------
char string_right_justify(
			char *sDestinatie,		//sirul in care facem justificarea la dreapta
			char *sSursa, //sirul pe care dorim sa il trcem la dreapta
			char cBlankChar)
{
	char nLungimeDestinatie;
	char nLungimeSursa;
	char i=0;

	nLungimeDestinatie = strlen(sDestinatie);
	nLungimeSursa = strlen(sSursa);
	
    if(nLungimeDestinatie<nLungimeSursa)
    {
        return -1;
    }

	for(i=0; i<nLungimeDestinatie; i++)
	{
		*(sDestinatie + i) = cBlankChar;
		WDR();
	}

	strcpy(sDestinatie + nLungimeDestinatie - nLungimeSursa,sSursa + '\0');
	
	return 0;
}


//---------------------------------------------------------------------------------------
char string_left_justify(
			char *sDestinatie,		//sirul in care facem justificarea la stanga
			char *sSursa, //sirul pe care dorim sa il trcem la dreapta
			char cBlankChar)
{
	char nLungimeDestinatie;
	char nLungimeSursa;
	char i=0;

	nLungimeDestinatie = strlen(sDestinatie);
	nLungimeSursa = strlen(sSursa);

    if(nLungimeDestinatie<nLungimeSursa)
    {
        return -1;
    }
    
    strcpy(sDestinatie, sSursa);
	for(i=nLungimeSursa; i<nLungimeDestinatie; i++)
	{
		*(sDestinatie + i) = cBlankChar;
		WDR();
	}
	*(sDestinatie + i) = 0x00;
	return 0;
}


//---------------------------------------------------------------------------------------
void string_reset(char *sSirDeResetat, char cResetChar, char cTerminatorChar, unsigned int nLungimeFaraTerm)
{
	unsigned int i;
	for( i=0; i<nLungimeFaraTerm; i++)
		*(sSirDeResetat + i) = cResetChar;

	*(sSirDeResetat + i) = cTerminatorChar;
}


//---------------------------------------------------------------------------------------
//Adauga la un string atatea caractere 'cExpandCar' pana cand strindul ajunge la lungimea 'nLungime'
//Returneaza lungimea stringului expandat sau -1 daca lungimea originala este mai mare decat 'nLungime'
//ATENTIE Cand se acolo spatiu pentru string trebuie sa fie suficient astfel incat apelurile
//functiei sa nu scrie alte locatii
signed int string_expand(char *sSirDeExpandat, char cExpandCar, unsigned int nLungime)
{
	unsigned int i;
	unsigned int nLungOrig;
	
	if(strlen(sSirDeExpandat) > nLungime)
	{
	    return -1;
	}
	
	nLungOrig = strlen(sSirDeExpandat);
	
	for( i=0; i< (nLungime - nLungOrig); i++)
	{
		*(sSirDeExpandat + i + nLungOrig) = cExpandCar;
	}

	*(sSirDeExpandat + nLungime) = '\0';
	return nLungime;
}

//---------------------------------------------------------------------------------
long abs_long(long nValoare)
{
    return (nValoare>0)?nValoare:-nValoare;
}

//---------------------------------------------------------------------------------
void pin_Toggle(volatile unsigned char *pPORT, char nPin)
{
	//setez dorectia ca iesire 
	*(pPORT-1) |= (1 << nPin);
	*pPORT ^= (1 << nPin);
}

//---------------------------------------------------------------------------------
void pin_On(volatile unsigned char *pPORT, char nPin)
{
	//setez dorectia ca iesire 
	*(pPORT-1) |= (1 << nPin);
	
	*pPORT |= (1 << nPin);
}

//---------------------------------------------------------------------------------
void pin_Off(volatile unsigned char *pPORT, char nPin)
{
	//setez dorectia ca iesire 
	*(pPORT-1) |= (1 << nPin);
	*pPORT &= ~(1 << nPin);
}

//---------------------------------------------------------------------------------
char getCharBit(char nValoare, char nPozitie)
{
    return (( (0x01 << nPozitie) & nValoare)==0)? 0:1;
}

char getIntBit(unsigned int nValoare, char nPozitie)
{
    return (( (0x01 << nPozitie) & nValoare)==0)? 0:1;
}

char setCharBit(char *nValoare, char nPozitie, bool bBitValue)
{
    if(bBitValue)
    {   //setare
        *nValoare |= (0x01 << nPozitie);
    }
    else
    {   //resetare
        *nValoare &= ~(0x01 << nPozitie);
    }
    return *nValoare;
}

char setIntBit(unsigned int *nValoare, char nPozitie, bool bBitValue)
{
    if(bBitValue)
    {   //setare
        *nValoare |= (0x01 << nPozitie);
    }
    else
    {   //resetare
        *nValoare &= ~(0x01 << nPozitie);
    }
    return *nValoare;
}
