/*
 * lcd_driver.c
 *
 * Created: 7/31/2018 10:24:50 AM
 *  Author: gabriel.brasoveanu
 */ 

#include "spi_driver.h"
#include "lcd_driver.h"
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/pgmspace.h>

static const uint8_t Font[] = {
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x3E, 0x5B, 0x4F, 0x5B, 0x3E,
	0x3E, 0x6B, 0x4F, 0x6B, 0x3E,
	0x1C, 0x3E, 0x7C, 0x3E, 0x1C,
	0x18, 0x3C, 0x7E, 0x3C, 0x18,
	0x1C, 0x57, 0x7D, 0x57, 0x1C,
	0x1C, 0x5E, 0x7F, 0x5E, 0x1C,
	0x00, 0x18, 0x3C, 0x18, 0x00,
	0xFF, 0xE7, 0xC3, 0xE7, 0xFF,
	0x00, 0x18, 0x24, 0x18, 0x00,
	0xFF, 0xE7, 0xDB, 0xE7, 0xFF,
	0x30, 0x48, 0x3A, 0x06, 0x0E,
	0x26, 0x29, 0x79, 0x29, 0x26,
	0x40, 0x7F, 0x05, 0x05, 0x07,
	0x40, 0x7F, 0x05, 0x25, 0x3F,
	0x5A, 0x3C, 0xE7, 0x3C, 0x5A,
	0x7F, 0x3E, 0x1C, 0x1C, 0x08,
	0x08, 0x1C, 0x1C, 0x3E, 0x7F,
	0x14, 0x22, 0x7F, 0x22, 0x14,
	0x5F, 0x5F, 0x00, 0x5F, 0x5F,
	0x06, 0x09, 0x7F, 0x01, 0x7F,
	0x00, 0x66, 0x89, 0x95, 0x6A,
	0x60, 0x60, 0x60, 0x60, 0x60,
	0x94, 0xA2, 0xFF, 0xA2, 0x94,
	0x08, 0x04, 0x7E, 0x04, 0x08,
	0x10, 0x20, 0x7E, 0x20, 0x10,
	0x08, 0x08, 0x2A, 0x1C, 0x08,
	0x08, 0x1C, 0x2A, 0x08, 0x08,
	0x1E, 0x10, 0x10, 0x10, 0x10,
	0x0C, 0x1E, 0x0C, 0x1E, 0x0C,
	0x30, 0x38, 0x3E, 0x38, 0x30,
	0x06, 0x0E, 0x3E, 0x0E, 0x06,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x5F, 0x00, 0x00,
	0x00, 0x07, 0x00, 0x07, 0x00,
	0x14, 0x7F, 0x14, 0x7F, 0x14,
	0x24, 0x2A, 0x7F, 0x2A, 0x12,
	0x23, 0x13, 0x08, 0x64, 0x62,
	0x36, 0x49, 0x56, 0x20, 0x50,
	0x00, 0x08, 0x07, 0x03, 0x00,
	0x00, 0x1C, 0x22, 0x41, 0x00,
	0x00, 0x41, 0x22, 0x1C, 0x00,
	0x2A, 0x1C, 0x7F, 0x1C, 0x2A,
	0x08, 0x08, 0x3E, 0x08, 0x08,
	0x00, 0x80, 0x70, 0x30, 0x00,
	0x08, 0x08, 0x08, 0x08, 0x08,
	0x00, 0x00, 0x60, 0x60, 0x00,
	0x20, 0x10, 0x08, 0x04, 0x02,
	0x3E, 0x51, 0x49, 0x45, 0x3E, // 0
	0x00, 0x42, 0x7F, 0x40, 0x00, // 1
	0x72, 0x49, 0x49, 0x49, 0x46, // 2
	0x21, 0x41, 0x49, 0x4D, 0x33, // 3
	0x18, 0x14, 0x12, 0x7F, 0x10, // 4
	0x27, 0x45, 0x45, 0x45, 0x39, // 5
	0x3C, 0x4A, 0x49, 0x49, 0x31, // 6
	0x41, 0x21, 0x11, 0x09, 0x07, // 7
	0x36, 0x49, 0x49, 0x49, 0x36, // 8
	0x46, 0x49, 0x49, 0x29, 0x1E, // 9
	0x00, 0x00, 0x14, 0x00, 0x00,
	0x00, 0x40, 0x34, 0x00, 0x00,
	0x00, 0x08, 0x14, 0x22, 0x41,
	0x14, 0x14, 0x14, 0x14, 0x14,
	0x00, 0x41, 0x22, 0x14, 0x08,
	0x02, 0x01, 0x59, 0x09, 0x06,
	0x3E, 0x41, 0x5D, 0x59, 0x4E,
	0x7C, 0x12, 0x11, 0x12, 0x7C, // A
	0x7F, 0x49, 0x49, 0x49, 0x36, // B
	0x3E, 0x41, 0x41, 0x41, 0x22, // C
	0x7F, 0x41, 0x41, 0x41, 0x3E, // D
	0x7F, 0x49, 0x49, 0x49, 0x41, // E
	0x7F, 0x09, 0x09, 0x09, 0x01, // F
	0x3E, 0x41, 0x41, 0x51, 0x73, // G
	0x7F, 0x08, 0x08, 0x08, 0x7F, // H
	0x00, 0x41, 0x7F, 0x41, 0x00, // I
	0x20, 0x40, 0x41, 0x3F, 0x01, // J
	0x7F, 0x08, 0x14, 0x22, 0x41, // K
	0x7F, 0x40, 0x40, 0x40, 0x40, // L
	0x7F, 0x02, 0x1C, 0x02, 0x7F, // M
	0x7F, 0x04, 0x08, 0x10, 0x7F, // N
	0x3E, 0x41, 0x41, 0x41, 0x3E, // O
	0x7F, 0x09, 0x09, 0x09, 0x06, // P
	0x3E, 0x41, 0x51, 0x21, 0x5E, // Q
	0x7F, 0x09, 0x19, 0x29, 0x46, // R
	0x26, 0x49, 0x49, 0x49, 0x32, // S
	0x03, 0x01, 0x7F, 0x01, 0x03, // T
	0x3F, 0x40, 0x40, 0x40, 0x3F, // U
	0x1F, 0x20, 0x40, 0x20, 0x1F, // V
	0x3F, 0x40, 0x38, 0x40, 0x3F, // W
	0x63, 0x14, 0x08, 0x14, 0x63, // X
	0x03, 0x04, 0x78, 0x04, 0x03, // Y
	0x61, 0x59, 0x49, 0x4D, 0x43, // Z
	0x00, 0x7F, 0x41, 0x41, 0x41,
	0x02, 0x04, 0x08, 0x10, 0x20,
	0x00, 0x41, 0x41, 0x41, 0x7F,
	0x04, 0x02, 0x01, 0x02, 0x04,
	0x40, 0x40, 0x40, 0x40, 0x40,
	0x00, 0x03, 0x07, 0x08, 0x00,
	0x20, 0x54, 0x54, 0x78, 0x40, // a
	0x7F, 0x28, 0x44, 0x44, 0x38, // b
	0x38, 0x44, 0x44, 0x44, 0x28, // c
	0x38, 0x44, 0x44, 0x28, 0x7F, // d
	0x38, 0x54, 0x54, 0x54, 0x18, // e
	0x00, 0x08, 0x7E, 0x09, 0x02, // f
	0x18, 0xA4, 0xA4, 0x9C, 0x78, // g
	0x7F, 0x08, 0x04, 0x04, 0x78, // h
	0x00, 0x44, 0x7D, 0x40, 0x00, // i
	0x20, 0x40, 0x40, 0x3D, 0x00, // j
	0x7F, 0x10, 0x28, 0x44, 0x00, // k
	0x00, 0x41, 0x7F, 0x40, 0x00, // l
	0x7C, 0x04, 0x78, 0x04, 0x78, // m
	0x7C, 0x08, 0x04, 0x04, 0x78, // n
	0x38, 0x44, 0x44, 0x44, 0x38, // o
	0xFC, 0x18, 0x24, 0x24, 0x18, // p
	0x18, 0x24, 0x24, 0x18, 0xFC, // q
	0x7C, 0x08, 0x04, 0x04, 0x08, // r
	0x48, 0x54, 0x54, 0x54, 0x24, // s
	0x04, 0x04, 0x3F, 0x44, 0x24, // t
	0x3C, 0x40, 0x40, 0x20, 0x7C, // u
	0x1C, 0x20, 0x40, 0x20, 0x1C, // v
	0x3C, 0x40, 0x30, 0x40, 0x3C, // w
	0x44, 0x28, 0x10, 0x28, 0x44, // x
	0x4C, 0x90, 0x90, 0x90, 0x7C, // y
	0x44, 0x64, 0x54, 0x4C, 0x44, // z
	0x00, 0x08, 0x36, 0x41, 0x00,
	0x00, 0x00, 0x77, 0x00, 0x00,
	0x00, 0x41, 0x36, 0x08, 0x00,
	0x02, 0x01, 0x02, 0x04, 0x02,
	0x3C, 0x26, 0x23, 0x26, 0x3C,
	0x1E, 0xA1, 0xA1, 0x61, 0x12,
	0x3A, 0x40, 0x40, 0x20, 0x7A,
	0x38, 0x54, 0x54, 0x55, 0x59,
	0x21, 0x55, 0x55, 0x79, 0x41,
	0x21, 0x54, 0x54, 0x78, 0x41,
	0x21, 0x55, 0x54, 0x78, 0x40,
	0x20, 0x54, 0x55, 0x79, 0x40,
	0x0C, 0x1E, 0x52, 0x72, 0x12,
	0x39, 0x55, 0x55, 0x55, 0x59,
	0x39, 0x54, 0x54, 0x54, 0x59,
	0x39, 0x55, 0x54, 0x54, 0x58,
	0x00, 0x00, 0x45, 0x7C, 0x41,
	0x00, 0x02, 0x45, 0x7D, 0x42,
	0x00, 0x01, 0x45, 0x7C, 0x40,
	0xF0, 0x29, 0x24, 0x29, 0xF0,
	0xF0, 0x28, 0x25, 0x28, 0xF0,
	0x7C, 0x54, 0x55, 0x45, 0x00,
	0x20, 0x54, 0x54, 0x7C, 0x54,
	0x7C, 0x0A, 0x09, 0x7F, 0x49,
	0x32, 0x49, 0x49, 0x49, 0x32,
	0x32, 0x48, 0x48, 0x48, 0x32,
	0x32, 0x4A, 0x48, 0x48, 0x30,
	0x3A, 0x41, 0x41, 0x21, 0x7A,
	0x3A, 0x42, 0x40, 0x20, 0x78,
	0x00, 0x9D, 0xA0, 0xA0, 0x7D,
	0x39, 0x44, 0x44, 0x44, 0x39,
	0x3D, 0x40, 0x40, 0x40, 0x3D,
	0x3C, 0x24, 0xFF, 0x24, 0x24,
	0x48, 0x7E, 0x49, 0x43, 0x66,
	0x2B, 0x2F, 0xFC, 0x2F, 0x2B,
	0xFF, 0x09, 0x29, 0xF6, 0x20,
	0xC0, 0x88, 0x7E, 0x09, 0x03,
	0x20, 0x54, 0x54, 0x79, 0x41,
	0x00, 0x00, 0x44, 0x7D, 0x41,
	0x30, 0x48, 0x48, 0x4A, 0x32,
	0x38, 0x40, 0x40, 0x22, 0x7A,
	0x00, 0x7A, 0x0A, 0x0A, 0x72,
	0x7D, 0x0D, 0x19, 0x31, 0x7D,
	0x26, 0x29, 0x29, 0x2F, 0x28,
	0x26, 0x29, 0x29, 0x29, 0x26,
	0x30, 0x48, 0x4D, 0x40, 0x20,
	0x38, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x38,
	0x2F, 0x10, 0xC8, 0xAC, 0xBA,
	0x2F, 0x10, 0x28, 0x34, 0xFA,
	0x00, 0x00, 0x7B, 0x00, 0x00,
	0x08, 0x14, 0x2A, 0x14, 0x22,
	0x22, 0x14, 0x2A, 0x14, 0x08,
	0xAA, 0x00, 0x55, 0x00, 0xAA,
	0xAA, 0x55, 0xAA, 0x55, 0xAA,
	0x00, 0x00, 0x00, 0xFF, 0x00,
	0x10, 0x10, 0x10, 0xFF, 0x00,
	0x14, 0x14, 0x14, 0xFF, 0x00,
	0x10, 0x10, 0xFF, 0x00, 0xFF,
	0x10, 0x10, 0xF0, 0x10, 0xF0,
	0x14, 0x14, 0x14, 0xFC, 0x00,
	0x14, 0x14, 0xF7, 0x00, 0xFF,
	0x00, 0x00, 0xFF, 0x00, 0xFF,
	0x14, 0x14, 0xF4, 0x04, 0xFC,
	0x14, 0x14, 0x17, 0x10, 0x1F,
	0x10, 0x10, 0x1F, 0x10, 0x1F,
	0x14, 0x14, 0x14, 0x1F, 0x00,
	0x10, 0x10, 0x10, 0xF0, 0x00,
	0x00, 0x00, 0x00, 0x1F, 0x10,
	0x10, 0x10, 0x10, 0x1F, 0x10,
	0x10, 0x10, 0x10, 0xF0, 0x10,
	0x00, 0x00, 0x00, 0xFF, 0x10,
	0x10, 0x10, 0x10, 0x10, 0x10,
	0x10, 0x10, 0x10, 0xFF, 0x10,
	0x00, 0x00, 0x00, 0xFF, 0x14,
	0x00, 0x00, 0xFF, 0x00, 0xFF,
	0x00, 0x00, 0x1F, 0x10, 0x17,
	0x00, 0x00, 0xFC, 0x04, 0xF4,
	0x14, 0x14, 0x17, 0x10, 0x17,
	0x14, 0x14, 0xF4, 0x04, 0xF4,
	0x00, 0x00, 0xFF, 0x00, 0xF7,
	0x14, 0x14, 0x14, 0x14, 0x14,
	0x14, 0x14, 0xF7, 0x00, 0xF7,
	0x14, 0x14, 0x14, 0x17, 0x14,
	0x10, 0x10, 0x1F, 0x10, 0x1F,
	0x14, 0x14, 0x14, 0xF4, 0x14,
	0x10, 0x10, 0xF0, 0x10, 0xF0,
	0x00, 0x00, 0x1F, 0x10, 0x1F,
	0x00, 0x00, 0x00, 0x1F, 0x14,
	0x00, 0x00, 0x00, 0xFC, 0x14,
	0x00, 0x00, 0xF0, 0x10, 0xF0,
	0x10, 0x10, 0xFF, 0x10, 0xFF,
	0x14, 0x14, 0x14, 0xFF, 0x14,
	0x10, 0x10, 0x10, 0x1F, 0x00,
	0x00, 0x00, 0x00, 0xF0, 0x10,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
	0xFF, 0xFF, 0xFF, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0xFF,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x38, 0x44, 0x44, 0x38, 0x44,
	0x7C, 0x2A, 0x2A, 0x3E, 0x14,
	0x7E, 0x02, 0x02, 0x06, 0x06,
	0x02, 0x7E, 0x02, 0x7E, 0x02,
	0x63, 0x55, 0x49, 0x41, 0x63,
	0x38, 0x44, 0x44, 0x3C, 0x04,
	0x40, 0x7E, 0x20, 0x1E, 0x20,
	0x06, 0x02, 0x7E, 0x02, 0x02,
	0x99, 0xA5, 0xE7, 0xA5, 0x99,
	0x1C, 0x2A, 0x49, 0x2A, 0x1C,
	0x4C, 0x72, 0x01, 0x72, 0x4C,
	0x30, 0x4A, 0x4D, 0x4D, 0x30,
	0x30, 0x48, 0x78, 0x48, 0x30,
	0xBC, 0x62, 0x5A, 0x46, 0x3D,
	0x3E, 0x49, 0x49, 0x49, 0x00,
	0x7E, 0x01, 0x01, 0x01, 0x7E,
	0x2A, 0x2A, 0x2A, 0x2A, 0x2A,
	0x44, 0x44, 0x5F, 0x44, 0x44,
	0x40, 0x51, 0x4A, 0x44, 0x40,
	0x40, 0x44, 0x4A, 0x51, 0x40,
	0x00, 0x00, 0xFF, 0x01, 0x03,
	0xE0, 0x80, 0xFF, 0x00, 0x00,
	0x08, 0x08, 0x6B, 0x6B, 0x08,
	0x36, 0x12, 0x36, 0x24, 0x36,
	0x06, 0x0F, 0x09, 0x0F, 0x06,
	0x00, 0x00, 0x18, 0x18, 0x00,
	0x00, 0x00, 0x10, 0x10, 0x00,
	0x30, 0x40, 0xFF, 0x01, 0x01,
	0x00, 0x1F, 0x01, 0x01, 0x1E,
	0x00, 0x19, 0x1D, 0x17, 0x12,
	0x00, 0x3C, 0x3C, 0x3C, 0x3C,
	0x00, 0x00, 0x00, 0x00, 0x00,
};

