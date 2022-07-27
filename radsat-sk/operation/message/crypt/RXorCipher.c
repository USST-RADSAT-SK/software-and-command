/**
 * @file RXorCipher.c
 * @date February 9, 2022
 * @author Tyrel Kostyk (tck290)
 */

#include <RXorCipher.h>
#include <RKey.h>
#include <hal/errors.h>


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Decrypt a message using the XOR Cipher technique.
 *
 * @param buffer The message to be decrypted. Modified by function.
 * @param size The size of the message in bytes.
 * @return 0 on success, otherwise failure.
 */
int xorDecrypt(uint8_t* buffer, uint8_t size) {

	// ensure that the buffer is not NULL
	if (buffer == 0)
		return E_INPUT_POINTER_NULL;

	// grab the key; fail if key is invalid
	uint8_t key = 1;//privateKey();
	if (key == 0) return -1;

	// decrypt (XOR) every byte
	int newValue = 0;
	for (int i = 0; i < size; i++) {
		newValue = buffer[i] ^ key;
		buffer[i] = newValue;
	}

	return 0;
}

