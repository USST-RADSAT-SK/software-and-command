/*
 * RCommunicationStateMachine.h
 *
 *  Created on: Aug 20, 2022
 *      Author: bpitz
 */

#ifndef RCOMMUNICATIONSTATEMACHINE_H_
#define RCOMMUNICATIONSTATEMACHINE_H_

void ackReceived();
void nackReceived();
void beginPass();
void beginFileTransfer();
void ceaseTransmission();
void resumeTransmission();
int updateTime(unsigned int epochTime);
void resetSat();
void sendNack();
void sendAck();
uint8_t getNextFrame(uint8_t*);


#endif /* RCOMMUNICATIONSTATEMACHINE_H_ */
