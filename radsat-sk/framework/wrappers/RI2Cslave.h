/**
 *
 * @file RI2Cslave.h
 * @date June 8, 2021
 * @author Addi Amaya (caa746)
 *
 */


#ifndef RI2CSLAVE_H_
#define RI2CSLAVE_H_


#include <hal/Drivers/I2Cslave.h>

#define TIMEOUT_DELAY 0

/**
 * initializes I2C Slave driver
 *
 * @note see I2CSlaveCommandList for more details on commands
 * @param address Address of the slave on the I2C bus.
 * @param command Command code (first byte sent by the I2C master)
 * @param commandParamSize Number of bytes to read after receiving the command. If the command does not have any parameters, this should be 0
 * @param commandListSize Number of commands
 * @return 0 for success, otherwise review I2C.h for return definition
 *
 */
uint32_t i2cInitSlave(uint8_t address, uint8_t command, uint16_t commandParamSize, uint16_t commandListSize);

/**
 * Writes data TO the master
 *
 * @pre i2cInitSlave must be successful
 * @param data Pointer to a location where the data to write is stored.
 * @param size Number of bytes to transfer
 * @return 0 for success, otherwise review I2Cslave.h for return definition
 *
 */
uint32_t i2cSlaveTx(const uint8_t* data, uint16_t size);

/**
 * Reads data from the I2C bus
 *
 * @note This function simply waits until actual data is available and returns when the i2c master sends a command
 * @param data A pointer to the location where the driver should store the received data
 * @return The number of bytes received as a command
 *
 */
uint32_t i2cSlaveRx(uint8_t* data);

#endif /* RI2CSLAVE_H_ */

