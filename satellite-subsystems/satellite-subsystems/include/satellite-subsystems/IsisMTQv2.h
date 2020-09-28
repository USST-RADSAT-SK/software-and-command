/**
 * @file IsisMTQv2.h
 * @brief ISIS Magnetorquer board version 2
 */

#ifndef ISISMTQV2_H_
#define ISISMTQV2_H_

// Operational Commands
#define ISIS_MTQ_I2CCMD_RESET					0xAA ///< I2C command performs a software reset of the MCU.
#define ISIS_MTQ_I2CCMD_NOP                 	0x02 ///< I2C command performs a no-operation.
#define ISIS_MTQ_I2CCMD_CANCELOP            	0x03 ///< I2C command mode idle cancels any ongoing actuation.
#define ISIS_MTQ_I2CCMD_STARTMTM    			0x04 ///< I2C command start a 3-axis measurement of the magnetic field.
#define ISIS_MTQ_I2CCMD_STARTMTQ_ACTCURR		0x05 ///< I2C command turns on the MTQ at the specified level for a given duration. Current input.
#define ISIS_MTQ_I2CCMD_STARTMTQ_ACTDIP     	0x06 ///< I2C command turns on the MTQ at the specified level for a given duration. Dipole input.
#define ISIS_MTQ_I2CCMD_STARTMTQ_ACTPWM     	0x07 ///< I2C command turns on the MTQ at the specified level for a given duration. PWM duty cycle input.
#define ISIS_MTQ_I2CCMD_SWITCH_SELFMODE     	0x08 ///< I2C command swith to self-test mode.
#define ISIS_MTQ_I2CCMD_SWITCH_DETMODE      	0x09 ///< I2C command switch to detumble mode for a given duration.

// Data Request Commands
#define ISIS_MTQ_I2CCMD_GET_SYSTEMSTATE      	0x41 ///< I2C command return system status information.
#define ISIS_MTQ_I2CCMD_GET_MTM_RAWDATA      	0x42 ///< I2C command returns raw MTM measurement data from previously started measurement.
#define ISIS_MTQ_I2CCMD_GET_MTM_CALDATA      	0x43 ///< I2C command returns calibrated MTM data from previously started measurement.
#define ISIS_MTQ_I2CCMD_GET_COIL_CURRMSR		0x44 ///< I2C command returns coil current measurements.
#define ISIS_MTQ_I2CCMD_GET_COIL_TEMPMSR		0x45 ///< I2C command returns coil temperature measurements.
#define ISIS_MTQ_I2CCMD_GET_CMDACT_DIPOLE		0x46 ///< I2C command returns commanded actuation dipole.
#define ISIS_MTQ_I2CCMD_GET_SELFTESTDATA		0x47 ///< I2C command returns the result of the last executed self-test.
#define ISIS_MTQ_I2CCMD_GET_DETUMBLEDATA        0x48 ///< I2C command returns telemetry of the detumble mode.
#define ISIS_MTQ_I2CCMD_GET_HK_RAWDATA          0x49 ///< I2C command returns raw housekeeping data.
#define ISIS_MTQ_I2CCMD_GET_HK_ENGDATA       	0x4A ///< I2C command returns engineering housekeeping data.

// Configuration Commands
#define ISIS_MTQ_I2CCMD_GET_PARAMETER      		0x81 ///< I2C command get configuration parameter.
#define ISIS_MTQ_I2CCMD_SET_PARAMETER          	0x82 ///< I2C command set configuration parameter.
#define ISIS_MTQ_I2CCMD_RESET_PARAMETER         0x83 ///< I2C command reset configuration parameter to its default hard-coded value.

// Data sizes (excluding CC and STAT) in bytes
#define IMTQ_INPUTS_SIZE 	 	  				8
#define IMTQ_SYSTEMSTATE_SIZE 	  				7
#define IMTQ_MAGAXIS_SIZE         				13
#define IMTQ_COILCURRENT_SIZE     				6
#define IMTQ_COILTEMP_SIZE        				6
#define IMTQ_CMDACTDIP_SIZE       				6
#define IMTQ_SELDATA_STEP_SIZE 					38
#define IMTQ_DETDATA_SIZE         				66
#define IMTQ_HKDATA_SIZE       	  				22

