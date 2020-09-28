/**
 * @file IsisTxS.h
 * @brief ISIS S-band transmitter
 */

#ifndef ISISTXS_H_
#define ISISTXS_H_

/**
 *	The enumeration contains the possible attenuation options for the ISIS TXS.
 */
typedef enum _isis_txs_attenuation_t
{
    txs_0dB_attenuation = 0x00,
    txs_4_5dB_attenuation = 0x01,
    txs_9dB_attenuation = 0x02,
    txs_13_5dB_attenuation = 0x03
} isis_txs_attenuation_t;

/**
 *	The enumeration contains the possible bitrate options for the ISIS TXS.
 */
typedef enum _isis_txs_bitrate_t
{
    txs_bitrate_full = 0x01,
    txs_bitrate_half = 0x02,
    txs_bitrate_quarter = 0x03,
    txs_bitrate_eighth = 0x04
} isis_txs_bitrate_t;

/**
 *	The enumeration contains the possible state options for the ISIS TXS.
 */
typedef enum _isis_txs_state_t
{
    txs_state_off = 0x00,
    txs_state_on = 0x01
} isis_txs_state_t;

/**
 * The struct contains the parameter for the temperature threshold mechanism on the TXS
 */
typedef union __attribute__ ((__packed__)) _isis_txs_tempthreshold_t
{
	unsigned char raw;
	struct __attribute__ ((__packed__))
	{
		unsigned char threshold_enabled : 1, /**< Flag to enable (1) or disable (0) temperature monitoring on the TXS */
		temperature : 7; /**< Temperature in positive deg. C above which to switch off and reset the TXS */
	} fields;
} isis_txs_tempthreshold_t;

/**
 *	The enumeration contains the possible output modes options for the ISIS TXS.
 */
typedef enum _isis_txs_output_mode_t
{
    txs_output_mode_external = 0x01,
    txs_output_mode_nominal = 0x00
} isis_txs_output_mode_t;

#define LENGTH_HOUSEKEEPINGDATA			20 /**< The define size for a the TXS housekeeping data. */

/**
 *	The struct contains the possible first internal status variables for the TXS.
 */
typedef union _txs_status_data1_t
{
	unsigned char rawValue; /**< rawValue is the real value inside the union of the TXS Status Byte.  */
	struct
	{
		unsigned char TXS_STATE : 2, /**< TXS_STATE = 0x00 = OFF State, = 0x01 = ON State. */
		TX_ENABLE : 1, /**< TX_ENABLE = '0' indicates that the transmit functionality is being disabled. TX_ENABLE = '1' indicates that the transmit functionality is enabled. */
        PA_ENABLE : 1, /**< PA_ENABLE = '0' indicates that the power amplifier is inactive while PA_ENABLE = '1' indicates that the power amplifier is active. */
		PLL_LOCK : 2, /**< PLL_LOCK = 0x00 = PLL Inactive, = 0x01 = PLL Success, = 0x02 = PLL Failed, = 0x03 = PLL Busy. */
		TXS_MODE : 1; /**< TXS_MODE = 0x00 = Nominal, 0x01 = External. */
	} fields; /**< Individual status bits of the TXS Status Byte. */
} txs_status_data1_t;

/**
 *	The struct contains the possible second internal status variables for the TXS.
 */
typedef union _txs_status_data2_t
{
	unsigned char rawValue; /**< rawValue is the real value inside the union of the TXS Status Byte 2. */
	struct
	{
		unsigned char ATT_CTRL : 2, /**< Attenuation Control Bits. */
		LAST_RESET_CAUSE : 2, /**< Last Reset Cause Information Bits. */
		BIT_RATE_CTRL : 4; /**< Bit Rate Control Bits. */
	} fields; /**< Individual status bits of the TXS Status Byte. */
} txs_status_data2_t;

/**
 *	The enumeration contains the possible state changed values for the ISIS TXS.
 */