void lcdPortInit(volatile uint8_t *portLcd, unsigned char RSPin, unsigned char DCPin)
{
	lcdInit.lcdPort = portLcd;
	lcdInit.DCPin = DCPin;
	lcdInit.RSPin = RSPin;
	
	//DDR - set DDR and make RS, D/C output pin
	*(lcdInit.lcdPort-1) |= (1<<lcdInit.DCPin) | (1<<lcdInit.DCPin);
	*(lcdInit.lcdPort) &= ~((1<<lcdInit.RSPin) | (1<<lcdInit.DCPin));
}

void lcdSendData(uint8_t data)
{
	// Put D/C HIGH (sending data)
	*(lcdInit.lcdPort) |= (1<<lcdInit.DCPin);
	// Put SS to LOW (select this device)
	*(spiInit.spiPort) &= ~(1<<spiInit.SSPin);
	// Send data
	spiWrite(data);
	// Put SS to HIGH (deselect this device)
	*(spiInit.spiPort) |= (1<<spiInit.SSPin);
}

void lcdSendCommand(uint8_t data)
{
	// Put D/C LOW (sending command)
	*(lcdInit.lcdPort) &= ~(1<<lcdInit.DCPin);
	// Put SS to LOW (select this device)
	*(spiInit.spiPort) &= ~(1<<spiInit.SSPin);
	// Send data
	spiWrite(data);
	// Put SS to HIGH (deselect this device)
	*(spiInit.spiPort) |= (1<<spiInit.SSPin);
}