/**
 * Enumeration of all IMTQ modes
 */
typedef enum __attribute__ ((__packed__)) _imtq_mode_t
{
	imtq_mode_idle = 0, ///< Idle mode
	imtq_mode_selftest = 1, ///< Selftest mode
	imtq_mode_detumble = 2 ///< Detumble mode
} imtq_mode_t;

/**
 *  Enumeration of all IMTQ parameter types.
 */
typedef enum __attribute__ ((__packed__)) _imtq_gnlparam_t
{
    imtq_int8_val = 0x01, ///< The parameter is an integer type signed 8 bits value
    imtq_uint8_val = 0x02, ///< The parameter is an integer type unsigned 8 bits value
    imtq_int16_val = 0x03, ///< The parameter is an integer type signed 16 bits value
    imtq_uint16_val = 0x04, ///< The parameter is an integer type unsigned 16 bits value
    imtq_int32_val = 0x05, ///< The parameter is an integer type signed 32 bits value
    imtq_uint32_val = 0x06, ///< The parameter is an integer type unsigned 32 bits value
    imtq_float_val = 0x07, ///< The parameter is a float type value
    imtq_int64_val = 0x08, ///< The parameter is an integer type signed 8 bits value
    imtq_uint64_val = 0x09, ///< The parameter is an integer type signed 8 bits value
    imtq_double_val = 0x0A ///< The parameter is a double type value
} imtq_gnlparam_t;

/**
 *  Enumeration of all self-test axis-direction possibilities.
 */
typedef enum __attribute__ ((__packed__)) _imtq_sel_axdir_t
{
	imtq_sel_axdir_all = 0x00, ///< All axes
	imtq_sel_axdir_posx = 0x01, ///< X+ axis
	imtq_sel_axdir_negx = 0x02, ///< X- axis
	imtq_sel_axdir_posy = 0x03, ///< Y+ axis
	imtq_sel_axdir_negy = 0x04, ///< Y- axis
	imtq_sel_axdir_posz = 0x05, ///< Z+ axis
	imtq_sel_axdir_negz = 0x06 ///< Z- axis
} imtq_sel_axdir_t;

/**
 *  Enumeration of all possible failures.
 */
typedef enum __attribute__ ((__packed__)) _imtq_sel_error_t
{
	imtq_sel_error_none = 0x00, ///< Self Test no error.
	imtq_sel_error_i2c_failure = 0x01, ///< Self Test i2c failure.
	imtq_sel_error_spi_failure = 0x02, ///< Self Test spi failure.
	imtq_sel_error_adc_failure = 0x04, ///< Self Test adc failure.
	imtq_sel_error_pwm_failure = 0x08, ///< Self Test pwm failure.
	imtq_sel_error_tc_failure = 0x10, ///< Self Test tc failure.
	imtq_sel_error_mtm_outofrange = 0x20, ///< Self Test mtm out of range failure.
	imtq_sel_error_coilcurr_outofrange = 0x40 ///< Self Test coil current out of range failure.
} imtq_sel_error_t;

/**
 *  Enumeration of all possible test steps.
 */
typedef enum __attribute__ ((__packed__)) _imtq_sel_step_t
{
	imtq_sel_step_init = 0x00, ///< Self-test initial step
	imtq_sel_step_posx = 0x01, ///< Self-test +x step
	imtq_sel_step_negx = 0x02, ///< Self-test -x step
	imtq_sel_step_posy = 0x03, ///< Self-test +y step
	imtq_sel_step_negy = 0x04, ///< Self-test -y step
	imtq_sel_step_posz = 0x05, ///< Self-test +z step
	imtq_sel_step_negz = 0x06, ///< Self-test -z step
	imtq_sel_step_fina = 0x07 ///< Self-test final step
} imtq_sel_step_t;

/**
 *  Enumeration of all possible error codes.
 */
