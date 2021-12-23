/**
 * @file RUart.h
 * @date May 5, 2021
 * @author Addi Amaya (caa746) & Jacob Waskowic (jaw352)
 */

#ifndef RUART_H_
#define RUART_H_

#include <hal/Drivers/UART.h>

#define CAMERA_BAUD_RATE 57600
#define TIME_GUARD 0
#define RX_TIMEOUT 255

/***************************************************************************************************
                                             FUNCTION DECLARATIONS
***************************************************************************************************/

int uartTransmit(const uint8_t* data, uint16_t size);
int uartReceive(uint8_t* data, uint16_t size);
int uartInit(void);

#endif /* RUART_H_ */
