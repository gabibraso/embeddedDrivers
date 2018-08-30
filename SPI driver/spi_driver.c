#include "spi_driver.h"
#include "lcd_driver.h"
#include <avr/io.h>
#include <stdbool.h>


/**
 * \brief			 This method is used to initialize the SPI pins used
 * 
 * \param Port		- Reference to the port that providing the SPI inputs
 * \param SSPin		- Reference to the SS (Slave Select) pin used to select the Slave device
 * \param SCKPin	- Reference to the SCK (CLOCK) pin used to generate the clock signal
 * \param MOSIPin	- Reference to the MOSI (Master Output Slave Input) pin used to transmit
					  data from Master to Slave
 * \param MISOPin	- Reference to MISO (Master Input Slave Output) pin used to transmit
					  data from Slave to Master
 * \param dataOrder - Reference to 
 * \param spiMode   -
 * \param divider   -			  
 * 
 * \return void
 */
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
	SPCR |= (1<<SPE);
	SPSR &= ~(1<<SPIE);
	setSpiBitOrder(dataOrder);
	setSpiMode(spiMode);
	setSpiClockRate(divider);
	
	
	
}

/**
 * \brief				This method is used to select the first bit to be transmitted
 * 
 * \param dataOrder   - When this parameter take LSB_FIRST, the LSB of data word is transmitted first
					  - When this parameter take MSB_FIRST, the MSB of data word is transmitted first
 * 
 * \return void
 */
void setSpiBitOrder(uint8_t dataOrder)
{
	if(dataOrder == LSB_FIRST)
	{
		SPCR |= (1<<DORD);
	}
	
	if(dataOrder == MSB_FIRST)
	{
		SPCR &= ~(1<<DORD);
	}
}

/**
 * \brief		  This method is used to select SPI mode
 *		
 * \param mode	- This parameter can take two values: - MASTER_MODE -> the device is a master 
 *													  - SLAVE_MODE -> the device is a slave
 * \return void
 */
void setSpiMode(uint8_t spiMode)
{
	if(spiMode == MASTER_MODE)
	{
		SPCR |= (1<<MSTR);
	}
	
	if(spiMode == SLAVE_MODE)
	{
		SPCR &= ~(1<<MSTR);
	}
}

/**
 * \brief			This method is used to select the SPI clock rate
 * 
 * \param divider	- this parameter take the clock divider value 
 * 
 * \return void
 */
void setSpiClockRate(uint8_t divider)
{
	if(divider == SPI_CLOCK_DIV2) {SPCR &= ~((1<<SPR0) | (1<<SPR1)); SPSR |= (1<<SPI2X);}
	if(divider == SPI_CLOCK_DIV4) {SPCR &= ~((1<<SPR0) | (1<<SPR1)); SPSR &= ~(1<<SPI2X);}
	if(divider == SPI_CLOCK_DIV8) {SPCR |= (1<<SPR0); SPCR &= ~(1<<SPR1); SPSR |= (1<<SPI2X);}
	if(divider == SPI_CLOCK_DIV16) {SPCR |= (1<<SPR0); SPCR &= ~(1<<SPR1); SPSR &= ~(1<<SPI2X);}	
	if(divider == SPI_CLOCK_DIV32) {SPCR |= (1<<SPR1); SPCR &= ~(1<<SPR0); SPSR |= (1<<SPI2X);}
	if(divider == SPI_CLOCK_DIV64) {SPCR &= ~(1<<SPR0); SPCR |= (1<<SPR1); SPSR &= ~(1<<SPI2X);}	
	if(divider == SPI_CLOCK_DIV128) {SPCR |= (1<<SPR0) | (1<<SPR1); SPSR &= ~(1<<SPI2X);}
		
}

void spiWritePooling(uint8_t data)
{
	SPDR = data;
	while((SPSR & (1<<SPIF)) == 0);
}

/**
 * \brief					That method is used to enable the SPI interrupt and to transmit the first byte of data.
							After the first byte was transmitted the ISR routine is executed.
							SpiCounters is incremented by 1 (a byte was transmitted) and if this counter
							is equal with the number of bytes that we want to transmit, transmission is
							complete and then put the SS pin back to HIGH and reset the interrupt enable. 
							If the counter is not equal with the the number of bytes that we want to transmit,
							the SPDR register will be equal with the value of the sTxBuffer address + spiCounter (sTxBuffer + 1 for e.g.).
							After the next byte was transmitted the ISR routine is executed again.
							SpiCounter is incremented again by 1 (now his value will be 2) and as before if this counter is 
							equal with the number of bytes that we want to transmit, transmission is complete and so on.
							
 * 
 * \param sTxBuffer			- the Tx buffer that store the bits to be transmitted 
 * \param sRxBuffer			- the Rx buffer that store received bits 
 * \param nBytesToTransmit	- number of bytes that we want to transmit
 * 
 * \return void
 */
void spiInterruptTransmit(uint8_t *sTxBuffer, uint8_t *sRxBuffer, uint8_t nBytesToTransmit)
{
	bytesToTransmit = nBytesToTransmit;
	spiTxBuffer = sTxBuffer;
	spiRxBuffer = sRxBuffer;
	spiStatus = SPI_IDLE;
	// enable interrupt 
	SPCR |= (1<<SPIE);
	sei();
		
		
	if(spiStatus == SPI_IDLE)
	{
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


