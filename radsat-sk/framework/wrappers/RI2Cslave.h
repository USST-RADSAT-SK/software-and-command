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


uint32_t i2cInitSlave(uint8_t address, uint8_t command, uint16_t commandParamSize, uint16_t commandListSize);
uint32_t i2cSlaveTx(const uint8_t* data, uint16_t size);
uint32_t i2cSlaveRx(uint8_t* data);

#endif /* RI2CSLAVE_H_ */

