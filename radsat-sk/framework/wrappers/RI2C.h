/**
 *
 * @file RI2C.h
 * @date June 8, 2021
 * @author Addi Amaya (caa746)
 *
 */


#ifndef RI2C_H_
#define RI2C_H_


#include <hal/Drivers/I2C.h>


//Slaveaddress for each components
#define DOSIMETER1 0x4A
#define DOSIMETER2 0x48
#define PDB 0x2B
#define RX 0x60
#define TX 0x61
#define ANTENNA 0x31


#define I2C_BUSSPEED_HZ 100000 //100000 according to 0937/0938/1192/1335 datasheet
#define I2C_TRANSFERTIMEOUT 0
#define WRITEREAD_DELAY 0


/***************************************************************************************************
                                             FUNCTION DECLARATIONS
***************************************************************************************************/

/**
 * Initializes the I2C driver
 *
 * @return 0 for success, otherwise review I2C.h for return definition
 */
uint32_t i2cInit();

/**
 * Writes data to a slave using DMA
 *
 * @note this is a blocking call
 * @pre i2cInit must be successful
 * @param slaveAddress address of the slave where to make the transfer
 * @param data Memory location of the data to be written
 * @param size Number of bytes to be written to the I2C slave
 * @return 0 for success, otherwise review I2C.h for return definition
 */
uint32_t i2cBlockingTransmit(uint16_t slaveAddress, const uint8_t* data, uint16_t size);

/**
 * Reads data from a slave
 *
 * @note this is a blocking call
 * @pre i2cInit must be successful
 * @param slaveAddress Address of the slave where to make the transfer.
 * @param data Memory location to store the data read from the I2C slave. Must be able to store size bytes.
 * @param size Number of bytes to be read from the I2C slave.
 * @return 0 for success, otherwise review I2C.h for return definition
 */
uint32_t i2cBlockingRecieve(uint16_t slaveAddress, uint8_t* data, uint16_t size);

/**
 * Writes and then reads data to/from a slave on the I2C
 *
 * @note If tx->readSize is zero but tx->writeSize is non-zero, this becomes a write transfer.
 * Conversely, If tx->writeSize is zero but tx->readSize is non-zero, this becomes a read transfer.
 * @note this is a blocking call
 * @pre i2cInit must be successful
 * @param slaveAddress Address of the slave where to make the transfer.
 * @param writeSize Number of bytes to be written to the I2C slave.
 * @param readSize Number of bytes to be read from the I2C slave
 * @param writeData Memory location of the data to be written to the I2C slave
 * @param readData Memory location to store the data read from the I2C slave
 * @return 0 for success, otherwise review I2C.h for return definition
 */
uint32_t i2cBlockingRxTx(uint16_t slaveAddress, uint16_t writeSize, uint16_t readSize, uint8_t* writeData, volatile uint8_t* readData);

/**
 * Queues a transfer into the I2C driver
 *
 * @note this is a non-blocking call
 * @pre i2cInit must be successful
 * @param slaveAddress Address of the slave where to make the transfer.
 * @param writeSize Number of bytes to be written to the I2C slave.
 * @param readSize Number of bytes to be read from the I2C slave
 * @param writeData Memory location of the data to be written to the I2C slave
 * @param readData Memory location to store the data read from the I2C slave
 * @return 0 for success, otherwise review I2C.h for return definition
 *
 */
uint32_t i2cQueue(uint16_t slaveAddress, uint16_t writeSize, uint16_t readSize, uint8_t* writeData, volatile uint8_t* readData);


#endif /* RI2C_H_ */
