/*
* avr_adc.h
*
* Created: 01.01.2009
* Author : Bogdan Tarnauca
*/

#ifndef _AVR_ADC_H_
	#define _AVR_ADC_H_

//Apps must include an "main_config.h" header where libraries related params, overrides, settings should be defined.
//This header file is application specific. Must be provided even it is empty.
#include "main_config.h"

#include "avr_timere_sw.h"

#define ADC_FULLSCALE						1023
#define ADC_SEQUENCE_READING_PERIOD_ERROR	0xFF	//-1
#define ADC_TIMERS_SETTING_ERROR			0xFE	//-2
#define ADC_INVALID_CHANNEL_ERROR			0xFD	//-3
#define ADC_MIN_SAMPLING_PERIOD_ERROR		0xFC	//-4

#if !defined ADC_MIN_SAMPLING_PERIOD
	#define ADC_MIN_SAMPLING_PERIOD	50
#endif

#if !defined ADC_CHANNELS_COUNT
	#define ADC_CHANNELS_COUNT		8
#endif

typedef enum
{
	ADC_REF_AREF = 0x00,
	ADC_REF_AVCC = 0x40,
	ADC_REF_RESERVED = 0x80,
	ADC_REF_INTERNAL = 0xC0,
} ADCReferenceSelect;

typedef enum
{
	ADC_RESULTS_ALINGAMENT_RIGHT = 0x00,
	ADC_RESULTS_ALINGAMENT_LEFT = 0x20,
} ADCResultsAlingament;

/*
void sampleReadingTimerCallback(void);
void sequenceReadingTimerCallback(void);
void adcStartSampleReading(void);
void adcStartSequenceReading(bool periodicReading, bool blockingRead);
void adcSequenceReadingAvereging();
void adcAveregingAndVoltsConversion(void);
*/

volatile unsigned char adcUsedChannels[ADC_CHANNELS_COUNT];
	//Vector care indica faptul ca un anumit canal este folosit sau nu.
	//din cele ADC_CHANNELS_COUNT ale ADC-ului putem uneori folosit doar cateva
	//in functie de aplicatie.

unsigned int adcSampleReadingTimerHandle;
unsigned int adcSequenceReadingTimerHandle;
unsigned int adcReadingsProcessingTimerHandle;

unsigned int adcSequenceSamplesNumber;
volatile unsigned int adcSequenceSamplesCounter;
	//se citesc adcSequenceSamplesCounter de esantioane intr-o secventa

volatile unsigned long adcSequenceReadingsSum[ADC_CHANNELS_COUNT];
volatile unsigned int adcSequenceReadingBuffer[ADC_CHANNELS_COUNT];
	//dublu bufferare.

volatile unsigned int adcSequenceReadingAverage[ADC_CHANNELS_COUNT];
volatile double adcInputsAsVoltage[ADC_CHANNELS_COUNT];

bool adcOngoingProcessing;

volatile unsigned char adcSelectedChannel;
volatile bool adcOngoingSequenceReading;
volatile bool adcRepetitiveReading;
volatile bool adcSamplingActive;
volatile bool adcSequenceReadingComplete;

volatile signed char adcFirstChannelUsed;
volatile unsigned char adcLastChannelUsed;
	//Din sirul adcUsedChannels care indica care sunt canalele ADC care sunt
	//utilizate CH0, CH1,...CH7 se va determina care este indexul primului si
	//al ultimului utilizat in aplicatia noastra pentru optimizarea citirilor

float adcVrefOrFullScaleVoltage;
	// [Process] (0..FullScaleVoltage)  --> [Voltage divider / amplifier] --> [uC ADC] (0..Vref)
	//At the ADC pins the full scale voltage is equal to the reference voltage
	//provided to the ADC (internal, external - AVCC, external AREF).
	//Vref corresponds to ADC_FULLSCALE
	//The voltages at the ADC pins are usually obtained either through an
	//voltage divider or an amplifier which scales the process voltage input range
	//to the microcontroller ADC input range (0..Aref)
	//If the process voltage input rage is identical for all channels, adcVrefOrFullScaleVoltage
	//can be set to the value of adcVrefOrFullScaleVoltage thus easing the int to float value
	//conversion
	//
	//If the process voltage input rage varies across the channels, adcVrefOrFullScaleVoltage
	//should be set to the value of Aref and a by channel case scaling should later be
	//used

