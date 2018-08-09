/*
* avr_twi_i2c.h
*
* Created: 01.01.2009 00:00:00
* Author : Bogdan Tarnauca
*/

#ifndef _AVR_TWI_I2C_H_
    #define _AVR_TWI_I2C_H_

//Apps must include an "main_config.h" header where libraries related params, overrides, settings should be defined.
//This header file is application specific. Must be provided even it is empty.
#include "main_config.h"

#include "avr_defines.h"
#include "avr_utilitare.h"


//
// TWI (I2C) MASTER
//
//Fisierul contine implementarea accesului la interfata TWI (I2C)
//in modul master. Microcontrollerul este singurul master de pe
//liniile magistralei, toate celelalte dispozitive de pe magistrala
//fiind considerate slave.

#define TWI_TIMEOUT_WAIT_LOOP_COUNT 2000
//values of twiStatus
#define TWI_IDLE                0x00
#define TWI_TRANSFER_ONGOING    0x01

#define TWI_LOST_ARBITRATION        0x38
#define TWI_SLAR_R_NACK_RECEIVED    0x48
#define TWI_SLAR_W_NACK_RECEIVED    0x20
#define TWI_DATA_TX_NACK_RECEIVED   0x20
#define TWI_UNKNOWN_STATUS      0xFF
#define TWI_STOP_COND_TIMEOUT   0xFE
#define TWI_WRITE_TIMEOUT       0xFD
#define TWI_READ_TIMEOUT        0xFC

#define TWI_RD      1
#define TWI_WR      0

//masca pentru bitii de stare din TWSR
#define TWS_MASK    0xF8

volatile unsigned char twiStatus;
volatile unsigned char twiRxCompleted;
volatile unsigned char twiTxCompleted;
volatile unsigned char twiAddress;
volatile unsigned char twiCommand;
volatile unsigned char twiCounter;
volatile unsigned char bytesToTransmit;
volatile unsigned char bytesToReceive;
volatile char *twiRxBuffer;
volatile char *twiTxBuffer;
volatile int timeoutLoopCount;


/**
*** @fn      char twi_Stop(bool bWaitForStop)
***
*** @brief   Generates a stop condition and waits until the stop condition is sent if bWaitForStop is set
***
*** @param   bWaitForStop indicates whether the function waits for the stop condition or exits directly
*** @return  If bWaitForStop is set it returns TWI_STOP_COND_TIMEOUT or TWI_IDLE.
***          If bWaitForStop is not set it returns TWI_TRANSFER_ON	(bool bWaitForStop);


*** @fn      char twi_Write(unsigned char nAddress, char *sTxBuffer, unsigned char nBytesToTransmit, bool bWaitForTransmit)
***
*** @brief   Initiates a twi write operation.
***
*** @param   nAddress TWI address of the recipient device
*** @param   *sTxBuffer The buffer containing the bytes to be transmited
*** @param   nBytesToTransmit number of bytes to be transmitted
*** @param   bWaitForTransmit Indicates whether the function waits for the bytes to be sent or exits directly
***
*** @return  If bWaitForStop is set it returns TWI_WRITE_TIMEOUT or TWI_IDLE or one of the TWI error codes.
***          If bWaitForTransmit is not set it returns TWI_TRANSFER_ONGOING
**/
unsigned char twi_Write(unsigned char nAddress, volatile char *sTxBuffer, unsigned char nBytesToTransmit, bool bWaitForTransmit);

/**
*** @fn      char twi_Read(unsigned char nAddress, char *sRxBuffer, unsigned char nBytesToReceive, bool bWaitForReceive)
***
*** @brief   Initiates a twi read operation.
***
*** @param   nAddress TWI address of the read device
*** @param   *sRxBuffer The buffer where the received bytes will be stored
*** @param   nBytesToReceive number of bytes to be received
*** @param   bWaitForReceive Indicates whether the function waits for the bytes to be read or exits directly
***
*** @return  If bWaitForStop is set it returns TWI_READ_TIMEOUT, TWI_IDLE or one of the TWI error codes.
***          If bWaitForTransmit is not set it returns TWI_TRANSFER_ONGOING
**/
unsigned char twi_Read(unsigned char nAddress, volatile char *sRxBuffer, unsigned char nBytesToReceive, bool bWaitForReceive);

/**
*** @fn      twi_init(volatile uint8_t *twiPort, unsigned char twiSCLPin, unsigned char twiSDAPin, unsigned char nTWBR, unsigned char nTWSR, unsigned char nTWAR)
***
*** @brief   Initializes the twi interface.
***
*** @param   *twiPort pointer to the port providing the twi pins
*** @param   twiSCLPin index of the SCL pin (0..7)
*** @param   twiSDAPin index of the SDA pin (0..7)
*** @param   nTWBR The value for the TWI Bit Rate Register
*** @param   nTWSR The value for the TWI Status Register
*** @param   nTWAR The value for the TWI Data Register
*** 
*** @return  none.
**/
void twi_Init(volatile uint8_t *twiPort, unsigned char twiSCLPin, unsigned char twiSDAPin, unsigned char nTWBR, unsigned char nTWSR, unsigned char nTWAR);

ISR(TWI_vect);

#endif /* _AVR_TWI_I2C_H_ */