#include "decrypt.h"


size_t decrypt(uint8_t* ciphertext, int ciphertextlen);

/**
 * @brief Encrypts a buffer in with an AES CBC cipher
 * @header "comms-processing/crypt/encrypt.h"
 * @param plaintext - plaintext value to be decrypt
 * @param plaintextLen - The length of the plaintext value
 * @return The actual size of the data
 */
size_t decrypt(uint8_t* ciphertext, int ciphertextlen){
	struct AES_ctx ctx;
	AES_init_ctx_iv(&ctx, key, iv);
	AES_ctx_set_iv(&ctx,iv);
	AES_CBC_decrypt_buffer(&ctx, ciphertext, ciphertextlen);
	size_t actualDataLength = pkcs7_padding_data_length(ciphertext, ciphertextlen, MODULUS);
	return actualDataLength;
}

