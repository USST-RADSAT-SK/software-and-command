/**
 *  @file IsisTRXUV.h
 *  @brief ISIS UHF up / VHF down transceiver (revision B3 and B4)
 *  @note based on TRXUV Manual 1.3.2
 */
#ifndef ISISTRXUV_H_
#define ISISTRXUV_H_

#define TRXUV_HIGH_CWCHAR_BOUND			1200  ///< High boundary for a define new CW character rate.
#define TRXUV_LOW_CWCHAR_BOUND			   1  ///< Low boundary for a define new CW character rate.
#define TRXUV_UPTIME_SIZE				   3  ///< Size for the up time buffer.
#define TRXUV_VOLT_CURRENT_BUFFER_SIZE	   6  ///< Size for the Voltage and Current consumption buffer.
#define TRXUV_ALL_TELEMETRY_SIZE		  16  ///< Size of the buffer for the complete telemetry read out.

/**
 *  Enumeration of all ADC Channels in the TRXUV.
 */
typedef enum __attribute__ ((__packed__)) _ISIStrxuvADCchannels
{
    adc_channel_telemetry_dopler_voltage = 0x10,
    adc_channel_telemetry_rssi = 0x11,
    adc_channel_telemetry_reflected_power = 0x12,
    adc_channel_telemetry_forward_power = 0x13,
    adc_channel_telemetry_tx_current = 0x14,
    adc_channel_telemetry_rx_current = 0x15,
    adc_channel_telemetry_pa_temp = 0x16,
    adc_channel_telemetry_bus_voltage = 0x17,
} ISIStrxuvADCchannels;

/**
 *  Enumeration of all output modes of the TRXUV.
 */
typedef enum __attribute__ ((__packed__)) _ISIStrxuvOutputMode
{
    trxuv_output_mode_external = 0x00,
    trxuv_output_mode_nominal = 0x01,
    trxuv_output_mode_loopback = 0x03
} ISIStrxuvOutputMode;

/**
 *  Enumeration of all idle states of the TRXUV.
 */
typedef enum __attribute__ ((__packed__)) _ISIStrxuvIdleState
{
    trxuv_idle_state_off = 0x00,
    trxuv_idle_state_on = 0x01
} ISIStrxuvIdleState;

/**
 *  Enumeration of bitrate options of the TRXUV.
 */
typedef enum __attribute__ ((__packed__)) _ISIStrxuvBitrate
{
    trxuv_bitrate_1200 = 0x01, ///< Transmission Bitrate 1200 bps.
    trxuv_bitrate_2400 = 0x02, ///< Transmission Bitrate 2400 bps.
    trxuv_bitrate_4800 = 0x04, ///< Transmission Bitrate 4800 bps.
    trxuv_bitrate_9600 = 0x08 ///< Transmission Bitrate 9600 bps.
} ISIStrxuvBitrate;

/**
 * Enumeration list of bitrate options of the TRXUV when reporting the status
 */
typedef enum __attribute__ ((__packed__)) _ISIStrxuvBitrateStatus
{
    trxuv_bitratestatus_1200 = 0x00, ///< Transmission Bitrate 1200 bps.
    trxuv_bitratestatus_2400 = 0x01, ///< Transmission Bitrate 2400 bps.
    trxuv_bitratestatus_4800 = 0x02, ///< Transmission Bitrate 4800 bps.
    trxuv_bitratestatus_9600 = 0x03 ///< Transmission Bitrate 9600 bps.
} ISIStrxuvBitrateStatus;

/**
 *  Enumeration of TRXUV components.
 */
typedef enum __attribute__ ((__packed__)) _ISIStrxuvComponent
{
    trxuv_imc = 0x00, ///< TRXUV IMC component.
    trxuv_itc = 0x01 ///< TRXUV ITC component.
} ISIStrxuvComponent;

/**
 *  Enumeration of TRXUV Downlink mode enumeration.
 */
typedef enum __attribute__ ((__packed__)) _ISIStrxuvDownlink
{
    trxuv_downlink_ax25 = 0x00, ///< TRXUV downlink AX25 mode.
    trxuv_downlink_cw = 0x01 ///< TRXUV downlink CW mode.
} ISIStrxuvDownlink;

/**
 * Enumeration list of TRXVU beacon status.
 */
typedef enum __attribute__ ((__packed__)) _ISIStrxuvBeacon
{
    trxuv_beacon_none = 0x00, ///< TRXUV beacon none status.
    trxuv_beacon_active = 0x01 ///< TRXUV beacon active status.
} ISIStrxuvBeacon;

