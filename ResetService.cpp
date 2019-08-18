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
    // init the internal watchdog
    MAP_WDT_A_clearTimer();                                  // Clear the watchdog to prevent spurious triggers
    MAP_WDT_A_initIntervalTimer( WDT_A_CLOCKSOURCE_SMCLK,    // set the watchdog to trigger every 178s
                                 WDT_A_CLOCKITERATIONS_2G ); // (about 3 minutes)

    // init external watchdog pins
    MAP_GPIO_setOutputLowOnPin( WDIPort, WDIPin );
    MAP_GPIO_setAsOutputPin( WDIPort, WDIPin );
}

void ResetService::kickInternalWatchDog()
{
    MAP_WDT_A_clearTimer();
}

void ResetService::kickExternalWatchDog()
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

                    // now reset the MCU
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
