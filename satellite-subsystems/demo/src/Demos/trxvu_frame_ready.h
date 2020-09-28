/*
 * trxvu_frame_ready.h
 *
 *  Created on: 18 mei 2018
 *      Author: PBOT
 */

#ifndef TRXVU_FRAME_READY_H_
#define TRXVU_FRAME_READY_H_

typedef void (*TRXVU_FR_Callback_t)( void );

void TRXVU_FR_Enable(TRXVU_FR_Callback_t callback);
void TRXVU_FR_Disable( void );
int TRXVU_FR_IsReady( void );

#endif /* TRXVU_FRAME_READY_H_ */
