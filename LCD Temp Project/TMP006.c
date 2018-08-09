/*
 * TMP006.c
 *
 * Created: 7/25/2018 12:42:08 PM
 *  Author: Danut.Tazlaoanu
 */ 


#include "TMP006.h"


int8_t tmp006_readRegister(uint8_t registerToRead, int16_t *readValue)
{
	twiSlaveAddress = TMP006_I2C_SENSOR_ADDRESS;
	
	twiBytesToTransfer = 1;
	twiTxMessage[0] = registerToRead;
	twiAppStatus = twi_Write(twiSlaveAddress, twiTxMessage, twiBytesToTransfer, true);
	
	if(twiAppStatus == TWI_SLAR_W_NACK_RECEIVED)
		{
			return ERROR_TWI_WRITE_READREGISTER;
		}
	
	twiBytesToTransfer = 2;
	twiAppStatus = twi_Read(twiSlaveAddress, twiRxMessage, twiBytesToTransfer, true);
	
	if(twiAppStatus == TWI_SLAR_R_NACK_RECEIVED)
		{
			return ERROR_TWI_READ_READREGISTER;
		}
	
	*readValue = (((uint16_t)twiRxMessage[0] << 8) | (uint16_t)twiRxMessage[1]);
	
	_delay_ms(5);
	twi_Stop(true);
	
	return TWI_TRANSFER_STATUS_OK;
}

int8_t tmp006_writeRegister(uint8_t registerToWrite, uint16_t dataToWrite)
{
	twiSlaveAddress = TMP006_I2C_SENSOR_ADDRESS;
	
	twiBytesToTransfer = 3;

	twiTxMessage[0]=registerToWrite;
	twiTxMessage[1]=(uint8_t)((dataToWrite & 0xFF00) >> 8);
	twiTxMessage[2]=(uint8_t)(dataToWrite & 0x00FF);
	
	twiAppStatus = twi_Write(twiSlaveAddress, twiTxMessage, twiBytesToTransfer, true);
	if(twiAppStatus == TWI_SLAR_W_NACK_RECEIVED)
		{
			return ERROR_TWI_WRITE_WRITEREGISTER;
		}
	
	_delay_ms(5);
	twi_Stop(true);
	
	return TWI_TRANSFER_STATUS_OK;
}

