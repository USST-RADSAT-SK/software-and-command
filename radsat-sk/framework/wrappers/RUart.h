/**
 * @file RUart.h
 * @date May 5, 2021
 * @author Addi Amaya (caa746) & Jacob Waskowic (jaw352)
 */

#ifndef RUART_H_
#define RUART_H_

#include "includes/hal/Drivers/UART.h"

#define AT91C_US_USMODE_NORMAL 0
#define CAMERA_BAUD_RATE 57600
#define TIME_GUARD 0
#define RX_TIMEOUT 255
#define TRUE 1

int UART_blockingTransmit(const uint8_t* data, uint16_t size);
int UART_blockingTransmit(uint8_t* data, uint16_t size);
int UART_init();

#endif /* RUART_H_ */