void Initialize_LCD(void)
{
	//Reset the LCD controller
	*(lcdInit.lcdPort) &= ~(1<<lcdInit.RSPin);
	_delay_us(10);
	*(lcdInit.lcdPort) |= (1<<lcdInit.RSPin);
	_delay_ms(120);

	//SLPOUT (11h): Sleep Out ("Sleep Out"  is chingrish for "wake")
	//The DC/DC converter is enabled, Internal display oscillator
	//is started, and panel scanning is started.
	lcdSendCommand(SLPOUT);
	_delay_ms(120);

	//FRMCTR1 (B1h): Frame Rate Control (In normal mode/ Full colors)
	//Set the frame frequency of the full colors normal mode.
	// * Frame rate=fosc/((RTNA + 20) x (LINE + FPA + BPA))
	// * 1 < FPA(front porch) + BPA(back porch) ; Back porch ?0
	//Note: fosc = 333kHz
	lcdSendCommand(FRMCTR1);//In normal mode(Full colors)
	lcdSendData(0x02);//RTNB: set 1-line period
	lcdSendData(0x35);//FPB:  front porch
	lcdSendData(0x36);//BPB:  back porch

	//FRMCTR2 (B2h): Frame Rate Control (In Idle mode/ 8-colors)
	//Set the frame frequency of the Idle mode.
	// * Frame rate=fosc/((RTNB + 20) x (LINE + FPB + BPB))
	// * 1 < FPB(front porch) + BPB(back porch) ; Back porch ?0
	//Note: fosc = 333kHz
	lcdSendCommand(FRMCTR2);//In Idle mode (8-colors)
	lcdSendData(0x02);//RTNB: set 1-line period
	lcdSendData(0x35);//FPB:  front porch
	lcdSendData(0x36);//BPB:  back porch

	//FRMCTR3 (B3h): Frame Rate Control (In Partial mode/ full colors)
	//Set the frame frequency of the Partial mode/ full colors.
	// * 1st parameter to 3rd parameter are used in line inversion mode.
	// * 4th parameter to 6th parameter are used in frame inversion mode.
	// * Frame rate=fosc/((RTNC + 20) x (LINE + FPC + BPC))
	// * 1 < FPC(front porch) + BPC(back porch) ; Back porch ?0
	//Note: fosc = 333kHz
	lcdSendCommand(FRMCTR3);//In partial mode + Full colors
	lcdSendData(0x02);//RTNC: set 1-line period
	lcdSendData(0x35);//FPC:  front porch
	lcdSendData(0x36);//BPC:  back porch
	lcdSendData(0x02);//RTND: set 1-line period
	lcdSendData(0x35);//FPD:  front porch
	lcdSendData(0x36);//BPD:  back porch

	//INVCTR (B4h): Display Inversion Control
	lcdSendCommand(INVCTR);
	lcdSendData(0x07);
	// 0000 0ABC
	// |||| ||||-- NLC: Inversion setting in full Colors partial mode
	// |||| |||         (0=Line Inversion, 1 = Frame Inversion)
	// |||| |||--- NLB: Inversion setting in idle mode
	// |||| ||          (0=Line Inversion, 1 = Frame Inversion)
	// |||| ||---- NLA: Inversion setting in full Colors normal mode
	// |||| |----- Unused: 0

	//PWCTR1 (C0h): Power Control 1
	lcdSendCommand(PWCTR1);
	lcdSendData(0x02);// VRH[4:0] (0-31) Sets GVDD
	// VRH=0x00 => GVDD=5.0v
	// VRH=0x1F => GVDD=3.0v
	// Each tick is a variable step:
	// VRH[4:0] |  VRH | GVDD
	//   00000b | 0x00 | 5.00v
	//   00001b | 0x01 | 4.75v
	//   00010b | 0x02 | 4.70v <<<<<
	//   00011b | 0x03 | 4.65v
	//   00100b | 0x04 | 4.60v
	//   00101b | 0x05 | 4.55v
	//   00110b | 0x06 | 4.50v
	//   00111b | 0x07 | 4.45v
	//   01000b | 0x08 | 4.40v
	//   01001b | 0x09 | 4.35v
	//   01010b | 0x0A | 4.30v
	//   01011b | 0x0B | 4.25v
	//   01100b | 0x0C | 4.20v
	//   01101b | 0x0D | 4.15v
	//   01110b | 0x0E | 4.10v
	//   01111b | 0x0F | 4.05v
	//   10000b | 0x10 | 4.00v
	//   10001b | 0x11 | 3.95v
	//   10010b | 0x12 | 3.90v
	//   10011b | 0x13 | 3.85v
	//   10100b | 0x14 | 3.80v
	//   10101b | 0x15 | 3.75v
	//   10110b | 0x16 | 3.70v
	//   10111b | 0x17 | 3.65v
	//   11000b | 0x18 | 3.60v
	//   11001b | 0x19 | 3.55v
	//   11010b | 0x1A | 3.50v
	//   11011b | 0x1B | 3.45v
	//   11100b | 0x1C | 3.40v
	//   11101b | 0x1D | 3.35v
	//   11110b | 0x1E | 3.25v
	//   11111b | 0x1F | 3.00v
	lcdSendData(0x02);// 010i i000
	// |||| ||||-- Unused: 0
	// |||| |----- IB_SEL0:
	// ||||------- IB_SEL1:
	// |||-------- Unused: 010
	// IB_SEL[1:0] | IB_SEL | AVDD
	//         00b | 0x00   | 2.5�A   <<<<<
	//         01b | 0x01   | 2.0�A
	//         10b | 0x02   | 1.5�A
	//         11b | 0x03   | 1.0�A

	//PWCTR2 (C1h): Power Control 2
	// * Set the VGH and VGL supply power level
	//Restriction: VGH-VGL <= 32V
	lcdSendCommand(PWCTR2);
	lcdSendData(0xC5);// BT[2:0] (0-15) Sets GVDD
	// BT[2:0] |    VGH      |     VGL
	//    000b | 4X |  9.80v | -3X |  -7.35v
	//    001b | 4X |  9.80v | -4X |  -9.80v
	//    010b | 5X | 12.25v | -3X |  -7.35v
	//    011b | 5X | 12.25v | -4X |  -9.80v
	//    100b | 5X | 12.25v | -5X | -12.25v
	//    101b | 6X | 14.70v | -3X |  -7.35v   <<<<<
	//    110b | 6X | 14.70v | -4X |  -9.80v
	//    111b | 6X | 14.70v | -5X | -12.25v

	//PWCTR3 (C2h): Power Control 3 (in Normal mode/ Full colors)
	// * Set the amount of current in Operational amplifier in
	//   normal mode/full colors.
	// * Adjust the amount of fixed current from the fixed current
	//   source in the operational amplifier for the source driver.
	// * Set the Booster circuit Step-up cycle in Normal mode/ full
	//   colors.
	lcdSendCommand(PWCTR3);
	lcdSendData(0x0D);// AP[2:0] Sets Operational Amplifier Bias Current
	// AP[2:0] | Function
	//    000b | Off
	//    001b | Small
	//    010b | Medium Low
	//    011b | Medium
	//    100b | Medium High
	//    101b | Large          <<<<<
	//    110b | reserved
	//    111b | reserved
	lcdSendData(0x00);// DC[2:0] Booster Frequency
	// DC[2:0] | Circuit 1 | Circuit 2,4
	//    000b | BCLK / 1  | BCLK / 1  <<<<<
	//    001b | BCLK / 1  | BCLK / 2
	//    010b | BCLK / 1  | BCLK / 4
	//    011b | BCLK / 2  | BCLK / 2
	//    100b | BCLK / 2  | BCLK / 4
	//    101b | BCLK / 4  | BCLK / 4
	//    110b | BCLK / 4  | BCLK / 8
	//    111b | BCLK / 4  | BCLK / 16

	//PWCTR4 (C3h): Power Control 4 (in Idle mode/ 8-colors)
	// * Set the amount of current in Operational amplifier in
	//   normal mode/full colors.
	// * Adjust the amount of fixed current from the fixed current
	//   source in the operational amplifier for the source driver.
	// * Set the Booster circuit Step-up cycle in Normal mode/ full
	//   colors.
	lcdSendCommand(PWCTR4);
	lcdSendData(0x8D);// AP[2:0] Sets Operational Amplifier Bias Current
	// AP[2:0] | Function
	//    000b | Off
	//    001b | Small
	//    010b | Medium Low
	//    011b | Medium
	//    100b | Medium High
	//    101b | Large          <<<<<
	//    110b | reserved
	//    111b | reserved
	lcdSendData(0x1A);// DC[2:0] Booster Frequency
	// DC[2:0] | Circuit 1 | Circuit 2,4
	//    000b | BCLK / 1  | BCLK / 1
	//    001b | BCLK / 1  | BCLK / 2
	//    010b | BCLK / 1  | BCLK / 4  <<<<<
	//    011b | BCLK / 2  | BCLK / 2
	//    100b | BCLK / 2  | BCLK / 4
	//    101b | BCLK / 4  | BCLK / 4
	//    110b | BCLK / 4  | BCLK / 8
	//    111b | BCLK / 4  | BCLK / 16

	//PPWCTR5 (C4h): Power Control 5 (in Partial mode/ full-colors)
	// * Set the amount of current in Operational amplifier in
	//   normal mode/full colors.
	// * Adjust the amount of fixed current from the fixed current
	//   source in the operational amplifier for the source driver.
	// * Set the Booster circuit Step-up cycle in Normal mode/ full
	//   colors.
	lcdSendCommand(PWCTR5);
	lcdSendData(0x8D);// AP[2:0] Sets Operational Amplifier Bias Current
	// AP[2:0] | Function
	//    000b | Off
	//    001b | Small
	//    010b | Medium Low
	//    011b | Medium
	//    100b | Medium High
	//    101b | Large          <<<<<
	//    110b | reserved
	//    111b | reserved
	lcdSendData(0xEE);// DC[2:0] Booster Frequency
	// DC[2:0] | Circuit 1 | Circuit 2,4
	//    000b | BCLK / 1  | BCLK / 1
	//    001b | BCLK / 1  | BCLK / 2
	//    010b | BCLK / 1  | BCLK / 4
	//    011b | BCLK / 2  | BCLK / 2
	//    100b | BCLK / 2  | BCLK / 4
	//    101b | BCLK / 4  | BCLK / 4
	//    110b | BCLK / 4  | BCLK / 8  <<<<<
	//    111b | BCLK / 4  | BCLK / 16

	//VMCTR1 (C5h): VCOM Control 1
	lcdSendCommand(VMCTR1);
	lcdSendData(0x51);// Default: 0x51 => +4.525
	// VMH[6:0] (0-100) Sets VCOMH
	// VMH=0x00 => VCOMH= +2.5v
	// VMH=0x64 => VCOMH= +5.0v
	lcdSendData(0x4D);// Default: 0x4D => -0.575
	// VML[6:0] (4-100) Sets VCOML
	// VML=0x04 => VCOML= -2.4v
	// VML=0x64 => VCOML=  0.0v

	//GMCTRP1 (E0h): Gamma �+�polarity Correction Characteristics Setting
	lcdSendCommand(GAMCTRP1);
	lcdSendData(0x0a);
	lcdSendData(0x1c);
	lcdSendData(0x0c);
	lcdSendData(0x14);
	lcdSendData(0x33);
	lcdSendData(0x2b);
	lcdSendData(0x24);
	lcdSendData(0x28);
	lcdSendData(0x27);
	lcdSendData(0x25);
	lcdSendData(0x2C);
	lcdSendData(0x39);
	lcdSendData(0x00);
	lcdSendData(0x05);
	lcdSendData(0x03);
	lcdSendData(0x0d);

	//GMCTRN1 (E1h): Gamma �-�polarity Correction Characteristics Setting
	lcdSendCommand(GAMCTRN1);
	lcdSendData(0x0a);
	lcdSendData(0x1c);
	lcdSendData(0x0c);
	lcdSendData(0x14);
	lcdSendData(0x33);
	lcdSendData(0x2b);
	lcdSendData(0x24);
	lcdSendData(0x28);
	lcdSendData(0x27);
	lcdSendData(0x25);
	lcdSendData(0x2D);
	lcdSendData(0x3a);
	lcdSendData(0x00);
	lcdSendData(0x05);
	lcdSendData(0x03);
	lcdSendData(0x0d);

	//COLMOD (3Ah): Interface Pixel Format
	// * This command is used to define the format of RGB picture
	//   data, which is to be transferred via the MCU interface.
	lcdSendCommand(COLMOD);
	lcdSendData(0x06);// Default: 0x06 => 18-bit/pixel
	// IFPF[2:0] MCU Interface Color Format
	// IFPF[2:0] | Format
	//      000b | reserved
	//      001b | reserved
	//      010b | reserved
	//      011b | 12-bit/pixel
	//      100b | reserved
	//      101b | 16-bit/pixel
	//      110b | 18-bit/pixel   <<<<<
	//      111b | reserved

	//DISPON (29h): Display On
	// * This command is used to recover from DISPLAY OFF mode. Output
	//   from the Frame Memory is enabled.
	// * This command makes no change of contents of frame memory.
	// * This command does not change any other status.
	// * The delay time between DISPON and DISPOFF needs 120ms at least
	lcdSendCommand(DISPON);//Display On
	_delay_ms(1);

	//MADCTL (36h): Memory Data Access Control
	lcdSendCommand(MADCTL);
	lcdSendData(0xC0);// YXVL RH--
	// |||| ||||-- Unused: 0
	// |||| ||---- MH: Horizontal Refresh Order
	// |||| |        0 = left to right
	// |||| |        1 = right to left
	// |||| |----- RGB: RGB vs BGR Order
	// ||||          0 = RGB color filter panel
	// ||||          1 = BGR color filter panel
	// ||||------- ML: Vertical Refresh Order
	// |||           0 = top to bottom
	// |||           1 = bottom to top
	// |||-------- MV: Row / Column Exchange
	// ||--------- MX: Column Address Order  <<<<<
	// |---------- MY: Row Address Order

}

