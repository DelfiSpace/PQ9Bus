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

#ifndef PQ9FRAME_H_
#define PQ9FRAME_H_

#include "DataFrame.h"
#include "CRC16CCITT.h"

class PQ9Frame : public DataFrame

{
 protected:
     unsigned char buffer[260];
     CRC16CCITT crc;

     unsigned int frameSize;

 public:
     unsigned char getDestination();
     void setDestination(unsigned char destination);
     unsigned char getSource();
     void setSource(unsigned char source);
     unsigned char getPayloadSize();
     void setPayloadSize(unsigned char size);
     unsigned char *getPayload();
     void copy(DataFrame &destination);

     virtual void PrepareTransmit();
     unsigned int getFrameSize();
     virtual unsigned char *getFrame();
};

#endif /* PQ9FRAME_H_ */
