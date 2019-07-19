/*
 * Copyright (c) 2017 by Stefano Speretta <s.speretta@tudelft.nl>
 *
 * CRC16-CCITT: a library to implement the CRC16-CCITT algorithm
 * on MSP432 microcontrollers.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License 
 * version 3, both as published by the Free Software Foundation.
 *
 */
 
#ifndef CRC16CCITT_H
#define CRC16CCITT_H

#include <driverlib.h>

class CRC16CCITT
{
private: 
	unsigned short seed;
	bool complemented;
	
public:
	CRC16CCITT();
	CRC16CCITT(unsigned short, bool);
	void init();
	void newChar(unsigned char);
	void newShort(unsigned short);
	unsigned short getCRC();
	
protected:

};

#endif	/* CRC16CCITT_H_ */