void setLcdForWriteAt_X_Y(uint8_t xStart, uint8_t yStart, uint8_t xStop, uint8_t yStop )
{
	lcdSendCommand(CASET); //Column address set
	//Write the parameters for the "column address set" command
	lcdSendData(0x00);     //Start MSB = XS[15:8]
	lcdSendData(xStart); //Start LSB = XS[ 7:0]
	lcdSendData(0x00);     //End MSB   = XE[15:8]
	lcdSendData(xStop);     //End LSB   = XE[ 7:0]
	
	lcdSendCommand(RASET); //Row address set
	//Write the parameters for the "row address set" command
	lcdSendData(0x00);     //Start MSB = YS[15:8]
	lcdSendData(yStart); //Start LSB = YS[ 7:0]
	lcdSendData(0x00);     //End MSB   = YE[15:8]
	lcdSendData(yStop);     //End LSB   = YE[ 7:0]
	
	//Write the "write data" command to the LCD
	//RAMWR (2Ch): Memory Write
	lcdSendCommand(RAMWR); //write data
}

void fillLCD(uint32_t color)
{
	register int
	i;
	setLcdForWriteAt_X_Y(0,0,130,130);

	//Fill display with a given RGB value
	for (i =0; i < (130 * 132); i++)
	{
		lcdSendData(color);
		lcdSendData(color >> 8);
		lcdSendData(color >> 16);
	}
}

