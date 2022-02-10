/**
 * @file RXorCipher.c
 * @date February 9, 2022
 * @author Tyrel Kostyk (tck290)
 */

#include <RXorCipher.h>
#include <RFram.h>


/***************************************************************************************************
                                   DEFINITIONS & PRIVATE GLOBALS
***************************************************************************************************/

/** The hardcoded address of the XOR Cipher Key in FRAM memory. */
#define XOR_CIPHER_KEY_ADDR		((uint32_t)0x1001AABB)	// TODO: set to real address location


/** The 1-byte private key used for the XOR Cipher operations. */
static uint8_t key = 0;


/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS
***************************************************************************************************/

uint8_t privateKey(void);

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Decrypt a message using the XOR Cipher technique.
 *
 * @param buffer The message to be decrypted. Modified by function.
 * @param size The size of the message in bytes.
 */
void xorDecrypt(uint8_t* buffer, uint8_t size) {

	// ensure that the buffer is not NULL
	if (buffer == 0)
		return;

	// grab the key
	uint8_t key = privateKey();

	// decrypt (XOR) every byte
	int newValue = 0;
	for (int i = 0; i < size; i++) {
		newValue = buffer[i] ^ key;
		buffer[i] = newValue;
	}
}


/***************************************************************************************************
                                         PRIVATE FUNCTIONS
***************************************************************************************************/

uint8_t privateKey(void) {

	// obtain the key from memory
	if (key == 0)
		framRead(&key, XOR_CIPHER_KEY_ADDR, sizeof(key));

	return key;
}

