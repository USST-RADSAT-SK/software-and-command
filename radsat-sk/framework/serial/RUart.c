/**
 * @file RUart.c
 * @date May 3, 2021
 * @author Addi Amaya (caa746) & Jacob Waskowic (jaw352)
 */

#include <RUart.h>
#include <hal/errors.h>


/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

#define CAMERA_BAUD_RATE	(57600)
#define DEBUG_BAUD_RATE		(115200)
#define TIME_GUARD			(0)
#define RX_TIMEOUT			(255)


/***************************************************************************************************
                                         PRIVATE VARIABLES
***************************************************************************************************/

/** UART Config struct for UART0 port; the port connected to the Camera subsystem */
static UARTconfig cameraConfig = {AT91C_US_USMODE_NORMAL | AT91C_US_CLKS_CLOCK | AT91C_US_CHRL_8_BITS |
								  AT91C_US_PAR_NONE | AT91C_US_OVER_16 | AT91C_US_NBSTOP_1_BIT,
								  CAMERA_BAUD_RATE, TIME_GUARD, rs232_uart, RX_TIMEOUT};

/** Simple int array to track if each port has been initialized */
static int initialized[UART_BUS_COUNT] = {0};


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Initializes UART and enables receiving on the bus
 *
 * @param bus: the bus to initialize; either debug port or camera port.
 * @return 0 for success, non-zero for failure. See hal/Drivers/UART.h for details
 */
int uartInit(UARTbus bus) {

	// only allow initialization once (exit gracefully)
	if (initialized[bus])
		return 0;

	// select appropriate pre-made configuration
	UARTconfig config = {0};
	if (bus == UART_CAMERA_BUS)
		config = cameraConfig;
	else
		return -1;

	// start the UART bus
	int error = UART_start(bus, config);

	if (error != 0)
		return error;

	// enable RX on the UART bus
	error = UART_setRxEnabled(bus, TRUE);

	// set flag upon successful initialization
	if (error == 0)
		initialized[bus] = 1;

	return error;
}


/**
 * Sends the given data over the specified UART port
 *
 * @note this is a semi-blocking call (only the calling FreeRTOS task is put to sleep)
 * @param bus The bus to initialize; either debug port or camera port
 * @param data A pointer to the data to send over UART
 * @param size The number of bytes to be sent
 * @return 0 for success, non-zero for failure. See hal/Drivers/UART.h for details
 */
int uartTransmit(UARTbus bus, const uint8_t* data, uint16_t size) {

	// UART port must be initialized first
	if (!initialized[bus])
		return E_NOT_INITIALIZED;

	int error = UART_write(bus, data, size);
	return error;
}


/**
 * Receives data over UART and stores it in the given buffer
 *
 * @note this is a semi-blocking call (only the calling FreeRTOS task is put to sleep)
 * @param bus the bus to initialize; either debug port or camera port.
 * @param data A buffer to store the received data in
 * @param size The number of bytes to receive over UART
 * @return 0 for success, non-zero for failure. See hal/Drivers/UART.h for details
 */
int uartReceive(UARTbus bus, uint8_t* data, uint16_t size) {

	// UART port must be initialized first
	if (!initialized[bus])
		return E_NOT_INITIALIZED;

	int error = UART_read(bus, data, size);
	return error;
}
