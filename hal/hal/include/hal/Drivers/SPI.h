/*!
 * @file	SPI.h
 * @brief	SPI master communication.
 * @warning User may not change the entries in this header file. Values such as
 * define preprocessor directives are for information and not for configuration.
 * @date	Jan 16, 2013
 * @author	Akhil Piplani
 */

#ifndef SPI_H_
#define SPI_H_

#include <at91/commons.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#define MHZ(x)	(x*1000*1000)

#define SPI_MAX_CLOCK_DIVIDER	254
/*!
 * Minimum speed of the SPI bus clock signal.
 */
#define SPI_MIN_BUS_SPEED	(BOARD_MCK / SPI_MAX_CLOCK_DIVIDER)
/*!
 * Maximum speed of the SPI bus clock signal.
 */
#define SPI_MAX_BUS_SPEED	MHZ(10)

/*!
 *  State of the Driver.
 */
typedef enum _SPIdriverState {
	idle_spiState  = 0x00,         //!< idle_spiState
	uninitialized_spiState  = 0x33,//!< uninitialized_spiState
	error_spiState = 0x55,         //!< error_spiState
	transfer_spiState = 0xFF       //!< transfer_spiState
} SPIdriverState;

/*!
 * The SPI mode in which a transfer should take place.
 */
typedef enum _SPItransferMode {
	mode0_spi = 0x2,//!< mode0_spi
	mode1_spi = 0x0,//!< mode1_spi
	mode2_spi = 0x3,//!< mode2_spi
	mode3_spi = 0x1 //!< mode3_spi
} SPImode;

/*!
 * The SPI bus on which the transfer should be made.
 */
typedef enum _SPIbus {
	bus0_spi = 0,//!< bus0_spi Used for transfers within the board.
	bus1_spi = 1,//!< bus1_spi Used for transfers ourside the board.
	both_spi = 2 //!< Only to be used with SPI_start and SPI_stop.
} SPIbus;

/*!
 * The SPI slave with which a transfer should be made.
 */
typedef enum _SPIslave {
	slave0_spi = 0, //!< slave0_spi In case of bus0, this is the FRAM.
	slave1_spi = 1, //!< slave1_spi In case of bus0, this is the RTC.
	slave2_spi = 2, //!< slave2_spi In case of bus0, this is the Supervisor Microcontroller(PIC).
	slave3_spi = 3, //!< slave3_spi Only available for bus1, uses a GPIO-connector pin, see board.h.
	slave4_spi = 4, //!< slave4_spi Only available for bus1, uses a GPIO-connector pin, see board.h.
	slave5_spi = 5, //!< slave5_spi Only available for bus1, uses a GPIO-connector pin, see board.h.
	slave6_spi = 6, //!< slave6_spi Only available for bus1, uses a GPIO-connector pin, see board.h.
	slave7_spi = 7, //!< slave7_spi Only available for bus1, uses a GPIO-connector pin, see board.h.
} SPIslave;

typedef struct _SPIslaveParameters {
	SPIbus bus; //!< The SPI bus on which the transfer should be made.
	SPImode mode; //!< The SPI mode in which a transfer should take place.
	SPIslave slave; //!< The SPI slave with which a transfer should be made.
	unsigned char dlybs; //!< Delay between CS-low to first clock transition. See AT91SAM9G20 datasheet.
	unsigned char dlybct; //!< Delay between consecutive bytes to the same slave. See AT91SAM9G20 datasheet.
	unsigned int busSpeed_Hz; //!< The speed in Hertz of the SPI clock signal. Minimum SPI_MIN_BUS_SPEED and Maximum SPI_MAX_BUS_SPEED.
	/*!
	 * For adding delays between successive transaction to the same slave.
	 * The driver will wait the specified amount of ticks before it proceeds to process the next transfer in the queue.
	 */
	portTickType postTransferDelay;
} SPIslaveParameters;

typedef struct _SPItransfer {
	unsigned int transferSize; //!< Number of bytes to transfer.
	const unsigned char *writeData; //!< Location of the data to be written. Both buffers must be provided and must accommodate transferSize bytes.
	volatile unsigned char *readData; //!< Location where the driver can write the data read from the SPI bus. Both buffers must be provided and must accommodate transferSize bytes.

	const SPIslaveParameters *slaveParams; //!< SPI transaction parameters that usually stay the same for a given slave.

	// Following members are not inputs for SPI_writeRead
	xSemaphoreHandle semaphore; //!< This binary sempahore will be passed to the callback. If the task is blocking on this semaphore for the transfer to complete, the callback can release it to unblock the task.
	void (*callback)(SystemContext context, xSemaphoreHandle sem); //!< Callback to indicate the transfer is finished, For the SPI-1 bus the context is always ISR.
} SPItransfer;

/*!
 * Initializes the specified SPI bus(es).
 * @param bus The SPI bus to initialize.
 * @param spi1_max_cs Specify the maximum chip select that can be used for SPI bus 1
 * Please note that the first 3 chip selects, i.e. 0, 1, and 2, will always be initialized and
 * that this parameter will only be taken into account when initializing SPI bus 1 and ignored otherwise
 * @return -1 on error, 0 for success.
 */
int SPI_start(SPIbus bus, SPIslave spi1_max_cs);

/*!
 * De-initializes the specified SPI bus(es).
 * @param bus The SPI bus to stop.
 */
void SPI_stop(SPIbus bus);

/*!
 * Queues a transfer to be made by the SPI driver.
 * The SPI driver can concurrently accept multiple transfers from multiple tasks.
 * Tasks within the driver sequentially execute the queued transfers using DMA.
 * This allows the calling task to continue with its work without having to block
 * for its transfer or for transfers from preceding tasks.
 *
 * When the transfer is done, the driver will call the specified callback.
 *
 * @note Both write and read buffers of equal size must be specified as SPI is a full duplex bus.
 * @param tx SPItransfer structure specifying the details of the tasks.
 * The recommended way of using this structure is to have one structure with its
 * unchanging values, such as mode initialized once and only the particulars of the
 * specific transfer such as transferSize and writeData modified for individual transfers.
 * @return -3 if the bus/mode parameters are wrong or input pointers are null or transferSize is 0.
 * -2 if the bus speed is out of range or the driver is not initialized,
 * -1 if adding transfer to the queue fails,
 * 0 on success.
 */
int SPI_queueTransfer(SPItransfer *tx);

/*!
 * Performs a blocking transfer by sleeping the task until the specified transfer is complete.
 * This allows other tasks to execute while the transfer is made using DMA but the calling task
 * is blocked.
 * @param tx SPItransfer structure specifying the details of the tasks.
 * @note Both write and read buffers of equal size must be specified as SPI is a full duplex bus.
 * @return -4 if creating a semaphore for the transfer fails,
 * -3 if the bus/mode parameters are wrong or input pointers are null or transferSize is 0.
 * -2 if the bus speed is out of range,
 * -1 if adding transfer to the queue fails,
 * 0 on success.
 */
int SPI_writeRead(SPItransfer *tx);

/*!
 * Returns the state of driver for the given bus.
 * @param bus Bus for which the state should be returned
 * @return If the bus is neither bus0_spi nor bus1_spi, returns idle_spiState.
 * otherwise, state of driver for the given bus.
 */
SPIdriverState SPI_getDriverState(SPIbus bus);

#endif /* SPI_H_ */
