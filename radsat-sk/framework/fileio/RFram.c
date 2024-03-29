/**
 * @file RFram.c
 * @date May 18 2021
 * @author Tyrel Kostyk
 */

#include <RFram.h>
#include <hal/errors.h>
#include <hal/Storage/FRAM.h>
#include <RCommon.h>


/***************************************************************************************************
                                         PRIVATE VARIABLES
***************************************************************************************************/

/** Simple int to track if the FRAM driver has been initialized */
static int initialized = 0;


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Initialize the FRAM peripheral driver.
 * @return 0 for success, non-zero for failure. See hal/Storage/FRAM.h for details.
 */
int framInit(void) {

	// only allow initialization once
	if (initialized)
		return E_IS_INITIALIZED;

	int error = FRAM_start();

	if (error == SUCCESS)
		initialized = 1;

	// TODO: record errors (if present) to System Manager

	return error;
}


/**
 * Read data out of the FRAM peripheral
 *
 * @note This is a blocking call
 * @param data The pointer to where the extracted data will be copied.
 * @param address The FRAM address to begin extracting data from.
 * @param size The number of bytes to copy out of the FRAM peripheral.
 * @return 0 for success, non-zero for failure. See hal/Storage/FRAM.h for details.
 */
int framRead(uint8_t* data, uint32_t address, uint32_t size) {

	// FRAM driver must be initialized first
	if (!initialized)
		return E_NOT_INITIALIZED;

	int error = FRAM_read(data, address, size);

	// TODO: record errors (if present) to System Manager

	return error;
}


/**
 * Write data into the FRAM peripheral
 *
 * @note This is a blocking call
 * @param data The pointer to where the data will be copied from.
 * @param address The FRAM address to begin writing data to.
 * @param size The number of bytes to copy into the FRAM peripheral.
 * @return 0 for success, non-zero for failure. See hal/Storage/FRAM.h for details.
 */
int framWrite(uint8_t* data, uint32_t address, uint32_t size) {

	// FRAM driver must be initialized first
	if (!initialized)
		return E_NOT_INITIALIZED;

	int error = FRAM_writeAndVerify(data, address, size);

	// TODO: record errors (if present) to System Manager

	return error;
}
