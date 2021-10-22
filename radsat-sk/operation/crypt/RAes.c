/**
 * @file RAes.c
 * @date April 26 2021
 * @author Thomas Slotboom (ths943)
 */

#include "RAes.h"

/***************************************************************************************************
                                          GLOBAL VARIABLES
***************************************************************************************************/

/** A 128 bit key, in the future we will pull this from fram */
static uint8_t privateKey[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
     0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * @brief Decrypts a buffer in with an AES CBC cipher
 * @param cipherText plaintext value to be decrypted
 * @param cipherTextLength The length of the plaintext value
 * @return The actual size of the decrypted data
 */
uint32_t aesDecrypt(uint8_t* cipherText, uint32_t cipherTextLength, uint8_t* initializationVector) {
	// sets up the context for decryption and initiailizes the initialization vector
	struct AES_ctx context;
	// at some point we will have to pull the key from memory
	AES_init_ctx_iv(&context, privateKey, initializationVector);

	AES_CBC_decrypt_buffer(&context, cipherText, cipherTextLength);
	uint32_t actualDataLength = (uint32_t)pkcs7_padding_data_length(cipherText, cipherTextLength, AES_DECRYPT_MODULUS_SIZE);
	return actualDataLength;
}
