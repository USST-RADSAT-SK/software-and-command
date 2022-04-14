/**
 * @file RErrors.h
 * @date March 5th, 2022
 * @author Matthew Buglass (mab839)
 */

#ifndef RERRORS_H_
#define RERRORS_H_

#include <stdint.h>
#include <hal/errors.h>
#include <RMessage.h>

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/



/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

uint8_t reportError(int8_t errorCode, uint8_t system, char* errorMsg, uint8_t logError);


#endif /* RERRORS_H_ */
