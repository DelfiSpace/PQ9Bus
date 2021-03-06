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

#ifndef PQ9Bus_H
#define PQ9Bus_H

#include "DataBus.h"
#include <driverlib.h>
#include "CRC16CCITT.h"
#include "PQ9Frame.h"

/* Device specific includes */
#include "inc/msp432p401r.h"

enum InternalState { WaitForAddress, Size, Source, PayloadByte, CRC };

class PQ9Bus : public DataBus
{
protected:
	

private: 
	/* MSP specific modules */
	uint32_t module;
	CRC16CCITT crc;
	unsigned long TXEnablePort;
	unsigned long TXEnablePin;
	uint8_t modulePort;
	uint8_t address;
	uint8_t frameSizeCounter;
	uint16_t modulePins;
	unsigned int baudrate;
	unsigned char status;
	InternalState state;
    PQ9Frame rxFrame;
    unsigned char rxCRC1;

	/* Internal states */
	void (*user_onReceive)( DataFrame & );

	void _initMain( void ); 
	
	/* stub functions to handle interrupts */
	void _handleReceive( PQ9Frame &newFrame );
	
    uint16_t softwareCRC ( uint16_t crc1, uint8_t data, uint16_t poly );

	/* Interrupt handlers: they are declared as friends to be accessible from outside 
	the class (the interrupt engine) but have access to member functions */
	friend void EUSCIA0_IRQHandler( void );
	friend void EUSCIA1_IRQHandler( void );
	friend void EUSCIA2_IRQHandler( void );
	friend void EUSCIA3_IRQHandler( void );

public:
	PQ9Bus(uint8_t mod, unsigned long port, unsigned long pin);
	~PQ9Bus();
	
	void begin(unsigned int baudrate, uint8_t address);
	void transmit( DataFrame &frame );
	void setReceiveHandler( void (*islHandle)( DataFrame & ) );
	virtual uint8_t getAddress( void );
};

#endif	/* PQ9Bus_H_ */
