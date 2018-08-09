/*
* avr_usart.h
*
* Created: 01.01.2009
* Author : Bogdan Tarnauca
*/

#ifndef _MAIN_DEFINES_H_
#define _MAIN_DEFINES_H_

#define NR_LINII_AFISOR 4
#define NR_COLOANE_AFISOR 20

#define RX_TERM 0x0D
#define L_BUF_RX 128
#define L_BUF_TX 128
#define LED_TOGGLE_COUNT 20000

volatile unsigned char *ledPort;
unsigned char ledPinMask;
unsigned int mainCycleCount;

char sNewLine[3];
char snTelegramaRx[L_BUF_RX];
char snTelegramaTx[L_BUF_TX];

#endif /* _MAIN_DEFINES_H_ */