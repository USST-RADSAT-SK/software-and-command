/*
 * @file RI2Cslave.c
 * @date June 8, 2021
 * @author Addi Amaya (caa746)
 */

#include <stdint.h>
#include <string.h>
#include "RI2Cslave.h"

//TODO: figure out what "hasResponse" does in I2CslaveCommandList
/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/


/**
 * initializes I2C Slave driver
 *
 * @note see I2CSlaveCommandList for more details on commands
 * @param address Address of the slave on the I2C bus.
 * @param command Command code (first byte sent by the I2C master)
 * @param commandParamSize Number of bytes to read after receiving the command. If the command does not have any parameters, this should be 0
 * @param commandListSize Number of commands
 * @return 0 for success, otherwise review I2Cslave.h for return definition
 *
 */
uint32_t i2cInitSlave(uint8_t address, uint8_t command, uint16_t commandParamSize, uint16_t commandListSize) {

	struct _I2Cslave_CommandList master;
	master.command = command;
	master.commandParameterSize = commandParamSize;


	uint16_t err = I2Cslave_start(address, &master, commandListSize);

	if (err != 0) {
		return err;
	}

	return 0;

}

/**
 * Writes data TO the master
 *
 * @pre i2cInitSlave must be successful
 * @param data Pointer to a location where the data to write is stored.
 * @param size Number of bytes to transfer
 * @return 0 for success, otherwise review I2Cslave.h for return definition
 *
 */
uint32_t i2cSlaveTx(const uint8_t* data, uint16_t size) {

	uint16_t err = I2C_write(data, size, TIMEOUT_DELAY);

	if (err < 0) {
		return err;
	}

	return 0;
}

/**
 * Reads data from the I2C bus
 *
 * @note This function simply waits until actual data is available and returns when the i2c master sends a command
 * @param data A pointer to the location where the driver should store the received data
 * @return The number of bytes received as a command
 *
 */
uint32_t i2cSlaveRx(uint8_t* data){
	return I2Cslave_read(data);
}
