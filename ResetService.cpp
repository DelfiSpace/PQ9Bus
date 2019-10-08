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

void resetHandler()
{
    // TODO: replace this with a powercycle to protect also the RS485 driver
    // for now, at least reset, till the power cycle gets implemented in HW
    MAP_SysCtl_rebootDevice();
}

ResetService::ResetService(const unsigned long WDport, const unsigned long WDpin) :
        WDIPort(WDport), WDIPin(WDpin) {}

void ResetService::init()
{
    // initialize the internal watch-dog
    MAP_WDT_A_clearTimer();                                  // Clear the watch-dog to prevent spurious triggers
    MAP_WDT_A_initIntervalTimer( WDT_A_CLOCKSOURCE_SMCLK,    // set the watch-dog to trigger every 178s
                                 WDT_A_CLOCKITERATIONS_2G ); // (about 3 minutes)

    // select the interrupt handler
    MAP_WDT_A_registerInterrupt(&resetHandler);

    // start the timer
    MAP_WDT_A_startTimer();
}

void ResetService::refreshConfiguration()
{
    // select the interrupt handler
    MAP_WDT_A_registerInterrupt(&resetHandler);

    // ensure the timer is running: this only forces the
    // timer to run (in case it got disabled for any reason)
    // but it does not reset it, making sure the watch-dog
    // cannot be disabled by mistake
    MAP_WDT_A_startTimer();

    // initialize external watch-dog pins
    MAP_GPIO_setOutputLowOnPin( WDIPort, WDIPin );
    MAP_GPIO_setAsOutputPin( WDIPort, WDIPin );
}

void ResetService::kickInternalWatchDog()
{
    // reset the internal watch-dog timer
    MAP_WDT_A_clearTimer();
}

void ResetService::kickExternalWatchDog()
{
    // toggle the WDI pin of the external watch-dog
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
        workingBuffer.setPayloadSize(3);
        workingBuffer.getPayload()[0] = RESET_SERVICE;

        if ((command.getPayloadSize() == 3) && (command.getPayload()[1] == RESET_REQUEST))
        {
            workingBuffer.getPayload()[2] = command.getPayload()[2];
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

                    // toggle the WDI pin 3 times, just to be sure
                    // the external watch-dog resets...
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