/**
 *  Struct for defining ISIS TRXUV I2C Address.
 */
typedef struct _ISIStrxuvI2CAddress
{
    unsigned char addressImc; ///< I2C address of the IMC.
    unsigned char addressItc; ///< I2C address of the ITC.
} ISIStrxuvI2CAddress;

/**
 *  Struct for defining ISIS TRXUV buffers length.
 */
typedef struct _ISIStrxuvFrameLengths
{
    unsigned int maxAX25frameLengthTX; ///< AX25 maximum frame size for transmission.
    unsigned int maxAX25frameLengthRX; ///< AX25 maximum frame size for reception.
    unsigned int maxCWframeLength; ///< CW maximum frame size for transmission.
} ISIStrxuvFrameLengths;

/**
 *  Struct for the TRXUV telemetry.
 */
typedef union __attribute__ ((__packed__)) _ISIStrxuvTelemetry
{
	/** Raw value array with Telemetry data*/
    unsigned char raw[TRXUV_ALL_TELEMETRY_SIZE];
    /** Telemetry values*/
    struct __attribute__ ((__packed__))
    {
        unsigned short rx_doppler; ///< Telemetry receiver doppler.
        unsigned short rx_rssi; ///< Telemetry rssi measurement.
        unsigned short tx_reflpower; ///< Telemetry reflected power.
        unsigned short tx_fwdpower; ///< Telemetry forward power.
        unsigned short tx_current; ///< Telemetry transmitter current.
        unsigned short rx_current; ///< Telemetry receiver doppler.
        unsigned short pa_temp; ///< Telemetry power amplifier temperature.
        unsigned short bus_volt; ///< Telemetry bus voltage.
    } fields;
} ISIStrxuvTelemetry;

/**
 *  Struct for the TRXUV reception frame.
 */
typedef struct __attribute__ ((__packed__)) _ISIStrxuvRxFrame
{
    unsigned char rx_length; ///< Reception frame length.
    unsigned char* rx_framedata; ///< Reception frame data.
} ISIStrxuvRxFrame;

/**
 *  Struct for the TRXUV current buffer status.
 */
typedef union __attribute__ ((__packed__)) _ISIStrxuvTransmitterState
{
	/** Raw value that contains the current transmitter state*/
    unsigned char raw;
    struct __attribute__ ((__packed__))
    {
        ISIStrxuvOutputMode transmitter_output_mode : 2; ///< Transmitter output mode.
        ISIStrxuvBitrateStatus transmitter_bitrate : 2; ///< Transmitter current bitrate.
        ISIStrxuvDownlink transmitter_downlink_type : 1;  ///< Transmitter downlink type.
        ISIStrxuvBeacon transmitter_beacon : 1; ///< Transmitter beacon mode status.
        ISIStrxuvIdleState transmitter_idle_state : 2; ///< Transmitter current idle state.
    }fields;
} ISIStrxuvTransmitterState;

/**
 *  @brief      Initialize the ISIS TRXUV with the corresponding i2cAddress from the array of TRXUV I2C Address structure.
 *  @note       This function can only be called once.
 *  @param[in]  address array of TRXUV I2C Address structure.
 *  @param[in]  maxFrameLengths array of maximum frame length structures for TRXUV.
 *  @param[in]	default_bitrates initial default bitrates.
 *  @param[in]  number number of attached TRXUV in the system to be initialized.
 *  @return     Error code according to <hal/errors.h>
 */
int IsisTrxuv_initialize(ISIStrxuvI2CAddress *address, ISIStrxuvFrameLengths *maxFrameLengths, ISIStrxuvBitrate* default_bitrates, unsigned char number);

