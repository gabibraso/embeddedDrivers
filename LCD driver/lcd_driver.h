/*
* lcd_driver.h
*
* Created: 7/31/2018 10:25:38 AM
*  Author: gabriel.brasoveanu
*/
#include <avr/io.h>
#define F_CPU 7372800ul
#include <stdbool.h>
#include <string.h>
#include <avr/delay.h>

#ifndef LCD_DRIVER_H_
#define LCD_DRIVER_H_

/************************************************************************/
/*                  LCD ST7735 control register                         */
/************************************************************************/
#define SLPIN     0x10  	// Put LCD in sleep mode
#define SLPOUT    0x11  	// Turn off sleep mode
#define DISPOFF   0x28      // The output from Frame Memory is disabled and blank page inserted
#define DISPON    0x29  	// Is used to recover from DISPLAY OFF mode
#define CASET     0x2A  	// Column Address Set#define RASET     0x2B  	// Row Address Set
#define RAMWR     0x2C  	// Memory Write
#define RAMRD     0x2E  	// Memory Read
#define PTLAR     0x30  	// Used to define the partial mode display area
#define MADCTL    0x36  	// Used to define read/ write scanning direction of frame memory
#define COLMOD    0x3A  	// Used to define the format of RGB picture data, which is to be transferred via the MCU interface
#define FRMCTR1   0xB1  	// Used to set the frame frequency of the full colors normal mode
#define FRMCTR2   0xB2  	// Used to set the frame frequency of the Idle mode
#define FRMCTR3   0xB3  	// Used to set the frame frequency of the Partial mode/ full colors
#define INVCTR    0xB4  	// Display Inversion mode control
#define PWCTR1    0xC0  	// Power control 1
#define PWCTR2    0xC1  	// Power control 2
#define PWCTR3    0xC2  	// Power control 3 (in Normal mode/ Full colors)
#define PWCTR4    0xC3  	// Power control 4 (in Idle mode/ 8-colors)
#define PWCTR5    0xC4  	// Power Control 5 (in Partial mode/ full-colors)
#define VMCTR1    0xC5  	// VCOM voltage setting control 1
#define GAMCTRP1  0xE0      // Gamma '+' polarity Correction Characteristics Setting
#define GAMCTRN1  0xE1      // Gamma '-' polarity Correction Characteristics Setting

#define LCD_WIDTH 130
#define LCD_HEIGHT 133
uint8_t lcdTxCommands[32];
uint8_t lcdTxData[32];

typedef struct{
	volatile uint8_t *lcdPort;
	unsigned char RSPin;
	unsigned char DCPin;
} lcdPins;

lcdPins lcdInit;

/**
* \brief		  Used to define the used pins
*
* \param Port	- Reference to port that providing the LCD inputs
* \param RSPin	- Reference to RS pin that is used to reset the LCD
* \param DCPin	- Reference to DC pin that is used to select the type of data that we transmit
*				  0 - command / 1 - data
*
* \return void
*/
void lcdInitialize(volatile uint8_t *Port, unsigned char RSPin, unsigned char DCPin);

/**
* \brief       Used to send data to LCD by polling method
*
* \param data	- The buffer that store the bits to be transmitted
*
* \return void
*/
void lcdSendDataByPolling(uint8_t data);

/**
* \brief		Used to send commands to LCD by polling method
*
* \param data   - The buffer that store the bits to be transmitted
*
* \return void
*/
void lcdSendCommandByPolling(uint8_t data);

/**
* \brief					Used to send data to LCD by interrupt method
*
* \param dataToTransmit		- The buffer that store the bits to be transmitted
* \param dataToReceive		- The buffer that store received bits
* \param numberOfBytes      - Number of bytes that we want to transmit
*
* \return void
*/
void lcdSendDataByInterrupt(uint8_t *dataToTransmit, uint8_t *dataToReceive, uint8_t numberOfBytes);


/**
* \brief				   Used to send commands to LCD by interrupt method
*
* \param dataToTransmit		- The buffer that store the bits to be transmitted
* \param dataToReceive		- The buffer that store received bits
* \param numberOfBytes	    - Number of bytes that we want to transmit
*
* \return void
*/
void lcdSendCommandByInterrupt(uint8_t *dataToTransmit, uint8_t *dataToReceive, uint8_t numberOfBytes);