typedef enum __attribute__ ((__packed__)) _imtq_accepted_error_t
{
	imtq_cmd_accepted = 0x00, ///< Stat cmd error accepted
	imtq_cmd_rejected = 0x01, ///< Stat cmd error rejected
	imtq_cmd_rejected_invalid = 0x02, ///< Rejected: invalid command code
	imtq_cmd_rejected_parmiss = 0x03, ///< Rejected: parameter missing
	imtq_cmd_rejected_parinv = 0x04, ///< Rejected: parameter invalid
	imtq_cmd_rejected_ccunav = 0x05, ///< Rejected: CC unavailable in current mode
	imtq_cmd_reserved = 0x06, ///< Reserved value
	imtq_cmd_internal_error = 0x07 ///< Internal error occurred during processing
} imtq_accepted_error_t;

/**
 *  Struct for command default response structure.
 */
typedef union __attribute__ ((__packed__)) _imtq_statcmd_t
{
    /** Raw value **/
    unsigned char raw[2];
    struct __attribute__ ((__packed__))
    {
        unsigned char command_code; ///< iMTQ response command code.
        unsigned char cmd_error : 4, ///< iMTQ error command code.
        iva_z : 1, ///< iMTQ measurement retrieval code, Z axis.
        iva_y : 1, ///< iMTQ measurement retrieval code, Y axis.
        iva_x : 1, ///< iMTQ measurement retrieval code, X axis.
        new : 1; ///< iMTQ new stat register flag.
    }fields;
} imtq_statcmd_t;

/**
 *  Struct for iMTQ activation inputs.
 */
typedef union __attribute__ ((__packed__)) _imtq_inputs_t
{
    /** Raw value **/
    unsigned char raw[IMTQ_INPUTS_SIZE];
    struct __attribute__ ((__packed__))
    {
        short input[3]; ///< iMTQ coil input vector
        unsigned short duration; ///< iMTQ input activation time.
    }fields;
} imtq_inputs_t;

/**
 *  Struct for iMTQ current system state.
 */
typedef union __attribute__ ((__packed__)) _imtq_systemstate_t
{
    /** Raw value **/
    unsigned char raw[IMTQ_SYSTEMSTATE_SIZE];
    struct __attribute__ ((__packed__))
    {
    	imtq_mode_t mode; ///< iMTQ system state mode.
        unsigned char err; ///< iMTQ system state error.
        unsigned char conf; ///< iMTQ system state configuration.
        unsigned int uptime; ///< iMTQ system state uptime.
    }fields;
} imtq_systemstate_t;

/**
 *  Struct for iMTQ raw magnetometer axes.
 */
typedef union __attribute__ ((__packed__)) _imtq_raw_magf_t
{
    /** Raw value **/
    unsigned char raw[IMTQ_MAGAXIS_SIZE];
    struct __attribute__ ((__packed__))
    {
        int raw_magf[3]; ///< iMTQ raw magnetometer vector.
        unsigned char coilact; ///< iMTQ coil was actuating during measurement.
    }fields;
} imtq_raw_magf_t;

/**
 *  Struct for iMTQ calibrated magnetometer axes.
 */
typedef union __attribute__ ((__packed__)) _imtq_cal_magf_t
{
    /** Raw value **/
    unsigned char raw[IMTQ_MAGAXIS_SIZE];
    struct __attribute__ ((__packed__))
    {
        int cal_magf[3]; ///< iMTQ calibrated magnetometer vector.
        unsigned char coilact; ///< iMTQ coil was actuating during measurement.
    }fields;
} imtq_cal_magf_t;
    
/**
 *  Struct for iMTQ coil current axes.
 */
typedef union __attribute__ ((__packed__)) _imtq_coilcurr_t
{
    /** Raw value **/
    unsigned char raw[IMTQ_COILCURRENT_SIZE];
    struct __attribute__ ((__packed__))
    {
        short coilcurr[3]; ///< iMTQ coil current vector.
    }fields;
} imtq_coilcurr_t;

/**
 *  Struct for iMTQ coil temperature axes.
 */
typedef union __attribute__ ((__packed__)) _imtq_coiltemp_t
{
    /** Raw value **/
    unsigned char raw[IMTQ_COILTEMP_SIZE];
    struct __attribute__ ((__packed__))
    {
        short coiltemp[3]; ///< iMTQ coil temperature vector.
    }fields;
} imtq_coiltemp_t;

/**
 *  Struct for iMTQ commanded actuation dipole.
 */
