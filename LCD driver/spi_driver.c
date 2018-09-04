#include "spi_driver.h"
#include "lcd_driver.h"
#include <avr/io.h>


void spi_Init(	volatile uint8_t *Port,
unsigned char SSPin,
unsigned char SCKPin,
unsigned char MOSIPin,
unsigned char MISOPin,
uint8_t dataOrder,
uint8_t spiMode,
uint8_t divider
)

{
	spiStruct.spiPort = Port;
	spiStruct.SSPin = SSPin;
	spiStruct.SCKPin = SCKPin;
	spiStruct.MOSIPin = MOSIPin;
	spiStruct.MISOPin = MISOPin;
	
	if(spiStruct.MOSIPin == '\x00')
	{
		// put in HIGH Z state
		*(spiStruct.spiPort-1) &= ~(1<<spiStruct.MOSIPin);
		*(spiStruct.spiPort) &= ~(1<<spiStruct.MOSIPin);
	}else
	{
		//make it as output
		*(spiStruct.spiPort-1) |= (1<<spiStruct.MOSIPin);
	}
	
	if(spiStruct.MISOPin == '\x00')
	{
		// put in HIGH Z state
		*(spiStruct.spiPort-1) &= ~(1<<spiStruct.MISOPin);
		*(spiStruct.spiPort) &= ~(1<<spiStruct.MISOPin);
	}else
	{
		//make it as input
		*(spiStruct.spiPort-1) &= ~(1<<spiStruct.MISOPin);
	}
	
	//DDR - set DDR and make SCK, SS output pin
	*(spiStruct.spiPort-1) |= (1<<spiStruct.SSPin)|(1<<spiStruct.SCKPin);
	
	//set the SS pin to HIGH
	*(spiStruct.spiPort) |=  (1<<spiStruct.SSPin);
	*(spiStruct.spiPort) &=  ~(1<<spiStruct.SCKPin);
	
	//Set SPI
	SPCR |= (1<<SPE);			//SPI Enable ON
	SPSR &= ~(1<<SPIE);			//SPI Interrupt Enable OFF
	setSpiBitOrder(dataOrder);	//Set SPI data order
	setSpiMode(spiMode);		//Set SPI mode
	setSpiClockRate(divider);	//Set SPI clock divider
	
}


void setSpiBitOrder(uint8_t dataOrder)
{
	if(dataOrder == LSB_FIRST)
	{// DORD = 1 -> LSB First
		SPCR |= (1<<DORD);
	}
	
	if(dataOrder == MSB_FIRST)
	{// DORD = 0 -> MSB First
		SPCR &= ~(1<<DORD);
	}
}


void setSpiMode(uint8_t spiMode)
{
	if(spiMode == MASTER_MODE)
	{//MSTR = 1 -> Master Mode
		SPCR |= (1<<MSTR);
	}
	
	if(spiMode == SLAVE_MODE)
	{//MSTR = 0 -> Slave Mode
		SPCR &= ~(1<<MSTR);
	}
}


void setSpiClockRate(uint8_t divider)
{	// SPI2x = 1; SPR1 = 0; SPR0 = 0  --> Fosc/2	
	if(divider == SPI_CLOCK_DIV2) {SPCR &= ~((1<<SPR0) | (1<<SPR1)); SPSR |= (1<<SPI2X);}
	// SPI2x = 0; SPR1 = 0; SPR0 = 0  --> Fosc/4	
	if(divider == SPI_CLOCK_DIV4) {SPCR &= ~((1<<SPR0) | (1<<SPR1)); SPSR &= ~(1<<SPI2X);}
	// SPI2x = 1; SPR1 = 0; SPR0 = 1  --> Fosc/8		
	if(divider == SPI_CLOCK_DIV8) {SPCR |= (1<<SPR0); SPCR &= ~(1<<SPR1); SPSR |= (1<<SPI2X);}
	// SPI2x = 0; SPR1 = 0; SPR0 = 1  --> Fosc/16		
	if(divider == SPI_CLOCK_DIV16) {SPCR |= (1<<SPR0); SPCR &= ~(1<<SPR1); SPSR &= ~(1<<SPI2X);}
	// SPI2x = 1; SPR1 = 1; SPR0 = 0  --> Fosc/32		
	if(divider == SPI_CLOCK_DIV32) {SPCR |= (1<<SPR1); SPCR &= ~(1<<SPR0); SPSR |= (1<<SPI2X);}
	// SPI2x = 1; SPR1 = 1; SPR0 = 1  --> Fosc/64		
	if(divider == SPI_CLOCK_DIV64) {SPCR &= ~(1<<SPR0); SPCR |= (1<<SPR1); SPSR &= ~(1<<SPI2X);}
	// SPI2x = 0; SPR1 = 1; SPR0 = 1  --> Fosc/128		
	if(divider == SPI_CLOCK_DIV128) {SPCR |= (1<<SPR0) | (1<<SPR1); SPSR &= ~(1<<SPI2X);}
}

void spiTransmitByPolling(uint8_t data)
{
	SPDR = data;
	while((SPSR & (1<<SPIF)) == 0);
}


void spiTransmitByInterrupt(uint8_t *sTxBuffer, uint8_t *sRxBuffer, uint8_t nBytesToTransmit)
{
	bytesToTransmit = nBytesToTransmit;
	spiTxBuffer = sTxBuffer;
	spiRxBuffer = sRxBuffer;
	spiStatus = SPI_IDLE;
	
		if(spiStatus == SPI_IDLE)
		{
			// enable interrupt
			SPCR |= (1<<SPIE);
			sei();
			//Put SS to LOW (select this device)
			*(spiStruct.spiPort) &= ~(1<<spiStruct.SSPin);
			SPDR = *spiTxBuffer;
			spiCounter = 0;
			spiStatus = SPI_TRANSFER_ONGOING;
		}
	
}

ISR(SPI_STC_vect)
{
	spiRxBuffer = SPDR;
	spiCounter++;
	if(bytesToTransmit == spiCounter)
	{//transmission complete reset the interrupt enable
		SPCR &= ~(1<<SPIE);
		spiStatus = SPI_IDLE;
		// Put SS to HIGH (deselect this device)
		*(spiStruct.spiPort) |= (1<<spiStruct.SSPin);
	}else
	{
		SPDR = *(spiTxBuffer + spiCounter);
	}
	
}


