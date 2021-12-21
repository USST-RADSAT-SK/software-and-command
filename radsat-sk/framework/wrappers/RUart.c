/**
 * @file RUart.c
 * @date May 3, 2021
 * @author Addi Amaya (caa746) & Jacob Waskowic (jaw352)
 */

#include <stdint.h>
#include "RUart.h"

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

UARTconfig cameraConfig = {AT91C_US_USMODE_NORMAL | AT91C_US_CLKS_CLOCK | AT91C_US_CHRL_8_BITS |
						   AT91C_US_PAR_NONE | AT91C_US_OVER_16 | AT91C_US_NBSTOP_1_BIT,
						   CAMERA_BAUD_RATE, TIME_GUARD, rs232_uart, RX_TIMEOUT};

/**
 * Sends the given data over UART
 * @param data: A pointer to the data to send over UART
 * @param size: The number of bytes to be sent
 * @return 0 for success, non-zero for failure. See hal/Drivers/UART.h for details.
 */
int uartTransmit(const uint8_t* data, uint16_t size) {

	int error = UART_write(bus0_uart, data, size);
	return error;
}


/**
 * Receives data over UART and stores it in the given buffer
 * @param data: A buffer to store the received data in
 * @param size: The number of bytes to receive over UART
 * @return 0 for success, non-zero for failure. See hal/Drivers/UART.h for details.
 */
int uartReceive(uint8_t* data, uint16_t size) {

	int error = UART_read(bus0_uart, data, size);
	return error;
}


/**
 * Initializes UART and enables receiving on the bus
 * @return 0 for success, non-zero for failure. See hal/Drivers/UART.h for details.
 */
int uartInit(void) {

	int error = UART_start(bus0_uart, cameraConfig);
	if (error != 0) {
		return error;
	}
	return UART_setRxEnabled(bus0_uart, TRUE);
}
