/*
* avr_adc.c
*
* Created: 01.01.2009
* Author : Bogdan Tarnauca
*/

#include "avr_adc.h"

volatile unsigned char adcSelectedChannel;
volatile bool adcOngoingSequenceReading = false;
volatile bool adcRepetitiveReading = false;
volatile bool adcSamplingActive = false;
volatile bool adcSequenceReadingComplete = false;
float adcOffsetCompensation = 0;
float adcGainCompensation = 1;

#ifdef __ICCAVR
//#pragma interrupt_handler timer2_compa_isr:TIMER2_COMPA_ISR_VECTOR_NO
//void timer2_compa_isr(void)
#error Define ISR vector for ICC AVR
#else
ISR(ADC_vect)
#endif
{
	unsigned int adcValue;
	unsigned char lastChannelRead;
	//canalul care a fost citit la ultima conversie, cea care a generat aceasta intrerupere
	
	unsigned char channelIndex;

	lastChannelRead = ADMUX & 0x0F;
	adcSelectedChannel = lastChannelRead;
	
	//Formez cei 10 biti ai conversiei curente
	adcValue = ADCL;
	adcValue |= (unsigned int)ADCH<<8;

	//Daca s-a resetatat contorul care indica numarul de esantioane pe care se face medierea
	//inseamna trebuie sa incep o noua insumare a valorilor pentru a face o noua mediere, altfel
	//adaug la valoarea existenta valoarea proaspat citita;
	if(adcSequenceSamplesCounter == 0)
	adcSequenceReadingsSum[lastChannelRead] = adcValue;
	else
	adcSequenceReadingsSum[lastChannelRead] += adcValue;
	

	if (lastChannelRead == adcLastChannelUsed)
	{
		//Daca am ajuns la ultimul canal utilizat inseamana ca am citit valorile pentru toate celelalte canale si pot incrementa
		//contorul de mediere pentru toate canalele. Contorul trebuie incrementat doar dupa ce am citit cate un esantion pentru
		//fiecare canal utilizat
		adcSequenceSamplesCounter++;
		adcSelectedChannel = adcFirstChannelUsed;

		//selectez (prescriu) canalul care ve fi citit la urmatoarea achizitie
		ADMUX = (ADMUX & 0xF0) | adcSelectedChannel;
		
		//Daca am facut un ciclu complet de citire voi indica acest lucru prin adcSequenceReadingComplete
		if(adcSequenceSamplesCounter == adcSequenceSamplesNumber)
		{
			adcSequenceReadingComplete = true; //se reseteaza (in bucla principala ) dupa ce am prelucrar rezultatul
			
			//Dubla bufferare
			for(channelIndex=adcFirstChannelUsed; channelIndex<=adcLastChannelUsed; channelIndex++)
			{
				adcSequenceReadingBuffer[channelIndex] = adcSequenceReadingsSum[channelIndex];
			}
			
			if(adcIsrSamplingCompleteHandler != null)
			{
				adcIsrSamplingCompleteHandler();
			}
			
			if(adcIsrSequenceReadingCompleteHandler != null)
			{
				adcIsrSequenceReadingCompleteHandler();
			}

			//Pentru a evita procesari consumatoare de timp in interiorul ISR
			//Voi forta un timer care sa realizeze in bucla main aceasta procesare
			//Astfel timerul este marcat ca expirat iar codul asociat va fi executat imediat de se va
			//face evaluarea timerelor (respectand ordinea de executie a celorlalte timere)
			timerSW_Forteaza(adcReadingsProcessingTimerHandle);
		}
		else
		{
			//am citit ultimul canal utilizabil deci am o esantionare completa (ATENTIE: dar nu o citire completa.
			//O citire completa implica citirea tuturor esantioanelor pentru toate canalele active)
			if(adcIsrSamplingCompleteHandler != null)
			{
				adcIsrSamplingCompleteHandler();
			}
		}
		adcSamplingActive = false;
	}
	else
	{
		//selectez urmatorul canal ADC care este utilizat in aplicatie
		//Din bucla ies intotdeauna cand dau de urmatorul canal marcat ca fiind utilizat
		do{
			adcSelectedChannel++;
		}
		while(!adcUsedChannels[adcSelectedChannel]);
		
		//selectez (prescriu) canalul care ve fi citit la urmatoarea achizitie
		ADMUX = (ADMUX & 0xF0) | adcSelectedChannel;

		//Daca ultimul canal citit nu este ultimul utilizabil inseamna ca nu am facut o esantionare
		//completa a tuturor canalelor. Prin urmare comand o noua achizitie
		//pornesc conversia aici daca inca nu am esantionat toate canalele
		ADCSRA |= 0x40;
	}
}

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
//Fiecare citire poate fi realizata prin citirea unuia sau mai multor esantioane (pentru toate canalele active)
//Acest timer stabileste perioada cu care sunt citite esantionele (adica o serie de citiri a tuturor canalelor active)
//si comanda acest lucru
void sampleReadingTimerCallback(void)
{
	//adc_bEsantionareActiva este setat in false in ISR atunci cand s-a citit cate un sample pentru fiecare canal
	if(adcSequenceSamplesCounter <= adcSequenceSamplesNumber)
	{
		adcStartSampleReading();
	}
}

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
//Fiecare citire poate fi realizata prin citirea unuia sau mai multor esantioane (pentru toate canalele active)
//Acest timer stabileste perioada cu care se face o citire (o citire implica esantionarea tuturor canalelor active,
//o data sau de mai multe ori.)
void sequenceReadingTimerCallback(void)
{
	if(adcRepetitiveReading)
	{
		//Timerul care comanda citirile si timerul care comanda esantionarile lucreaza independent.
		//Deoarece ambele sunt timere SW perioada setata poate sa varieze intr-o oarecare masura in functie de
		//incarcarea aplicatiei si timpul scurs intre doua evaluari ale timerelor.
		//Din acest motiv, daca perioada de esantionare si numarul de esantioane sunt alese astfel incat produsul
		//lor sa fie la limita perioadei de citire, (de ex. Tcitire = 1000ms, Tesan=50ms, Nesantioate=20) este posibil
		//ca intervalul in care s-ar citi toate esantioanele sa depaseasca totusi perioada de citire completa
		// Tesan * Nesantiaone + TRezidualintreEsantionari > Tcitire.
		//In acest caz nu se va mai comanda inca o citire pentru a permite achizitia tuturor easntioanelor de la
		//citirea anterioara. In acest mod se pierde o citire.
		//Este deci recomandat ca Tesan si Nesantioane sa nu fie alese la limita Tcitire
		if(adcSequenceReadingComplete)
		{
			adcStartSequenceReading(true, false);
		}
		else
		{
			if(adcSequenceReadingErrorHandler != null)
			adcSequenceReadingErrorHandler();
		}
	}
}

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
* \param sequenceReadingPeriod - Defines the periodicity (in milliseconds) for the sequence readings
* \param samplingPeriod - Defines the periodicity (in milliseconds) for the sampling of the active ADC channels inside a
*							sequence reading window
* \param sequenceReadingSamplesCount - The number of samples read for each active ADC channel inside a sequence reading window
* \param    activeChannelsMask - Indicates which of the ADC channels are used for analog acquisition
* \param    ADC_PORT - Reference to the port providing the ADC inputs
* \param    selectedReference - The reference type (Internal, external, AVCC). See ADCReferenceSelect.
* \param    resultsAlignament - The results alignment type (left, right). See ADCResultsAlingament
* \param    isrSamplingCompleteHandler - Pointer to a function which will be called inside the ADC ISR after a sampling 
*           of all active channels is complete. Can be null.
* \param    isrSequenceReadingCompleteHandler - Pointer to a function which will be called inside the ADC ISR after a
            sequence reading is complete. Can be null.
