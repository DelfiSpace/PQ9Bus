/*
 * Code written by Chia Jiun Wei @ 22 May 2017
 * <J.W.Chia@tudelft.nl>
 *
 * RS485: a library to provide full hardware-driven RS485 functionality
 * to the TI MSP432 family of microcontrollers. It is possible to use
 * this library in Energia (the Arduino port for MSP microcontrollers)
 * or in other toolchains.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License 
 * version 3, both as published by the Free Software Foundation.
 *
 */
 
#include <RS485.h> 

/* A reference list of RS485 RS485_instancess */
RS485 * RS485_instancess[4];		// pointer to the instantiated	RS485 classes

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
			RS485_instancess[M]->_handleReceive(); \
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
  
  
/**** CONSTRUCTORS Default ****/
RS485::RS485(CRC16CCITT &val): crc(val) 
{	//MSP432 launchpad used EUSCI_A2_BASE as default 
	this->module = EUSCI_A2_BASE;
	RS485_instancess[2] = this;
}

/**** CONSTRUCTORS User Defined****/
RS485::RS485(CRC16CCITT &val, uint8_t mod): crc(val) 
{
	 switch (mod) 
	{	   
		case 0:
			this->module = EUSCI_A0_BASE;
			RS485_instancess[0] = this;
			break;

		case 1:
			this->module = EUSCI_A1_BASE;
			RS485_instancess[1] = this;
			break;
			
		case 2:
			this->module = EUSCI_A2_BASE;
			RS485_instancess[2] = this;
			break;
			
		case 3:
			this->module = EUSCI_A3_BASE;
			RS485_instancess[3] = this;
			break;
	}	
}

/**** DESTRUCTORS Reset the module ****/
RS485::~RS485() 
{
	MAP_UART_disableModule(this->module);
	MAP_UART_unregisterInterrupt(this->module);
	
		/* Deregister from the moduleMap */
	switch (module) 
	{
		case EUSCI_A0_BASE:
			RS485_instancess[0] = 0;
			break;
			
		case EUSCI_A1_BASE:
			RS485_instancess[1] = 0;
			break;
			
		case EUSCI_A2_BASE:
			RS485_instancess[2] = 0;
			
			break;
		case EUSCI_A3_BASE:
			RS485_instancess[3] = 0;
			break;
	}
}

/**** Begin RS485 ****/
void RS485::begin(unsigned int baudrate)
{	
	MAP_UART_disableModule(this->module);	//disable UART operation for configuration settings

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
void RS485::transmit( uint_fast8_t address, uint8_t * TxBuffer, uint8_t TxBufferSize)
{
	MAP_UART_transmitAddress(this->module, address);
	
	for (int index = 0; index < TxBufferSize; index++) 
	{
		MAP_UART_transmitData(this->module, TxBuffer[index]);
	}
}

/**** Function to validate received address 
 * Parameter:
 * uint_fast8_t address: receiver address
 * Return:
 * 1 - Pass
 * 0 - Fail 
 ****/
uint8_t RS485::validateAddress( uint_fast8_t address)
{		
	if (MAP_UART_receiveData(this->module) == address)
	{
		MAP_UART_resetDormant(this->module);	//for data to be transferred to RXBUF
		MAP_UART_disableInterrupt( this->module, EUSCI_A_UART_RECEIVE_INTERRUPT );
		return 1;
	}
	else
	{
		return 0;
	}
}

/**** RX 
 * Parameter:
 * uint8_t * RxBuffer: Receive buffer pointer
 * uint8_t RxBufferSize: Size of receive buffer
 * Return:
 * uint8_t 1:CRC passed, data is moved in to RxBuffer
 * 		   0:CRC failed
****/
uint8_t RS485::receive(uint8_t * RxBuffer, uint8_t RxBufferSize)
{	
	uint8_t temp[RxBufferSize];
	uint16_t val = 0xFFFF;
	crc.init();
	
	for (int index = 0; index < RxBufferSize; index++) 
	{
		temp[index] = MAP_UART_receiveData(this->module);
	}
		
	//CRC check
	for (int index = 0; index < RxBufferSize; index++)
	{
		crc.newChar(temp[index]);
		val = softwareCRC(val, temp[index], 0x1021);
	}
	
	if (val == crc.getCRC())
	{	
		for (int index = 0; index < RxBufferSize; index++)
		{
			RxBuffer[index] = temp[index];
		}
		
		MAP_UART_setDormant(this->module);
		MAP_UART_enableInterrupt( this->module, EUSCI_A_UART_RECEIVE_INTERRUPT );
		return 1;
	}
	else
	{
		MAP_UART_setDormant(this->module);
		MAP_UART_enableInterrupt( this->module, EUSCI_A_UART_RECEIVE_INTERRUPT );
		return 0;
	}
}

/**** RX Interrupt Handler ****/
void RS485::onReceive( void (*islHandle)(void) ) 
{
	user_onReceive = islHandle;	//parse handler function
	
	if ( islHandle )
	{
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
uint16_t RS485::softwareCRC (uint16_t crc1, uint8_t data, uint16_t poly)
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
/**** Initialise UART Pin Configuration based on EUSCI used ****/
void RS485::_initMain( void )
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
}

/**
 * Internal process handling the rx, and calling the user's interrupt handles
 */
void RS485::_handleReceive() 
{
	// do something only if there is a handler registered
	if (user_onReceive)
	{
		// call the user-defined data transfer handler
		user_onReceive();
	}
}