/*
 * PQ9Sender.h
 *
 *  Created on: 1 Jan 2020
 *      Author: stefanosperett
 */

#ifndef _PQ9SENDER_H_
#define _PQ9SENDER_H_

#include "PQ9Frame.h"

class PQ9Sender
{
public:
    virtual void transmit( PQ9Frame &frame ) = 0;
    virtual unsigned char getAddress( void ) = 0;
};

#endif  /* _PQ9SENDER_H_ */
