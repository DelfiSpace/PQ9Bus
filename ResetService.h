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

#define RESET_SERVICE           19
#define RESET_ERROR              0
#define RESET_REQUEST            1
#define RESET_RESPONSE           2

#define RESET_SOFT               1
#define RESET_HARD               2
#define RESET_POWERCYCLE         3

class ResetService: public Service
{
 protected:
     const unsigned long WDIPort;
     const unsigned long WDIPin;

 public:
     ResetService( const unsigned long port, const unsigned long pin );
     virtual bool process( PQ9Frame &command, PQ9Bus &interface, PQ9Frame &workingBbuffer );
     void init();
     void refreshConfiguration();
     void kickExternalWatchDog();
     void kickInternalWatchDog();

};

#endif /* RESETSERVICE_H_ */
