/*
 * Copyright (c) 2016 by Stefano Speretta <s.speretta@tudelft.nl>
 *
 * PQ9Frame: a library to handle PQ9 frames.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * version 3, both as published by the Free Software Foundation.
 *
 */

#include "PQ9Frame.h"

unsigned char PQ9Frame::getDestination()
{
    return buffer[0];
}

void PQ9Frame::setDestination(unsigned char destination)
{
    buffer[0] = destination;
}

unsigned char PQ9Frame::getSource()
{
    return buffer[2];
}

void PQ9Frame::setSource(unsigned char source)
{
    buffer[2] = source;
}

unsigned char PQ9Frame::getPayloadSize()
{
    return buffer[1];
}

void PQ9Frame::setPayloadSize(unsigned char size)
{
    buffer[1] = size;
}

unsigned char *PQ9Frame::getPayload()
{
    return &(buffer[3]);
}

void PQ9Frame::copy(DataFrame &destination)
{
    destination.setDestination(getDestination());
    destination.setSource(getSource());
    destination.setPayloadSize(getPayloadSize());
    for (int i = 0; i < getPayloadSize(); i++)
    {
        destination.getPayload()[i] = getPayload()[i];
    }
}

void PQ9Frame::PrepareTransmit(){
    crc.init();
    crc.newChar(this->getDestination());
    crc.newChar(this->getPayloadSize());
    crc.newChar(this->getSource());
    for (int i = 0; i < this->getPayloadSize(); i++)
    {
        crc.newChar(this->getPayload()[i]);
    }
    unsigned short computedCRC = crc.getCRC();
    buffer[3 + this->getPayloadSize()] = (*((unsigned char*)  &(computedCRC)+1));
    buffer[3 + this->getPayloadSize() + 1] = (*((unsigned char*)  &(computedCRC)+0));
    this->frameSize = this->getPayloadSize() + 5;
}

unsigned int PQ9Frame::getFrameSize(){
    return frameSize;
}

unsigned char* PQ9Frame::getFrame(){
    return &buffer[0];
}
