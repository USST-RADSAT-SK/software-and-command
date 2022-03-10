/**
 * @file RMessageOutput.h
 * @date March 10, 2022
 * @author Matthew Buglass (mab839)
 */

#ifndef RMESSAGEOUPUT_H_
#define RMESSAGEOUPUT_H_

#include <stdint.h>


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

void printAll(void);
void printAck(void);
void printNack(void);
void printFileTransferMessages(void);

#endif /* RMESSAGEOUPUT_H_ */
