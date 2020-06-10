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
          this->getBuffer()[1] = resp;
      };
      virtual unsigned char getMessageType(){
          return this->getBuffer()[1];
      };

      virtual void setService(unsigned char address){
          this->getBuffer()[0] = address;
      };
      virtual unsigned char getService(){
          return this->getBuffer()[0];
      };

      virtual unsigned char* getDataPayload(){
          return &(this->getBuffer()[2]);
      };
      virtual void setPayloadSize(unsigned int size){
          this->setSize(2+size);
      };
      virtual unsigned int getPayloadSize(){
          return this->getSize() - 2;
      };
};

#endif /* PQ9MESSAGE_H_ */
