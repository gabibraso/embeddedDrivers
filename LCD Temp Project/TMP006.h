/*
 * TMP006.h
 *
 * Created: 7/25/2018 12:42:27 PM
 *  Author: Danut.Tazlaoanu
 */ 

#ifndef _TMP006_H_
	#define _TMP006_H_


#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "avr_defines.h"
#include "main_defines.h"
#include "avr_usart.h"
#include "avr_twi_i2c.h"


//------------------------------------------------------------------------------------------------
// SENSOR INFO
//------------------------------------------------------------------------------------------------
#define TMP006_I2C_SENSOR_ADDRESS	0x40
#define TMP006_MANUFACTURER_ID_ADDRESS	0xFE
#define TMP006_MANUFACTURER_ID	0x5449
#define TMP006_DEVICE_ID_ADDRESS	0xFF
#define TMP006_DEVICE_ID	0x0067


//------------------------------------------------------------------------------------------------
// REGISTER MAP
//------------------------------------------------------------------------------------------------
#define TMP006_VOBJ	0x00	
#define TMP006_TAMB	0x01
#define TMP006_CONFIG_REGISTER	0x02


//------------------------------------------------------------------------------------------------
// GENERAL CONFIGURATION
//------------------------------------------------------------------------------------------------
#define TMP006_CONFIG_RESET	0x8000
#define TMP006_CONFIG_SENSOR_ENABLED	0x7000

//------------------------------------------------------------------------------------------------
// SAMPLING CONFIGURATION
//------------------------------------------------------------------------------------------------
#define TMP006_CONFIG_1SAMPLE  0x0000	// 1 sample -> 4 conv/sec		-> 0.25s
#define TMP006_CONFIG_2SAMPLE  0x0200	// 2 sample -> 2 conv/sec		-> 0.5s
#define TMP006_CONFIG_4SAMPLE  0x0400	// 4 sample -> 1 conv/sec		-> 0.1s
#define TMP006_CONFIG_8SAMPLE  0x0600	// 8 sample -> 0.5 conv/sec		-> 2s
#define TMP006_CONFIG_16SAMPLE 0x0800	// 16 sample -> 0.25 conv/sec	-> 4s

#define TMP006_CONFIG_DRDY_EN	0x0100 // Data ready pin enable
#define TMP006_CONFIG_DRDY	0x0080	// Data ready bit


//------------------------------------------------------------------------------------------------
// FOR IR SENSOR
//------------------------------------------------------------------------------------------------
#define TMP006_S0 6.4  // * 10^-14
#define TMP006_A1 0.00175
#define TMP006_A2 -0.00001678
#define TMP006_B0 -0.0000294
#define TMP006_B1 -0.00000057
#define TMP006_B2 0.00000000463
#define TMP006_C2 13.4
#define TMP006_TREF 298.15


//------------------------------------------------------------------------------------------------
// PROGRAM STATUS
//------------------------------------------------------------------------------------------------
#define TWI_TRANSFER_STATUS_OK -10
#define TWI_TRANSFER_STATUS_NOT_OK -11;

#define TMP006_INIT_OK -12
#define TMP006_INIT_NOT_OK -13;

#define TMP006_SLEEP_OK -14;
#define TMP006_SLEEP_NOT_OK -15;

#define TMP006_WAKE_OK -16;
#define TMP006_WAKE_NOT_OK -17;

#define TMP006_SENSOR_THERMOPILE_OK -18;
#define TMP006_SENSOR_TERMOPILE_NOT_OK -19;

#define TMP006_SENSOR_TAMBIENT_OK -20;
#define TMP006_SENSOR_TAMBIENT_NOT_OK -21;


//------------------------------------------------------------------------------------------------
// ERRORS
//------------------------------------------------------------------------------------------------
#define ERROR_TWI_WRITE_READREGISTER 10;
#define ERROR_TWI_READ_READREGISTER 11;

#define ERROR_TWI_WRITE_WRITEREGISTER 12;

#define ERROR_INIT_READ_CONFIG_REGISTER 13;
#define ERROR_INIT_READ_MANUFACTURER_REGISTER 14;
#define ERROR_INIT_READ_DEVICEID_REGISTER 15;
#define ERROR_INIT_WRITE_CONFIG_REGISTER 16;

#define ERROR_SLEEP_READ_CONFIG_REGISTER 17;
#define ERROR_WAKE_READ_CONFIG_REGISTER 18;

#define ERROR_TERMOPILE_READ_TAMB_REGISTER 19;
#define ERROR_TERMOPILE_READ_VOBJ_REGISTER 20;

#define ERROR_TAMBIENT_READ_TAMB_REGISTER 21;



//------------------------------------------------------------------------------------------------
// tmp006_readRegister FUNCTION
//------------------------------------------------------------------------------------------------



//------------------------------------------------------------------------------------------------
// tmp006_writeRegister FUNCTION
//------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------
// tmp006_init FUNCTION
//------------------------------------------------------------------------------------------------
volatile int8_t statusInitWriteConfigurationRegister;
volatile int8_t statusInitReadConfigurationRegister;
volatile uint16_t configurationRegister;

volatile int8_t statusInitReadManufacturerRegister;
volatile uint16_t manufacturerIdRegister;

volatile int8_t statusInitReadDeviceIdRegister;
volatile uint16_t deviceIdRegister;

int samplingTime;


//------------------------------------------------------------------------------------------------
// tmp006_sleep FUNCTION
//------------------------------------------------------------------------------------------------
volatile int8_t statusSleepReadConfigurationRegister;
volatile uint16_t powerControl;

//------------------------------------------------------------------------------------------------
// tmp006_wake FUNCTION
//------------------------------------------------------------------------------------------------
volatile int8_t statusWakeReadConfigurationRegister;

//------------------------------------------------------------------------------------------------
// tmp006_read_Tambient FUNCTION
//------------------------------------------------------------------------------------------------
volatile int8_t statusTambientReadTambRegister;
volatile uint16_t tAmbientRegisterValue;
volatile double tAmbientDoubleRegisterValue;
volatile char ambientTempString[15];


//------------------------------------------------------------------------------------------------
// tmp006_read_termopileSensor FUNCTION
//------------------------------------------------------------------------------------------------
volatile int8_t statusThermopileReadTambRegister;
volatile int16_t tAmbientThermopileRegisterValue;
volatile double tAmbientThermopileDoubleRegisterValue;

volatile int8_t statusThermopileReadVobjRegister;
volatile int16_t tThermopileVobjRegisterValue;

volatile double tThermopileVobjDoubleRegisterValue;

double thermopileDoubleRegisterValueMinusReference;
double ensitivityOfThermopileSensor;
double offsetVoltage;
double seebackCoefficientsOfThermopile;
double objectTemperature;

volatile char thermopileTempString[15];

//------------------------------------------------------------------------------------------------
// tmp006_main FUNCTION
//------------------------------------------------------------------------------------------------



//------------------------------------------------------------------------------------------------
// Function prototypes
//------------------------------------------------------------------------------------------------
int8_t tmp006_readRegister(uint8_t registerToRead, int16_t *readValue);

int8_t tmp006_writeRegister(uint8_t registerToWrite, uint16_t dataToWrite);

int8_t tmp006_init(uint16_t numberOfSamples);

int8_t tmp006_sleep(void);

int8_t tmp006_wake(void);

int8_t tmp006_read_Tambient(void);

int8_t tmp006_read_termopileSensor(void);

void tmp006_main();


#endif /* _TMP006_H_ */