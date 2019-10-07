/*
 * PingService.h
 *
 *  Created on: 25 Jul 2019
 *      Author: stefanosperett
 */

#ifndef PINGSERVICE_H_
#define PINGSERVICE_H_

#include "PQ9Bus.h"
#include "PQ9Frame.h"
#include "Service.h"

#define PING_SERVICE            17
#define PING_ERROR               0
#define PING_REQUEST             1
#define PING_RESPONSE            2

class PingService: public Service
{
 public:
     virtual bool process( PQ9Frame &command, PQ9Bus &interface, PQ9Frame &workingBbuffer );
};

#endif /* PINGSERVICE_H_ */
