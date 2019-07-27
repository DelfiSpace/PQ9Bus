/*
 * SoftwareUpdateService.cpp
 *
 *  Created on: 27 Jul 2019
 *      Author: stefanosperett
 */

#include "SoftwareUpdateService.h"

extern DSerial serial;

bool SoftwareUpdateService::process(PQ9Frame &command, PQ9Bus &interface, PQ9Frame &workingBuffer)
{

    if (command.getPayload()[0] == SOFTWAREUPDATE_SERVICE)
    {
        serial.println("SoftwareUpdate Service");

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
