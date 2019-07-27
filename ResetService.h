/*
 * ResetService.h
 *
 *  Created on: 27 Jul 2019
 *      Author: stefanosperett
 */

#ifndef RESETSERVICE_H_
#define RESETSERVICE_H_

#include "PQ9Bus.h"
#include "PQ9Frame.h"
#include "Service.h"
#include "DSerial.h"

#define RESET_SERVICE                     19

class ResetService: public Service
{
 public:
     virtual bool process( PQ9Frame &command, PQ9Bus &interface, PQ9Frame &workingBbuffer );

};
#endif /* RESETSERVICE_H_ */
