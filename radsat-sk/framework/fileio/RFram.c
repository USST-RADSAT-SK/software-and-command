
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
 * @return 0 for success, 1 for failure.
 */
uint16_t framInit(void) {
	int16_t err = FRAM_start();

	if (err != 0) {
		return 1;
	}

	return 0;
}


/**
 * @brief Read data out of the FRAM peripheral.
 * @param data The pointer to where the extracted data will be copied.
 * @param address The FRAM address to begin extracting data from.
 * @param size The number of bytes to copy out of the FRAM peripheral.
 * @return 0 for success, 1 for failure. See hal/Storage/FRAM.h for more details.
 */
uint16_t framRead(uint8_t* data, uint32_t address, uint32_t size) {

	int16_t err = FRAM_read(data, address, size);

	if (err != 0) {
		return 1;
	}

	return 0;
}


/**
 * @brief Write data into the FRAM peripheral.
 * @param data The pointer to where the data will be copied from.
 * @param address The FRAM address to begin writing data to.
 * @param size The number of bytes to copy into the FRAM peripheral.
 * @return 0 for success, 1 for failure. See hal/Storage/FRAM.h for more details.
 */
uint16_t framWrite(uint8_t* data, uint32_t address, uint32_t size) {

	int16_t err = FRAM_writeAndVerify(data, address, size);

	if (err != 0) {
		return 1;
	}

	return 0;

}
