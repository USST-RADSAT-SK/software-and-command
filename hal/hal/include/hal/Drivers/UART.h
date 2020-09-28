/*!
 * @file	UART.h
 * @brief	UART communication.
 * @warning User may not change the entries in this header file. Values such as
 * define preprocessor directives are for information and not for configuration.
 * @date	Feb 18, 2013
 * @author	Akhil Piplani
 */

#ifndef UART_H_
#define UART_H_

#include <hal/boolean.h>
#include <at91/commons.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#define KHZ(x)	(x*1000)
#define MHZ(x)	(x*1000*1000)

#define UART_RS232		0
#define UART_RS422		1

#define UART_MAX_CLOCK_DIVIDER			65535

/*!
 * Maximum number of RX or TX transfers that can be queued
 */
#define UART_QUEUE_SIZE 32

/*!
 * Maximum speed of the UART bus in RS232 mode.
 */
#define UART_MAX_BUS_SPEED_RS232			KHZ(500)

/*!
 * Maximum speed of the UART bus in RS422 modes.
 */
#define UART_MAX_BUS_SPEED_RS422			MHZ(10)

/*!
 * Minimum speed of the UART bus with over-sampling factor of 16.
 * Using over-sampling of 16 is recommended.
 */
#define UART_MIN_BUS_SPEED_OS16	((BOARD_MCK / UART_MAX_CLOCK_DIVIDER) / 16)

/*!
 * Minimum speed of the UART bus with over-sampling factor of 8.
 */
#define UART_MIN_BUS_SPEED_OS8	((BOARD_MCK / UART_MAX_CLOCK_DIVIDER) / 8)

typedef enum _UARTdriverState {
	idle_uartState  = 0x00,         //!< idle_uartState
	uninitialized_uartState  = 0x33,//!< uninitialized_uartState
	error_uartState = 0x55,         //!< error_uartState
	transfer_uartState = 0xFF       //!< transfer_uartState
} UARTdriverState;

typedef enum _UARTbus {
	bus0_uart      = 0,
	bus2_uart      = 1,
	UART_BUS_COUNT = 2
} UARTbus;

/*!
 * Defines which type of bus is used for UART2.
 * The UART2 carries a configurable transceiver chip that can support a variety of UART bus types.
 * @note This affects the maximum baud-rate value accepted by the driver for UART2.
 */
typedef enum _UART2busType {
	rs232_uart                 = 0,
	rs422_noTermination_uart   = 1,
	rs422_withTermination_uart = 2
} UART2busType;

typedef enum _UARTtransferStatus { // Do not change the values here.
	done_uart         = 0,
	pending_uart      = 1,
	error_uart        = 2,
	framingError_uart = 4,
	parityError_uart  = 8,
	framingAndParityError_uart = 12,
	overrunError_uart = 16
} UARTtransferStatus;

typedef enum _UARTdirection {
	write_uartDir  = 0,
	read_uartDir   = 1,
	UART_DIR_COUNT = 2
} UARTdirection;

typedef struct _UARTconfig {
	/*! Value to be written to the UART mode register.
	 *  For UART0, use only normal (AT91C_US_USMODE_NORMAL) mode for the lowest 4 bits. \n
	 *
	 *  For UART2, use only amongst:\n
	 *  normal (AT91C_US_USMODE_NORMAL) or \n
	 *  hardware-handshaking (AT91C_US_USMODE_HWHSH) \n
	 *
	 *  Use only MCK for Clock Selection. See AT91SAM9G20 datasheet.
	 */
	unsigned int mode;
	unsigned int baudrate; //!< Its suggested to use one of the standard RS-232 baudrates.
	unsigned char timeGuard; //!< Value to be written to the timeguard register. An idle time of (timeGuard*bitPeriod) is added after each byte. See AT91SAM9G20 datasheet.
	UART2busType busType; //!< The bus-type to be configured for UART2.
	unsigned short rxtimeout; //!< Timeout value for reception in baudrate ticks, so timeout in seconds is value / baudrate. The timeout only starts counting after the first byte of the transfer has been received. If a timeout is specified it affects all read functions (UART_read, UART_writeRead, UART_queueTransfer).
} UARTconfig;

/*!
 * A generic UART transfer specification structure that allows the user to configure all aspects of the transfer.
 * This is passed to UART_queueTransfer.
 */
typedef struct _UARTgenericTransfer {
	UARTbus bus;              ///< The UART bus over which the transfer should be made.
	UARTdirection direction;  ///< Direction of the transfer.
	const unsigned char *writeData; ///< Location of data to be sent.
	volatile unsigned char *readData;  ///< Location where the driver should store data received over UART
	unsigned int writeSize;   ///< Number of bytes to send.
	unsigned int readSize;    ///< Number of bytes to receive.
	/*!
	 * For adding delays between successive transactions.
	 * The driver will wait the specified amount of ticks before it proceeds to process the next transfer in the queue.
	 */
	portTickType postTransferDelay;
	UARTtransferStatus *result; ///< The driver will store the result of the transfer at this location.

	xSemaphoreHandle semaphore; //!< This binary semaphore will be passed to the callback. If the task is blocking on this semaphore for the transfer to complete, the callback can release it to unblock the task.
	void (*callback)(SystemContext context, xSemaphoreHandle sem); //!< Callback to indicate the transfer is finished, for the UART driver the context is always isr.
} UARTgenericTransfer;

