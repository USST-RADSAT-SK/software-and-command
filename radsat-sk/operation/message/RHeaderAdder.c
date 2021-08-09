/**
 * @file RHeaderAdder.c
 * @date August 7 2021
 * @author Grace Jegede (gkj724)
 */

#include "RHeaderAdder.h"

/**
 * @brief takes an empty buffer, a message, an iv, and the message length. Joins message to iv
 * places them in the buffer
 * @param message
 * @param iv
 * @param messageLength
 * @param newBuffer
 * @return None
 */
void joinTwoStrings(char* message, char* iv, size_t messageLength, char* newBuffer){

    // appends no more than (bufferSize-messageLength) characters of iv to the end of the string in newBuffer
    strncat(newBuffer, iv, BUFFERSIZE-messageLength);

    // appends no more than (bufferSize-strlen(newBuffer)) characters of message to the end of string in newBuffer
    strncat(newBuffer, message, BUFFERSIZE-strlen(newBuffer));
}
