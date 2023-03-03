/**
 * @file RI2c.c
 * @date June 7, 2021
 * @author Addi Amaya (caa746)
 */

#include <RI2c.h>
#include <string.h>
#include <hal/errors.h>
#include <hal/Drivers/I2C.h>
#include <RCommon.h>


/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

#define I2C_BUS_SPEED_HZ		100000	// 100000 according to 0937/0938/1192/1335 datasheet
#define I2C_TRANSFER_TIMEOUT	50		// 5 ticks (5ms)


/***************************************************************************************************
                                         PRIVATE VARIABLES
***************************************************************************************************/

/** Simple int to track if the I2C port has been initialized */
static int initialized = 0;


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Initializes the I2C driver
 * @return 0 for success, non-zero for failure. See hal/Drivers/I2C.h for details.
 */
int i2cInit(void) {

	// only allow initialization once (exit gracefully)
	if (initialized)
		return SUCCESS;

	int error = I2C_start(I2C_BUS_SPEED_HZ, 2500); //I2C_BUS_SPEED_HZ, I2C_TRANSFER_TIMEOUT);

	// TODO: record errors (if present) to System Manager

	if (error == 0)
		initialized = 1;

	return error;
}



/*!
 * Blocks the bus, preventing any further transmissions on it until released. If there is a transmission
 * ongoing, it will wait until *timeout* or the bus is released
 * @param timeout Length of time to wait for current transmission to finish (portMAX_DELAY to wait forever)
 * @return 0 on success, 1 if there was a timeout
 */
int i2cWait(void){
	int error = I2C_blockBus(250);
	if (error){
		errorPrint("I2C Read FAILEd!!!!!!!");
	}
	return error;
}

/*!
 * Release back the bus.
 */
void i2cRelease(void){
	I2C_releaseBus();
	return;
}


/**
 * Writes data to a slave component over I2C
 *
 * @note this is a semi-blocking call (only the calling FreeRTOS task is put to sleep)
 * @pre i2cInit must be successful
 *
 * @param slaveAddress Address of the slave where to make the transfer
 * @param data Memory location of the data to be written
 * @param size Number of bytes to be written to the I2C slave
 * @return 0 for success, non-zero for failure. See hal/Drivers/I2C.h for details.
 */
int i2cTransmit(uint16_t slaveAddress, const uint8_t* data, uint16_t size) {

	// I2C driver must be initialized
	if (!initialized)
		return E_NOT_INITIALIZED;

	//if(i2cWait()) return -1;
	int error = I2C_write(slaveAddress, data, size);
	//i2cRelease();

	// TODO: record errors (if present) to System Manager

	return error;
}


/**
 * Reads data from a slave component over I2C
 *
 * @note this is a semi-blocking call (only the calling FreeRTOS task is put to sleep)
 * @pre i2cInit must be successful
 *
 * @param slaveAddress Address of the slave where to make the transfer.
 * @param data Memory location to store the data read from the I2C slave. Must be able to store size bytes.
 * @param size Number of bytes to be read from the I2C slave.
 * @return 0 for success, non-zero for failure. See hal/Drivers/I2C.h for details.
 */
int i2cRecieve(uint16_t slaveAddress, uint8_t* data, uint16_t size) {

	// I2C driver must be initialized
	if (!initialized)
		return E_NOT_INITIALIZED;

	debugPrint("before I2C read\n");
	int error = I2C_read(slaveAddress, data, size);
	debugPrint("after I2C read\n");
	// TODO: record errors (if present) to System Manager

	return error;
}


/**
 * Writes to the slave, then reads a response
 *
 * @note this is a semi-blocking call (only the calling FreeRTOS task is put to sleep)
 * @pre i2cInit must be successful
 *
 * @param slaveAddress I2C address of the slave to communicate with.
 * @param writeSize Number of bytes to be written to the I2C slave.
 * @param readSize Number of bytes to be read from the I2C slave
 * @param writeData Memory location of the data to be written to the I2C slave
 * @param readData Memory location to store the data read from the I2C slave
 * @param delay Length of delay (in ticks, i.e. ms) between write and read operations.
 * @return 0 for success, non-zero for failure. See hal/Drivers/I2C.h for details.
 *
 */
int i2cTalk(uint16_t slaveAddress, uint16_t writeSize, uint16_t readSize, uint8_t* writeData,
			uint8_t* readData, uint32_t delay) {

	if (!initialized)
		return E_NOT_INITIALIZED;

	// set up read & write transfer
	I2Ctransfer transfer;
	transfer.slaveAddress = slaveAddress;
	transfer.writeSize = writeSize;
	transfer.readSize = readSize;
	transfer.writeData = writeData;
	transfer.readData = readData;
	transfer.writeReadDelay = delay;

	int error = I2C_writeRead(&transfer);

	if (error){
		errorPrint("I2C error = %d", error);
	}
	// TODO: record errors (if present) to System Manager

	return error;
}
