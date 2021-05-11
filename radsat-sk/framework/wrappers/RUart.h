/**
 * @file RUart.h
 * @date May 5, 2021
 * @author Addi Amaya (caa746) & Jacob Waskowic (jaw352)
 */

#ifndef RUART_H_
#define RUART_H_

#include "hal/Drivers/UART.h"

#define CAMERA_BAUD_RATE 57600
#define TIME_GUARD 0
#define RX_TIMEOUT 255

int UART_tx(const unsigned char* data, unsigned int size);
int UART_rx(unsigned char* data, unsigned int size);
int UART_init();

#endif /* RUART_H_ */
