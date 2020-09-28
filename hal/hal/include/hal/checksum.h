/** @file checksum.h
 * Collection of checksum functions
 */

#ifndef CHECKSUM_H
#define CHECKSUM_H

#include "hal/boolean.h"

#define CRC8_DEFAULT_STARTREMAINDER		0xFF
#define CRC8_POLYNOMIAL					0x07

#define CRC16_DEFAULT_STARTREMAINDER	0xFFFF
#define CRC16_POLYNOMIAL				0x1021

#define CRC32_DEFAULT_STARTREMAINDER	0
#define CRC32_POLYNOMIAL				0x04C11DB7
#define CRC32_POLYNOMIAL_REVERSED		0xEDB88320

/**
 * Generate a LUT for CRC 32 calculations with a certain polynomial
 *
 * @param[in] polynomial Reverse of 32-bit CRC polynomial to be used
 * @param[out] LUT Pointer to memory block where LUT can be stored, needs to be at least 256 * sizeof(unsigned int)
 */
void checksum_prepareLUTCRC32(unsigned int polynomial, unsigned int* LUT);

/**
 * Calculates a CRC 32 checksum according to ISO 3309, using a LUT
 *
 * @param[in] data Pointer to data to calculate the checksum for.
 * @param[in] length Length of the data in bytes.
 * @param[in] LUT Pointer to LUT to use for CRC calculations, prepared using checksum_prepareLUTCRC32()
 * @param[in] start_remainder Remainder to start CRC calculation with
 * @return 32-bit CRC checksum.
 */
unsigned int checksum_calculateCRC32LUT(const unsigned char* data, unsigned int length, const unsigned int* LUT, unsigned int start_remainder);

/**
 * Calculates a CRC 32 checksum according to ISO 3309, using bitwise operations
 *
 * @param[in] data Pointer to data to calculate the checksum for.
 * @param[in] length Length of the data in bytes.
 * @param[in] polynomial Reverse of 32-bit CRC polynomial to be used.
 * @param[in] start_remainder Remainder to start CRC calculation with
 * @return 32-bit CRC checksum.
 */
unsigned int checksum_calculateCRC32(const unsigned char* data, unsigned int length, unsigned int polynomial, unsigned int start_remainder);

/*!
 * @brief Verifies CRC32 Calculation by comparing a calculated CRC frame with input CRC frame, using bitwise calculation
 *
 * @param data Pointer to data to verify the checksum for. This should contain a CRC32 value stored at the end of the array.
 * @param length Length of the data in bytes including the CRC32 value stored at the end of the array.
 * @param[in] polynomial Reverse of 32-bit CRC polynomial to be used.
 * @return TRUE (checksum correct) or FALSE (checksum incorrect).
 */
Boolean checksum_verifyCRC32(const unsigned char* data, unsigned int length, unsigned int polynomial);

/**
 * Generate a LUT for CRC 16 calculations with a certain polynomial
 *
 * @param[in] polynomial 16-bit CRC polynomial to be used
 * @param[out] LUT Pointer to memory block where LUT can be stored, needs to be at least 256 * sizeof(unsigned short)
 */
void checksum_prepareLUTCRC16(unsigned short polynomial, unsigned short* LUT);

/**
 * Calculates a CRC 16 checksum according to CRC-16 CCITT, using a LUT
 *
 * @param[in] data Pointer to data to calculate the checksum for.
 * @param[in] length Length of the data in bytes.
 * @param[in] LUT Pointer to LUT to use for CRC calculations, prepared using checksum_prepareLUTCRC16()
 * @param[in] start_remainder Remainder to start CRC calculation with
 * @param[in] endofdata Indicates whether this is the end of larger datablock (TRUE) or not yet (FALSE)
 * @return 16-bit CRC checksum.
 */
unsigned short checksum_calculateCRC16LUT(const unsigned char* data, unsigned int length, const unsigned short* LUT, unsigned short start_remainder, Boolean endofdata);

/**
 * Calculates a CRC 16 checksum according to CRC-16 CCITT, using bitwise calculation
 *
 * @param[in] data Pointer to data to calculate the checksum for.
 * @param[in] length Length of the data in bytes.
 * @param[in] polynomial 16-bit CRC polynomial to be used.
 * @param[in] start_remainder Remainder to start CRC calculation with
 * @param[in] endofdata Indicates whether this is the end of larger datablock (TRUE) or not yet (FALSE)
 * @return 16-bit CRC checksum.
 */
unsigned short checksum_calculateCRC16(const unsigned char* data, unsigned int length, unsigned short polynomial, unsigned short start_remainder, Boolean endofdata);

/*!
 * @brief Verifies CRC16 Calculation by comparing a calculated CRC frame with input CRC frame, using bitwise calculation
 *
 * @param data Array of data to verify the checksum for. This should contain a CRC16 value stored at the end of the array.
 * @param length Length of the data in bytes including the CRC16 value stored at the end of the array.
 * @param[in] polynomial 16-bit CRC polynomial to be used.
 * @return TRUE (checksum correct) or FALSE (checksum incorrect).
 */
Boolean checksum_verifyCRC16(const unsigned char* data, unsigned int length, unsigned short polynomial);

/**
 * Generate a LUT for CRC 8 calculations with a certain polynomial
 *
 * @param[in] polynomial 8-bit CRC polynomial to be used
 * @param[out] LUT Pointer to memory block where LUT can be stored, needs to be at least 256 * sizeof(unsigned char)
 */
void checksum_prepareLUTCRC8(unsigned char polynomial, unsigned char* LUT);

/**
 * Calculates a CRC 8 checksum according to CRC-8 CCITT, using a LUT
 *
 * @param[in] data Pointer to data to calculate the checksum for.
 * @param[in] length Length of the data in bytes.
 * @param[in] LUT Pointer to LUT to use for CRC calculations, prepared using checksum_prepareLUTCRC8()
 * @param[in] start_remainder Remainder to start CRC calculation with
 * @param[in] endofdata Indicates whether this is the end of larger datablock (TRUE) or not yet (FALSE)
 * @return 8-bit CRC checksum.
 */
unsigned char checksum_calculateCRC8LUT(unsigned char* data, unsigned int length, unsigned char* LUT, unsigned char start_remainder, Boolean endofdata);

/**
 * Calculates a CRC 8 checksum according to CRC-8 CCITT, using bitwise calculation
 *
 * @param[in] data Pointer to data to calculate the checksum for.
 * @param[in] length Length of the data in bytes.
 * @param[in] polynomial 8-bit CRC polynomial to be used.
 * @param[in] start_remainder Remainder to start CRC calculation with
 * @param[in] endofdata Indicates whether this is the end of larger datablock (TRUE) or not yet (FALSE)
 * @return 8-bit CRC checksum.
 */
unsigned char checksum_calculateCRC8(unsigned char* data, unsigned int length, unsigned char polynomial, unsigned char start_remainder, Boolean endofdata);

/*!
 * @brief Verifies CRC8 Calculation by comparing a calculated CRC frame with input CRC frame, using bitwise calculation
 *
 * @param data Pointer to data to verify the checksum for. This should contain a CRC8 value stored at the end of the array.
 * @param length Length of the data in bytes including the CRC8 value stored at the end of the array.
 * @param[in] polynomial 16-bit CRC polynomial to be used.
 * @return TRUE (checksum correct) or FALSE (checksum incorrect).
 */
Boolean checksum_verifyCRC8(unsigned char* data, unsigned int length, unsigned char polynomial);

#endif // CHECKSUM_H
