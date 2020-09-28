/*!
 * @file	I2C.h
 * @brief	I2C master communication driver.
 * @warning User may not change the entries in this header file. Values such as
 * define preprocessor directives are for information and not for configuration.
 * @date	Oct 26, 2012
 * @author	Akhil Piplani
 */

#ifndef I2C_H_
#define I2C_H_

#include <at91/commons.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

/*!
 * State of the Driver.
 */
typedef enum _I2CdriverState {
	idle_i2cState  = 0x00,         //!< idle_i2cState
	uninitialized_i2cState  = 0x33,//!< uninitialized_i2cState
	write_i2cState = 0x55,         //!< write_i2cState
	read_i2cState  = 0xAA,         //!< read_i2cState
	error_i2cState = 0xFF          //!< error_i2cState
} I2CdriverState;

/*!
 * Possible directions of an I2C transfer.
 */
typedef enum _I2Cdirection {
	write_i2cDir     = 0x55,//!< write_i2cDir
	read_i2cDir      = 0xAA,//!< read_i2cDir
	writeRead_i2cDir = 0xFF //!< writeRead_i2cDir
} I2Cdirection;

/*!
 * Status of a transfer being processed by the I2C driver.
 */
typedef enum _I2CtransferStatus {
	done_i2c = 0,                //!< done_i2c
	pending_i2c = 1,             //!< pending_i2c
	writeDone_i2c = 2,           //!< writeDone_i2c
	writeDoneReadStarted_i2c = 3,//!< writeDoneReadStarted_i2c
	writeError_i2c = 4,          //!< writeError_i2c
	readError_i2c = 5,           //!< readError_i2c
	timeoutError_i2c = 6,        //!< timeoutError_i2c
	error_i2c = 7                //!< error_i2c
} I2CtransferStatus;

/*!
 * A generic descriptor for an I2C transfer where every type of parameter of the transfer can be specified.
 * This is passed to I2C_queueTransfer.
 */
typedef struct _I2CgenericTransfer {
	unsigned int slaveAddress; //!< Address of the slave where to make the transfer.
	I2Cdirection direction; //!< Direction of the I2C transfer.
	unsigned int writeSize; //!< Number of bytes to be written to the I2C slave (used for write_i2cDir and writeRead_i2cDir).
	unsigned int readSize; //!< Number of bytes to be read from the I2C slave (used for read_i2cDir and writeRead_i2cDir).
	const unsigned char *writeData; //!< Memory location of the data to be written to the I2C slave (used for write_i2cDir and writeRead_i2cDir).
	volatile unsigned char *readData; //!< Memory location to store the data read from the I2C slave (used for read_i2cDir and writeRead_i2cDir).
	portTickType writeReadDelay; //!< A delay inserted between writing to an I2C slave and reading back from it (used only for writeRead_i2cDir).

	I2CtransferStatus *result; //!< The driver will store the result of a transfer in this location at the end of the transfer.

	// The following members are not inputs for blocking transfers.
	xSemaphoreHandle semaphore; //!< This will be passed to the callback. If the task is blocking on this semaphore for the transfer to complete, the callback can release it to unblock the task.
	void (*callback)(SystemContext context, xSemaphoreHandle sem); //!< Name of a function that will be called when the transfer is complete.

	// The following members are only used internally by the driver.
	portTickType writeCompleteTime; //!< Internal use by driver
	xTaskHandle callingTaskHandle; //!< Internal use by driver
} I2CgenericTransfer;

/*!
 * A descriptor for an I2C transfer that is passed to I2C_writeRead.
 */
typedef struct _I2CwriteReadTransfer {
	unsigned int slaveAddress; //!< Address of the slave where to make the transfer.
	unsigned int writeSize; //!< Number of bytes to be written to the I2C slave.
	unsigned int readSize; //!< Number of bytes to be read from the I2C slave.
	unsigned char *writeData; //!< Memory location of the data to be written to the I2C slave.
	volatile unsigned char *readData; //!< Memory location to store the data read from the I2C slave.
	portTickType writeReadDelay; //!< A delay inserted between writing to an I2C slave and reading back from it.
} I2Ctransfer;

/*!
 * Initializes the I2C driver.
 * @param i2cBusSpeed_Hz Speed of the I2C bus in Hertz. (Minimum: 10000, Maximum 400000)
 * @param i2cTransferTimeout Timeout for a byte to be transferred in 1/10th of a tick.
 * @note When debugging the code use portMAX_DELAY to make this timeout infinite.
 * Otherwise, stepping through code can trigger the I2C timeout.
 * If this value is less than 1, the driver uses a timeout of 1.
 * @return -3 if creating queues to handle transfers fails,
 * -2 if Initializing the TWI peripheral fails,
 * -1 if creating the task that consumes I2C transfer requests failed,
 * 0 on success.
 */
