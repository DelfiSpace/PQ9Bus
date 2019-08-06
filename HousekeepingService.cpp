/*
 * TelemetryService.cpp
 *
 *  Created on: 4 Aug 2019
 *      Author: stefanosperett
 */

#include "HousekeepingService.h"

extern DSerial serial;

bool HousekeepingService::process(PQ9Frame &command, PQ9Bus &interface, PQ9Frame &workingBuffer)
{
    if (command.getPayload()[0] == HOUSEKEEPING_SERVICE)
    {
        // prepare response frame
        workingBuffer.setDestination(command.getSource());
        workingBuffer.setSource(interface.getAddress());
        workingBuffer.getPayload()[0] = HOUSEKEEPING_SERVICE;

        if (command.getPayload()[1] == HOUSEKEEPING_REQUEST)
        {
            // respond to housekeeping request
            workingBuffer.getPayload()[1] = HOUSEKEEPING_RESPONSE;
            for (int i = 0; i < getContainerToRead()->size(); i++)
            {
                workingBuffer.getPayload()[i + 2] = getContainerToRead()->getArray()[i];
            }
            workingBuffer.setPayloadSize(2 + getContainerToRead()->size());
        }
        else
        {
            // unknown request
            workingBuffer.getPayload()[1] = HOUSEKEEPING_ERROR;
            workingBuffer.setPayloadSize(2);
        }

        // send response
        interface.transmit(workingBuffer);
        // command processed
        return true;
    }
    else
    {
        // this command is related to another service,
        // report the command was not processed
        return false;
    }
}

void HousekeepingService::stageTelemetry()
{
    telemetryIndex++;
    telemetryIndex %= 2;
}