/**
 *  @brief       Reset the ISIS TRXUV Component.
 *  @param[in]   index index of ISIS TRXUV I2C bus address.
 *  @param[in]   component TRXUV component, either ITC or IMC.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisTrxuv_componentReset(unsigned char index, ISIStrxuvComponent component);

/**
 *  @brief       Reset the ISIS TRXUV IMC and ITC.
 *  @param[in]   index index of ISIS TRXUV I2C bus address.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisTrxuv_reset(unsigned char index);

/**
 *  @brief       Tell the TRXUV to transmit an AX.25 message with default callsigns and specified content.
 *  @param[in]   index index of ISIS TRXUV I2C bus address.
 *  @param[in]   data Pointer to the array containing the data to put in the AX.25 message.
 *  @param[in]   length Length of the data to be put in the AX.25 message.
 *  @param[out]  avail Number of the available slots in the transmission buffer of the ITC after the frame has been added.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisTrxuv_itcSendAX25DefClSign(unsigned char index, unsigned char *data, unsigned char length, unsigned char *avail);

/**
 *  @brief       Tell the TRXUV to transmit an AX.25 message with override callsigns and specified content.
 *  @param[in]   index index of ISIS TRXUV I2C bus address.
 *  @param[in]	 fromCallsign This variable will define the new 7 characters from callsign.
 *  @param[in]	 toCallsign This variable will define the new 7 characters to callsign.
 *  @param[in]   data Pointer to the array containing the data to put in the AX.25 message.
 *  @param[in]   length Length of the data to be put in the AX.25 message.
 *  @param[out]  avail Number of the available slots in the transmission buffer of the ITC after the frame has been added.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisTrxuv_itcSendAX25OvrClSign(unsigned char index, unsigned char *fromCallsign, unsigned char *toCallsign, unsigned char *data, unsigned char length, unsigned char *avail);

/**
 *  @brief       Tell the TRXUV to transmit a CW message.
 *  @param[in]   index index of ISIS TRXUV I2C bus address.
 *  @param[in]   data Pointer to the array containing the message to be transmitted.
 *  @param[in]   length Length of the message.
 *  @param[out]  avail Number of the available slots in the transmission buffer of the ITC after the frame has been added.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisTrxuv_itcSendCwMessage(unsigned char index, char *data, unsigned char length, unsigned char *avail);

/**
 *  @brief       Tell the TRXUV to set the parameters for the AX25 Beacon with default callsigns.
 *  @param[in]   index index of ISIS TRXUV I2C bus address.
 *  @param[in]   data Pointer to the array containing the message to be transmitted.
 *  @param[in]   length Length of the message.
 *  @param[in]   interval Interval of beacon transmission.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisTrxuv_itcSetAx25BeaconDefClSign(unsigned char index, unsigned char *data, unsigned char length, unsigned short interval);

/**
 *  @brief       Tell the TRXUV to set the parameters for the AX25 Beacon with override callsigns.
 *  @param[in]   index index of ISIS TRXUV I2C bus address.
 *  @param[in]	 fromCallsign This variable will define the new 7 characters from callsign.
 *  @param[in]	 toCallsign This variable will define the new 7 characters to callsign.
 *  @param[in]   data Pointer to the array containing the message to be transmitted.
 *  @param[in]   length Length of the message.
 *  @param[in]   interval Interval of beacon transmission.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisTrxuv_itcSetAx25BeaconOvrClSign(unsigned char index, unsigned char *fromCallsign, unsigned char *toCallsign, unsigned char *data, unsigned char length, unsigned short interval);

/**
 *  @brief       Tell the TRXUV to set the CW beacon.
 *  @param[in]   index index of ISIS TRXUV I2C bus address.
 *  @param[in]   data Pointer to the array containing the message to be transmitted.
 *  @param[in]   length Length of the message.
 *  @param[in]   interval Interval of beacon transmission.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisTrxuv_itcSetCwBeacon(unsigned char index, char *data, unsigned char length, unsigned short interval);

/**
 *  @brief       Tell the TRXUV to clear the current beacon.
 *  @param[in]   index index of ISIS TRXUV I2C bus address.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisTrxuv_itcClearBeacon(unsigned char index);

/**
 *  @brief       Tell the TRXUV to set a new default to callsign name.
 *  @param[in]   index index of ISIS TRXUV I2C bus address.
 *  @param[in]   toCallsign This variable will define the new 7 characters default to callsign.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisTrxuv_itcSetDefToClSign(unsigned char index, unsigned char *toCallsign);

/**
 *  @brief       Tell the TRXUV to set a new default from callsign name.
 *  @param[in]   index index of ISIS TRXUV I2C bus address.
 *  @param[in]   fromCallsign This variable will define the new 7 characters default from callsign.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisTrxuv_itcSetDefFromClSign(unsigned char index, unsigned char *fromCallsign);

/**
 *  @brief       Set the idle state of the TRXUV transmitter, i.e. the state in between transmission.
 *  @param[in]   index index of ISIS TRXUV I2C bus address.
 *  @param[in]   state The desired idle state of the TRXUV.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisTrxuv_itcSetIdlestate(unsigned char index, ISIStrxuvIdleState state);

/**
 *  @brief       Set the output mode of the TRXUV.
 *  @param[in]   index index of ISIS TRXUV I2C bus address.
 *  @param[in]   mode Output mode desired.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisTrxuv_itcSetOutputMode(unsigned char index, ISIStrxuvOutputMode mode);

/**
 *  @brief       Set the AX.25 bitrate of the TRXUV transmitter.
 *  @param[in]   index index of ISIS TRXUV I2C bus address.
 *  @param[in]   bitrate The desired AX.25 bitrate of the TRXUV.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisTrxuv_itcSetAx25Bitrate(unsigned char index, ISIStrxuvBitrate bitrate);

/**
 *  @brief       Set the CW character transmission rate of the TRXUV transmitter.
 *  @param[in]   index index of ISIS TRXUV I2C bus address.
 *  @param[in]   newValue The new value to calculate the CW character rate.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisTrxuv_itcSetCWCharRate(unsigned char index, unsigned short newValue);

/**
 *  @brief       Retrieve the current time of operation of the TRXUV transmitter.
 *  @param[in]   index index of ISIS TRXUV I2C bus address.
 *  @param[out]  uptime This array of 3 characters contains the operation time of the transmitter (Minutes, Hours and Days, in that order).
 *  @return      Error code according to <hal/errors.h>
 */
