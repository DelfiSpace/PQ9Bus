/*
 * TelemetryContainer.h
 *
 *  Created on: 4 Aug 2019
 *      Author: stefanosperett
 */

#ifndef TELEMETRYCONTAINER_H_
#define TELEMETRYCONTAINER_H_

class TelemetryContainer
{
 protected:

 public:
    virtual ~TelemetryContainer( ) {};
    virtual int size() = 0;
    virtual unsigned char * getArray() = 0;
};
#endif /* TELEMETRYCONTAINER_H_ */
