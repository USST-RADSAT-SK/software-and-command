/*!
 * @file	I2Cslave.h
 * @brief	I2C slave communication.
 * @date	Feb 7, 2011
 * @author	Akhil Piplani
 */

#ifndef I2CCOMM_H_
#define I2CCOMM_H_

#include "hal/boolean.h"
#include <at91/commons.h>
#include <freertos/FreeRTOS.h>

/*!
 * @brief Describes the I2C Slave Driver state.
 */
typedef enum _I2CslaveDriverState {
	uninitialized_i2cSlaveDriverState = 0x00,//!< uninitialized_i2cSlaveDriverState
	idle_i2cSlaveDriverState = 0x5A,         //!< idle_i2cSlaveDriverState
	slaveAcess_i2cSlaveDriverState = 0x55,   //!< slaveAcess_i2cSlaveDriverState
	read_i2cSlaveDriverState = 0xAA,         //!< read_i2cSlaveDriverState
	write_i2cSlaveDriverState = 0xFF         //!< write_i2cSlaveDriverState
} I2CslaveDriverState;

/*!
 * Information used by the driver to automatically receive a fixed number of
 * bytes. Note, the number of bytes specified should not include the command.
 * itself. Any command not in this list is treated as an invalid command.
 *
 * This driver expects the following I2C transaction format:\n
 * 1) Every request for data from the I2C master is preceded by a command that describes what data is being requested.
 * Therefore, an I2C slave never tries to write data on the I2C bus on its own.\n
 * 2) Commands may or may not have a response.
 */
typedef struct _I2Cslave_CommandList
{
    unsigned char command; //!< Command code (first byte sent by the I2C master).
    //One byte of padding will occur here
    unsigned short commandParameterSize; //!< Number of bytes to read after receiving the command. If the command does not have any parameters, this should be 0.
    Boolean hasResponse; //!< The user code replies to to this command with a response data packet i.e. I2Cslave_write is called in response to this command.
} I2CslaveCommandList;

/*!
 *
 * Sets up the I2C slave driver.
 *
 * @param address Address of the slave on the I2C bus.
 * @param commandList Pointer to an array of I2CslaveCommandList structures which the driver
 * queries to find out the number of bytes to automatically receive after receiving a certain command.
 * @param commandListSize Number of commands in the above list.
 * i.e. ((size of commandList array in bytes) / sizeof(I2CslaveCommandList))
 *
 * @return -5 when no command list is provided or its size is invalid or the address is not a 7-bit number,
 * -4 if a commandParameterSize is too large (>=512),
 * -3 if command codes in the list are not unique,
 * -2 if memory allocation or starting tasks fails,
 * -1 if pin configuration fails,
 * 0 on success.
 *
 * @note The command codes in the commandList must be unique.
 * @see I2CslaveCommandList
 */
int I2Cslave_start(unsigned char address, I2CslaveCommandList *commandList, unsigned int commandListSize);

/*!
 * @brief Disables all I2C related interrupts and powers the TWI peripheral down.
 */
void I2Cslave_stop(void);

/*!
 * Returns the internal receive buffer size. This is identical to the largest
 * command provided in I2Cslave_start
 * @return Buffer sized used for receiving messages
 */
unsigned short I2Cslave_getBufferSize(void);

/*!
 * @brief Writes data TO the master on the I2C bus.
 * @param data Pointer to a location where the data to write is stored.
 * @param size Number of bytes to transfer.
 * @param timeout Number of milliseconds to wait for the transfer to complete. Use portMAX_DELAY to wait indefinitely.
 *
 * @return -3 if the driver has not been initialized,
 * -2 if size is 0,
 * -1 if timeout occurs while attempting to send data,
 * otherwise the number of bytes written to the bus.
 *
 * @note If this value is positive but not equal to size, then the I2C-master terminated the transfer early with a NACK.
 * @note If this function is called while a write transfer is in progress,
 *  it will wait for the previous transfer to complete before the current one is started.
 * @note This function must always be called in response to a command from the I2C master.
 *
 * @note IMPORTANT: When the I2C master requests data, the driver will clock-stretch the I2C bus until this function is called.
 * This allows you some time to send the data.\n
 * However, there is a timeout on the clock-stretching of 200ms to avoid indefinite clock-stretching.
 * If this timeout is reached, the driver will send 0xEE to the I2C master (indicating an error).
 *
 */
int I2Cslave_write(const unsigned char *data, unsigned long size, portTickType timeout);

/*!
 * @brief Reads data from the I2C bus.
 * In reality, the I2C slave can start receiving data as soon as it is initialized.
 * This function simply waits until actual data is available and returns when the i2c master sends a command.
 * @param[in,out] data A pointer to the location where the driver should store the received data. This buffer must be at least I2C_SLAVE_RECEIVE_BUFFER_SIZE bytes large.
 * @return The number of bytes received as a command,
 * if the return value is -1, the driver has not been initialized,
 * if the return value is less than the number of bytes expected for the command code, the I2C-master terminated the transfer early.
 */
int I2Cslave_read(unsigned char *data);

/*!
 * @brief Retrieves the current state of the I2C driver.
 * @return driver state.
 */
I2CslaveDriverState I2Cslave_getDriverState(void);

/*!
 * @brief Mutes the I2C interface from the higher layers but does not block the
 * I2C bus.
 *
 * This function will cause the I2C driver to accept any amount of data from the
 * master and reply with any amount of junk when requested by the master.
 * This can be used to prevent the I2C bus from hanging when this subsystem is
 * busy doing something that will make it unresponsive on the I2C bus.
 *
 * @param replyByte A byte that will be repeatedly transmitted when the
 * master asks for data.
 *
 * @see I2Cslave_unMute()
 */
void I2Cslave_mute(unsigned char replyByte);

/*!
 * @brief Unmutes the I2C interface. This function should be called after calling
 * I2C_Mute when the subsystem has finished doing tasks that made it unavailable
 * on the I2C bus.
 *
 * @see I2Cslave_mute()
 */
void I2Cslave_unMute(void);

#endif /* I2CCOMM_H_ */
