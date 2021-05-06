/**
 * @file RUart.c
 * @date May 3, 2021
 * @author Addi Amaya (caa746) & Jacob Waskowic (jaw352)
 */


#include "RUart.h"

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/


/**
 * Sends the given data over UART
 * @param data: A pointer to the data to send over UART
 * @param size: The number of bytes to be sent
 * @return An integer error code. 0 indicates success
 */
int UART_tx(const unsigned char* data, unsigned int size) {
	return UART_write(bus0_uart, data, size);
}


/**
 * Receives data over UART and stores it in the given buffer
 * @param data: A buffer to store the received data in
 * @param size: The number of bytes to recieve over UART
 * @return An integer error code. 0 indicates success
 */
int UART_rx(unsigned char* data, unsigned int size) {
	return UART_read(bus0_uart, data, size);
}
