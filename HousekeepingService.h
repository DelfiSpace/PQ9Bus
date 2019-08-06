/*
 * TelemetryService.h
 *
 *  Created on: 4 Aug 2019
 *      Author: stefanosperett
 */

#ifndef HOUSEKEEPINGSERVICE_H_
#define HOUSEKEEPINGSERVICE_H_

#include "PQ9Bus.h"
#include "PQ9Frame.h"
#include "Service.h"
#include "TelemetryContainer.h"
#include "DSerial.h"

#define HOUSEKEEPING_SERVICE             3
#define HOUSEKEEPING_ERROR               0
#define HOUSEKEEPING_REQUEST             1
#define HOUSEKEEPING_RESPONSE            2

class HousekeepingService: public Service
{
protected:
    int telemetryIndex = 0;

 public:
    bool process( PQ9Frame &command, PQ9Bus &interface, PQ9Frame &workingBbuffer );
    void stageTelemetry();
    virtual TelemetryContainer* getContainerToRead();
    virtual TelemetryContainer* getContainerToWrite();
};

#endif /* HOUSEKEEPINGSERVICE_H_ */
