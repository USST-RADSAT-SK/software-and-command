#include "checksumTest.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <hal/checksum.h>
#include <stdio.h>
#include <string.h>

// Test string
static const char* testdata = "The quick brown fox jumps over the lazy dog";

// Big data block for performance comparison, size = 100 kB
#define BIGDATABLOCKSIZE	102400
static unsigned char bigdatablock[BIGDATABLOCKSIZE];

// Storage space for lookup tables
static unsigned int LUT_crc32[256];
static unsigned short LUT_crc16[256];
static unsigned char LUT_crc8[256];

Boolean checksumTest(void)
{
	unsigned int crc32_intermediate, crc32_final;
	unsigned short crc16_intermediate, crc16_final;
	unsigned char crc8_intermediate, crc8_final;
	portTickType time_startcalc;

	// Prepare LUTs
	checksum_prepareLUTCRC32(CRC32_POLYNOMIAL_REVERSED, LUT_crc32);
	checksum_prepareLUTCRC16(CRC16_POLYNOMIAL, LUT_crc16);
	checksum_prepareLUTCRC8(CRC8_POLYNOMIAL, LUT_crc8);

	printf("\n----- CRC-32 calculation -----\n\n");

	crc32_final = checksum_calculateCRC32((unsigned char*)testdata, strlen(testdata), CRC32_POLYNOMIAL_REVERSED, CRC32_DEFAULT_STARTREMAINDER);
	printf("Single shot, bitwise: 0x%08x\n", crc32_final);

	crc32_final = checksum_calculateCRC32LUT((unsigned char*)testdata, strlen(testdata), LUT_crc32, CRC32_DEFAULT_STARTREMAINDER);
	printf("Single shot, LUT: 0x%08x\n", crc32_final);

	crc32_intermediate = checksum_calculateCRC32((unsigned char*)testdata, 10, CRC32_POLYNOMIAL_REVERSED, CRC32_DEFAULT_STARTREMAINDER);
	crc32_final = checksum_calculateCRC32((unsigned char*)testdata + 10, strlen(testdata) - 10, CRC32_POLYNOMIAL_REVERSED, crc32_intermediate);
	printf("Split up, bitwise: 0x%08x\n", crc32_final);

	crc32_intermediate = checksum_calculateCRC32LUT((unsigned char*)testdata, 10, LUT_crc32, CRC32_DEFAULT_STARTREMAINDER);
	crc32_final = checksum_calculateCRC32LUT((unsigned char*)testdata + 10, strlen(testdata) - 10, LUT_crc32, crc32_intermediate);
	printf("Split up, LUT: 0x%08x\n", crc32_final);

	time_startcalc = xTaskGetTickCount();
	crc32_final = checksum_calculateCRC32(bigdatablock, BIGDATABLOCKSIZE, CRC32_POLYNOMIAL_REVERSED, CRC32_DEFAULT_STARTREMAINDER);
	printf("Big data block, bitwise: 0x%08x - Execution time: %d ms\n", crc32_final, (unsigned int)(xTaskGetTickCount() - time_startcalc));

	time_startcalc = xTaskGetTickCount();
	crc32_final = checksum_calculateCRC32LUT(bigdatablock, BIGDATABLOCKSIZE, LUT_crc32, CRC32_DEFAULT_STARTREMAINDER);
	printf("Big data block, LUT: 0x%08x - Execution time: %d ms\n", crc32_final, (unsigned int)(xTaskGetTickCount() - time_startcalc));

	printf("\n----- CRC-16 calculation -----\n\n");

	crc16_final = checksum_calculateCRC16((unsigned char*)testdata, strlen(testdata), CRC16_POLYNOMIAL, CRC16_DEFAULT_STARTREMAINDER, TRUE);
	printf("Single shot, bitwise: 0x%04x\n", crc16_final);

	crc16_final = checksum_calculateCRC16LUT((unsigned char*)testdata, strlen(testdata), LUT_crc16, CRC16_DEFAULT_STARTREMAINDER, TRUE);
	printf("Single shot, LUT: 0x%04x\n", crc16_final);

	crc16_intermediate = checksum_calculateCRC16((unsigned char*)testdata, 10, CRC16_POLYNOMIAL, CRC16_DEFAULT_STARTREMAINDER, FALSE);
	crc16_final = checksum_calculateCRC16((unsigned char*)testdata + 10, strlen(testdata) - 10, CRC16_POLYNOMIAL, crc16_intermediate, TRUE);
	printf("Split up, bitwise: 0x%04x\n", crc16_final);

	crc16_intermediate = checksum_calculateCRC16LUT((unsigned char*)testdata, 10, LUT_crc16, CRC16_DEFAULT_STARTREMAINDER, FALSE);
	crc16_final = checksum_calculateCRC16LUT((unsigned char*)testdata + 10, strlen(testdata) - 10, LUT_crc16, crc16_intermediate, TRUE);
	printf("Split up, LUT: 0x%04x\n", crc16_final);

	time_startcalc = xTaskGetTickCount();
	crc16_final = checksum_calculateCRC16(bigdatablock, BIGDATABLOCKSIZE, CRC16_POLYNOMIAL, CRC16_DEFAULT_STARTREMAINDER, TRUE);
	printf("Big data block, bitwise: 0x%04x - Execution time: %d ms\n", crc16_final, (unsigned int)(xTaskGetTickCount() - time_startcalc));

	time_startcalc = xTaskGetTickCount();
	crc16_final = checksum_calculateCRC16LUT(bigdatablock, BIGDATABLOCKSIZE, LUT_crc16, CRC16_DEFAULT_STARTREMAINDER, TRUE);
	printf("Big data block, LUT: 0x%04x - Execution time: %d ms\n", crc16_final, (unsigned int)(xTaskGetTickCount() - time_startcalc));

	printf("\n----- CRC-8 calculation -----\n\n");

	crc8_final = checksum_calculateCRC8((unsigned char*)testdata, strlen(testdata), CRC8_POLYNOMIAL, CRC8_DEFAULT_STARTREMAINDER, TRUE);
	printf("Single shot, bitwise: %02x\n", crc8_final);

	crc8_final = checksum_calculateCRC8LUT((unsigned char*)testdata, strlen(testdata), LUT_crc8, CRC8_DEFAULT_STARTREMAINDER, TRUE);
	printf("Single shot, LUT: %02x\n", crc8_final);

	crc8_intermediate = checksum_calculateCRC8((unsigned char*)testdata, 10, CRC8_POLYNOMIAL, CRC8_DEFAULT_STARTREMAINDER, FALSE);
	crc8_final = checksum_calculateCRC8((unsigned char*)testdata + 10, strlen(testdata) - 10, CRC8_POLYNOMIAL, crc8_intermediate, TRUE);
	printf("Split up, bitwise: %02x\n", crc8_final);

	crc8_intermediate = checksum_calculateCRC8LUT((unsigned char*)testdata, 10, LUT_crc8, CRC8_DEFAULT_STARTREMAINDER, FALSE);
	crc8_final = checksum_calculateCRC8LUT((unsigned char*)testdata + 10, strlen(testdata) - 10, LUT_crc8, crc8_intermediate, TRUE);
	printf("Split up, LUT %02x\n", crc8_final);

	time_startcalc = xTaskGetTickCount();
	crc8_final = checksum_calculateCRC8(bigdatablock, BIGDATABLOCKSIZE, CRC8_POLYNOMIAL, CRC8_DEFAULT_STARTREMAINDER, TRUE);
	printf("Big data block, bitwise: %02x - Execution time: %d ms\n", crc8_final, (unsigned int)(xTaskGetTickCount() - time_startcalc));

	time_startcalc = xTaskGetTickCount();
	crc8_final = checksum_calculateCRC8LUT(bigdatablock, BIGDATABLOCKSIZE, LUT_crc8, CRC8_DEFAULT_STARTREMAINDER, TRUE);
	printf("Big data block, LUT: %02x - Execution time: %d ms\n", crc8_final, (unsigned int)(xTaskGetTickCount() - time_startcalc));

	printf("\n");

	return TRUE;
}
