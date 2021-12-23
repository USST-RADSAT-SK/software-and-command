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
                                             PUBLIC API
***************************************************************************************************/

int i2cInit(void);
int i2cTransmit(uint16_t slaveAddress, const uint8_t* data, uint16_t size);
int i2cRecieve(uint16_t slaveAddress, uint8_t* data, uint16_t size);
int i2cTalk(uint16_t slaveAddress, uint16_t writeSize, uint16_t readSize, uint8_t* writeData, uint8_t* readData, uint32_t delay);


#endif /* RI2C_H_ */
