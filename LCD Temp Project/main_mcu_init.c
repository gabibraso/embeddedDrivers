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
	
	DDRB  = 0xFF;
    PORTB = 0xFF;	// Used for debugging
	
	DDRC  = 0x7F;
    PORTC = 0x7F; //m103 output only
	
    DDRD  = 0xFF;
    PORTD = 0x00;
	
    DDRE  = 0xFF;
    PORTE = 0xFF;
	
    DDRF  = 0xFF;
    PORTF = 0xFF;
	
	DDRG  = 0xFF;
    PORTG = 0xFF;
 
    ledPort = &PORTG;
    ledPinMask = 0x01;
    
    errorLEDPort = &PORTG;
    errorLEDPinMask = 0x02;
    
    *errorLEDPort &= ~errorLEDPinMask; //turn of LED
    
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
    char errorCode;
    watchdog_init();
		
    //stop errant interrupts until set up
    CLI(); //disable all interrupts
 
    XDIV  = 0x00; //xtal divider
    XMCRA = 0x00; //external memory
    
    MCUCR = 0x00;
    EICRA = 0x00; //extended ext ints
    EICRB = 0x00; //extended ext ints
    EIMSK = 0x00;
    TIMSK = 0x00;
    ETIMSK = 0x00; //extended timer interrupt sources
    

    //IMPORTANT DO NOT USE THESE PINS FOR OTHER PURPOSES!!!!!
    //if a JTAG debugger is used PORTF7:4 will be allocated to the debugger.
    port_init();

    wdt_reset(); 

    //
    // NOTE:
    // Timer2 is used by the UART driver in order to generate a 10ms timebase for the timeout mechanism
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
    0,		//Sistem slave, asteapta comenzi. Sistemul poate astepta la nesfarsit aparitia unei comenzi.
    0       //Timeoutcar = 0, distanta ]n timp dintre caractere poate fi oricat
    );
    
    
	// I2C Init
    twi_Init(&PORTD, PIND0, PIND1, 0x01, 0x03, 0x01);    
    
    //
    // NOTE:
    // Timer0 is used by the SW Timers driver to generate the required 10ms tick
    //
    //This initializez the HW timer (Timer0) which generates the 10ms tick used for
    //controlling the SW timers
    timerSW_InitTimerHW();
    
    //Every 500 ms a number of 9 readings is performed every 50ms and the averaged value in volts is computed
    //by the adcSequenceReadingProcessor callback. By default (if sequenceReadingProcessor is set to null) 
    //adcSequenceReadingProcessor is set to the adcAveregingAndVoltsConversion function.
    //
    //For the sake of simplicity, AVCC is used for the reference (this is a typically a filtered VCC), 
    //thus vrefOrFullScaleVoltage has to be updated accordingly after measuring the VCC voltage of the board
    errorCode = adcInit(500,    //unsigned int sequenceReadingPeriod,
            50,     //unsigned int samplingPeriod,
            9,      //int sequenceReadingSamplesCount,
            0x0F,   //unsigned char activeChannelsMask,
            &PORTF,  //volatile unsigned char *ADC_PORT,
            ADC_REF_AVCC,    //ADCReferenceSelect selectedReference,
            ADC_RESULTS_ALINGAMENT_RIGHT,    //ADCResultsAlingament resultsAlignament,
            null,   //void (*isrSamplingCompleteHandler)(void),
            null, //void (*isrSequenceReadingCompleteHandler)(void),
            null, //adcSequenceReadingAvereging,    //void (*sequenceReadingProcessor)(void),
                  //If the actual value in volts is not needed (only hex value is OK) then
                  //set this to adcSequenceReadingAvereging instead of null.
                  //This will no longer compute the float values of the voltage.
            null,   //void (*sequenceReadingErrorHandler)(void),
            3.284f,   //float vrefOrFullScaleVoltage,
            0.0f,   //float offsetCompensation,
            1.0f    //float gainCompensation
            );
            
    if(errorCode){
        *errorLEDPort |= errorLEDPinMask;
    }
       
    sei(); //re-enable interrupts
    
    //all peripherals are now initialized
}
