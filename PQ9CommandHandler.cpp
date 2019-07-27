/*
 * Copyright (c) 2016 by Stefano Speretta <s.speretta@tudelft.nl>
 *
 * PQ9CommandHandler: a library to handle PQ9 commands.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * version 3, both as published by the Free Software Foundation.
 *
 */

#include "PQ9CommandHandler.h"

extern DSerial serial;

volatile bool dataReceived = false;
PQ9Frame rxBuffer, txBuffer;

void onReceive( PQ9Frame &newFrame )
{
    newFrame.copy(rxBuffer);
    dataReceived = true;
}

PQ9CommandHandler::PQ9CommandHandler(PQ9Bus &interface, Service **servArray, int count): bus(interface)
{
    services = servArray;
    servicesCount = count;
}

void PQ9CommandHandler::init()
{
    bus.setReceiveHandler(&onReceive);
}

void PQ9CommandHandler::commandLoop()
{
    if (dataReceived)
    {
        dataReceived = false;
        if (rxBuffer.getPayloadSize() > 1)
        {
            bool found = false;

            for (int i = 0; i < servicesCount; i++)
            {
                if (services[i]->process(rxBuffer, bus, txBuffer))
                {
                    found = true;
                }
            }

            if (!found)
            {
                serial.print("Unknown Command (");
                serial.print(rxBuffer.getPayload()[0], DEC);
                serial.println(")");
            }
        }
        else
        {
            // invalid payload size
            // what should we do here?
            serial.println("Invalid Command, size must be > 1");
        }
    }
}