void pushColor(uint32_t color)
{
	lcdSendData(color);
	lcdSendData(color >> 8);
	lcdSendData(color >> 16);
}

void drawPixel(uint8_t x, uint8_t y, uint32_t color)
{
	setLcdForWriteAt_X_Y(x,y,x+1,y+1);
	lcdSendData(color);
	lcdSendData(color >> 8);
	lcdSendData(color >> 16);
}

void drawHorizontalLine(uint8_t x, uint8_t y, uint8_t w, uint32_t color)
{
	setLcdForWriteAt_X_Y(x, y, x+w-1, y);
	while(w--)
	{
		lcdSendData(color);
		lcdSendData(color >> 8);
		lcdSendData(color >> 16);
	}
}

void drawVerticalLine(uint8_t x, uint8_t y, uint8_t h, uint32_t color)
{
	setLcdForWriteAt_X_Y(x, y, x, y+h-1);
	while(h--)
	{
		lcdSendData(color);
		lcdSendData(color >> 8);
		lcdSendData(color >> 16);
	}
}

void drawRect (uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint32_t color) {
	drawHorizontalLine(x, y, w, color);
	drawHorizontalLine(x, y+h-1, w, color);
	drawVerticalLine(x, y, h, color);
	drawVerticalLine(x+w-1, y, h, color);
}

