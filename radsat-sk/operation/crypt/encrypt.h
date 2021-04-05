#ifndef ENCRYPT_H_
#define ENCRYPT_H_
#include "crypt_globals.h"

int getPaddedBufSize(int plaintextLen);
void padBuffer(uint8_t* buff, uint8_t* paddedBuff, int buffLen, int newBuffLen);
void encrypt(uint8_t* plaintext, int plaintextLen);

#endif /* ENCRYPT_H_ */
