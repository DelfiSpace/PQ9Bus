/*
 * PQ9Message.h
 *
 *  Created on: Apr 22, 2020
 *      Author: Casper
 */

#ifndef PQ9MESSAGE_H_
#define PQ9MESSAGE_H_

#include "DataMessage.h"

class PQ9Message : public DataMessage

{
 protected:


 public:
      virtual void setMessageType(unsigned char resp){
          this->getPayload()[1] = 0;
      };
      virtual unsigned char getMessageType(){
          return this->getPayload()[1];
      };

      virtual void setService(unsigned char address){
          this->getPayload()[0] = 0;
      };
      virtual unsigned char getService(){
          return this->getPayload()[0];
      };

      virtual unsigned char* getDataPayload(){
          return &(this->getPayload()[2]);
      };
};

#endif /* PQ9MESSAGE_H_ */
