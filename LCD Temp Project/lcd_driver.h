/*
 * lcd_driver.h
 *
 * Created: 7/31/2018 10:25:38 AM
 *  Author: gabriel.brasoveanu
 */ 
#include <avr/io.h>

#ifndef LCD_DRIVER_H_
#define LCD_DRIVER_H_

/************************************************************************/
/*                  LCD ST7735 control register                         */
/************************************************************************/
#define SLPIN     (0x10)	// Put LCD in sleep mode
#define SLPOUT    (0x11)	// Turn off sleep mode
#define DISPOFF   (0x28)   // The output from Frame Memory is disabled and blank page inserted
#define DISPON    (0x29)	// Is used to recover from DISPLAY OFF mode
#define CASET     (0x2A)	// Column Address Set#define RASET     (0x2B)	// Row Address Set
#define RAMWR     (0x2C)	// Memory Write
#define RAMRD     (0x2E)	// Memory Read
#define PTLAR     (0x30)	// Used to define the partial mode display area
#define MADCTL    (0x36)	// Used to define read/ write scanning direction of frame memory
#define COLMOD    (0x3A)	// Used to define the format of RGB picture data, which is to be transferred via the MCU interface
#define FRMCTR1   (0xB1)	// Used to set the frame frequency of the full colors normal mode
#define FRMCTR2   (0xB2)	// Used to set the frame frequency of the Idle mode
#define FRMCTR3   (0xB3)	// Used to set the frame frequency of the Partial mode/ full colors
#define INVCTR    (0xB4)	// Display Inversion mode control
#define PWCTR1    (0xC0)	// Power control 1
#define PWCTR2    (0xC1)	// Power control 2
#define PWCTR3    (0xC2)	// Power control 3 (in Normal mode/ Full colors)
#define PWCTR4    (0xC3)	// Power control 4 (in Idle mode/ 8-colors)
#define PWCTR5    (0xC4)	// Power Control 5 (in Partial mode/ full-colors)
#define VMCTR1    (0xC5)	// VCOM voltage setting control 1
#define GAMCTRP1  (0xE0)   // Gamma '+' polarity Correction Characteristics Setting
#define GAMCTRN1  (0xE1)   // Gamma '-' polarity Correction Characteristics Setting


typedef struct{
	volatile uint8_t *lcdPort;
	unsigned char RSPin;
	unsigned char DCPin;
} lcdPins;

lcdPins lcdInit;

void lcdPortInit(volatile uint8_t *Port, unsigned char RSPin, unsigned char DCPin);
void Initialize_LCD(void);

void lcdSendData(uint8_t data);
void lcdSendCommand(uint8_t data);


void setLcdForWriteAt_X_Y(uint8_t xStart, uint8_t yStart, uint8_t xStop, uint8_t yStop );
void fillLCD(uint32_t color);
void fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint32_t color);
void pushColor(uint32_t color);

void drawPixel (uint8_t x, uint8_t y, uint32_t color);
void drawHorizontalLine(uint8_t x, uint8_t y, uint8_t w, uint32_t color);
void drawVerticalLine(uint8_t x, uint8_t y, uint8_t h, uint32_t color);
void drawRect (uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint32_t color);
void drawCircle(uint8_t x0, uint8_t y0, uint8_t r, uint32_t color);
void drawCharS(uint8_t x, uint8_t y, char c, uint32_t textColor, uint32_t bgColor, uint8_t size);

uint8_t drawString(uint8_t x, uint8_t y, char *pt, uint32_t textColor, uint32_t bgColor, uint8_t characterSize);



#endif /* LCD_DRIVER_H_ */