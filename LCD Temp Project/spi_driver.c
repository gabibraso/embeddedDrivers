#include "spi_driver.h"
#include "lcd_driver.h"
#include <avr/io.h>
#include <stdbool.h>

volatile bool mosiState = false;
volatile bool misoState = false;



void spiStructInit(volatile uint8_t *Port, unsigned char SSPin, unsigned char SCKPin, unsigned char MOSIPin, unsigned char MISOPin)
{
	spiInit.spiPort = Port;
	spiInit.SSPin = SSPin;
	spiInit.SCKPin = SCKPin;
	spiInit.MOSIPin = MOSIPin;
	spiInit.MISOPin = MISOPin;
		
	if(spiInit.MOSIPin == '\x00')
	{
		// put in HIGH Z state
		*(spiInit.spiPort-1) &= ~(1<<spiInit.MOSIPin);
		*(spiInit.spiPort) &= ~(1<<spiInit.MOSIPin);
	}else
	{
		mosiState = true;
	}
	
	if(spiInit.MISOPin == '\x00')
	{
		// put in HIGH Z state
		*(spiInit.spiPort-1) &= ~(1<<spiInit.MISOPin);
		*(spiInit.spiPort) &= ~(1<<spiInit.MISOPin);
	}else
	{
		misoState = true;
	}
}

void spiPortInit()
{
	//DDR - set DDR and make SCK, SS output pin
	*(spiInit.spiPort-1) |= (1<<spiInit.SSPin)|(1<<spiInit.SCKPin);
	
	//DDR - make MOSI pin as output
	if(mosiState)
	{
		//make it as output
		*(spiInit.spiPort-1) |= (1<<spiInit.MOSIPin);
	}
	
	//DDR - make MISO pin as input
	if(misoState)
	{
		//make it as input
		*(spiInit.spiPort-1) &= ~(1<<spiInit.MISOPin);
	}
	
	//set the SS pin to HIGH
	*(spiInit.spiPort) |=  (1<<spiInit.SSPin);
	*(spiInit.spiPort) &=  ~(1<<spiInit.SCKPin);
	
	//Set SPI
	SPCR = (1<<SPE) | (1<<MSTR) | (0<<SPR1) | (0<<SPR0);
	SPSR = (1<<SPI2X);
}

void spiWrite(uint8_t data)
{
	SPDR = data;
	while((SPSR & (1<<SPIF)) ==0);
}