* \param    sequenceReadingProcessor - Pointer to a custom function used for processing the results inside the main loop
            by a forced avr_timere_sw based timer after a sequence reading is complete.
            If null, the default adcAveregingAndVoltsConversion is executed and the results of the
            averaging are held into the adcSequenceReadingAverage array while the voltage conversion
            corrected for offset and gain errors (see the offsetCompensation and gainCompensation params)
            are held int the adcInputsAsVoltage array.
* \param    sequenceReadingErrorHandler - Pointer to a custom function used for error handling if a sequence reading is
            automatically re-triggered while another sequence reading is still underway. Can be null.
* \param    vrefOrFullScaleVoltage - The value in volts of the reference voltage of of the process full scale voltage.
*			This will be used for the actual conversion. Set this to the value
*			of the internal reference, the value at the AREF pin or the value of AVCC according to the setting of the
*			selectedReference parameter or to the value of the process full scale voltage depending on the cas.
*			See the description of adcVrefOrFullScaleVoltage
* \param    offsetCompensation - Float value indicating the ADC offset compensation correction used in the
*			adcAveregingAndVoltsConversion fuction
* \param gainCompensation - Float value indicating the ADC gain compensation correction used in the
*			adcAveregingAndVoltsConversion fuction
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
				)
{
	unsigned char channelIndex;
	
	adcSequenceReadingComplete = false;
	adcOngoingSequenceReading = false;
	adcRepetitiveReading = false;
	adcSamplingActive = false;
	adcSequenceSamplesCounter = 0;
	
	adcIsrSamplingCompleteHandler = isrSamplingCompleteHandler;
	adcIsrSequenceReadingCompleteHandler = isrSequenceReadingCompleteHandler;
	adcSequenceReadingErrorHandler = sequenceReadingErrorHandler;

	if(sequenceReadingProcessor != null)
	{
		adcSequenceReadingProcessor = sequenceReadingProcessor;
	}
	else
	{
		adcSequenceReadingProcessor = adcAveregingAndVoltsConversion;
	}

	adcVrefOrFullScaleVoltage = vrefOrFullScaleVoltage;
	adcVoltsPerBit = adcVrefOrFullScaleVoltage / ADC_FULLSCALE;
	adcOffsetCompensation = offsetCompensation;
	adcGainCompensation = gainCompensation;
	
	if(samplingPeriod < ADC_MIN_SAMPLING_PERIOD)
	{
		return ADC_MIN_SAMPLING_PERIOD_ERROR;
	}
	
	if(sequenceReadingSamplesCount*samplingPeriod >= sequenceReadingPeriod)
	{
		return ADC_SEQUENCE_READING_PERIOD_ERROR;
	}
	
	adcSequenceSamplesNumber = sequenceReadingSamplesCount;
	
	adcSampleReadingTimerHandle =  timerSW_Creaza(sampleReadingTimerCallback, samplingPeriod, timerPornit, true);
	adcSequenceReadingTimerHandle =  timerSW_Creaza(sequenceReadingTimerCallback, sequenceReadingPeriod, timerPornit, true);

	//Timerul care comanda executia in afara ISR-ului a rutinelor de procesare consumatoare de timp
	//dupa ce s-a terminat o citire. De fapt timerul este fortat atunci cand s-a finalizat o seventa de citiri si astfel
	//callback-ul asociat se va executa in functia main la prima evaluare a timerelor SW
	adcReadingsProcessingTimerHandle =  timerSW_Creaza(adcSequenceReadingProcessor, 0, timerOprit, false);

	if((adcSampleReadingTimerHandle == EROARE_TIMERE_INDISPONIBILE)
			|| (adcSequenceReadingTimerHandle == EROARE_TIMERE_INDISPONIBILE)
			|| (adcReadingsProcessingTimerHandle == EROARE_TIMERE_INDISPONIBILE))
	{
				
		return ADC_TIMERS_SETTING_ERROR;
	}

	if(activeChannelsMask == 0)
	{
		return ADC_INVALID_CHANNEL_ERROR;
	}
		

	adcFirstChannelUsed = -1;
	
	WDR();
	//Determin care este primul si ultimul ADC marcat ca utilizat in aplicatia noastra (dintre ADC0......ADCn)
	for(channelIndex=0; channelIndex<ADC_CHANNELS_COUNT; channelIndex++)
	{
		adcUsedChannels[channelIndex] = (( (0x01 << channelIndex) & activeChannelsMask)==0)? 0:1;   //or getCharBit(activeChannelsMask, channelIndex);
		
		if(adcUsedChannels[channelIndex])
		{
			if(adcFirstChannelUsed == -1)    //Daca n-am atribuit inca primul canal utilizabil
			adcFirstChannelUsed = channelIndex;
			adcLastChannelUsed = channelIndex;
		}
	}


	*(ADC_PORT-1) &=  ~activeChannelsMask; //Setez in 0 (intrari) pinii care sunt utilizati de ADC
	*ADC_PORT &=  ~activeChannelsMask; //Dezactivez pull-up-urile (Setez in 0 pinii care sunt utilizati de ADC)

	ADCSRA = 0x00; //disable adc
	
	ADMUX = 0x00;  //resetez setariele initiale
	ADMUX |= selectedReference | resultsAlignament; //Selectez referinta si alinierea rezultatului.
	//Canalul este selectat in functiile de citire.
	
	ACSR  = 0x80;
	//ADCSRB = 0x00;
	ADCSRA = 0x8F;
	return 0;
}


