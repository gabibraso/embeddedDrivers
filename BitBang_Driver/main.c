/*
 * main.c
 *
 * Created: 7/24/2018 10:35:32 AM
 *  Author: gabriel.brasoveanu
 */ 
#include "bit_bang.h"

#include <avr/io.h>



int main(void)
{

	bitBangStructInit(&PORTB,PORTB0,PORTB1,PORTB2,PORTB3);
	bitBangPortInit();
	
	while (1)
	{		
		
	}
}