/*
* avr_defines.h
*
* Created: 01.01.2009
* Author : Bogdan Tarnauca
*/

#ifndef _AVR_DEFINES_H_
	#define _AVR_DEFINES_H_

//Apps must include an "main_config.h" header where libraries related params, overrides, settings should be defined.
//This header file is application specific. Must be provided even it is empty.
#include "main_config.h"

#include "stdbool.h"

// #ifndef bool
// typedef unsigned char bool;
// #endif
// 
// #ifndef FALSE
// #define FALSE 0
// #endif
// 
// #ifndef TRUE
// #define TRUE 1
// #endif
// 
// 
// #ifndef false
// #define false 0
// #endif
// 
// #ifndef true
// #define true 1
// #endif

#ifndef null
#define null 0
#endif

#ifndef BIT
#define BIT(x)	(1 << (x))
#endif

#define _asm	asm			/* old style */
#define WDR() 	asm("wdr")
#define SEI()	asm("sei")
#define CLI()	asm("cli")
#define NOP()	asm("nop")
#define _WDR() 	asm("wdr")
#define _SEI()	asm("sei")
#define _CLI()	asm("cli")
#define _NOP()	asm("nop")


#ifdef __WIN_AVR            //daca folosim compilatorul WinAVR
    #include <avr/io.h>
    #include <avr/interrupt.h>
    #include <util/delay.h>

#else
    #ifdef __ATMEGA8
        #include <iom8v.h>
        #define SPI_STC_ISR_VECTOR_NO       11  //interrupt_handler spi_stc_isr
        #define TWI_ISR_VECTOR_NO           18  //interrupt_handler twi_isr
        #define TIMER1_OVF_ISR_VECTOR_NO    9   //interrupt_handler timer1_ovf_isr
        #define TIMER2_OVF_ISR_VECTOR_NO    5   //interrupt_handler timer1_ovf_isr
    #endif

    #ifdef __ATMEGA168
        #include <iom168v.h>
        #define SPI_STC_ISR_VECTOR_NO       18  //interrupt_handler spi_stc_isr
        #define TWI_ISR_VECTOR_NO           25  //interrupt_handler twi_isr
        #define TIMER1_OVF_ISR_VECTOR_NO    14  //interrupt_handler timer1_ovf_isr
        #define TIMER2_OVF_ISR_VECTOR_NO    10  //interrupt_handler timer1_ovf_isr
        #define PCINT0_ISR_VECTOR_NO        4   //interrupt_handler pcint0_isr
        #define PCINT1_ISR_VECTOR_NO        5   //interrupt_handler pcint1_isr
        #define PCINT2_ISR_VECTOR_NO        6   //interrupt_handler pcint2_isr
    #endif

    #ifdef __ATMEGA16
        #include <iom16v.h>
        #define SPI_STC_ISR_VECTOR_NO       11  //interrupt_handler spi_stc_isr
        #define TWI_ISR_VECTOR_NO           18  //interrupt_handler twi_isr
    #endif

    #ifdef __ATMEGA128
       #include <iom128v.h>
       #define SPI_STC_ISR_VECTOR_NO       18  //interrupt_handler spi_stc_isr
       #define TWI_ISR_VECTOR_NO           34  //interrupt_handler twi_isr
       #define TIMER2_OVF_ISR_VECTOR_NO    11  //interrupt_handler timer2_ovf_isr
    #endif

    #ifdef __ATMEGA2560
    #include <iom2560.h>
    //#define SPI_STC_ISR_VECTOR_NO       18  //interrupt_handler spi_stc_isr
    //#define TWI_ISR_VECTOR_NO           34  //interrupt_handler twi_isr
    //#define TIMER2_OVF_ISR_VECTOR_NO    11  //interrupt_handler timer2_ovf_isr
    #endif

    #ifdef __ATMEGA32
        #include <iom32v.h>
        #define SPI_STC_ISR_VECTOR_NO     13  //interrupt_handler spi_stc_isr
        #define TWI_ISR_VECTOR_NO         20  //interrupt_handler twi_isr
    #endif
#endif

//#ifdef __USING_CONSTANTE
//	#include "AVR_CONSTANTE.h"
//#endif

/*
#ifdef __USING_UTILITARE
    #include "AVR_UTILITARE.h"
#endif

#ifdef __USING_TIMERE_SW
    #include "AVR_TIMERE_SW.h"
#endif

#ifdef __USING_LCD_TEXT
    #include "Driver_LCD_Text_NxM.h"
#endif

#ifdef __USING_PROTOCOL
    #include "ABT_PROTOCOL.h"
#endif

#ifdef __USING_TWI_I2C
    #include "AVR_TWI_I2C.h"
#endif

#ifdef __USING_SPI
    #include "AVR_SPI.h"
#endif

#ifdef __USING_ADC
    #include "AVR_ADC.h"
#endif

#ifdef __USING_TASTATURA
    #include "Driver_Tastaturi_Intrari.h"
#endif

#ifdef __USING_TEXT_UI
	#include "text_UI.h"
#endif

#ifdef __USING_USART
    #ifdef __ATMEGA8
        #include "AVR_USART_MEGA8_NO_STRUCT.h"
    #elif defined __ATMEGA168
        #include "AVR_USART_MEGA168_NO_STRUCT.h"
    #elif defined __ATMEGA16
        #include "AVR_USART_MEGA16_NO_STRUCT.h"
    #elif defined __ATMEGA128
        #include "AVR_USART_MEGA128_NO_STRUCT.h"
    #elif defined __ATMEGA2560
		#include "AVR_USART_MEGA2560_NO_STRUCT.h"
    #elif defined __ATMEGA32
        #include "AVR_USART_MEGA32_NO_STRUCT.h"
    #endif
#endif
*/

#endif /* _AVR_DEFINES_H_ */
