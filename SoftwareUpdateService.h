/*
 * SoftwareUpdateService.h
 *
 *  Created on: 27 Jul 2019
 *      Author: stefanosperett
 */

#ifndef SOFTWAREUPDATESERVICE_H_
#define SOFTWAREUPDATESERVICE_H_

#include "PQ9Bus.h"
#include "PQ9Frame.h"
#include "Service.h"
#include "DSerial.h"

#define SOFTWAREUPDATE_SERVICE            18

class SoftwareUpdateService: public Service
{
 public:
     virtual bool process( PQ9Frame &command, PQ9Bus &interface, PQ9Frame &workingBbuffer );

};

#endif /* SOFTWAREUPDATESERVICE_H_ */
