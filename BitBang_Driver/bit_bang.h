#ifndef _BIT_BANG_H_
	#define _BIT_BANG_H_
	
#include <avr/io.h>
#include <stdbool.h>
#define F_CPU 7372800ul


typedef struct{
 	volatile uint8_t *bitBangPort;
 	unsigned char SSPin;
 	unsigned char SCKPin;
 	unsigned char MOSIPin;
	unsigned char MISOPin;
 } BitBangPins;

BitBangPins bitBangInit;

void bitBangStructInit(volatile uint8_t *Port, unsigned char SSPin, unsigned char SCKPin, unsigned char MOSIPin, unsigned char MISOPin);
void bitBangTransfer(uint8_t data);
void bitBangPortInit();

#endif