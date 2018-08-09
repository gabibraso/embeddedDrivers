#ifndef _SPI_DRIVER_H_
	#define _SPI_DRIVER_H_
	
#include <avr/io.h>


typedef struct{
 	volatile uint8_t *spiPort;
 	unsigned char SSPin;
 	unsigned char SCKPin;
 	unsigned char MOSIPin;
	unsigned char MISOPin;
 } spiPins;

spiPins spiInit;

void spiStructInit(volatile uint8_t *Port, unsigned char SSPin, unsigned char SCKPin, unsigned char MOSIPin, unsigned char MISOPin);
void spiPortInit();
void spiWrite(uint8_t data);


#endif