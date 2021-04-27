/**
 * @file RCrypt.h
 * @date April 26 2021
 * @author Thomas Slotboom (ths943)
 */

#ifndef RCRYPT_H_
#define RCRYPT_H_

#include <stdint.h>
#include "aes.h"
#include "pkcs7Padding.h"

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/
#define MODULUS ((uint8_t)16)

uint32_t aesDecrypt(uint8_t* cipherText, uint8_t cipherTextLength);

#endif /* RCRYPT_H_ */
