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

#include "PQ9Frame.h"

#define PQ9Frame()
class PQ9Frame

{
 protected:
     unsigned char buffer[258];

 public:
     unsigned char getDestination();
     void setDestination(unsigned char destination);
     unsigned char getSource();
     void setSource(unsigned char source);
     unsigned char getPayloadSize();
     void setPayloadSize(unsigned char size);
     unsigned char *getPayload();
     void copy(PQ9Frame &destination);
};

#endif /* PQ9FRAME_H_ */
