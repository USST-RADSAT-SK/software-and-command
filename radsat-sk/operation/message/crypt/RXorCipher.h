/**
 * @file RXorCipher.h
 * @date February 9, 2022
 * @author Tyrel Kostyk (tck290)
 */


#ifndef RXORCIPHER_H_
#define RXORCIPHER_H_

#include <stdint.h>


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

void xorDecrypt(uint8_t* buffer, uint8_t size);

#endif /* RXORCIPHER_H_ */
