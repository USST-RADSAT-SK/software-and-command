/**
 * @file RAes.h
 * @date April 26 2021
 * @author Thomas Slotboom (ths943)
 */

#ifndef RAES_H_
#define RAES_H_

#include <stdint.h>
#include <aes.h>
#include <pkcs7Padding.h>

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

/** Modulus size used for padding of encrypted buffers */
#define AES_DECRYPT_MODULUS_SIZE ((uint8_t)16)

uint32_t aesDecrypt(uint8_t* cipherText, uint8_t cipherTextLength);

#endif /* RAES_H_ */