int8_t tmp006_init(uint16_t numberOfSamples)
{
	//I2C Module initialization
	twi_Init(&PORTD, PIND0, PIND1, 0x01, 0x03, 0x01);
	
	
	sprintf(snTelegramaTx, "--------------------------------------\n\r");
	uart1_Transmite(snTelegramaTx,strlen(snTelegramaTx), true);
	_delay_ms(5);
	
	
	// Write the config register
	statusInitWriteConfigurationRegister = tmp006_writeRegister(TMP006_CONFIG_REGISTER, TMP006_CONFIG_SENSOR_ENABLED | TMP006_CONFIG_DRDY_EN | numberOfSamples);
	if(statusInitWriteConfigurationRegister != TWI_TRANSFER_STATUS_OK)
		return ERROR_INIT_WRITE_CONFIG_REGISTER;
	
	// Read from config register
	statusInitReadConfigurationRegister = tmp006_readRegister(TMP006_CONFIG_REGISTER, &configurationRegister);
	if(statusInitReadConfigurationRegister != TWI_TRANSFER_STATUS_OK)
		return ERROR_INIT_READ_CONFIG_REGISTER;
	
	sprintf(snTelegramaTx, "Configuration register : 0x%2x\n\r", configurationRegister);
	uart1_Transmite(snTelegramaTx,strlen(snTelegramaTx), true);
	_delay_ms(5);
	
	
	// Test manufacturer ID
	statusInitReadManufacturerRegister = tmp006_readRegister(TMP006_MANUFACTURER_ID_ADDRESS, &manufacturerIdRegister);
	if(statusInitReadManufacturerRegister != TWI_TRANSFER_STATUS_OK)
		return ERROR_INIT_READ_MANUFACTURER_REGISTER;
		
	sprintf(snTelegramaTx, "Manufacturer ID : 0x%2x\n\r", manufacturerIdRegister);
	uart1_Transmite(snTelegramaTx,strlen(snTelegramaTx), true);
	_delay_ms(5);

	if (manufacturerIdRegister == TMP006_MANUFACTURER_ID)
		{
			PORTB |= ( 1<< PORTB2);
			PORTB &= ~( 1 << PORTB3);
			}
	else
		{
			PORTB |= ( 1<< PORTB3);
			PORTB &= ~( 1 << PORTB2);
		}

	// Test device ID
	statusInitReadDeviceIdRegister = tmp006_readRegister(TMP006_DEVICE_ID_ADDRESS, &deviceIdRegister);
	if(statusInitReadDeviceIdRegister != TWI_TRANSFER_STATUS_OK)
		return ERROR_INIT_READ_DEVICEID_REGISTER;
		
	sprintf(snTelegramaTx, "Device ID : 0x%2x\n\r", deviceIdRegister);
	uart1_Transmite(snTelegramaTx,strlen(snTelegramaTx), true);
	_delay_ms(5);

	if (deviceIdRegister == TMP006_DEVICE_ID)
	{
		PORTB |= ( 1<< PORTB4);
		PORTB &= ~( 1 << PORTB5);
	}
	else
	{
		PORTB |= ( 1<< PORTB5);
		PORTB &= ~( 1 << PORTB4);
	}
	
	sprintf(snTelegramaTx, "--------------------------------------\n\r");
	uart1_Transmite(snTelegramaTx,strlen(snTelegramaTx), true);
	_delay_ms(5);
	
	
	if (numberOfSamples==TMP006_CONFIG_1SAMPLE)
		samplingTime = 250;
	else if (numberOfSamples==TMP006_CONFIG_2SAMPLE)
		samplingTime = 500;
	else if (numberOfSamples==TMP006_CONFIG_4SAMPLE)
		samplingTime = 1000;
	else if (numberOfSamples==TMP006_CONFIG_8SAMPLE)
		samplingTime = 2000;
	else if (numberOfSamples==TMP006_CONFIG_16SAMPLE)
		samplingTime = 4000;
	
	return TMP006_INIT_OK;
	
}

int8_t tmp006_sleep(void)
{
	powerControl &= ~(TMP006_CONFIG_SENSOR_ENABLED);
	tmp006_writeRegister(TMP006_CONFIG_REGISTER, powerControl);

	// Config register display
	statusSleepReadConfigurationRegister = tmp006_readRegister(TMP006_CONFIG_REGISTER, &powerControl);
	if(statusSleepReadConfigurationRegister != TWI_TRANSFER_STATUS_OK)
		return ERROR_SLEEP_READ_CONFIG_REGISTER;

	sprintf(snTelegramaTx, "Config. reg. in sleep mode : 0x%2x\n\r", powerControl);
	uart1_Transmite(snTelegramaTx,strlen(snTelegramaTx), true);
	_delay_ms(5);
	
	return TMP006_SLEEP_OK;
}

int8_t tmp006_wake(void)
{
	powerControl = (TMP006_CONFIG_SENSOR_ENABLED);
	tmp006_writeRegister(TMP006_CONFIG_REGISTER, powerControl);

	// Config register display
	statusWakeReadConfigurationRegister = tmp006_readRegister(TMP006_CONFIG_REGISTER, &powerControl);
	if(statusWakeReadConfigurationRegister != TWI_TRANSFER_STATUS_OK)
		return ERROR_WAKE_READ_CONFIG_REGISTER;
		
	sprintf(snTelegramaTx, "Config. reg. in wake mode : 0x%2x\n\r", powerControl);
	uart1_Transmite(snTelegramaTx,strlen(snTelegramaTx), true);
	_delay_ms(5);
	
	return TMP006_WAKE_OK;
}

int8_t tmp006_read_Tambient(void)
{
	statusTambientReadTambRegister = tmp006_readRegister(TMP006_TAMB, &tAmbientRegisterValue);
	
	if(statusTambientReadTambRegister != TWI_TRANSFER_STATUS_OK)
		return ERROR_TAMBIENT_READ_TAMB_REGISTER;
		
	tAmbientRegisterValue = tAmbientRegisterValue >> 2;
	tAmbientDoubleRegisterValue = (double)tAmbientRegisterValue;
	tAmbientDoubleRegisterValue = tAmbientDoubleRegisterValue/32;


	//sprintf(snTelegramaTx, "Ambient temperature: %.2lf\n\r", tAmbientDoubleRegisterValue);
	
	dtostrf(tAmbientDoubleRegisterValue, 5, 2, ambientTempString);
	uart1_Transmite(ambientTempString,strlen(ambientTempString), true);
	
	_delay_ms(5);
	
	return TMP006_SENSOR_TAMBIENT_OK;
}