typedef union __attribute__ ((__packed__)) _imtq_cmdactdip_t
{
    /** Raw value **/
    unsigned char raw[IMTQ_CMDACTDIP_SIZE];
    struct __attribute__ ((__packed__))
    {
        short cmdactdip[3]; ///< iMTQ commanded actuation dipole vector.
    }fields;
} imtq_cmdactdip_t;

/**
 *  IMTQv2 self-test structure which contains the results of a single self-test step.
 */
typedef union __attribute__ ((__packed__)) _imtq_seldata_step_t
{
    /** Raw value **/
    unsigned char raw[IMTQ_SELDATA_STEP_SIZE + sizeof(imtq_statcmd_t)];
    struct __attribute__ ((__packed__))
    {
    	imtq_statcmd_t rsp_code; ///< self-test step response code.
    	imtq_sel_error_t error; ///< self-test step error.
    	imtq_sel_step_t step; ///< self-test step.
        int raw_magf[3]; ///< raw magnetic field measurement vector.
        int cal_magf[3]; ///< calibrated magnetic field measurement vector.
        short coilcurr[3]; ///< coil current measurement vector.
        short coiltemp[3]; ///< coil temperature measurement vector.
    }fields;
} imtq_seldata_step_t;

/**
 *  Struct for iMTQ self-test which can contain the results of a single-axis-direction selftest.
 */
typedef union __attribute__ ((__packed__)) _imtq_seldata_single_axdir_t
{
    /** Raw value **/
	unsigned char raw[3 * (IMTQ_SELDATA_STEP_SIZE + sizeof(imtq_statcmd_t))];
    struct __attribute__ ((__packed__))
    {
    	imtq_seldata_step_t step_init; ///< self-test step INIT result information; this step measures the initial magnetic field and coil currents while not torquing
    	imtq_seldata_step_t step_axac; ///< self-test axis actuation result information; this step measures the magnetic field and coil currents while torquing the requested axis-direction
    	imtq_seldata_step_t step_fina; ///< self-test step FINA result information; this step measures the final magnetic field and coil currents while not torquing
    }fields;
} imtq_seldata_single_axdir_t;

/**
 *  Struct for iMTQ self test which can contain the results of an all-axis-direction selftest.
 */
typedef union __attribute__ ((__packed__)) imtq_seltdata_all_axdir_t
{
    /** Raw value **/
	unsigned char raw[8 * (IMTQ_SELDATA_STEP_SIZE + sizeof(imtq_statcmd_t))];
    struct __attribute__ ((__packed__))
    {
    	imtq_seldata_step_t step_init; ///< self-test step INIT result information; this step measures the initial magnetic field and coil currents while not torquing
    	imtq_seldata_step_t step_posx; ///< self-test step +X result information; this step measures the magnetic field and coil currents while torquing +X
    	imtq_seldata_step_t step_negx; ///< self-test step -X result information; this step measures the magnetic field and coil currents while torquing -X
    	imtq_seldata_step_t step_posy; ///< self-test step +Y result information; this step measures the magnetic field and coil currents while torquing +Y
    	imtq_seldata_step_t step_negy; ///< self-test step -Y result information; this step measures the magnetic field and coil currents while torquing -Y
    	imtq_seldata_step_t step_posz; ///< self-test step +Z result information; this step measures the magnetic field and coil currents while torquing +Z
    	imtq_seldata_step_t step_negz; ///< self-test step -Z result information; this step measures the magnetic field and coil currents while torquing -Z
    	imtq_seldata_step_t step_fina; ///< self-test step FINA result information; this step measures the final magnetic field and coil currents while not torquing
    }fields;
} imtq_seldata_all_axdir_t;

/**
 *  Struct for iMTQ detumble data results.
 */
typedef union __attribute__ ((__packed__)) _imtq_detumble_data_t
{
    /** Raw value **/
    unsigned char raw[IMTQ_DETDATA_SIZE];
    struct __attribute__ ((__packed__))
    {
        int cal_magf[3]; ///< iMTQ calibrated magnetometer vector.
        int filt_magf[3]; ///< iMTQ filtered magnetometer vector.
        int bdot[3]; ///< iMTQ B-dot vector.
        short cmdactdip[3]; ///< iMTQ commanded dipole vector.
        short coilcurr_cmd[3]; ///< iMTQ commanded current vector.
        short coilcurr_meas[3]; ///< iMTQ measured current vector.
    }fields;
} imtq_detumble_data_t;

