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

ResetService::ResetService(const unsigned long WDport, const unsigned long WDpin)
{
    WDIPort = WDport;
    WDIPin = WDpin;
}

void ResetService::init()
{
    MAP_GPIO_setOutputLowOnPin( WDIPort, WDIPin );
    MAP_GPIO_setAsOutputPin( WDIPort, WDIPin );
}

void ResetService::kickHardwareWatchDog()
{
    MAP_GPIO_setOutputHighOnPin( WDIPort, WDIPin );
    MAP_GPIO_setOutputLowOnPin( WDIPort, WDIPin );
}

bool ResetService::process(PQ9Frame &command, PQ9Bus &interface, PQ9Frame &workingBuffer)
{
    if (command.getPayload()[0] == RESET_SERVICE)
    {
        // prepare response frame
        workingBuffer.setDestination(command.getSource());
        workingBuffer.setSource(interface.getAddress());
        workingBuffer.setPayloadSize(2);
        workingBuffer.getPayload()[0] = RESET_SERVICE;

        if ((command.getPayloadSize() == 3) && (command.getPayload()[1] == RESET_REQUEST))
        {
            switch(command.getPayload()[2])
            {
                case RESET_SOFT:
                    workingBuffer.getPayload()[1] = RESET_RESPONSE;
                    // send response: doing it here to make sure
                    // a response is sent before reset but not 2
                    interface.transmit(workingBuffer);

                    //MAP_WDT_A_setPasswordViolationReset(WDT_A_HARD_RESET);
                    MAP_SysCtl_rebootDevice();
                    break;

                case RESET_HARD:
                    workingBuffer.getPayload()[1] = RESET_RESPONSE;
                    // send response: doing it here to make sure
                    // a response is sent before reset but not 2
                    interface.transmit(workingBuffer);

                    // toggle the WDI pin 3 times, just to be sure...
                    MAP_GPIO_setOutputHighOnPin( WDIPort, WDIPin );
                    MAP_GPIO_setOutputLowOnPin( WDIPort, WDIPin );
                    MAP_GPIO_setOutputHighOnPin( WDIPort, WDIPin );
                    MAP_GPIO_setOutputLowOnPin( WDIPort, WDIPin );
                    MAP_GPIO_setOutputHighOnPin( WDIPort, WDIPin );
                    MAP_GPIO_setOutputLowOnPin( WDIPort, WDIPin );
                    break;

                    // not implemented yet, give error to notify it
                /*case RESET_POWERCYCLE:
                    serial.println("POWERCYCLE");
                    workingBuffer.getPayload()[1] = RESET_RESPONSE;
                    // send response: doing it here to make sure
                    // a response is sent before reset but not 2
                    interface.transmit(workingBuffer);
                    break;*/

                default:
                    workingBuffer.getPayload()[1] = RESET_ERROR;
                    // send response: doing it here to make sure
                    // a response is sent before reset but not 2
                    interface.transmit(workingBuffer);
                    break;
            }
        }
        else
        {
            // unknown request
            workingBuffer.getPayload()[1] = RESET_ERROR;
            // send response: doing it here to make sure
            // a response is sent before reset but not 2
            interface.transmit(workingBuffer);
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