float adcOffsetCompensation;
	//Compensarea de offset in mV. Aceasta valoare (pozitita sau negativa)
	//se adauga la valoarea masurata dupa conversia in tensiune

float adcGainCompensation;
	//Compensarea de gain. Aceasta valoare se inmulteste cu valoarea masurata
	//la care in prealabil s-a facut si corectia de offest

float adcVoltsPerBit;
	//adcVrefOrFullScaleVoltage/ADC_FULLSCALE. Volti pe bit

void (*adcIsrSamplingCompleteHandler)(void);
	//Pointer la functia care poate fie apelata (ATENTIE: in ISR) atunci cand s-a incheiat
	//o esantionare completa. O esantionare completa se executa atunci cand toate canalele
	//active au fost citite

void (*adcIsrSequenceReadingCompleteHandler)(void);
	//Pointer la functia care poate fie apelata (ATENTIE: in ISR) atunci cand s-a incheiat
	//o citire completa. O citire completa se executa atunci cand toate esantionarile
	//indicate adcSequenceSamplesNumber au fost executate

void (*adcSequenceReadingErrorHandler)(void);
	//Pointer la functia handler a erorii care apare atunci cand achizitia tuturor esantioanelor
	//este mai lung decat timpul unei citiri.

void (*adcSequenceReadingProcessor)(void);
	//Pointer to the data processing function which will be automatically triggered right after
	//all the samples of a sequence reading have been read. A default value for this points
	//to the void adcAveregingAndVoltsConversion() function

