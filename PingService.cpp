/*
 * PingService.cpp
 *
 *  Created on: 25 Jul 2019
 *      Author: stefanosperett
 */
#include "PingService.h"

extern DSerial serial;

bool PingService::process(PQ9Frame &command, PQ9Bus &interface, PQ9Frame &workingBuffer)
{
    if (command.getPayload()[0] == PING_SERVICE)
    {
        serial.println("PingService");
        if (command.getPayload()[1] == PING_REQUEST)
        {
            workingBuffer.setDestination(command.getSource());
            workingBuffer.setSource(interface.getAddress());
            workingBuffer.setPayloadSize(2);
            workingBuffer.getPayload()[0] = PING_SERVICE;
            workingBuffer.getPayload()[1] = PING_RESPONSE;
            interface.transmit(workingBuffer);
        }
        else
        {
            // TODO: how to handle this error?
            serial.print("Unexpected Ping subtype (");
            serial.print(command.getPayload()[1], DEC);
            serial.println(")");
        }
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
