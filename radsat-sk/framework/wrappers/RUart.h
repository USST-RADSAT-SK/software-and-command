/**
 * @file RUart.h
 * @date May 5, 2021
 * @author Addi Amaya (caa746) & Jacob Waskowic (jaw352)
 */

#ifndef RUART_H_
#define RUART_H_

#include "hal/Drivers/UART.h"

int UART_tx(const unsigned char* data, unsigned int size);
int UART_rx(unsigned char* data, unsigned int size);
int UART_init();

#endif /* RUART_H_ */