void drawCircle(uint8_t x0, uint8_t y0, uint8_t r, uint32_t color)
 {
	int8_t f = 1 - r;
	int8_t ddF_x = 1;
	int8_t ddF_y = -2 * r;
	int8_t x = 0;
	int8_t y = r;

	drawPixel(x0  , y0+r, color);
	drawPixel(x0  , y0-r, color);
	drawPixel(x0+r, y0  , color);
	drawPixel(x0-r, y0  , color);

	while (x<y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		
		drawPixel(x0 + x, y0 + y, color);
		drawPixel(x0 - x, y0 + y, color);
		drawPixel(x0 + x, y0 - y, color);
		drawPixel(x0 - x, y0 - y, color);
		drawPixel(x0 + y, y0 + x, color);
		drawPixel(x0 - y, y0 + x, color);
		drawPixel(x0 + y, y0 - x, color);
		drawPixel(x0 - y, y0 - x, color);
	}
}

void fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint32_t color) {
	
	for (uint8_t i=x; i<x+w; i++) {
		drawVerticalLine(i, y, h, color);
	}
}
	
void drawCharS(uint8_t x, uint8_t y, char c, uint32_t textColor, uint32_t bgColor, uint8_t size){
	uint8_t line; // vertical column of pixels of character in font
	uint8_t i, j;
	
	for (i=0; i<6; i++ ) {
		if (i == 5)
		line = 0x0;
		else
		line = Font[(c*5)+i];
		for (j = 0; j<8; j++) 
		{
			if (line & 0x1) 
				{
					if (size == 1) // default size
						drawPixel(x+i, y+j, textColor);
					else 
					{  // big size
						fillRect(x+(i*size), y+(j*size), size, size, textColor);
					}
				} 
			else if (bgColor != textColor) 
				{
					if (size == 1) // default size
						drawPixel(x+i, y+j, bgColor);
					else 
					{  // big size
						fillRect(x+i*size, y+j*size, size, size, bgColor);
					}
				}
		line >>= 1;
		}
	}
}

uint8_t drawString(uint8_t x, uint8_t y, char *pt, uint32_t textColor, uint32_t bgColor, uint8_t characterSize){

	// If the text has reached the maximum point on the Y axis return 0
	// Maximum point on the Y axis is 128 - characterSize*6
	if(characterSize*6+y>128) return 0; 
	
	// If the text has reached the maximum point on the X axis return 0
	// Maximum point on the X axis is 128 - characterSize*6*numberOfCharacters
	if(characterSize*6*strlen(pt)+x>128) return 0;
	
	while(*pt){
		// Print first character
		drawCharS(x, y, *pt, textColor, bgColor, characterSize);
		pt++;
		// Move to the next x point
		x = x+characterSize*6; 	
	}
	
}