typedef enum _state_change_values_t
{
	power_on_reset = 0x00, /**< The Power On Reset has occured. */
	i2c_command = 0x01, /**< The I2C Command caused the TXS to change the mode. */
	pll_cant_lock = 0x02, /**< The PLL cannot lock. */
	temperature_switch = 0x03 /**< The temperature of the Power Amplifier getting too high that caused the state to change. */
} state_change_values_t;

/**
 * Structure for reporting data buffer space availability within the TXS
 */
typedef struct __attribute__ ((__packed__)) _isis_txs_buffavailable_t
{
	unsigned short bufferspace; /**< Number of bytes of data buffer space available */
	unsigned short maxframelength; /**< Maximum length of dataframe that can be added in bytes */
} isis_txs_buffavailable_t;

/**
 *	The union contains the TXS telemetry variables
 */
typedef union __attribute__ ((__packed__)) _isis_txs_telemetry_t
{
    unsigned char raw[LENGTH_HOUSEKEEPINGDATA];/**< Raw value array with Telemetry data*/
    struct __attribute__ ((__packed__))
    {
    	txs_status_data1_t first_status;
    	txs_status_data2_t second_status;
        unsigned int uptime;
        unsigned short txs_current;
        unsigned short txs_voltage;
        unsigned short txs_board_temp;
        unsigned short txs_pa_temp;
        state_change_values_t txs_state_info;
        unsigned char txs_temp_threshold;
        isis_txs_buffavailable_t txs_available_bytes;
    } fields;/**< Telemetry values*/
} isis_txs_telemetry_t;

/**
 *  @brief		Initialize the ISIS TXS with the corresponding TCM i2cAddress.
 *  @note		This function can only be called once.
 *  @param[in]	i2c_address array of TXS I2C Address structure.
 *  @param[in]	maxAX25frameLength Array with maximum AX.25 frame lengths
 *  @param[in]	number number of attached TXS in the system to be initialized.
 *
 *  @return  	Error code according to <hal/errors.h>
 */
int IsisTxsInitialize(unsigned char* i2c_address, unsigned int* maxAX25frameLength, unsigned char number);

/**
 *  @brief		Set the current modulation gain of the TXS transmitter.
 *  @param[in]	index index of ISIS TXS I2C bus address.
 *  @param[in]	att_index attenuation index for the TXS.
 *	@return 	Error code according to <hal/errors.h>
 */
int IsisTxsSetModulatorAttenuator(unsigned char index, isis_txs_attenuation_t att_index);

/**
 *  @brief		Set the AX.25 bitrate of the TXS transmitter.
 *  @param[in]	index index of ISIS TXS I2C bus address.
 *  @param[in]	bitrate AXbitrate of the TX.
 *	@return		Error code according to <hal/errors.h>
 */
int IsisTxsSetAx25Bitrate(unsigned char index, isis_txs_bitrate_t bitrate);

/**
 *  @brief		Set the current state of the TXS transmitter.
 *  @param[in]	index index of ISIS TXS I2C bus address.
 *  @param[in]	state The desired idle state of the TXS.
 *	@return		Error code according to <hal/errors.h>
 */
int IsisTxsSetState(unsigned char index, isis_txs_state_t state);

/**
 *  @brief		Tell the TXS to set a new default to callsign name.
 *  @param[in]	index index of ISIS TXS I2C bus address.
 *  @param[in]	to_callsign This variable will define the new 7 characters default to callsign.
 *	@return		Error code according to <hal/errors.h>
 */
int IsisTxsSetDefToClSign(unsigned char index, unsigned char* to_callsign);

/**
 *  @brief		Tell the TXS to set a new default from callsign name.
 *  @param[in]	index index of ISIS TXS I2C bus address.
 *  @param[in]	from_callsign This variable will define the new 7 characters default from callsign.
 *	@return		Error code according to <hal/errors.h>
 */
int IsisTxsSetDefFromClSign(unsigned char index, unsigned char* from_callsign);

