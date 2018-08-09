/*
* avr_twi_i2c.c
*
* Created: 01.01.2009 00:00:00
* Author : Bogdan Tarnauca
*/

#include "avr_twi_i2c.h"

char twi_Stop(bool bWaitForStop)
{
    twiStatus = TWI_TRANSFER_ONGOING; 
    timeoutLoopCount = TWI_TIMEOUT_WAIT_LOOP_COUNT;
    
    TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);

    if(bWaitForStop){
        do{
            WDR();
            timeoutLoopCount--;
        } 
        while( ((TWCR & (1<<TWSTO)) != 0) && timeoutLoopCount>0); 
    
        if(timeoutLoopCount == 0)
            twiStatus = TWI_STOP_COND_TIMEOUT;
        else
            twiStatus = TWI_IDLE;
    }
    
    return twiStatus;
}

unsigned char twi_Write(unsigned char nAddress, volatile char *sTxBuffer, unsigned char nBytesToTransmit, bool bWaitForTransmit)
{
    timeoutLoopCount = TWI_TIMEOUT_WAIT_LOOP_COUNT;
    twiAddress = nAddress;
    twiCommand = TWI_WR;
    bytesToTransmit = nBytesToTransmit;
    twiTxBuffer = sTxBuffer;

    twiTxCompleted = false;
    twiRxCompleted = false;
    twiStatus = TWI_TRANSFER_ONGOING;

    //TWCR = (1<<TWSTA) | (1<<TWEN) | (1<<TWIE) | (1<<TWINT);
    TWCR = (1<<TWINT) | (0<<TWEA) | (1<<TWSTA) | (0<<TWSTO) | (0<<TWWC) | (1<<TWEN) | (1<<TWIE);


    if(bWaitForTransmit){
        do{
            WDR();
            timeoutLoopCount--;
        }
        while((!twiTxCompleted) && (twiStatus == TWI_TRANSFER_ONGOING) && timeoutLoopCount>0);
    
        if(timeoutLoopCount == 0)
            twiStatus = TWI_WRITE_TIMEOUT;
    }

    return twiStatus;  //are sens evaluarea acestei valori doar daca
                        //se asteapta finalizarea transmisiei (bWaitForTransmit = 1)
                        //altfel valoarea de retur este lipsita de semnificatie.
}


unsigned char twi_Read(unsigned char nAddress, volatile char *sRxBuffer, unsigned char nBytesToReceive, bool bWaitForReceive)
{
    timeoutLoopCount = TWI_TIMEOUT_WAIT_LOOP_COUNT;
    
    twiAddress = nAddress;
    twiCommand = TWI_RD;
    bytesToReceive = nBytesToReceive;
    twiRxBuffer = sRxBuffer;

    twiTxCompleted = false;
    twiRxCompleted = false;
    twiStatus = TWI_TRANSFER_ONGOING;

    //TWCR = (1<<TWSTA) | (1<<TWEN) | (1<<TWIE) | (1<<TWINT);
    TWCR = (1<<TWINT) | (0<<TWEA) | (1<<TWSTA) | (0<<TWSTO) | (0<<TWWC) | (1<<TWEN) | (1<<TWIE);

    if(bWaitForReceive){
        do{ 
            WDR();
            timeoutLoopCount--;
        } 
        while((!twiRxCompleted) && (twiStatus == TWI_TRANSFER_ONGOING)  && timeoutLoopCount>0);
    
        if(timeoutLoopCount == 0)
            twiStatus = TWI_READ_TIMEOUT;
    }
    
    return twiStatus;  //are sens evaluarea acestei valori doar daca
                        //se asteapta finalizarea receptiei (bWaitForReceive = 1)
                        //altfel valoarea de retur este lipsita de semnificatie.
}

void twi_Init(volatile uint8_t *twiPort, unsigned char twiSCLPin, unsigned char twiSDAPin, unsigned char nTWBR, unsigned char nTWSR, unsigned char nTWAR)
{
	//TODO: Add port setting for sda and scl with internal pullups

    //DDR - set as inputs in order to set the pullups
    *(twiPort-1) |= (1<<twiSDAPin)|(1<<twiSCLPin);
    //set the pullups
    *(twiPort) |=  (1<<twiSDAPin)|(1<<twiSCLPin);

    TWCR= 0x00; //disable twi
    TWBR = nTWBR;
    TWSR = nTWSR;
    TWAR = nTWAR;  //pentru versiunea MASTER acest parametru nu conteaza
                   //pastrat asa pentru eventuale dezvoltari ulterioare
    //TWCR= 0x65; //enable twi
    TWCR= 0x00; 
}

