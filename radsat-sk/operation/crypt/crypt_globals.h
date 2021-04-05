#ifndef CRYPT_GLOBALS_H_
#define CRYPT_GLOBALS_H_

#include <stdio.h>
#include <stdint.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "aes.h"
#include "pkcs7_padding.h"

/* A 128 bit key */
extern uint8_t key[];


/* A 128 bit IV */
extern uint8_t iv[];

#define MODULUS 16  

#endif /* CRYPT_GLOBALS_H_ */