/**
* \brief			Used to set the LCD cursor to the desired position on display
*
* \param xStart		- start x coordinate
* \param yStart		- start y coordinate
* \param xStop		- stop x coordinate
* \param yStop		- stop y coordinate
*
* \return void
*/
void SetLcdCursor(uint8_t xStart, uint8_t yStart, uint8_t xStop, uint8_t yStop );

/**
* \brief			Used to fill the LCD display with a desired color
*
* \param color		- the address of the variable that contain the color in hexadecimal system that we want to use (e.g 0x000000 - black)
*
* \return void
*/
void fillLCD(uint32_t* color);

/**
* \brief			Used to draw a simple horizontal line on display
*
* \param x			- start x coordinate
* \param y			- start y coordinate
* \param w			- line width  (w + x need to be less or equal with the LCD width)
* \param color		- the address of the variable that contain the color of the line (hexadecimal) that we want
*
* \return void
*/
void drawHorizontalLine(uint8_t x, uint8_t y, uint8_t w, uint32_t *color);


/**
* \brief			Used to draw a simple vertical line on display
*
* \param x			- start x coordinate
* \param y			- start y coordinate
* \param h			- line height (h + y need to be less or equal with the LCD height)
* \param color		- the address of the variable that contain the color of the line (hexadecimal) that we want
*
* \return void
*/
void drawVerticalLine(uint8_t x, uint8_t y, uint8_t h, uint32_t *color);


/**
* \brief			Used to draw a rectangle
*
* \param x			- start x coordinate
* \param y			- start y coordinate
* \param w			- width of the rectangle  (w + x need to be less or equal with the LCD width)
* \param h			- height of the rectangle (h + y need to be less or equal with the LCD height)
* \param color		- the address of the variable that contain the color of the rectangle (hexadecimal) that we want
*
* \return void
*/
void drawRect (uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint32_t *color);


/**
* \brief       Used to draw a single pixel on display
*
* \param x		- start x coordinate
* \param y		- start y coordinate
* \param color	- the address of the variable that contain the color of the pixel (hexadecimal) that we want
*
* \return void
*/
void drawPixel (uint8_t x, uint8_t y, uint32_t *color);


/**
* \brief		Used to fill a rectangle
*
* \param x		- start x coordinate
* \param y		- start y coordinate
* \param w		- width of the rectangle  (w + x need to be less or equal with the LCD width)
* \param h		- height of the rectangle (h + y need to be less or equal with the LCD height)
* \param color	- the address of the variable that contain the color of the rectangle (hexadecimal) that we want
*
* \return void
*/
void fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint32_t *color);


/**
 * \brief		Used to draw a circle on display
 * 
 * \param xStart	- start x coordinate
 * \param yStart	- start y coordinate
 * \param r			- the radius of the circle
 * \param color		- the address of the variable that contain the color of the circle (hexadecimal) that we want
 * 
 * \return void
 */
void drawCircle(uint8_t xStart, uint8_t yStart, uint8_t r, uint32_t *color);


/**
 * \brief			Used to draw a character on display
 * 
 * \param x			- start x coordinate 
 * \param y			- start y coordinate
 * \param c			- the ASCII code of character that we want to draw (in hexadecimal code)
 * \param textColor	- the address of the variable that contain the color of the character that we want
 * \param bgColor	- the address of the variable that contain the background color of the character that we want
 * \param size		- the size of character
 * 
 * \return void
 */
void drawCharS(uint8_t x, uint8_t y, char c, uint32_t *textColor, uint32_t *bgColor, uint8_t size);

/**
 * \brief			Used to draw a string on display
 * 
 * \param x				- start x coordinate
 * \param y				- start y coordinate
 * \param pt			- the address of the variable that contain the string that we want to draw
 * \param textColor		- the address of the variable that contain the color of the string that we want
 * \param bgColor		- the address of the variable that contain the background color of the string that we want
 * \param characterSize - the size of characters
 * 
 * \return uint8_t
 */
uint8_t drawString(uint8_t x, uint8_t y, char *pt, uint32_t *textColor, uint32_t *bgColor, uint8_t characterSize);



#endif /* LCD_DRIVER_H_ */