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

void sendAll(void);
void sendAck(void);
void sendNack(void);
void sendFileTransferMessages(void);

#endif /* RMESSAGEOUPUT_H_ */
