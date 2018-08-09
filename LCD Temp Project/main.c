/*
* main.c
*
* Created: 22.05.2018
* Author : Bogdan Tarnauca
*/

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "main_config.h"
#include "main_defines.h"  //Application related defines and variables
#include "avr_usart.h"     //USART driver from the libabr_uart library
#include "avr_twi_i2c.h"   //TWI (I2C) driver
#include "avr_timere_sw.h" //SW timers support
#include "avr_adc.h"       //ADC support
#include "TMP006.h"        //TMP006
#include "spi_driver.h"    //SPI driver
#include "lcd_driver.h"    //LCD driver
#include "main_MCU_init.h" //MCU initialization (ports, UART, etc.)
	


int main(void)
{
	init_devices();
	tmp006_init(0x0000);
	spiStructInit(&PORTB, PORTB0, PORTB1, PORTB2, PORTB3);
	spiPortInit();
	lcdPortInit(&PORTB, PORTB6, PORTB7);
	Initialize_LCD();
	
	char *temp1 = "Ambiental";
	char *temp2 = "Temperature";
	char *oTemp1 = "Object";
	char *oTemp2 = "Temperature";
	
	fillLCD(0x000000);
	drawRect(2,3,128,48,0xFFFFFF);
	drawRect(2,50,128,80,0xFFFFFF);
	
	
	drawString(36, 9, temp1, 0xFFFFFF, 0x000000,1);
	drawString(31, 18, temp2, 0xFFFFFF, 0x000000,1);
	drawString(46, 58, oTemp1, 0xFFFFFF, 0x000000,1);
	drawString(31, 67, oTemp2, 0xFFFFFF, 0x000000,1);
	
	while(1)
	{
		
		tmp006_read_Tambient();			 // ambientTempString
		tmp006_read_termopileSensor();   // thermopileTempString
		
	    drawString(34, 30, ambientTempString, 0xFF0000, 0x000000,2);		
		drawString(20, 80, thermopileTempString, 0x008000, 0x000000,3);
	}
	
}
