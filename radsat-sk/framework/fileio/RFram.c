
/**
 * @file RFram.c
 * @date May 18 2021
 * @author Tyrel Kostyk
 */

#include <RFram.h>
#include <hal/Storage/FRAM.h>


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * @brief Initialize the FRAM peripheral driver.
 * @return 0 for success, non-zero for failure. See hal/Storage/FRAM.h for details.
 */
int framInit(void) {

	int error = FRAM_start();
	return error;
}


/**
 * @brief Read data out of the FRAM peripheral.
 * @param data The pointer to where the extracted data will be copied.
 * @param address The FRAM address to begin extracting data from.
 * @param size The number of bytes to copy out of the FRAM peripheral.
 * @return 0 for success, non-zero for failure. See hal/Storage/FRAM.h for details.
 */
int framRead(uint8_t* data, uint32_t address, uint32_t size) {

	int error = FRAM_read(data, address, size);
	return error;
}


/**
 * @brief Write data into the FRAM peripheral.
 * @param data The pointer to where the data will be copied from.
 * @param address The FRAM address to begin writing data to.
 * @param size The number of bytes to copy into the FRAM peripheral.
 * @return 0 for success, non-zero for failure. See hal/Storage/FRAM.h for details.
 */
int framWrite(uint8_t* data, uint32_t address, uint32_t size) {

	int error = FRAM_writeAndVerify(data, address, size);
	return error;
}