ISR(TWI_vect)
{
    WDR();
    twiStatus = TWSR & TWS_MASK;

    //------------------------------------------------------------------------------------------------
    // START
    //------------------------------------------------------------------------------------------------
    if( ((TWSR & TWS_MASK) == 0x08) || ((TWSR & TWS_MASK) == 0x10))
    {   
		//start sau repeated start condition
        //dupa (re)start condition transmit fie un SLA+W (AAAA_AAA0)fie un SLA+R (AAAA_AAA1)
        //pentru a scrie sau a citi dispozitivul cu adresa AAA_AAAA
        TWDR = (twiAddress << 1) | twiCommand;
        twiRxCompleted = false;
        twiTxCompleted = false;
        twiCounter = 0;
        twiStatus = TWI_TRANSFER_ONGOING;
        TWCR = (1<<TWINT) | (0<<TWEA) | (0<<TWSTA) | (0<<TWSTO) | (0<<TWWC) | (1<<TWEN) | (1<<TWIE);
        return;
    }
	
    //------------------------------------------------------------------------------------------------
    // READ
    //------------------------------------------------------------------------------------------------
    if( (TWSR & TWS_MASK) == 0x40)
    {   
		//SLA+R transmis, ACK receptionat
        twiStatus = TWI_TRANSFER_ONGOING;
        if(bytesToReceive == 1)
        {   //Daca am doar un byte de receptionat atunci NU voi seta TWEA astfel incat la urmatorul byte receptionat
            //sa te transmita NACK.
            TWCR = (1<<TWINT) | (0<<TWEA) | (0<<TWSTA) | (0<<TWSTO) | (0<<TWWC) | (1<<TWEN) | (1<<TWIE);
            
        }
        else //am de receptionat mai multi bytes
        {   //activez TWEA pentru a transmite ACK dupa fiecare byte receptionat
            TWCR = (1<<TWINT) | (1<<TWEA) | (0<<TWSTA) | (0<<TWSTO) | (0<<TWWC) | (1<<TWEN) | (1<<TWIE);
        }
        return;
    }
    //------------------------------------------------------------------------------------------------
    if( (TWSR & TWS_MASK) == 0x50)
    {   //byte receptionat, ACK a fost returnat
        twiStatus = TWI_TRANSFER_ONGOING;
        *(twiRxBuffer + twiCounter) = TWDR;

        if( (bytesToReceive != 1) && (twiCounter == bytesToReceive-1) ) 
        {   //se aplica doar pentru nTWI_BytesToReceive>1. Cand nTWI_BytesToReceive = 1, activarea NACK este comandata 
            //chiar dupa ce a fost transmis SLA+R (TWSR = 0x40)
            //dupa penultimul byte receptionat resetez TWEA (enable aknowledge) pentru ca la
            //ultimul byte sa nu mai dau ACK. Astfel indic transmitatorului ca am receptionat ultimul byte
            TWCR = (1<<TWINT) | (0<<TWEA) | (0<<TWSTA) | (0<<TWSTO) | (0<<TWWC) | (1<<TWEN) | (1<<TWIE);
        }
        else{
            TWCR = (1<<TWINT) | (1<<TWEA) | (0<<TWSTA) | (0<<TWSTO) | (0<<TWWC) | (1<<TWEN) | (1<<TWIE);
        }
        twiCounter++;
        return;
    }
    //------------------------------------------------------------------------------------------------
    if( (TWSR & TWS_MASK) == 0x58)
    {   //byte receptionat, NACK a fost returnat - aceasta situatie apare in momentul in care este
        //receptionat ultimul byte (dupa ultimul byte receptionat masterul trebuie sa transmita un NACK)
        twiStatus = TWI_TRANSFER_ONGOING;
        *(twiRxBuffer + twiCounter) = TWDR;
        twiRxCompleted = true;
        twiStatus = TWI_IDLE;
        TWCR = (1<<TWINT) | (0<<TWEA) | (0<<TWSTA) | (1<<TWSTO) | (0<<TWWC) | (1<<TWEN) | (1<<TWIE);
        return;
        //nTWI_Counter++; -- nu este necesar din moment ce acesta a fost ultimul byte receptionat.
    }
	
    //------------------------------------------------------------------------------------------------
    // WRITE
    //------------------------------------------------------------------------------------------------
    if( (TWSR & TWS_MASK) == 0x18)
    {   //SLA+W transmis, ACK receptionat
        twiStatus = TWI_TRANSFER_ONGOING;
        twiCounter = 0;
        TWDR = *twiTxBuffer;  //pregatesc pentru transmisie primul caracter din buffer
        TWCR = (1<<TWINT) | (0<<TWEA) | (0<<TWSTA) | (0<<TWSTO) | (0<<TWWC) | (1<<TWEN) | (1<<TWIE);
        return;
    }
    //------------------------------------------------------------------------------------------------
    if( (TWSR & TWS_MASK) == 0x28)
    {   //byte transmis, ACK receptionat
        twiStatus = TWI_TRANSFER_ONGOING;
        twiCounter++;
        if(twiCounter == bytesToTransmit)
        {           
            //Reset the TWINT interupt flag and generate a stop condition and wait to be set.
            TWCR = (1<<TWINT) | (0<<TWEA) | (0<<TWSTA) | (1<<TWSTO) | (0<<TWWC) | (1<<TWEN) | (1<<TWIE);
            do{
                WDR();
            }
            while( ((TWCR & (1<<TWSTO)) != 0));
            
            twiStatus = TWI_IDLE;
            twiTxCompleted = true;
        }
        else
        {
            TWDR = *(twiTxBuffer + twiCounter) ;
            TWCR = (1<<TWINT) | (0<<TWEA) | (0<<TWSTA) | (0<<TWSTO) | (0<<TWWC) | (1<<TWEN) | (1<<TWIE);
        }
        return;
    }
	
    //------------------------------------------------------------------------------------------------
    // ERORI
    //------------------------------------------------------------------------------------------------
    if( (TWSR & TWS_MASK) == 0x38 || (TWSR & TWS_MASK) == 0x48 || (TWSR & TWS_MASK) == 0x20 || (TWSR & TWS_MASK) == 0x30)
    { 
        //TWSR == 0x38, Arbitrare pierduta	
        //TWSR == 0x48 SLA+R transmis, NACK receptionat
        //TWSR == 0x20 SLA+W transmis, NACK receptionat
        //TWSR == 0x30 Data byte transmis, NACK receptionat
        
        twiStatus = TWSR & TWS_MASK;
        
        TWCR = (1<<TWINT) | (0<<TWEA) | (0<<TWSTA) | (0<<TWSTO) | (0<<TWWC) | (1<<TWEN) | (1<<TWIE);
    }
    else  //alt cod netratat
    {
        twiStatus = TWI_UNKNOWN_STATUS;
    }
	
	TWCR = 0;
}
