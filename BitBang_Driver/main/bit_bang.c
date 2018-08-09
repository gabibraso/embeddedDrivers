#include "bit_bang.h"

#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>

volatile bool mosiState = false;
volatile bool misoState = false;



void bitBangStructInit(volatile uint8_t *Port, unsigned char SSPin, unsigned char SCKPin, unsigned char MOSIPin, unsigned char MISOPin)
{
	bitBangInit.bitBangPort = Port;
	bitBangInit.SSPin = SSPin;
	bitBangInit.SCKPin = SCKPin;
	bitBangInit.MOSIPin = MOSIPin;
	bitBangInit.MISOPin = MISOPin;
		
	if(bitBangInit.MOSIPin == '\x00')
	{
		// put in HIGH Z state
		*(bitBangInit.bitBangPort-1) &= ~(1<<bitBangInit.MOSIPin);
		*(bitBangInit.bitBangPort) &= ~(1<<bitBangInit.MOSIPin);
	}else
	{
		mosiState = true;
	}
	
	if(bitBangInit.MISOPin == '\x00')
	{
		// put in HIGH Z state
		*(bitBangInit.bitBangPort-1) &= ~(1<<bitBangInit.MISOPin);
		*(bitBangInit.bitBangPort) &= ~(1<<bitBangInit.MISOPin);
	}else
	{
		misoState = true;
	}
}

void bitBangPortInit()
{
	//DDR - set DDR and make SCK, SS output pin
	*(bitBangInit.bitBangPort-1) |= (1<<bitBangInit.SSPin)|(1<<bitBangInit.SCKPin);
	
	//DDR - make MOSI pin as output
	if(mosiState)
	{
		//make it as output
		*(bitBangInit.bitBangPort-1) |= (1<<bitBangInit.MOSIPin);
	}
	
	//DDR - make MISO pin as input
	if(misoState)
	{
		//make it as input
		*(bitBangInit.bitBangPort-1) &= ~(1<<bitBangInit.MISOPin);
	}
	
	//set the SS pin to HIGH
	*(bitBangInit.bitBangPort) |=  (1<<bitBangInit.SSPin);
	*(bitBangInit.bitBangPort) &=  ~(1<<bitBangInit.SCKPin);
	//disable SPI
	SPCR = 0;
}

void bitBangTransfer(uint8_t data)
{
	//unsigned char bitNumberToTransfer = 9;
	for(uint8_t mask=0x80;mask;mask>>=1)
	{
		if(data & mask)
		{
			// Set MOSI high if bit is 1 
			*(bitBangInit.bitBangPort) |= (1<<bitBangInit.MOSIPin);
		}
		else 
		{
			// Set MOSI low if bit is 0 
			*(bitBangInit.bitBangPort) &= ~(1<<bitBangInit.MOSIPin)	;
		}
		
		// Set clock HIGH
		*(bitBangInit.bitBangPort) |= (1<<bitBangInit.SCKPin);
		// Set clock LOW
		*(bitBangInit.bitBangPort) &= ~(1<<bitBangInit.SCKPin);
	}
}





