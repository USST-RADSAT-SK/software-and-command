#ifndef RCRYPT_H_
#define RCRYPT_H_
#include "RCryptGlobals.h"

int getPaddedBufSize(int plaintextLen);
void padBuffer(uint8_t* buff, uint8_t* paddedBuff, int buffLen, int newBuffLen);
void encrypt(uint8_t* plaintext, int plaintextLen);
size_t decrypt(uint8_t* plaintext, int plaintextLen);

#endif /* RCRYPT_H_ */
