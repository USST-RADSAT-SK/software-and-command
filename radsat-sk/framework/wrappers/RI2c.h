/**
 *
 * @file RI2c.h
 * @date June 8, 2021
 * @author Addi Amaya (caa746)
 */

#ifndef RI2C_H_
#define RI2C_H_

#include <stdint.h>


/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

#define I2C_BUSSPEED_HZ 100000 //100000 according to 0937/0938/1192/1335 datasheet
#define I2C_TRANSFERTIMEOUT 0
#define WRITEREAD_DELAY 0


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int i2cInit(void);
int i2cTransmit(uint16_t slaveAddress, const uint8_t* data, uint16_t size);
int i2cRecieve(uint16_t slaveAddress, uint8_t* data, uint16_t size);
int i2cQueue(uint16_t slaveAddress, uint16_t writeSize, uint16_t readSize, uint8_t* writeData, volatile uint8_t* readData);


#endif /* RI2C_H_ */
