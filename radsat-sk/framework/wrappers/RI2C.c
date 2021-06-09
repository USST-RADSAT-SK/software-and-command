/*
 * @file RI2C.c
 * @date June 7, 2021
 * @author Addi Amaya (caa746)
 */

#include <stdint.h>
#include <string.h>
#include "RI2C.h"


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

uint32_t i2cInit() {

	uint16_t err = I2C_start(I2C_BUSSPEED_HZ, I2C_TRANSFERTIMEOUT);

	if (err != 0) {
		return err;
	}

	return 0;
}

uint32_t i2cBlockingTransmit(uint16_t slaveAddress, const uint8_t* data, uint16_t size) {

	uint16_t err = I2C_write(slaveAddress, data, size);

	if (err != 0) {
		return err;
	}

	return 0;
}

uint32_t i2cBlockingRecieve(uint16_t slaveAddress, uint8_t* data, uint16_t size) {

	uint16_t err = I2C_read(slaveAddress, data, size);

	if (err != 0) {
		return err;
	}

	return 0;
}

uint32_t i2cBlockingRxTx(uint16_t slaveAddress, uint16_t writeSize, uint16_t readSize, uint8_t* writeData, volatile uint8_t* readData) {

	struct _I2CwriteReadTransfer master;
	master.slaveAddress = slaveAddress;
	master.writeSize = writeSize;
	master.readSize = readSize;
	strcpy(master.writeData, writeData);
	strcpy(master.readData, readData);
	master.writeReadDelay = WRITEREAD_DELAY;

	uint16_t err = I2C_writeRead(&master);

	if (err != 0) {
		return err;
	}

	return 0;
}

uint32_t i2cQueue(uint16_t slaveAddress, uint16_t writeSize, uint16_t readSize, uint8_t* writeData, volatile uint8_t* readData) {

	struct _I2CgenericTransfer master;
	master.slaveAddress = slaveAddress;
	master.direction = 0xFF;
	master.writeSize = writeSize;
	master.readSize = readSize;
	strcpy(master.writeData, writeData);
	strcpy(master.readData, readData);
	master.writeReadDelay = WRITEREAD_DELAY;
	uint16_t status = I2C_getCurrentTransferStatus();
	master.result = status;

	uint16_t err = I2C_queueTransfer(&master);

	if (err != 0) {
		return err;
	}

	return 0;

}