int I2C_start(unsigned int i2cBusSpeed_Hz, unsigned int  i2cTransferTimeout);

/*!
 * Sets the timeout for a read or write transaction to complete.
 * @param i2cTransferTimeout Timeout for a read or write transaction to complete in 1/10th of a tick.
 * If this value is less than 1, the driver uses a timeout of 1.
 * @note When debugging the code, use portMAX_DELAY to make this timeout infinite.
 * Otherwise, stepping through code can trigger the I2C timeout.
 */
void I2C_setTransferTimeout(unsigned int i2cTransferTimeout);

/*!
 * De-initializes the I2C driver.
 */
void I2C_stop(void);

/*!
 * Blocks the bus, preventing any further transmissions on it until released. If there is a transmission
 * ongoing, it will wait until *timeout* or the bus is released
 * @param timeout Length of time to wait for current transmission to finish (portMAX_DELAY to wait forever)
 * @return 0 on success, 1 if there was a timeout
 */
int I2C_blockBus(portTickType timeout);

/*!
 * Release back the bus.
 */
void I2C_releaseBus(void);

/*!
 * Writes data to a slave on the I2C bus using DMA.
 * This is a blocking call. The task will be swapped out of context while the transfer is in progress.
 * @param slaveAddress Address of the slave where to make the transfer.
 * @param data Memory location of the data to be written to the I2C slave.
 * @param size Number of bytes to be written to the I2C slave.
 * @return -3 if creating a semaphore to wait for the transfer fails,
 * -2 if the input transfer parameters are wrong or the driver is not initialized,
 * -1 if queuing the transfer fails,
 * 0 on success,
 * 4 on write error,
 * 6 on timeout error,
 * 7 on general error.
 * @note Negative values imply there was an initialization error,
 * so the transfer was never attempted on the I2C bus,
 * positive values mean a transfer was attempted on the I2C bus but failed.
 * @note Positive error values correspond in value to the members of I2CtransferStatus.
 */
int I2C_write(unsigned int slaveAddress, const unsigned char *data, unsigned int size);

/*!
 * Reads data from a slave on the I2C bus using DMA.
 * This is a blocking call. The task will be swapped out of context while the transfer is in progress.
 * @param slaveAddress Address of the slave where to make the transfer.
 * @param data Memory location to store the data read from the I2C slave. Must be able to store size bytes.
 * @param size Number of bytes to be read from the I2C slave.
 * @return -3 if creating a semaphore to wait for the transfer fails,
 * -2 if the input transfer parameters are wrong,
 * -1 if queuing the transfer fails,
 * 0 on success,
 * 5 on read error,
 * 6 on timeout error,
 * 7 on general error.
 * @note Negative values imply there was an initialization error,
 * so the transfer was never attempted on the I2C bus,
 * positive values mean a transfer was attempted on the I2C bus but failed.
 * @note Positive error values correspond in value to the members of I2CtransferStatus.
 */
int I2C_read(unsigned int slaveAddress, unsigned char *data, unsigned int size);

/*!
 * Writes and then reads data to/from a slave on the I2C bus using DMA.
 * This is a blocking call. The task will be swapped out of context while the transfer is in progress.
 * @param tx Pointer to a I2CwriteReadTransfer structure that specifies the parameters for this transfer.
 * @note If tx->readSize is zero but tx->writeSize is non-zero, this becomes a write transfer.
 * Conversely, If tx->writeSize is zero but tx->readSize is non-zero, this becomes a read transfer.
 * @return -3 if creating a semaphore to wait for the transfer fails,
 * -2 if the input transfer parameters are wrong,
 * -1 if queuing the transfer fails,
 * 0 on success.
 * 4 on write error,
 * 5 on read error,
 * 6 on timeout error,
 * 7 on general error.
 * @note Negative values imply there was an initialization error,
 * so the transfer was never attempted on the I2C bus,
 * positive values mean a transfer was attempted on the I2C bus but failed.
 * @note Positive error values correspond in value to the members of I2CtransferStatus.
 */
int I2C_writeRead(I2Ctransfer *tx);

/*!
 * Queues a transfer into the I2C driver. This is a non-blocking call.
 * The driver goes through its internal queue and processes transfers using DMA.
 * @param tx I2Ctransfer containing parameters for the transfer.
 * @return -2 if the input transfer parameters are wrong,
 * -1 if queuing the transfer fails,
 * 0 on success.
 */
int I2C_queueTransfer(I2CgenericTransfer *tx);

/*!
 * Returns the current state of the I2C driver.
 * @return I2CdriverState.
 */
I2CdriverState I2C_getDriverState(void);

/*!
 * Returns the status of the current transfer.
 * @return I2CtransferStatus
 */
I2CtransferStatus I2C_getCurrentTransferStatus(void);

#endif /* I2C_H_ */
