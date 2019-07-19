/*
 * Code written by Chia Jiun Wei @ 22 May 2017
 * <J.W.Chia@tudelft.nl>
 *
 * PQ9Bus: a library to provide full hardware-driven PQ9Bus functionality
 * to the TI MSP432 family of microcontrollers. It is possible to use
 * this library in Energia (the Arduino port for MSP microcontrollers)
 * or in other toolchains.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License 
 * version 3, both as published by the Free Software Foundation.
 *
 */
 
#include <PQ9Bus.h>

/* A reference list of PQ9Bus PQ9Bus_instancess */
PQ9Bus * PQ9Bus_instances[4];		// pointer to the instantiated	PQ9Bus classes

/**
 * The main (global) interrupt	handler
 * It ain't pretty, but using this as a macro should increase the performance tremendously
 */
#define IRQHANDLER(M) \
    { \
    uint32_t status = MAP_UART_getEnabledInterruptStatus( EUSCI_A## M ##_BASE ); \
            MAP_UART_clearInterruptFlag( EUSCI_A## M ##_BASE, status ); \
         \
        /* Receive interrupt flag */ \
        if (status & UCRXIFG) \
        { \
            if (MAP_UART_queryStatusFlags(EUSCI_A## M ##_BASE, EUSCI_A_UART_ADDRESS_RECEIVED)) \
            { \
                /* This is an address bit */ \
                if (MAP_UART_receiveData(EUSCI_A## M ##_BASE) == PQ9Bus_instances[M]->address) \
                { \
                    /* Received the correct address */ \
                    /* Enable UART */ \
                    MAP_UART_resetDormant(EUSCI_A## M ##_BASE); \
                    PQ9Bus_instances[M]->buffer[0] = PQ9Bus_instances[M]->address; \
                    PQ9Bus_instances[M]->crc.init(); \
                    PQ9Bus_instances[M]->crc.newChar(PQ9Bus_instances[M]->buffer[0]); \
                    /* change state */ \
                    PQ9Bus_instances[M]->state = Size; \
                } \
            } \
            else if (PQ9Bus_instances[M]->state == Size) \
            { \
                /* Received the frame size */ \
                PQ9Bus_instances[M]->buffer[1] = MAP_UART_receiveData(EUSCI_A## M ##_BASE); \
                PQ9Bus_instances[M]->crc.newChar(PQ9Bus_instances[M]->buffer[1]); \
                PQ9Bus_instances[M]->frameSize = PQ9Bus_instances[M]->buffer[1]; \
                PQ9Bus_instances[M]->frameSizeCounter = 0; \
                PQ9Bus_instances[M]->state = Source; \
            } \
            else if (PQ9Bus_instances[M]->state == Source) \
            { \
                /* Received the frame source */ \
                PQ9Bus_instances[M]->buffer[2] = MAP_UART_receiveData(EUSCI_A## M ##_BASE); \
                PQ9Bus_instances[M]->crc.newChar(PQ9Bus_instances[M]->buffer[2]); \
                PQ9Bus_instances[M]->state = PayloadByte; \
            } \
            else if (PQ9Bus_instances[M]->state == PayloadByte) \
            { \
                /* Received the frame payload byte */ \
                if (PQ9Bus_instances[M]->frameSizeCounter < PQ9Bus_instances[M]->frameSize) \
                { \
                    PQ9Bus_instances[M]->buffer[PQ9Bus_instances[M]->frameSizeCounter + 3] = MAP_UART_receiveData(EUSCI_A## M ##_BASE); \
                    PQ9Bus_instances[M]->crc.newChar(PQ9Bus_instances[M]->buffer[PQ9Bus_instances[M]->frameSizeCounter + 3]); \
                    PQ9Bus_instances[M]->frameSizeCounter++; \
                } \
                else if (PQ9Bus_instances[M]->frameSizeCounter == PQ9Bus_instances[M]->frameSize) \
                { \
                    PQ9Bus_instances[M]->buffer[PQ9Bus_instances[M]->frameSize + 3] = MAP_UART_receiveData(EUSCI_A## M ##_BASE); \
                    PQ9Bus_instances[M]->state = CRC; \
                } \
                else \
                { \
                    PQ9Bus_instances[M]->state = WaitForAddress; \
                    MAP_UART_setDormant(EUSCI_A## M ##_BASE); \
                } \
            } \
            else if (PQ9Bus_instances[M]->state == CRC) \
            { \
                PQ9Bus_instances[M]->state = WaitForAddress; \
                MAP_UART_setDormant(EUSCI_A## M ##_BASE); \
                PQ9Bus_instances[M]->buffer[PQ9Bus_instances[M]->frameSize + 4] = MAP_UART_receiveData(EUSCI_A## M ##_BASE); \
                \
                unsigned short computedCRC = PQ9Bus_instances[M]->crc.getCRC(); \
                unsigned char crc1 = (unsigned char)((computedCRC >> 8) & 0xFF); \
                unsigned char crc2 = (unsigned char)(computedCRC & 0xFF); \
                \
                if ((crc1 == PQ9Bus_instances[M]->buffer[PQ9Bus_instances[M]->frameSize + 3]) && \
                    (crc2 == PQ9Bus_instances[M]->buffer[PQ9Bus_instances[M]->frameSize + 4])) \
                { \
                    PQ9Bus_instances[M]->_handleReceive(PQ9Bus_instances[M]->buffer); \
                } \
            } \
            else \
            { \
                PQ9Bus_instances[M]->state = WaitForAddress; \
                MAP_UART_setDormant(EUSCI_A## M ##_BASE); \
            } \
        } \
	}
  
/**** ISR/IRQ Handles ****/
void EUSCIA0_IRQHandler( void ) 
{
	IRQHANDLER(0);
}

void EUSCIA1_IRQHandler( void ) 
{
	IRQHANDLER(1);
}

void EUSCIA2_IRQHandler( void ) 
{
	IRQHANDLER(2);
}

void EUSCIA3_IRQHandler( void ) 
{
	IRQHANDLER(3);
}

/**** CONSTRUCTOR ****/
PQ9Bus::PQ9Bus(uint8_t mod, unsigned long port, unsigned long pin)
{
    TXEnablePort = port;
    TXEnablePin = pin;

	 switch (mod) 
	{	   
		case 0:
			this->module = EUSCI_A0_BASE;
			PQ9Bus_instances[0] = this;
			break;

		case 1:
			this->module = EUSCI_A1_BASE;
			PQ9Bus_instances[1] = this;
			break;
			
		case 2:
			this->module = EUSCI_A2_BASE;
			PQ9Bus_instances[2] = this;
			break;
			
		case 3:
			this->module = EUSCI_A3_BASE;
			PQ9Bus_instances[3] = this;
			break;
	}	
}

/**** DESTRUCTORS Reset the module ****/
PQ9Bus::~PQ9Bus()
{
	MAP_UART_disableModule(this->module);
	MAP_UART_unregisterInterrupt(this->module);
	
		/* Deregister from the moduleMap */
	switch (module) 
	{
		case EUSCI_A0_BASE:
			PQ9Bus_instances[0] = 0;
			break;
			
		case EUSCI_A1_BASE:
			PQ9Bus_instances[1] = 0;
			break;
			
		case EUSCI_A2_BASE:
			PQ9Bus_instances[2] = 0;
			
			break;
		case EUSCI_A3_BASE:
			PQ9Bus_instances[3] = 0;
			break;
	}
}

/**** Begin PQ9Bus ****/
void PQ9Bus::begin(unsigned int baudrate, uint8_t address)
{	
	MAP_UART_disableModule(this->module);	//disable UART operation for configuration settings

	this->address = address;
	this->baudrate = baudrate;

	_initMain();	//UART pins init
	
    eUSCI_UART_Config Config;

    //Default Configuration, macro found in uart.h
    Config.selectClockSource	= EUSCI_A_UART_CLOCKSOURCE_SMCLK;
    Config.parity				= EUSCI_A_UART_NO_PARITY;
    Config.msborLsbFirst		= EUSCI_A_UART_LSB_FIRST;
    Config.numberofStopBits		= EUSCI_A_UART_ONE_STOP_BIT;
    Config.uartMode 			= EUSCI_A_UART_ADDRESS_BIT_MULTI_PROCESSOR_MODE;
			
	unsigned int n = MAP_CS_getSMCLK() / baudrate;
	
	if (n > 16)
	{
		Config.overSampling = EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION; // Oversampling
		Config.clockPrescalar = n >> 4;										 // BRDIV = n / 16
		Config.firstModReg = n - (Config.clockPrescalar << 4);			 // UCxBRF = int((n / 16) - int(n / 16)) * 16
	}
	else
	{
		Config.overSampling = EUSCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION; // Oversampling
		Config.clockPrescalar = n;											  // BRDIV = n
		Config.firstModReg = 0;												  // UCxBRF not used
	}
		
    Config.secondModReg = 0;	// UCxBRS = 0

    MAP_UART_initModule(this->module, &Config);
	
	MAP_UART_enableModule(this->module);		// enable UART operation
	MAP_UART_setDormant(this->module);			//address bit multi processor mode, only address will triggered RXIFG
}

/**** TX 
 * Parameter:
 * uint_fast8_t address: receiver address
 * uint8_t * TxBuffer: Transmit buffer pointer
 * uint8_t TxBufferSize: Size of Transmit buffer
****/
void PQ9Bus::transmit( uint_fast8_t destination, uint_fast8_t source, uint8_t * TxBuffer, uint8_t TxBufferSize)
{
    MAP_GPIO_setOutputHighOnPin( TXEnablePort, TXEnablePin );

	crc.init();
	MAP_UART_transmitAddress(this->module, destination);
	crc.newChar(destination);
	
	MAP_UART_transmitData(this->module, TxBufferSize);
	crc.newChar(TxBufferSize);
	
	MAP_UART_transmitData(this->module, source);
	crc.newChar(source);

	for (int index = 0; index < TxBufferSize; index++) 
	{
		MAP_UART_transmitData(this->module, TxBuffer[index]);
		crc.newChar(TxBuffer[index]);
	}
	
	unsigned short computedCRC = crc.getCRC();
	MAP_UART_transmitData(this->module, (*((unsigned char*)  &(computedCRC)+1)) );
	MAP_UART_transmitData(this->module, (*((unsigned char*)  &(computedCRC)+0)) );

	// Workaround for USCI42 errata
	// introduce a 2 bytes delay to make sure the UART buffer is flushed
	uint32_t d = MAP_CS_getMCLK() * 4 / this->baudrate;
	for(uint32_t k = 0; k < d;  k++)
	{
	    __asm("  nop");
	}

	MAP_GPIO_setOutputLowOnPin( TXEnablePort, TXEnablePin );
}

/**** RX Interrupt Handler ****/
void PQ9Bus::onReceive( void (*islHandle)(unsigned char *) )
{
	user_onReceive = islHandle;	//parse handler function
	
	if ( islHandle )
	{
	    status = 0;

		uint32_t status = MAP_UART_getEnabledInterruptStatus(this->module);
		
		// clear the receive interrupt to avoid spurious triggers the first time
		MAP_UART_clearInterruptFlag( this->module, status );
		
		// enable receive interrupt
		MAP_UART_enableInterrupt( this->module, EUSCI_A_UART_RECEIVE_INTERRUPT );
	}
	else
	{
		// disable receive interrupt
		MAP_UART_disableInterrupt( this->module, EUSCI_A_UART_RECEIVE_INTERRUPT );
	}	
}

/**** CRC calculator ****/
uint16_t PQ9Bus::softwareCRC (uint16_t crc1, uint8_t data, uint16_t poly)
{
  for (unsigned char i = 0; i < 8; i++)
  {
    if (((( crc1 & 0x8000) >> 8) ^ (data & 0x80)) != 0)
    {
      crc1 <<= 1;
      crc1 ^= poly;
    }
    else
    {
      crc1 <<= 1;
    }
    data <<= 1;
  }
  return crc1;
}

/**** PRIVATE ****/
/**** Initialize UART Pin Configuration based on EUSCI used ****/
void PQ9Bus::_initMain( void )
{
	switch (module) 
	{	
		case EUSCI_A0_BASE:
		
		modulePort = EUSCI_A0_PORT;
		modulePins = EUSCI_A0_PINS;
		
		// transmit / receive interrupt request handler
		MAP_UART_registerInterrupt(this->module, EUSCIA0_IRQHandler);
		
		break;
		
		case EUSCI_A1_BASE:
		
		modulePort = EUSCI_A1_PORT;
		modulePins = EUSCI_A1_PINS;
		
		// transmit / receive interrupt request handler
		MAP_UART_registerInterrupt(this->module, EUSCIA1_IRQHandler);
		
		break;
		
		case EUSCI_A2_BASE:
		
		modulePort = EUSCI_A2_PORT;
		modulePins = EUSCI_A2_PINS;
		
		// transmit / receive interrupt request handler
		MAP_UART_registerInterrupt(this->module, EUSCIA2_IRQHandler);
		
		break;
		
		case EUSCI_A3_BASE:
		
		modulePort = EUSCI_A3_PORT;
		modulePins = EUSCI_A3_PINS;
		
		// transmit / receive interrupt request handler
		MAP_UART_registerInterrupt(this->module, EUSCIA3_IRQHandler);
		
		break;		
	}	
	MAP_GPIO_setAsPeripheralModuleFunctionInputPin(modulePort, modulePins, GPIO_PRIMARY_MODULE_FUNCTION);

	MAP_GPIO_setAsPeripheralModuleFunctionInputPin( TXEnablePort,
                                        TXEnablePin, GPIO_PRIMARY_MODULE_FUNCTION );

    MAP_GPIO_setOutputLowOnPin( TXEnablePort, TXEnablePin );

    MAP_GPIO_setAsOutputPin( TXEnablePort, TXEnablePin );
}

/**
 * Internal process handling the rx, and calling the user's interrupt handles
 */
void PQ9Bus::_handleReceive(uint8_t *buffer)
{
	// do something only if there is a handler registered
	if (user_onReceive)
	{
	    // call the user-defined data transfer handler
	    user_onReceive(buffer);
	}
}
