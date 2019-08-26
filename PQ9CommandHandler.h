/*
 * PQ9CommandHandler.h
 *
 *  Created on: 24 Jul 2019
 *      Author: stefanosperett
 */

#ifndef PQ9COMMANDHANDLER_H_
#define PQ9COMMANDHANDLER_H_

#include "PQ9Frame.h"
#include "PQ9Bus.h"
#include "Service.h"
#include "PingService.h"
#include "DSerial.h"

class PQ9CommandHandler

{
 protected:
     PQ9Bus &bus;
     Service** services;
     const int servicesCount;


 public:
     PQ9CommandHandler( PQ9Bus &bus, Service **servArray, int count );
     void init();
     bool commandLoop();


};

#endif /* PQ9COMMANDHANDLER_H_ */
