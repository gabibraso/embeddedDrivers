/*
* main.c
*
* Created: 22.05.2018
* Author : Bogdan Tarnauca
*/

#include "main_mcu_init.h"

/**
*** @fn      watchdog_init(void)
***
*** @brief   Initialize the whatchdog. Prescale: 16K cycles
***
*** @param   none
*** @return  none.
**/
void watchdog_init(void)
{
    wdt_reset(); //this prevents a timeout on enabling
    WATCHDOG_CONTROL = (1<<WDE) || (1<<WDP2);
    //configure watchdog. System reset, not watchdog interrupt. 250ms period
    //ATENTION: Watchdog functionality has to be enabled also from the fusebits WDTON - fuse set ti o 0 (zero) to enable it.
}

/**
*** @fn      port_init(void);
***
*** @brief   Initialize the MCU ports
***
*** @param   none
*** @return  none.
**/
void port_init(void)
{
    //IMPORTANT DO NOT USE THESE PINS FOR OTHER PURPOSES!!!!!
    //if a JTAG debugger is used PORTF7:4 will be allocated to the debugger.
    DDRA  = 0xFF;
    PORTA = 0xFF;
    PORTB = 0xFF;
    DDRB  = 0xFF;
    PORTC = 0x7F; //m103 output only
    DDRC  = 0x7F;
    PORTD = 0x00;
    DDRD  = 0xFF;
    PORTE = 0xFF;
    DDRE  = 0xFF;
    PORTF = 0xFF;
    DDRF  = 0xFF;
    PORTG = 0xFF;
    DDRG  = 0xFF;
 
    ledPort = &PORTG;
    ledPinMask = 0x01;
}

/**
*** @fn      init_devices(void)
***
*** @brief   Initialize the MCU devices
***
*** @param   none
*** @return  none.
**/
void init_devices(void)
{
    watchdog_init();
		
    //stop errant interrupts until set up
    CLI(); //disable all interrupts
 
    XDIV  = 0x00; //xtal divider
    XMCRA = 0x00; //external memory

    //IMPORTANT DO NOT USE THESE PINS FOR OTHER PURPOSES!!!!!
    //if a JTAG debugger is used PORTF7:4 will be allocated to the debugger.
    port_init();

    wdt_reset(); 

    //
    // NOTE:
    // Timer2 is also used by the UART driver in order to generate a 10ms timebase for the timeout mechanism
    // The timer is initialized inside uartX_init
    //
    uart1_init(NULL,0x00, false,
    0x00,	//UCSR0A
    0x06,	//UCSR0C
    0x07,	//UBRR0L = set baud rate lo //57600 la 7.3728 Mhz
    //0x0B,	//UBRR0L = set baud rate lo //57600 la 11.0592 Mhz
    //0x10,	//UBRR0L = set baud rate lo //57600 la 16.000 Mhz
    0x00,	//UBRR0H = set baud rate hi
    0xF8,	//UCSR0B
    0,		//Sistem slave, asteapta comenzi. Nu se genereaza timeout decat la nivel de carcater
		    //Sistemul poate astepta la nesfarsit aparitia unei comenzi.
    0
    );
    
    sei(); //re-enable interrupts
      
    MCUCR = 0x00;
    EICRA = 0x00; //extended ext ints
    EICRB = 0x00; //extended ext ints
    EIMSK = 0x00;
    
    //Enable the Timer2 interrupt 
    //Timer2 Overflow Interrupt enabled. Used for the uart Timeout generation
    TIMSK = 0x40; //timer interrupt sources
    
    ETIMSK = 0x00; //extended timer interrupt sources
    SEI(); //re-enable interrupts
    //all peripherals are now initialized
}