/**
 *  Struct for iMTQ raw housekeeping data results.
 */
typedef union __attribute__ ((__packed__)) _imtq_rawhk_data_t
{
    /** Raw value **/
    unsigned char raw[IMTQ_HKDATA_SIZE];
    struct __attribute__ ((__packed__))
    {
    	unsigned short digit_volt; ///< Voltage measurement of the digital supply in raw ADC counts.
    	unsigned short anl_volt; ///< Voltage measurement of the analog supply in raw ADC counts.
    	unsigned short digit_curr; ///< Current measurement of the digital supply in raw ADC counts.
    	unsigned short anl_curr; ///< Current measurement of the analog supply in raw ADC counts.
    	unsigned short coilcurr[3]; ///< Coil current measurement in raw ADC counts.
    	unsigned short coiltemp[3]; ///< Coil temperature measurement in raw ADC counts.
    	unsigned short mcu_temp; ///< Temperature measurement of the MCU in raw ADC counts.
    }fields;
} imtq_rawhk_data_t;

/**
 *  Struct for iMTQ engineering housekeeping data results.
 */
typedef union __attribute__ ((__packed__)) _imtq_enghk_data_t
{
    /** Raw value **/
    unsigned char raw[IMTQ_HKDATA_SIZE];
    struct __attribute__ ((__packed__))
    {
    	unsigned short digit_volt; ///< Voltage measurement of the digital supply.
    	unsigned short anl_volt; ///< Voltage measurement of the analog supply.
    	unsigned short digit_curr; ///< Current measurement of the digital supply.
    	unsigned short anl_curr; ///< Current measurement of the analog supply.
    	short coilcurr[3]; ///< Coil current measurement.
    	short coiltemp[3]; ///< Coil temperature measurement.
    	short mcu_temp; ///< Temperature measurement of the MCU.
    }fields;
} imtq_enghk_data_t;

/**
 *  @brief      Initialize the iMTQ with the corresponding i2cAddress.
 *  @note       This function can only be called once.
 *  @param[in]  p_address array of iMTQ I2C Address structure.
 *  @param[in]  number number of attached iMTQ in the system to be initialized.
 *  @return     Error code according to <hal/errors.h>
 */
int IsisMTQv2_initialize(unsigned char *p_address, unsigned char number);

