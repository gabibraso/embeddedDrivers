#ifndef _SPI_DRIVER_H_
	#define _SPI_DRIVER_H_
	
#include <avr/io.h>
#include <avr/interrupt.h>


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

void spi_Init(	volatile uint8_t *Port, 
				unsigned char SSPin, 
				unsigned char SCKPin, 
				unsigned char MOSIPin, 
				unsigned char MISOPin, 
				uint8_t dataOrder,
				uint8_t spiMode,
				uint8_t divider
				);

void spiWritePooling(uint8_t data);

void spiInterruptTransmit(uint8_t *sTxBuffer, uint8_t *sRxBuffer, uint8_t nBytesToTransmit);
void setSpiBitOrder(uint8_t dataOrder);
void setSpiMode(uint8_t spiMode);
void setSpiClockRate(uint8_t divider);
#endif