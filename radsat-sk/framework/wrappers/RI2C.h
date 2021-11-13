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


uint32_t i2cInit();
uint32_t i2cTransmit(uint16_t slaveAddress, const uint8_t* data, uint16_t size);
uint32_t i2cRecieve(uint16_t slaveAddress, uint8_t* data, uint16_t size);
uint32_t i2cQueue(uint16_t slaveAddress, uint16_t writeSize, uint16_t readSize, uint8_t* writeData, volatile uint8_t* readData);


#endif /* RI2C_H_ */
