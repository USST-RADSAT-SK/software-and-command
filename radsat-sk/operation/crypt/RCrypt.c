#include "RCrypt.h"

int getPaddedBufSize(int plaintextLen);
void padBuffer(uint8_t* buff, uint8_t* paddedBuff, int buffLen, int newBuffLen);
void encrypt(uint8_t* plaintext, int plaintextLen);
size_t decrypt(uint8_t* ciphertext, int ciphertextlen);



/**
 * @brief Gets the next multiple of 16 from the size of the input buffer
 * @header "operations/crypt/encrypt.h"
 * @param plaintextLen - The size of the unpadded buffer containing data to be encrypted.
 * @pre need to have a variable storing the size of the plaintext buffer
 * @return The size of the new buffer to be created
 */
int getPaddedBufSize(int plaintextLen){
    // Special case for when the buffer size is already a multiple of 16:
    if (plaintextLen % MODULUS == 0){
        return plaintextLen + MODULUS; //If no padding is to be added, pad with 16 bytes
    }
	return plaintextLen + (MODULUS - (plaintextLen % MODULUS));
}

/**
 * @brief Pads a buffer to be a multiple of 16 bytes
 * @header "operations/crypt/encrypt.h"
 * @param buff - The incoming buffer that will be padded
 * @param paddedBuff - The outgoing buffer that will contain the padded data
 * @param buffLen - The length of the incoming buffer
 * @param newBuffLen - The length that the outgoing buffer will be   
 * @pre Need to know the size of the old and new buffers before calling the function
 * @return Null
 */
void padBuffer(uint8_t* buff, uint8_t* paddedBuff, int buffLen, int newBuffLen){
	memcpy(paddedBuff, buff, buffLen);
	pkcs7_padding_pad_buffer(paddedBuff, buffLen, newBuffLen, MODULUS);
}

/**
 * @brief Encrypts a buffer in with an AES CBC cipher
 * @header "operations/crypt/encrypt.h"
 * @param plaintext - plaintext value to be encrypted
 * @param plaintextLen - The length of the plaintext value
 * @return Null
 */
void encrypt(uint8_t* plaintext, int plaintextLen){
    struct AES_ctx ctx;
    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_encrypt_buffer(&ctx, plaintext, plaintextLen);
}

/**
 * @brief Encrypts a buffer in with an AES CBC cipher
 * @header "operations/crypt/encrypt.h"
 * @param plaintext - plaintext value to be decrypt
 * @param plaintextLen - The length of the plaintext value
 * @return The actual size of the data
 */
size_t decrypt(uint8_t* ciphertext, int ciphertextlen){
	struct AES_ctx ctx;
	AES_init_ctx_iv(&ctx, key, iv);
	AES_CBC_decrypt_buffer(&ctx, ciphertext, ciphertextlen);
	size_t actualDataLength = pkcs7_padding_data_length(ciphertext, ciphertextlen, MODULUS);
	return actualDataLength;
}
