/*
* main.c
*
* Created: 22.05.2018
* Author : Bogdan Tarnauca
*/

#ifndef _MAIN_CONFIG_H_
	#define  _MAIN_CONFIG_H_

#ifndef F_CPU
	//#define F_CPU 11059200UL
	//#define F_CPU 16000000UL
	#define F_CPU   7372800UL
#endif	
	
//complilam cu WinAVR - daca nu e definit inseamna ca folosim ICC AVR
#ifndef __WIN_AVR
	#define __WIN_AVR 
#endif	

//Define the MCU used in the application. Changes the structure of some ABT AVR libs.
//#define __ATMEGA168
#ifndef __ATMEGA128
	#define __ATMEGA128
#endif	
//#define __ATMEGA16
//#define __ATMEGA168


#if (defined __ATMEGA2560) || (defined __ATMEGA328)
    #define MCU_STATUS MCUCR
#elif (defined __ATMEGA128)
    #define MCU_STATUS MCUCSR
#else
    #warning target not defined. MCU contol and status register set to MCUCSR
    #define MCU_STATUS MCUCSR
#endif

//Different devices are using different names for this register
#if (defined __ATMEGA2560) || (defined __ATMEGA328)
    #define WATCHDOG_CONTROL WDTCSR
#elif (defined __ATMEGA128)
    #define WATCHDOG_CONTROL WDTCR
#else
    #warning target not defined. watchdog contol and status register set to WDTCSR
    #define WATCHDOG_CONTROL WDTCSR
#endif


#endif