/**
 *  @brief       Software Resets the iMTQ device.
 *  @param[in]   index index of iMTQ I2C bus address.
 *  @param[out]  p_rsp_code represents the default response for every command.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisMTQv2_softReset(unsigned char index, imtq_statcmd_t* p_rsp_code);
    
/**
 *  @brief       No operation command for the iMTQ device.
 *  @param[in]   index index of iMTQ I2C bus address.
 *  @param[out]  p_rsp_code represents the default response for every command.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisMTQv2_noOperation(unsigned char index, imtq_statcmd_t* p_rsp_code);
    
/**
 *  @brief       Cancel current operation in the iMTQ device.
 *  @param[in]   index index of iMTQ I2C bus address.
 *  @param[out]  p_rsp_code represents the default response for every command.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisMTQv2_cancelOperation(unsigned char index, imtq_statcmd_t* p_rsp_code);
    
/**
 *  @brief       Start MTM measurement in the iMTQ device.
 *  @param[in]   index index of iMTQ I2C bus address.
 *  @param[out]  p_rsp_code represents the default response for every command.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisMTQv2_startMTMMeasurement(unsigned char index, imtq_statcmd_t* p_rsp_code);

/**
 *  @brief       Start MTQ actuation with current parameters in the iMTQ device.
 *  @param[in]   index index of iMTQ I2C bus address.
 *  @param[in]   current_input current iMTQ input.
 *  @param[out]  p_rsp_code represents the default response for every command.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisMTQv2_startMTQActuationCurrent(unsigned char index, imtq_inputs_t current_input, imtq_statcmd_t* p_rsp_code);
    
/**
 *  @brief       Start MTQ actuation with dipole parameters in the iMTQ device.
 *  @param[in]   index index of iMTQ I2C bus address.
 *  @param[in]   dipole_input dipole iMTQ input.
 *  @param[out]  p_rsp_code represents the default response for every command.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisMTQv2_startMTQActuationDipole(unsigned char index, imtq_inputs_t dipole_input, imtq_statcmd_t* p_rsp_code);

/**
 *  @brief       Start MTQ actuation with pwm duty cycle parameters in the iMTQ device.
 *  @param[in]   index index of iMTQ I2C bus address.
 *  @param[in]   pwm_input pwm duty cycle iMTQ input.
 *  @param[out]  p_rsp_code represents the default response for every command.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisMTQv2_startMTQActuationPWM(unsigned char index, imtq_inputs_t pwm_input, imtq_statcmd_t* p_rsp_code);
    
/**
 *  @brief       Start self-test for the iMTQ device. This performs magnetic measurements and actuates the torquer coils to verify proper operation of these elements.
 *  @param[in]   index index of iMTQ I2C bus address.
 *  @param[in]   axis_input indicates the axis and direction of the self-test. Can be set to 'all' to test every axis and direction in succession with a single call. Use the appropriate GetSelftestDataSingle/All() command to retrieve results.
 *  @param[out]  p_rsp_code represents the default response for every command. NOTE: this is *not* the result of the self-test, it only indicates command acceptance.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisMTQv2_startSelfTest(unsigned char index, imtq_sel_axdir_t axis_input, imtq_statcmd_t* p_rsp_code);

/**
 *  @brief       Start detumble mode of the iMTQ device.
 *  @param[in]   index index of iMTQ I2C bus address.
 *  @param[in]   duration detumbling duration time.
 *  @param[out]  p_rsp_code represents the default response for every command.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisMTQv2_startDetumble(unsigned char index, unsigned short duration, imtq_statcmd_t* p_rsp_code);
    
/**
 *  @brief       Get current IMTQ system state.
 *  @param[in]   index index of iMTQ I2C bus address.
 *  @param[out]  p_sys_state structure with system state data.
 *  @param[out]  p_rsp_code represents the default response for every command.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisMTQv2_getSystemState(unsigned char index, imtq_systemstate_t* p_sys_state, imtq_statcmd_t* p_rsp_code);

/**
 *  @brief       Get raw magnetometer measurement result data from a previously started measurement using startMTMMeasurement.
 *  @param[in]   index index of iMTQ I2C bus address.
 *  @param[out]  p_raw_magf structure with raw magnetometer data.
 *  @param[out]  p_rsp_code represents the default response for every command.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisMTQv2_getRawMTMData(unsigned char index, imtq_raw_magf_t* p_raw_magf, imtq_statcmd_t* p_rsp_code);

/**
 *  @brief       Get calibrated magnetometer measurement result data from a previously started measurement using startMTMMeasurement.
 *  @param[in]   index index of iMTQ I2C bus address.
 *  @param[out]  p_cal_magf structure with calibrated magnetometer data.
 *  @param[out]  p_rsp_code represents the default response for every command.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisMTQv2_getCalMTMData(unsigned char index, imtq_cal_magf_t* p_cal_magf, imtq_statcmd_t* p_rsp_code);

/**
 *  @brief       Get coil current measurement.
 *  @param[in]   index index of iMTQ I2C bus address.
 *  @param[out]  p_coilcurr structure with coil current measurement.
 *  @param[out]  p_rsp_code represents the default response for every command.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisMTQv2_getCoilCurrent(unsigned char index, imtq_coilcurr_t* p_coilcurr, imtq_statcmd_t* p_rsp_code);

/**
 *  @brief       Get coil temperature measurement.
 *  @param[in]   index index of iMTQ I2C bus address.
 *  @param[out]  p_coiltemp structure with coil temperature measurement.
 *  @param[out]  p_rsp_code represents the default response for every command.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisMTQv2_getCoilTemperature(unsigned char index, imtq_coiltemp_t* p_coiltemp, imtq_statcmd_t* p_rsp_code);

/**
 *  @brief       Get commanded actuation dipole.
 *  @param[in]   index index of iMTQ I2C bus address.
 *  @param[out]  p_cmdactdip structure with commanded actuation dipole.
 *  @param[out]  p_rsp_code represents the default response for every command.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisMTQv2_getCmdActuationDipole(unsigned char index, imtq_cmdactdip_t* p_cmdactdip, imtq_statcmd_t* p_rsp_code);

/**
 *  @brief       Get selftest data results for a single-axis-direction self-test. Use this function only when the startSelfTest was called with an axis-direction other than 'all'.
 *  @param[in]   index index of iMTQ I2C bus address.
 *  @param[out]  p_selftest single axis self-test result information.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisMTQv2_getSelftestDataSingleAxis(unsigned char index, imtq_seldata_single_axdir_t* p_selftest);

/**
 *  @brief       Get selftest data results for an all-axis-direction self-test. Use this function only when the startSelfTest was called with an axis-direction equal to 'all'.
 *  @param[in]   index index of iMTQ I2C bus address.
 *  @param[out]  p_selftest all axis self-test result information.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisMTQv2_getSelftestDataAllAxis(unsigned char index, imtq_seldata_all_axdir_t* p_selftest);

/**
 *  @brief       Get detumble data results.
 *  @param[in]   index index of iMTQ I2C bus address.
 *  @param[out]  p_det_data structure with detumble data results.
 *  @param[out]  p_rsp_code represents the default response for every command.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisMTQv2_getDetumbleData(unsigned char index, imtq_detumble_data_t* p_det_data, imtq_statcmd_t* p_rsp_code);

/**
 *  @brief       Get housekeeping data in raw counts.
 *  @param[in]   index index of iMTQ I2C bus address.
 *  @param[out]  p_rawhk_data structure with raw housekeeping data results.
 *  @param[out]  p_rsp_code represents the default response for every command.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisMTQv2_getRawHKData(unsigned char index, imtq_rawhk_data_t* p_rawhk_data, imtq_statcmd_t* p_rsp_code);

/**
 *  @brief       Get housekeeping data in engineering values.
 *  @param[in]   index index of iMTQ I2C bus address.
 *  @param[out]  p_enghk_data structure with engineering housekeeping data results.
 *  @param[out]  p_rsp_code represents the default response for every command.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisMTQv2_getEngHKData(unsigned char index, imtq_enghk_data_t* p_enghk_data, imtq_statcmd_t* p_rsp_code);

/**
 *  @brief       Get system parameter size in bytes.
 *  @param[in]   param_id parameter id.
 *  @param[out]  p_par_sz size of the parameter data in bytes, inferred from the parameter id.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisMTQv2_getParSize(unsigned short param_id, unsigned int* p_par_sz);

/**
 *  @brief       Get iMTQ system parameters.
 *  @param[in]   index index of iMTQ I2C bus address.
 *  @param[in]   param_id parameter id.
 *  @param[out]  vp_rsp_param pointer to variable where requested parameter data will be placed.
 *  @param[out]  p_rsp_code represents the default response for every command.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisMTQv2_getParameter(unsigned char index, unsigned short param_id, void* vp_rsp_param, imtq_statcmd_t* p_rsp_code);

/**
 *  @brief       Set iMTQ system parameters.
 *  @param[in]   index index of iMTQ I2C bus address.
 *  @param[in]   param_id parameter id.
 *  @param[in]   vp_param_input parameter to set.
 *  @param[out]  vp_rsp_param pointer to variable where requested parameter data will be placed.
 *  @param[out]  p_rsp_code represents the default response for every command.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisMTQv2_setParameter(unsigned char index, unsigned short param_id, void* vp_param_input, void* vp_rsp_param, imtq_statcmd_t* p_rsp_code);

/**
 *  @brief       Reset iMTQ system parameters.
 *  @param[in]   index index of iMTQ I2C bus address.
 *  @param[in]   param_id parameter id.
 *  @param[out]  vp_rsp_param pointer to variable where requested parameter data will be placed.
 *  @param[out]  p_rsp_code represents the default response for every command.
 *  @return      Error code according to <hal/errors.h>
 */
int IsisMTQv2_resetParameter(unsigned char index, unsigned short param_id, void* vp_rsp_param, imtq_statcmd_t* p_rsp_code);
    
#endif /* ISISMTQV2_H_ */