int8_t tmp006_read_termopileSensor(void)
{
	statusThermopileReadTambRegister = tmp006_readRegister(TMP006_TAMB, &tAmbientThermopileRegisterValue);
	if(statusThermopileReadTambRegister != TWI_TRANSFER_STATUS_OK)
		return ERROR_TERMOPILE_READ_TAMB_REGISTER;
	
	tAmbientThermopileRegisterValue = tAmbientThermopileRegisterValue >> 2;
	tAmbientThermopileDoubleRegisterValue = tAmbientThermopileRegisterValue;

	statusThermopileReadVobjRegister = tmp006_readRegister(TMP006_VOBJ, &tThermopileVobjRegisterValue);
	if(statusThermopileReadVobjRegister != TWI_TRANSFER_STATUS_OK)
		return ERROR_TERMOPILE_READ_VOBJ_REGISTER;
		
	tThermopileVobjDoubleRegisterValue = tThermopileVobjRegisterValue;
		
	tThermopileVobjDoubleRegisterValue = tThermopileVobjDoubleRegisterValue * 156.25;		// 156.25   nV / LSB(bit)
	tThermopileVobjDoubleRegisterValue =tThermopileVobjDoubleRegisterValue / 1000000000;	// nV -> uV

	tAmbientThermopileDoubleRegisterValue *= 0.03125; // Convert to celsius
	tAmbientThermopileDoubleRegisterValue += 273.15; // Convert to kelvin
	 
	// Sensitivity of the thermopile sensor (S)
	thermopileDoubleRegisterValueMinusReference = tAmbientThermopileDoubleRegisterValue - TMP006_TREF;
	ensitivityOfThermopileSensor = (1 + (TMP006_A1*thermopileDoubleRegisterValueMinusReference) + (TMP006_A2*thermopileDoubleRegisterValueMinusReference*thermopileDoubleRegisterValueMinusReference) );
	ensitivityOfThermopileSensor *= TMP006_S0;				  // |
	ensitivityOfThermopileSensor /= 10000000; // *10^-7	  // |--> primary calibration factor S0
	ensitivityOfThermopileSensor /= 10000000; // *10^-7	  // |
	
	
	// Offset voltage that occurs due to sensor warm-up (Vos)
	offsetVoltage = TMP006_B0 + (TMP006_B1*thermopileDoubleRegisterValueMinusReference) + (TMP006_B2*thermopileDoubleRegisterValueMinusReference*thermopileDoubleRegisterValueMinusReference);
	
	// Models the Seebeck coefficients
	seebackCoefficientsOfThermopile = (tThermopileVobjDoubleRegisterValue - offsetVoltage) + (TMP006_C2*(tThermopileVobjDoubleRegisterValue-offsetVoltage)*(tThermopileVobjDoubleRegisterValue-offsetVoltage));
	
	// The radiant transfer of IR energy between the target object and the TMP006
	objectTemperature = sqrt( sqrt( (tAmbientThermopileDoubleRegisterValue * tAmbientThermopileDoubleRegisterValue * tAmbientThermopileDoubleRegisterValue * tAmbientThermopileDoubleRegisterValue) + (seebackCoefficientsOfThermopile/ensitivityOfThermopileSensor) ) );
	
	objectTemperature -= 273.15; // Kelvin -> Celsius
	_delay_ms(10);

	//sprintf(snTelegramaTx, "Object temperature: %.2lf\n\r", objectTemperature);
	
	dtostrf(objectTemperature, 5, 2, thermopileTempString);
	uart1_Transmite(thermopileTempString,strlen(thermopileTempString), true);
	
	return TMP006_SENSOR_THERMOPILE_OK;
}

void tmp006_main()
{	
		
		tmp006_read_Tambient();
		tmp006_read_termopileSensor();

}
