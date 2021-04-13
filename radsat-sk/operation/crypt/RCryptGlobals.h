#ifndef RCRYPT_GLOBALS_H_
#define RCRYPT_GLOBALS_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "Raes.h"
#include "RPkcs7Padding.h"

/** A 128 bit key **/
extern uint8_t key[];


/** A 128 bit IV **/
extern uint8_t iv[];

/** The modulus that the pkcs padding function will use to determine the amount of padding to add **/
#define MODULUS ((uint8_t)16)

#endif /* RCRYPT_GLOBALS_H_ */
