/*
 * PingService.cpp
 *
 *  Created on: 25 Jul 2019
 *      Author: stefanosperett
 */
#include "PingService.h"

bool PingService::process(PQ9Frame &command, PQ9Bus &interface, PQ9Frame &workingBuffer)
{
    if (command.getPayload()[0] == PING_SERVICE)
    {
        // prepare response frame
        workingBuffer.setDestination(command.getSource());
        workingBuffer.setSource(interface.getAddress());
        workingBuffer.setPayloadSize(2);
        workingBuffer.getPayload()[0] = PING_SERVICE;

        if (command.getPayload()[1] == PING_REQUEST)
        {
            // respond to ping
            workingBuffer.getPayload()[1] = PING_RESPONSE;
        }
        else
        {
            // unknown request
            workingBuffer.getPayload()[1] = PING_ERROR;
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
