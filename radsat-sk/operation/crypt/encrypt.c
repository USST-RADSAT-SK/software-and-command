#include "encrypt.h"

int getPaddedBufSize(int plaintextLen);
void padBuffer(uint8_t* buff, uint8_t* paddedBuff, int buffLen, int newBuffLen);
void encrypt(uint8_t* plaintext, int plaintextLen);


/**
 * @brief Gets the next multiple of 16 from the size of the input buffer
 * @header "comms-processing/crypt/encrypt.h"
 * @param plaintextLen - The size of the unpadded buffer containing data to be encrypted.
 * @pre need to have a variable storing the size of the plaintext buffer
 * @return The size of the new buffer to be created
 */
int getPaddedBufSize(int plaintextLen){
    // Special case for when the buffer size is already a multiple of 16:
    if (plaintextLen % MODULUS == 0){
	int newBufSize = plaintextLen + MODULUS;
        return newBufSize; //If no padding is to be added, pad with 16 bytes
    }
    else{
    	int newBufSize = plaintextLen +
        (MODULUS - (plaintextLen % MODULUS));
    	return newBufSize;
    }
}

/**
 * @brief Pads a buffer to be a multiple of 16 bytes
 * @header "comms-processing/crypt/encrypt.h"
 * @param buff - The incoming buffer that will be padded
 * @param paddedBuff - The outgoing buffer that will contain the padded data
 * @param buffLen - The length of the incoming buffer
 * @param newBuffLen - The length that the outgoing buffer will be   
 * @pre Need to know the size of the old and new buffers before calling the function
 * @return Null
 */
void padBuffer(uint8_t* buff, uint8_t* paddedBuff, int buffLen, int newBuffLen){
	memset(paddedBuff, 0, newBuffLen);
	int i;
	for (i=0; i<buffLen; i++){
		paddedBuff[i] = buff[i];
	}
	pkcs7_padding_pad_buffer(paddedBuff, buffLen, newBuffLen, MODULUS);
}

/**
 * @brief Encrypts a buffer in with an AES CBC cipher
 * @header "comms-processing/crypt/encrypt.h"
 * @param plaintext - plaintext value to be encrypted
 * @param plaintextLen - The length of the plaintext value
 * @return Null
 */
void encrypt(uint8_t* plaintext, int plaintextLen){
    struct AES_ctx ctx;
    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_encrypt_buffer(&ctx, plaintext, plaintextLen);
}
