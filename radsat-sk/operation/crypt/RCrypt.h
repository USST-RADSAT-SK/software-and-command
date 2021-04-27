/**
 * @file RCrypt.h
 * @date April 26 2021
 * @author Thomas Slotboom (ths943)
 */

#ifndef RCRYPT_H_
#define RCRYPT_H_
#include <stdint.h>

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/


//int getPaddedBufSize(int plaintextLen);
//void padBuffer(uint8_t* buff, uint8_t* paddedBuff, int buffLen, int newBuffLen);
//void encrypt(uint8_t* plaintext, int plaintextLen);
uint32_t aesDecrypt(uint8_t* cipherText, uint8_t cipherTextLength);

#endif /* RCRYPT_H_ */