int IsisTrxuv_itcGetUptime(unsigned char index, unsigned char *uptime);

/**
 *  @brief       Retrieve the current transmitter status.
 *  @param[in]   index index of ISIS TRXUV I2C bus address.
 *  @param[out]  currentItcState Pointer to the union where the current status should be stored.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisTrxuv_itcGetState(unsigned char index, ISIStrxuvTransmitterState *currentItcState);

/**
 *  @brief       Calculates the approximate time it will take for a certain transmission to complete.
 *  @param[in]   index index of ISIS TRXUV I2C bus address.
 *  @param[in]   length The length of the transmission in bytes.
 *  @return      The time estimate in milliseconds.
 */
unsigned short IsisTrxuv_itcEstimateTransmissionTime(unsigned char index, unsigned char length);

/**
 *  @brief       Retrieve the number of telecommand frames present in the receive buffer of the TRXUV.
 *  @param[in]   index index of ISIS TRXUV I2C bus address.
 *  @param[out]  frameCount The number of telecommand frames in the buffer.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisTrxuv_imcGetFrameCount(unsigned char index, unsigned char *frameCount);

/**
 *  @brief       Retrieve and delete a telecommand frame from the TRXUV.
 *  @param[in]   index index of ISIS TRXUV I2C bus address.
 *  @param[out]  rxframe Pointer to the struct where the telecommand frame should be store.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisTrxuv_imcGetCommandFrame(unsigned char index, ISIStrxuvRxFrame* rxframe);

/**
 *  @brief       Obtain individual telemetry values of the ADC Channels from the IMC.
 *  @param[in]   index index of ISIS TRXUV I2C bus address.
 *  @param[in]   channel ADC Channel selected.
 *  @param[out]  resultValue The ADC value read by the IMC.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisTrxuv_imcGetTelemetry(unsigned char index, ISIStrxuvADCchannels channel, unsigned short *resultValue);

/**
 *  @brief       Retrieve a block of telemetry from the TRXUV.
 *  @param[in]   index index of ISIS TRXUV I2C bus address.
 *  @param[out]  telemetry Pointer to the union where the telemetry should be stored.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisTrxuv_imcGetTelemetryAll(unsigned char index, ISIStrxuvTelemetry *telemetry);

/**
 *  @brief       Retrieve the telemetry of the power bus voltage and current consumptions .
 *  @param[in]   index index of ISIS TRXUV I2C bus address.
 *  @param[out]  dataRead Pointer to the data .
 *  @return      Error code according to <hal/errors.h>
 */
int IsisTrxuv_imcGetVoltageCurrent(unsigned char index, unsigned char *dataRead);

/**
 *  @brief       Retrieve the current time of operation of the TRXUV receiver.
 *  @param[in]   index index of ISIS TRXUV I2C bus address.
 *  @param[out]  uptime This array of 3 characters contains the operation time of the receiver (Minutes, Hours and Days, in that order)..
 *  @return      Error code according to <hal/errors.h>
 */
int IsisTrxuv_imcGetUptime(unsigned char index, unsigned char *uptime);

#endif /* ISISTRXUV_H_ */
