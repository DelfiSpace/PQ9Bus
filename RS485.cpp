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
  
/**** ISR/IRQ Handles ****/
//void EUSCIA0_IRQHandler( void ) 
//{
//	IRQHANDLER(0);
//}

// void EUSCIA1_IRQHandler( void ) 
// {
	// IRQHANDLER(1);
// }

// void EUSCIA2_IRQHandler( void ) 
// {
	// IRQHANDLER(2);
// }

// void EUSCIA3_IRQHandler( void ) 
// {
	// IRQHANDLER(3);
// }
  
  
/**** CONSTRUCTORS Default ****/
RS485::RS485() 
{	//MSP432 launchpad used EUSCI_A2_BASE as default 
	this->module = EUSCI_A2_BASE;
}

/**** CONSTRUCTORS User Defined****/
RS485::RS485(uint8_t mod)
{
	 switch (mod) 
	{	   
		case 0:
			this->module = EUSCI_A0_BASE;
			break;

		case 1:
			this->module = EUSCI_A1_BASE;
			break;
			
		case 2:
			this->module = EUSCI_A2_BASE;
			break;
			
		case 3:
			this->module = EUSCI_A3_BASE;
			break;
	}	
}

/**** DESTRUCTORS Reset the module ****/
RS485::~RS485() 
{
	MAP_UART_disableModule(this->module);
	MAP_UART_unregisterInterrupt(this->module);
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

/**** RX 
 * Parameter:
 * uint_fast8_t address: receiver address
 * uint8_t * RxBuffer: Receive buffer pointer
 * uint8_t RxBufferSize: Size of receive buffer
****/
void RS485::receive( uint_fast8_t address, uint8_t * RxBuffer, uint8_t RxBufferSize)
{	
	MAP_UART_setDormant(this->module);
	
	if (MAP_UART_receiveData(this->module) == address)
	{
		MAP_UART_resetDormant(this->module);
		
		for (int index = 0; index < RxBufferSize; index++) 
		{
			RxBuffer[index] = MAP_UART_receiveData(this->module);
		}
	}
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
		// MAP_UART_registerInterrupt(this->module, EUSCIA0_IRQHandler);
		
		break;
		
		case EUSCI_A1_BASE:
		
		modulePort = EUSCI_A1_PORT;
		modulePins = EUSCI_A1_PINS;
		
		// transmit / receive interrupt request handler
		// MAP_UART_registerInterrupt(this->module, EUSCIA1_IRQHandler);
		
		break;
		
		case EUSCI_A2_BASE:
		
		modulePort = EUSCI_A2_PORT;
		modulePins = EUSCI_A2_PINS;
		
		// transmit / receive interrupt request handler
		// MAP_UART_registerInterrupt(this->module, EUSCIA2_IRQHandler);
		
		break;
		
		case EUSCI_A3_BASE:
		
		modulePort = EUSCI_A3_PORT;
		modulePins = EUSCI_A3_PINS;
		
		// transmit / receive interrupt request handler
		// MAP_UART_registerInterrupt(this->module, EUSCIA3_IRQHandler);
		
		break;		
	}	
	MAP_GPIO_setAsPeripheralModuleFunctionInputPin(modulePort, modulePins, GPIO_PRIMARY_MODULE_FUNCTION);
}