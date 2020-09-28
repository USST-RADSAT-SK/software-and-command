/**
 * @file	dbgu_int.h
 * @brief	Interrupt based DBGU character reception
 */

#ifndef DBGU_TEST_H_
#define DBGU_TEST_H_

#include <stdio.h>

/**
 *  @brief       Initializes the DBGU for interrupt based reception.
 *  @param[in]   mode Operating mode to configure.
 *  @param[in]   baudrate Desired baudrate (e.g. 115200).
 *  @param[in]   mck Frequency of the system master clock in Hz.
 */
void DBGU_Init( unsigned int mode, unsigned int baudrate, unsigned int mck );

/**
 *  @brief    Checks whether a character has been placed in the reception buffer of the DBGU
 *  @return   1 if a character is available in the reception buffer, 0 otherwise
 */
unsigned int DBGU_IntIsRxReady(void);

/**
 *  @brief    Reads and returns a character from the DBGU
 *  @return   Character received or 0 if none was available
 */
unsigned char DBGU_IntGetChar(void);

#endif /* DBGU_TEST_H_ */