//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
void adcStartSampleReading(void)
{
	ADMUX = ADMUX | adcFirstChannelUsed;
	//adc_bCitireCompleta = false; - nu e nevoie, e deja setata in adc_StartCitire
	//adc_bCitireInCurs = true; - nu e nevoie, e deja setata in adc_StartCitire
	adcSamplingActive = true;
	ADCSRA |= 0x40;
}

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
void adcStartSequenceReading(bool periodicReading, bool blockingRead)
{
	adcRepetitiveReading = periodicReading;
	adcSequenceReadingComplete = false;
	adcOngoingSequenceReading = true;
	adcSamplingActive = true;
	adcSequenceSamplesCounter = 0;
	
	ADMUX = ADMUX | adcFirstChannelUsed;
	ADCSRA |= 0x40;
	
	if(blockingRead)
	{
		do
		{
			WDR();
		}
		while(!adcSequenceReadingComplete);
	}
}

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
void adcSequenceReadingAvereging()
{
	unsigned char channelIndex;
	adcOngoingProcessing = true;
	for(channelIndex=adcFirstChannelUsed; channelIndex<=adcLastChannelUsed; channelIndex++)
	{
		adcSequenceReadingAverage[channelIndex] = adcSequenceReadingsSum[channelIndex] / adcSequenceSamplesNumber;
	}
	adcOngoingProcessing = false;
}

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
void adcAveregingAndVoltsConversion(void)
{
	unsigned char channelIndex;
	adcOngoingProcessing = true;
	for(channelIndex=adcFirstChannelUsed; channelIndex<=adcLastChannelUsed; channelIndex++)
	{
		WDR();
		adcSequenceReadingAverage[channelIndex] = adcSequenceReadingsSum[channelIndex] / adcSequenceSamplesNumber;
		adcInputsAsVoltage[channelIndex] = ((float)adcSequenceReadingAverage[channelIndex]* adcVoltsPerBit+ adcOffsetCompensation) * adcGainCompensation;
	}
	adcOngoingProcessing = false;
}
