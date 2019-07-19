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
 #include <CRC16CCITT.h> 
 
/**
 *
 *   CRC16-CCITT class creator function. It uses 0xFFFF as default seed
 *   and does not complement the CRC at the end.
 *   
 *   Parameters:            None
 *
 */
CRC16CCITT::CRC16CCITT() 
{	
	seed = 0xFFFF;
	complemented = false;
}

/**
 *
 *   CRC16-CCITT class creator function with custom seed and the possibility 
 *   to complement the output.
 *   
 *   Parameters:
 *   unsigned short customSeed    seed
 *   bool com                     true to complement the output
 *
 */
CRC16CCITT::CRC16CCITT(unsigned short customSeed, bool com)
{
	seed = customSeed;
	complemented = com;
}

/**
 *
 *   Initialise the CRC state machine: it is used to provide the seed 
 *   at the beginning of the CRC calculation.
 *   
 *   Parameters:                  None          
 *
 */
void CRC16CCITT::init() 
{
	/* Setting the polynomial and feeding in the data */
	MAP_CRC32_setSeed(seed, CRC16_MODE);
}

/**
 *
 *   Provide a new byte for the CRC calculation.
 *   
 *   Parameters:  
 *   unsigned char                newdata new byte to use
 *
 */
void CRC16CCITT::newChar(unsigned char newdata)
{
	MAP_CRC32_set8BitDataReversed(newdata, CRC16_MODE);
}

/**
 *
 *   Provide a new short for the CRC calculation.
 *   
 *   Parameters:  
 *   unsigned short newdata       new short to use
 *
 */
void CRC16CCITT::newShort(unsigned short newdata)
{
	MAP_CRC32_set16BitDataReversed(newdata, CRC16_MODE);
}

/**
 *
 *   Returns the CRC computed so far
 *   
 *   Returns:
 *   unsigned short               CRC result
 *
 */
unsigned short CRC16CCITT::getCRC()
{
	if ( complemented )
	{
		return MAP_CRC32_getResult( CRC16_MODE ) ^ 0xFFFF;
	}
	else
	{
		return MAP_CRC32_getResult( CRC16_MODE );
	}
}
