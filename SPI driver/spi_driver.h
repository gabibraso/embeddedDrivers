#ifndef _SPI_DRIVER_H_
#define _SPI_DRIVER_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

typedef struct{
	volatile uint8_t *spiPort;
	unsigned char SSPin;
	unsigned char SCKPin;
	unsigned char MOSIPin;
	unsigned char MISOPin;
} spiPins;

spiPins spiStruct;

#define SPI_IDLE                0x00
#define SPI_TRANSFER_ONGOING    0x01
#define LSB_FIRST				0x02
#define MSB_FIRST				0x03
#define MASTER_MODE				0x04
#define SLAVE_MODE				0x05
#define SPI_CLOCK_DIV2			0x06
#define SPI_CLOCK_DIV4			0x07
#define SPI_CLOCK_DIV8			0x08
#define SPI_CLOCK_DIV16			0x09
#define SPI_CLOCK_DIV32			0x10
#define SPI_CLOCK_DIV64			0x11
#define SPI_CLOCK_DIV128		0x12


volatile uint8_t bytesToTransmit;
volatile uint8_t bytesToReceive;
volatile uint8_t spiCounter;
volatile uint8_t *spiRxBuffer;
volatile uint8_t *spiTxBuffer;
volatile uint8_t spiStatus;

uint32_t dataToTransmit;
uint8_t dataLength ;
uint32_t dataBuffer;


/**
* \brief			 This method is used to initialize the SPI pins used
*
* \param Port		- Reference to port that providing the SPI inputs
* \param SSPin		- Reference to the SS (Slave Select) pin used to select the Slave device
* \param SCKPin		- Reference to the SCK (CLOCK) pin used to generate the clock signal
* \param MOSIPin	- Reference to the MOSI (Master Output Slave Input) pin used to transmit
					  data from Master to Slave
* \param MISOPin	- Reference to MISO (Master Input Slave Output) pin used to transmit
				      data from Slave to Master
* \param dataOrder	- Reference to Data order bits (LSB_FIRST or MSB_FIRST)
* \param spiMode	- Reference to Master/Slave Select (MASTER_MODE or SLAVE_MODE)
* \param divider    - Reference to Oscillator Frequency (divide by 2,4,8,16,32,64,128)
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
);

/**
* \brief			 This method is used to select the first bit to be transmitted
*
* \param dataOrder   - When this parameter take LSB_FIRST, the LSB of data word is transmitted first
					 - When this parameter take MSB_FIRST, the MSB of data word is transmitted first
*
* \return void
*/
void setSpiBitOrder(uint8_t dataOrder);

/**
* \brief		  This method is used to select SPI mode
*
* \param mode	- This parameter can take two values:
*					 - MASTER_MODE -> the device is a master
*					 - SLAVE_MODE -> the device is a slave
* \return void
*/
void setSpiMode(uint8_t spiMode);

/**
* \brief			This method is used to select the SPI clock rate
*
* \param divider	- this parameter take the clock divider value
*
* \return void
*/
void setSpiClockRate(uint8_t divider);

/**
* \brief			This method is used to transmit 1 byte through SPI
*
* \param data		- the buffer that store the bits to be transmitted
*
* \return void
*/
void spiTransmitByPolling(uint8_t data);

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
void spiTransmitByInterrupt(uint8_t *sTxBuffer, uint8_t *sRxBuffer, uint8_t nBytesToTransmit);



#endif