/*!
 * Starts the selected UART peripheral with the given configuration.
 * @note the UART is enabled after this function
 * @param bus The selected UART bus.
 * @param config The configuration used to initialize the UART peripheral.
 * @return -2 if the bus is invalid OR
 * the baudrate is out of range OR
 * other than normal or hardware-handshaking modes is used
 * -1 if initializing FreeRTOS queues or UART related tasks fails.
 */
int UART_start(UARTbus bus, UARTconfig config);

/*!
 * Stops the selected UART peripheral.
 * @param bus The selected UART bus.
 * @return -1 if the selected bus is out of range, 0 on success.
 */
int UART_stop(UARTbus bus);

/*!
 * Enable/Disable the selected UART peripheral from receiving bytes.
 * @param bus The selected UART bus.
 * @param enable TRUE to enable, FALSE to disable byte reception.
 * @return -1 if the selected bus is out of range, 0 on success.
 */
int UART_setRxEnabled( UARTbus bus, Boolean enable );

/*!
 * Checks whether or not the RX is enabled for the specified bus.
 * @param bus The UART bus for which the check should be made.
 * @return TRUE if the selected bus's RX is enabled, otherwise FALSE.
 */
Boolean UART_isRxEnabled( UARTbus bus );

/*!
 * Queues a transfer to be started at the given UART peripheral.
 * @param tx A UART generic transfer structure.
 * @return -2 if input parameters are invalid,
 * -1 if adding the transfer to the queue fails,
 * 0 on success.
 */
int UART_queueTransfer(UARTgenericTransfer *tx);

/*!
 * Performs a blocking write transfer by sleeping the task until the specified transfer is complete.
 * This allows other tasks to execute while the transfer is made using DMA but the calling task
 * is blocked.
 * @param bus The UART bus over which the transfer should be made.
 * @param data Memory location of data to be sent.
 * @param size Number of bytes to be sent.
 * @return -3 if creating a semaphore for the transfer fails,
 * -2 if input parameters are invalid or the driver is not initialized,
 * -1 if adding the transfer to the queue fails,
 * 0 on success.
 * 2 for general errors during the transfer,
 * 4 for a framing error,
 * 8 for a parity error,
 * 12 if both framing and parity errors occur during the transfer,
 * 16 if an overrun error occurs.
 * @note If a negative error is returned, the transfer was never attempted
 * (for example due to an invalid input).
 * If a positive error is returned, an error occurred during the transfer.
 * @note The positive error values correspond to the members of UARTtransferStatus enumerated data type.
 */
int UART_write(UARTbus bus, const unsigned char *data, unsigned int size);

/*!
 * Performs a blocking read transfer by sleeping the task until the specified transfer is complete.
 * This allows other tasks to execute while the transfer is made using DMA but the calling task
 * is blocked.
 * @param bus The UART bus over which the transfer should be made.
 * @param data Memory location where the received data should be stored.
 * @param size Number of bytes to be received.
 * @return -3 if creating a semaphore for the transfer fails,
 * -2 if input parameters are invalid or the driver is not initialized,
 * -1 if adding the transfer to the queue fails,
 * 0 on success.
 * 2 for general errors during the transfer,
 * 4 for a framing error,
 * 8 for a parity error,
 * 12 if both framing and parity errors occur during the transfer,
 * 16 if an overrun error occurs.
 * @note If a negative error is returned, the transfer was never attempted
 * (for example due to an invalid input).
 * If a positive error is returned, an error occurred during the transfer.
 * @note The positive error values correspond to the members of UARTtransferStatus enumerated data type.
 */
int UART_read(UARTbus bus, unsigned char *data, unsigned int size);

/*!
 * Post transfer delay is used to add a delay between successive transactions on the bus.
 * In case of UART_queueTransfer, this value is passed as a part of the transfer specifier
 * and the value specified here is superseded by the value specified in the transfer specifier.
 * In case of all other functions (UART_writeRead, UART_write, UART_read) the driver will use
 * the value set using this function.
 * @note If no value is set using this function, a value of zero is used by the driver.
 * @param bus The bus for which this delay should be set.
 * @param delay Number of ticks the driver should wait between transactions.
 * @return -1 if the specified bus is not valid.
 */
int UART_setPostTransferDelay(UARTbus bus, portTickType delay);

/*!
 * This functions returns the actual amount of bytes read during the previous transfer. This
 * value might not necessarily be equal to the requested bytes to read because of a timeout
 * that might have occurred. This value is only updated at the end of a transfer.
 * @param bus The bus for which the previously read bytes should be returned.
 * @return -1 if the specified bus is not valid, else the actual bytes read during
 * the previous transfer.
 */
int UART_getPrevBytesRead( UARTbus bus );

/*!
 * Returns the state of driver for the given bus.
 * @param bus Bus for which the state should be returned
 * @param dir Direction of the transfer
 * @return If the bus is neither bus0_uart nor bus2_uart, returns idle_uartState.
 * otherwise, state of driver for the given bus.
 */
UARTdriverState UART_getDriverState(UARTbus bus, UARTdirection dir);

#endif /* UART_H_ */