#ifdef __ICCAVR
//#pragma interrupt_handler timer2_compa_isr:TIMER2_COMPA_ISR_VECTOR_NO
//void timer2_compa_isr(void)
#error Define ISR vector for ICC AVR
#else
ISR(ADC_vect);
#endif

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
//Fiecare citire poate fi realizata prin citirea unuia sau mai multor esantioane (pentru toate canalele active)
//Acest timer stabileste perioada cu care sunt citite esantionele (adica o serie de citiri a tuturor canalelor active)
//si comanda acest lucru
void sampleReadingTimerCallback(void);
//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
//Fiecare citire poate fi realizata prin citirea unuia sau mai multor esantioane (pentru toate canalele active)
//Acest timer stabileste perioada cu care se face o citire (o citire implica esantionarea tuturor canalelor active,
//o data sau de mai multe ori.)
void sequenceReadingTimerCallback(void);

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
/**
* \brief ADC acquisition initialization routine. It supports sequence readings to allow functions such as SW filtering
* after a number of subsequent samples have been read.
* Sequences are initiated every sequenceReadingPeriod milliseconds. A sequence of sequenceReadingSamplesCount samples
* are read from each of the selected ADC channels and saved into buffers which are later used for processing the data.
* Inside the sequence window, the sequenceReadingSamplesCount samples are read every samplingPeriod milliseconds.
* NOTE: The following condition must hold: sequenceReadingSamplesCount * samplingPeriod < sequenceReadingPeriod
* otherwise an ADC_SEQUENCE_READING_PERIOD_ERROR will be returned during the init and ADC will not be initialized.
* The ADC port is specified by ADC_PORT and the pins which are used for analog acquisition are specified through
* activeChannelsMask.
* The acquisition process as well as the execution of the processing function relies on the avr_timere_sw library.
* The sequencing, sampling and processing are scheduled and executed in the main loop of the application when
* the timerSW_Evalueaza() function evaluates the ADC related timers created during the ADC initialization.
* ISR based processing is thus kept to a minimum.
* Data processing after a sequence is complete is achieved by a custom function specified by sequenceReadingProcessor.
* If this is null, the default adcAveregingAndVoltsConversion is executed and the results of the
* averaging are held into the adcSequenceReadingAverage array while the voltage conversion corrected for offset and gain
* errors (see the offsetCompensation and gainCompensation params) are held int the adcInputsAsVoltage array.
* The function is executed automatically.
* After all the samples of a sequence have been read, the timer for the data processing function is forced so that
* in the main loop the function is executed the first time timerSW_Evalueaza() is called.
* The ISR can be extended with custom methods which are called inside the ISR after each set of samples (for the active
* channels) has been read or after the sequence reading is complete. Use this feature only if strictly necessary to avoid
* bloating the ISR
*
* \param sequenceReadingPeriod - Defines the periodicity (in tens of milliseconds) for the sequence readings
* \param samplingPeriod - Defines the periodicity (in tens of milliseconds) for the sampling of the active ADC channels inside a
*							sequence reading window
* \param sequenceReadingSamplesCount - The number of samples read for each active ADC channel inside a sequence reading window
* \param activeChannelsMask - Indicates which of the ADC channels are used for analog acquisition
* \param ADC_PORT - Reference to the port providing the ADC inputs
* \param selectedReference - The reference type (Internal, external, AVCC). See ADCReferenceSelect.
* \param resultsAlignament - The results alignment type (left, right). See ADCResultsAlingament
* \param isrSamplingCompleteHandler - Pointer to a function which will be called inside the ADC ISR after a sampling
of all active channels is complete. Can be null.
* \param isrSequenceReadingCompleteHandler - Pointer to a function which will be called inside the ADC ISR after a
sequence reading is complete. Can be null.
* \param sequenceReadingProcessor - Pointer to a custom function used for processing the results inside the main loop
by a forced avr_timere_sw based timer after a sequence reading is complete.
If null, the default adcAveregingAndVoltsConversion is executed and the results of the
averaging are held into the adcSequenceReadingAverage array while the voltage conversion
corrected for offset and gain errors (see the offsetCompensation and gainCompensation params)
are held int the adcInputsAsVoltage array.
* \param sequenceReadingErrorHandler - Pointer to a custom function used for error handling if a sequence reading is
automatically re-triggered while another sequence reading is still underway. Can be null.
* \param vrefOrFullScaleVoltage - The value in volts of the reference voltage of of the process full scale voltage.
*							This will be used for the actual conversion. Set this to the value
*							of the internal reference, the value at the AREF pin or the value of AVCC according to the setting of the
*							selectedReference parameter or to the value of the process full scale voltage depending on the cas.
*							See the description of adcVrefOrFullScaleVoltage
* \param offsetCompensation - Float value indicating the ADC offset compensation correction used in the
*								adcAveregingAndVoltsConversion fuction
* \param gainCompensation - Float value indicating the ADC gain compensation correction used in the
*								adcAveregingAndVoltsConversion fuction
*
* \return int	- 0 if initialization succeeded. A negative value is errors have been encountered. See ADC_MIN_SAMPLING_PERIOD_ERROR,
*				ADC_TIMERS_SETTING_ERROR, ADC_SEQUENCE_READING_PERIOD_ERROR
*/
int adcInit(	unsigned int sequenceReadingPeriod,
				unsigned int samplingPeriod,
				int sequenceReadingSamplesCount,
				unsigned char activeChannelsMask,
				volatile unsigned char *ADC_PORT,
				ADCReferenceSelect selectedReference,
				ADCResultsAlingament resultsAlignament,
				void (*isrSamplingCompleteHandler)(void),
				void (*isrSequenceReadingCompleteHandler)(void),
				void (*sequenceReadingProcessor)(void),
				void (*sequenceReadingErrorHandler)(void),
				float vrefOrFullScaleVoltage,
				float offsetCompensation,
				float gainCompensation
				);


//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
void adcStartSampleReading(void);

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
void adcStartSequenceReading(bool periodicReading, bool blockingRead);

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
void adcSequenceReadingAvereging();

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
void adcAveregingAndVoltsConversion(void);

#endif /* _AVR_ADC_H_ */