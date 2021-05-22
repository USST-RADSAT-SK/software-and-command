/**
 * @file RUart.h
 * @date May 22, 2021
 * @author Addi Amaya (caa746) & Julian Pham (ngp143)
 */

#ifndef RSDCARD_H_
#define RSDCARD_H_


uint32_t SD_init(uint16_t volID);

uint32_t SD_read(uint8_t filename);

uint32_t SD_write(uint8_t filename);


#endif /* RSDCARD_H_ */
