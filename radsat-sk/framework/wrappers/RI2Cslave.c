/*
 * @file RI2Cslave.c
 * @date June 8, 2021
 * @author Addi Amaya (caa746)
 */

#include <stdint.h>
#include <string.h>
#include "RI2Cslave.h"


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/


//TODO: figure out what "hasResponse" does in I2CslaveCommandList
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


uint32_t i2cSlaveTx(const uint8_t* data, uint16_t size) {

	uint16_t err = I2C_write(data, size, TIMEOUT_DELAY);

	if (err < 0) {
		return err;
	}

	return 0;
}

uint32_t i2cSlaveRx(uint8_t* data){
	return I2Cslave_read(data);
}
