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

#ifndef RS485_H
#define RS485_H

#include <driverlib.h>
#include <CRC16CCITT.h>

/* Device specific includes */
#include <inc/rs485_msp432p401r.h>


class RS485
{
protected:
	CRC16CCITT &crc;
	
private: 
	/* MSP specific modules */
	uint32_t module;
	uint8_t modulePort;
	uint16_t modulePins;
	
	/* Internal states */
	void (*user_onReceive)( void );
	
	void _initMain( void ); 
	
	/* stub functions to handle interrupts */
	void _handleReceive();
	
	/* Interrupt handlers: they are declared as friends to be accessible from outside 
	the class (the interrupt engine) but have access to member functions */
	friend void EUSCIA0_IRQHandler( void );
	friend void EUSCIA1_IRQHandler( void );
	friend void EUSCIA2_IRQHandler( void );
	friend void EUSCIA3_IRQHandler( void );

public:
	RS485(CRC16CCITT &val);
	RS485(CRC16CCITT &val, uint8_t mod);
	~RS485();
	
	void begin(unsigned int baudrate);
	void transmit( uint_fast8_t address, uint8_t * TxBuffer, uint8_t TxBufferSize);
	uint8_t validateAddress( uint_fast8_t address);
	uint8_t receive(uint8_t * RxBuffer, uint8_t RxBufferSize);
	void onReceive( void (*islHandle)(void) );
	uint16_t softwareCRC (uint16_t crc1, uint8_t data, uint16_t poly);
};

#endif	/* RS485_H_ */