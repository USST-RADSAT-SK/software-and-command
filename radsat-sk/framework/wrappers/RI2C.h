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

//TODO: Determine a speed for the i2c
#define I2C_BUSSPEED_HZ 200000
#define I2C_TRANSFERTIMEOUT 0
#define WRITEREAD_DELAY 0

uint32_t i2cInit();
uint32_t i2cBlockingTransmit(uint16_t slaveAddress, const uint8_t* data, uint16_t size);
uint32_t i2cBlockingRecieve(uint16_t slaveAddress, uint8_t* data, uint16_t size);
uint32_t i2cBlockingRxTx(uint16_t slaveAddress, uint16_t writeSize, uint16_t readSize, uint8_t* writeData, volatile uint8_t* readData);
uint32_t i2cQueue(uint16_t slaveAddress, uint16_t writeSize, uint16_t readSize, uint8_t* writeData, volatile uint8_t* readData);


#endif /* RI2C_H_ */
