/**
 * @file RCrypt.c
 * @date April 26 2021
 * @author Thomas Slotboom (ths943)
 */

#include "RCrypt.h"

/***************************************************************************************************
											GLOBAL VARIABLES
***************************************************************************************************/

/** A 128 bit key, in the future we will pull this from fram */
static uint8_t privateKey[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
     0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };


/** A 128 bit IV, in the future we will pull this from fram */
static uint8_t intializationVector[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
     0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };



///**
// * @brief Gets the next multiple of 16 from the size of the input buffer
// * @header "operations/crypt/encrypt.h"
// * @param plaintextLen - The size of the unpadded buffer containing data to be encrypted.
// * @pre need to have a variable storing the size of the plaintext buffer
// * @return The size of the new buffer to be created
// */
//int getPaddedBufSize(int plaintextLen){
//    // Special case for when the buffer size is already a multiple of 16:
//    if (plaintextLen % MODULUS == 0){
//        return plaintextLen + MODULUS; //If no padding is to be added, pad with 16 bytes
//    }
//	return plaintextLen + (MODULUS - (plaintextLen % MODULUS));
//}
//
///**
// * @brief Pads a buffer to be a multiple of 16 bytes
// * @header "operations/crypt/encrypt.h"
// * @param buff - The incoming buffer that will be padded
// * @param paddedBuff - The outgoing buffer that will contain the padded data
// * @param buffLen - The length of the incoming buffer
// * @param newBuffLen - The length that the outgoing buffer will be
// * @pre Need to know the size of the old and new buffers before calling the function
// * @return Null
// */
//void padBuffer(uint8_t* buff, uint8_t* paddedBuff, int buffLen, int newBuffLen){
//	memcpy(paddedBuff, buff, buffLen);
//	pkcs7_padding_pad_buffer(paddedBuff, buffLen, newBuffLen, MODULUS);
//}
//
///**
// * @brief Encrypts a buffer in with an AES CBC cipher
// * @header "operations/crypt/encrypt.h"
// * @param plaintext - plaintext value to be encrypted
// * @param plaintextLen - The length of the plaintext value
// * @return Null
// */
//void encrypt(uint8_t* plaintext, int plaintextLen){
//    struct AES_ctx ctx;
//    AES_init_ctx_iv(&ctx, key, iv);
//    AES_CBC_encrypt_buffer(&ctx, plaintext, plaintextLen);
//}

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * @brief Decrypts a buffer in with an AES CBC cipher
 * @param cipherText plaintext value to be decrypted
 * @param cipherTextLength The length of the plaintext value
 * @return The actual size of the decrypted data
 */
uint32_t aesDecrypt(uint8_t* cipherText, uint8_t cipherTextLength) {
	// sets up the context for decryption and initiailizes the initialization vector
	struct AES_ctx context;
	AES_init_ctx_iv(&context, privateKey, intializationVector);

	AES_CBC_decrypt_buffer(&context, cipherText, cipherTextLength);
	uint32_t actualDataLength = (uint32_t)pkcs7_padding_data_length(cipherText, cipherTextLength, MODULUS);
	return actualDataLength;
}