/**
 *  @brief		Tell the TXS to set a new default from callsign and to callsign names.
 *  @param[in]	index index of ISIS TXS I2C bus address.
 *  @param[in]	from_callsign This variable will define the new 7 characters default from callsign.
 *  @param[in]	to_callsign This variable will define the new 7 characters default to callsign.
 *	@return		Error code according to <hal/errors.h>
 */
int IsisTxsSetDefBothFromToClSign(unsigned char index, unsigned char* from_callsign, unsigned char* to_callsign);

/**
 *  @brief		Set the output mode of the TXS.
 *  @param[in]	index index of ISIS TXS I2C bus address.
 *  @param[in]	mode TXS desired mode.
 *  @return		Error code according to <hal/errors.h>
 */
int IsisTxsSetMode(unsigned char index, isis_txs_output_mode_t mode);

/**
 *  @brief      Sets the temperature threshold and defines the functionality state.
 *  @param[in]  index index of ISIS TXS I2C bus address.
 *  @param[in]  tempthreshold Structure with the temperature limit and the enable / disable flag
 *  @return		Error code according to <hal/errors.h>
 */
int IsisTxsSetTempThreshold(unsigned char index, isis_txs_tempthreshold_t tempthreshold);

/**
 *  @brief      Reset the ISIS TXS Component.
 *  @param[in]  index index of ISIS TX I2C bus address.
 *  @return		Error code according to <hal/errors.h>
 */
int IsisTxsReset(unsigned char index);

/**
 *  @brief      Tell the TXS to transmit an AX.25 message with override callsigns and specified content.
 *  @param[in]  index index of ISIS TXS I2C bus address.
 *  @param[in]	from_callsign This variable will define the new 7 characters from callsign.
 *  @param[in]	to_callsign This variable will define the new 7 characters to callsign.
 *  @param[in]  data Pointer to the array containing the data to put in the AX.25 message.
 *  @param[in]  length Length of the data to be put in the AX.25 message.
 *  @param[out] avail Pointer to storage for information regarding the TXS data buffer space availability after this frame has been added. If set to NULL then data will not be read from TXS.
 *  @return		Error code according to <hal/errors.h>
 */
int IsisTxsSendAX25OvrClSign(unsigned char index, unsigned char* from_callsign, unsigned char* to_callsign, unsigned char* data, unsigned short length, isis_txs_buffavailable_t* avail);

/**
 *  @brief      Tell the TXS to transmit an AX.25 message with default callsigns and specified content.
 *  @param[in]  index index of ISIS TXS I2C bus address.
 *  @param[in]  data Pointer to the array containing the data to put in the AX.25 message.
 *  @param[in]  length Length of the data to be put in the AX.25 message.
 *  @param[out] avail Pointer to storage for information regarding the TXS data buffer space availability after this frame has been added. If set to NULL then data will not be read from TXS.
 *  @return		Error code according to <hal/errors.h>
 */
int IsisTxsSendAX25DefClSign(unsigned char index, unsigned char* data, unsigned short length, isis_txs_buffavailable_t* avail);

/**
 *  @brief		Retrieve a block of telemetry from the TXS
 *  @param[in]  index index of ISIS TXS I2C bus address.
 *  @param[out] telemetry Pointer to the union where the telemetry should be stored.
 *  @return		Error code according to <hal/errors.h>
 */
int IsisTxsGetHousekeepingData(unsigned char index, isis_txs_telemetry_t* telemetry);

/**
 *  @brief      Retrieve the number of bytes available in the TXS available.
 *  @param[in]  index index of ISIS TXS I2C bus address.
 *  @param[out] avail Pointer to storage for information regarding the TXS data buffer space availability after this frame has been added. If set to NULL then data will not be read from TXS.
 *  @return		Error code according to <hal/errors.h>
 */
int IsisTxsGetAvailableBytes(unsigned char index, isis_txs_buffavailable_t* avail);

#endif /* ISISTXS_H_ */
