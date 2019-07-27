/*
 * ResetService.cpp
 *
 *  Created on: 27 Jul 2019
 *      Author: stefanosperett
 */


/*
 * SoftwareUpdateService.cpp
 *
 *  Created on: 27 Jul 2019
 *      Author: stefanosperett
 */

#include "ResetService.h"

extern DSerial serial;

bool ResetService::process(PQ9Frame &command, PQ9Bus &interface, PQ9Frame &workingBuffer)
{
    if (command.getPayload()[0] == RESET_SERVICE)
    {
        serial.println("Reset Service");

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

