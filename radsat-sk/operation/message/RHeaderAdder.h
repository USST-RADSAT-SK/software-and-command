/**
 * @file RHeaderAdder.h
 * @date August 7 2021
 * @author Grace Jegede (gkj724)
 */

#ifndef __RHEADERADDER_H__
#define __RHEADERADDER_H__

#include <string.h>
/*************************************************************************
                      DEFINITIONS
*************************************************************************/

/** Size of Buffer **/
#define BUFFERSIZE 30

void joinTwoStrings(char* message, char* iv, size_t messageLength, char* newBuffer);

#endif /* __RHEADERADDER_H__ */
