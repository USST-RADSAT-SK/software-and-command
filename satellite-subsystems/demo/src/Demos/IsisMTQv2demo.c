/*
 * IsisMTQv2demo.c
 *
 *  Created on: 13 mrt. 2015
 *      Author: malv
 */
#include "IsisMTQv2demo.h"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include <at91/utility/exithandler.h>
#include <at91/commons.h>
#include <at91/utility/trace.h>
#include <at91/peripherals/cp15/cp15.h>

#include <hal/Utility/util.h>
#include <hal/Timing/WatchDogTimer.h>
#include <hal/Drivers/I2C.h>
#include <hal/Drivers/LED.h>
#include <hal/boolean.h>
#include <hal/errors.h>

#include <satellite-subsystems/IsisMTQv2.h>

#include <Demos/common.h>

#include <stddef.h>			// for size_t
#include <stdlib.h>
#include <stdio.h>
#include <string.h>			// used for memset/memcpy/strncpy functions
#include <stdbool.h>		// 'normal' boolean values; note: these are incompatible with the ones defined by Boolean.h in the hal library

#include <math.h>			// for the exp function used in the user input parser function ing()
#include <float.h>			// floating point min and max values
#include <stdint.h>			// integer data type min and max values
#include <inttypes.h>		// printf format codes

//#if USING_GOM_EPS == 1
//#include <SatelliteSubsystems/GomEPS.h>
//#endif



///***************************************************************************************************************************
///
/// Implementers notice:
/// This file contains example code that aids building understanding of how to interface with the subsystem
/// using the subsystems library. Validation code has been kept to a minimum in an effort not to obfuscate
/// the code that is needed for calling the subsystem. Be cautious and critical when using portions of this
/// code in your own projects. It is *not* recommended to copy/paste this code into your projects,
/// instead use the example as a guide while building your modules separately.
///
///***************************************************************************************************************************


///***************************************************************************************************************************
///
/// Notes on the parameter system:
/// the parameter system requires an unsigned short parameter_index
/// and a pointer to the location of the data to be get/set/reset
/// the void pointer can be pointing to an array or directly to a strongly typed variable
///
/// note that the parameter index consists of: param-type in the highest nibble (4 bits), and the ordinal index in the
/// lower 3 nibbles (12 bits). Optionally the fifth to highest bit of the 16 bits is used to indicate read/only
/// e.g.: 0xA802 = third read-only double, 0xA002 = third read/write double
///
/// example 1: getting param 0x1000 (which is the first int8):
///		unsigned short param_id = 0x1000;
///		char param_val;										// value that receives the gotten variable information
///
///		rv = IsisMTQv2_getParameter(0, param_id, &param_val, &rsp_stat);
///
/// example 2: setting param 0xA000 (which is the first 8 byte double):
///		unsigned short param_id = 0xA000;
///		double param_inp = 12.01;							// value that is set
///		double param_outp;									// value that is read back after update for verification purposes
///
///		rv = IsisMTQv2_setParameter(0, param_id, &param_inp, &param_outp &rsp_stat);
///
/// example 3: setting param 0x7003 (which is the fourth 4 byte float) using a (little endian) byte array as input:
///		unsigned short param_id = 0x7003;
///		unsigned char barr_inp[4] = {0x10,0x32,0x54,0x76};	// 4 byte values making up the 4 byte float variable
///		float param_outp;									// value that is read back after update for verification purposes
///
///		rv = IsisMTQv2_setParameter(0, param_id, barr_inp, &param_outp, &rsp_stat);
///
///	Generally usage within the I2C master falls in either of two categories: direct-use and pass-through
/// *direct-use 	This entails having the I2C master changing the configuration parameters autonomously.
///					Use of pointers to typed variables is most convenient in this case. The system will only see a void
///					pointer, hence the user must ensure a pointer to a variable with the correct parameter type (i.e. size!)
///					is supplied with any param-id.
/// *pass-through 	This entails having the ground station provide a byte stream to be passed to the subsystem.
///					Use of pointers to byte arrays is most convenient in this case. The system automatically derives the
///  				amount of bytes that needs to be read from the pointer location.
///***************************************************************************************************************************


///***************************************************************************************************************************
///
/// Demo helper functions:
/// below are helper functions specific to the demo application for interacting with the user and presenting results
///
/// The subsystem calls can be found below!
///
///***************************************************************************************************************************

static void _parse_resp(imtq_statcmd_t* p_rsp_code)
{
	// this function parses the response that is provided as the result of
	// issuing a command to the subsystem. It provides information on whether the response
	// was accepted for processing, not necessarily whether the command succeeded because
	// generally too much processing time is required for executing the command to allow
	// waiting for a response. Generally separate calls can be made to verify successful
	// command execution, usually that call includes any output data gather during command
	// execution.
	// In case of getting measurement results the inva status indicates whether issues
	// were encountered during measurement causing the axis value to become suspect
	if(p_rsp_code == NULL)
	{
		TRACE_ERROR(" internal error: p_rsp_code is NULL");
		return;
	}

	if(p_rsp_code->fields.new)																	// is the new flag set?
	{
		printf(" \t new = %d (new response/data)\r\n", p_rsp_code->fields.new);					// indicate its a never before retrieved response
	}
	else
	{
		printf(" \t new = %d (old response/data)\r\n", p_rsp_code->fields.new);					// indicate we've read this response before
	}

	printf(" \t IVA x,y,z = %d, %d, %d \r\n", p_rsp_code->fields.iva_x, p_rsp_code->fields.iva_y,
				p_rsp_code->fields.iva_z); // parse axis invalid markers

	switch(p_rsp_code->fields.cmd_error)
	{
	case imtq_cmd_accepted: 		///< Accepted
		printf(" \t cmd_err = %d (command accepted) \r\n", p_rsp_code->fields.cmd_error);
		break;
	case imtq_cmd_rejected: 		///< Rejected: no reason indicated
		printf(" \t cmd_err = %d (!REJECTED! command rejected) \r\n", p_rsp_code->fields.cmd_error);
		break;
	case imtq_cmd_rejected_invalid: ///< Rejected: invalid command code
		printf(" \t cmd_err = %d (!REJECTED! command code invalid) \r\n", p_rsp_code->fields.cmd_error);
		break;
	case imtq_cmd_rejected_parmiss: ///< Rejected: parameter missing
		printf(" \t cmd_err = %d (!REJECTED! command parameter missing) \r\n", p_rsp_code->fields.cmd_error);
		break;
	case imtq_cmd_rejected_parinv: ///< Rejected: parameter invalid
		printf(" \t cmd_err = %d (!REJECTED! command parameter invalid) \r\n", p_rsp_code->fields.cmd_error);
		break;
	case imtq_cmd_rejected_ccunav: ///< Rejected: CC unavailable in current mode
		printf(" \t cmd_err = %d (!REJECTED! command unavailable in current mode) \r\n", p_rsp_code->fields.cmd_error);
		break;
	case imtq_cmd_reserved: 	   ///< Reserved value
		printf(" \t cmd_err = %d (!REJECTED! command caused return of reserved response code) \r\n", p_rsp_code->fields.cmd_error);
		break;
	case imtq_cmd_internal_error: ///< Internal error occurred during processing
		printf(" \t cmd_err = %d (!REJECTED! command caused internal error) \r\n", p_rsp_code->fields.cmd_error);
		break;
	}
}

static void _parse_selftest_step(imtq_seldata_step_t *p_step)
{
	// this function shows the self-test result information for a self-test-step to the user.

	if(p_step == NULL)										// if no valid step pointer is provided we'll only print out generic information
	{
		// only print info on the step data
		printf(" \t error = %s \r\n", "error bitflag indicating any problems that the IMTQv2 encountered while performing the self-test-step.");
		printf(" \t step = %s \r\n", "self-test-step indicating the coil operation that was being performed at the time the measurements were taken");
		printf(" \t raw_magf = %s \r\n", "magnetometer field strength in raw counts");
		printf(" \t cal_magf = %s \r\n", "magnetometer field strength in nano-tesla [10E-9 T]");
		printf(" \t coilcurr = %s \r\n", "coil current in [10E-4 A]");
		printf(" \t coiltemp = %s \r\n", "coil temperature in degrees C");

		return;
	}

	_parse_resp(&p_step->fields.rsp_code);																		// step response information

	printf(" \t error = %04x", p_step->fields.error);																// error field information (this is a bitflag field which can have multiple flags set at the same time)
	if(p_step->fields.error == imtq_sel_error_none) printf(" ( none ) \r\n");
	else
	{
		printf(" ( ");
		if(p_step->fields.error & imtq_sel_error_i2c_failure) printf("imtq_sel_error_i2c_failure ");
		if(p_step->fields.error & imtq_sel_error_spi_failure) printf("imtq_sel_error_spi_failure ");
		if(p_step->fields.error & imtq_sel_error_adc_failure) printf("imtq_sel_error_adc_failure ");
		if(p_step->fields.error & imtq_sel_error_pwm_failure) printf("imtq_sel_error_pwm_failure ");
		if(p_step->fields.error & imtq_sel_error_tc_failure) printf("imtq_sel_error_tc_failure ");
		if(p_step->fields.error & imtq_sel_error_mtm_outofrange) printf("imtq_sel_error_mtm_outofrange ");
		if(p_step->fields.error & imtq_sel_error_coilcurr_outofrange) printf("imtq_sel_error_coilcurr_outofrange ");
		if(p_step->fields.error & 0x80) printf("!UNKNOWN! ");
		printf(")\r\n");
	}

	printf(" \t step = %04x", p_step->fields.step);
	switch(p_step->fields.step)
	{
	case imtq_sel_step_init: printf(" ( init ) \r\n"); break;
	case imtq_sel_step_posx: printf(" ( +x ) \r\n"); break;
	case imtq_sel_step_negx: printf(" ( -x ) \r\n"); break;
	case imtq_sel_step_posy: printf(" ( +y ) \r\n"); break;
	case imtq_sel_step_negy: printf(" ( -y ) \r\n"); break;
	case imtq_sel_step_posz: printf(" ( +z ) \r\n"); break;
	case imtq_sel_step_negz: printf(" ( -z ) \r\n"); break;
	case imtq_sel_step_fina: printf(" ( fina ) \r\n"); break;
	default: printf(" ( !UNKNOWN! ) \r\n"); break;
	}

	printf(" \t raw_magf(x,y,z) = %d, %d, %d [-] \r\n", p_step->fields.raw_magf[0], p_step->fields.raw_magf[1], p_step->fields.raw_magf[2]);
	printf(" \t cal_magf(x,y,z) = %d, %d, %d [10E-9 T] \r\n", p_step->fields.cal_magf[0], p_step->fields.cal_magf[1], p_step->fields.cal_magf[2]);
	printf(" \t coilcurr(x,y,z) = %d, %d, %d [10E-4 mA] \r\n", p_step->fields.coilcurr[0], p_step->fields.coilcurr[1], p_step->fields.coilcurr[2]);
	printf(" \t coiltemp(x,y,z) = %d, %d, %d [degC] \r\n", p_step->fields.coiltemp[0], p_step->fields.coiltemp[1], p_step->fields.coiltemp[2]);
}

static void _get_axis_inputs(imtq_inputs_t* p_inp)
{
	// gets the axis information required for actuation

	double value = 0;

	// unHACK: input function test
//	ing_debug_show_input_test();

	if(p_inp == NULL)
	{
		TRACE_ERROR(" internal error: p_inp is NULL");
		return;
	}

	memset(p_inp, 0, sizeof(imtq_inputs_t));

	if(ing(" x-axis value [0]: ", &value, -32768, 32767, 0))
	{
		TRACE_ERROR("\r\n invalid number!\r\n");
		return;
	}
	printf("\r\n");
	p_inp->fields.input[0] = (signed short) value;

	if(ing(" y-axis value [0]: ", &value, -32768, 32767, 0))
	{
		TRACE_ERROR("\r\n invalid number!\r\n");
		return;
	}
	printf("\r\n");
	p_inp->fields.input[1] = (signed short) value;

	if(ing(" z-axis value [0]: ", &value, -32768, 32767, 0))
	{
		TRACE_ERROR("\r\n invalid number!\r\n");
		return;
	}
	printf("\r\n");
	p_inp->fields.input[2] = (signed short) value;

	if(ing(" duration ms (0=inf) [0]: ", &value, 0, 65535, 0))
	{
		TRACE_ERROR("\r\n invalid number!\r\n");
		return;
	}
	printf("\r\n");
	p_inp->fields.duration = (unsigned short) value;
}

///***************************************************************************************************************************
///
/// Demo helper functions end
///
///***************************************************************************************************************************

///***************************************************************************************************************************
///
/// IsisMTQv2 command demo functions:
/// below are the IsisMTQv2 interface commands making up the messaging interface.
/// these generally send the corresponding command to the IMTQv2 and present the results to the user
///
///***************************************************************************************************************************

static Boolean _softReset(Boolean info)
{
	imtq_statcmd_t rsp_stat;
	unsigned int rv;

	if(info)
	{
		printf("\r\n *** Information IsisMTQv2_softReset *** \r\n");
		printf(" Sends the softReset command %#04x to the IMTQv2 \r\n", ISIS_MTQ_I2CCMD_RESET);
		printf(" Used to perform a software reset of the IMTQv2 restarting the system. \r\n");
		printf(" Note: all information stored in volatile memory (e.g. configuration data) \r\n");
		printf(" is reset to their startup defaults. \r\n");

		return TRUE;
	}


	printf("\r\n Perform IsisMTQv2_softReset \r\n");

	rv = IsisMTQv2_softReset(0, &rsp_stat);				// reset the ISIS-EPS configuration to hardcoded defaults
	if(rv)
	{
		TRACE_ERROR(" return value=%d \r\n", rv);		// non-zero return value means error!
		return TRUE;									// indicates we should not exit to the higher demo menu
	}

	printf(" response: \r\n");
	_parse_resp(&rsp_stat);

	return TRUE;
}

static Boolean _noOperation(Boolean info)
{
	imtq_statcmd_t rsp_stat;
	unsigned int rv;

	if(info)
	{
		printf("\r\n *** Information noOperation *** \r\n");
		printf(" Sends the no-operation command %#04x to the IMTQv2 \r\n", ISIS_MTQ_I2CCMD_NOP);
		printf(" Does not affect the IMTQv2 other than providing a 'success' reply. \r\n");
		printf(" Can be used to verify availability of the IMTQv2 in a non-intrusive manner. \r\n");

		return TRUE;
	}

	printf("\r\n Perform noOperation \r\n");

	rv = IsisMTQv2_noOperation(0, &rsp_stat);
	if(rv)
	{
		TRACE_ERROR(" return value=%d \r\n", rv);		// non-zero return value means error!
		return TRUE;									// indicates we should not exit to the higher demo menu
	}

	printf(" response: \r\n");
	_parse_resp(&rsp_stat);

	return TRUE;
}

static Boolean _cancelOperation(Boolean info)
{
	imtq_statcmd_t rsp_stat;
	unsigned int rv;

	if(info)
	{
		printf("\r\n *** Information cancelOperation *** \r\n");
		printf(" Sends the cancel-operation command %#04x to the IMTQv2 \r\n", ISIS_MTQ_I2CCMD_CANCELOP);
		printf(" Stops any actuation and returns to idle mode. \r\n");

		return TRUE;
	}

	printf("\r\n Perform cancelOperation \r\n");

	rv = IsisMTQv2_cancelOperation(0, &rsp_stat);
	if(rv)
	{
		TRACE_ERROR(" return value=%d \r\n", rv);		// non-zero return value means error!
		return TRUE;									// indicates we should not exit to the higher demo menu
	}

	printf(" response: \r\n");
	_parse_resp(&rsp_stat);

	return TRUE;
}

static Boolean _startMTMMeasurement(Boolean info)
{
	imtq_statcmd_t rsp_stat;
	unsigned int rv;

	if(info)
	{
		printf("\r\n *** Information startMTMMeasurement *** \r\n");
		printf(" Sends the mtm start command %#04x to the IMTQv2 \r\n", ISIS_MTQ_I2CCMD_STARTMTM);
		printf(" Starts a magnetometer measurement. Only available in Idle mode \r\n");
		printf(" Use getRawMTMData %#04x or getCalMTMData %#04x to get the result \r\n", ISIS_MTQ_I2CCMD_GET_MTM_RAWDATA, ISIS_MTQ_I2CCMD_GET_MTM_CALDATA);
		printf(" once integration has completed. Integration duration can be configured.");

		return TRUE;
	}

	printf("\r\n Perform startMTMMeasurement \r\n");

	rv = IsisMTQv2_startMTMMeasurement(0, &rsp_stat);
	if(rv)
	{
		TRACE_ERROR(" return value=%d \r\n", rv);		// non-zero return value means error!
		return TRUE;									// indicates we should not exit to the higher demo menu
	}

	printf(" response: \r\n");
	_parse_resp(&rsp_stat);

	return TRUE;
}

static Boolean _startMTQActuationCurrent(Boolean info)
{
	imtq_statcmd_t rsp_stat;
	imtq_inputs_t input;
	unsigned int rv;

	if(info)
	{
		printf("\r\n *** Information startMTQActuationCurrent *** \r\n");
		printf(" Sends the actuation start current command %#04x to the IMTQv2 \r\n", ISIS_MTQ_I2CCMD_STARTMTQ_ACTCURR);
		printf(" Starts coil actuation providing actuation level in coil currents. Only available in Idle mode \r\n");
		printf(" Provide positive values for positive dipoles in IMTQv2 frame, negative values for negative dipoles. \r\n");
		printf(" An actuation duration in milliseconds needs to be provided. \r\n");
		printf(" Provide 0 for duration to keep the coils on indefinitely. \r\n");
		printf(" The cancel command %#04x can be used to stop actuation at any time. \r\n", ISIS_MTQ_I2CCMD_CANCELOP);

		return TRUE;
	}

	printf("\r\n Perform startMTQActuationCurrent \r\n");

	printf(" Provide actuation currents [10E-4 A] \r\n");
	_get_axis_inputs(&input);

	rv = IsisMTQv2_startMTQActuationCurrent(0, input, &rsp_stat);
	if(rv)
	{
		TRACE_ERROR(" return value=%d \r\n", rv);		// non-zero return value means error!
		return TRUE;									// indicates we should not exit to the higher demo menu
	}

	printf(" response: \r\n");
	_parse_resp(&rsp_stat);

	return TRUE;
}
static Boolean _startMTQActuationDipole(Boolean info)
{
	imtq_statcmd_t rsp_stat;
	imtq_inputs_t input;
	unsigned int rv;

	if(info)
	{
		printf("\r\n *** Information startMTQActuationDipole *** \r\n");
		printf(" Sends the actuation start dipole command %#04x to the IMTQv2 \r\n", ISIS_MTQ_I2CCMD_STARTMTQ_ACTDIP);
		printf(" Starts coil actuation providing actuation level in coil dipole. Only available in Idle mode \r\n");
		printf(" Provide positive values for positive dipoles in IMTQv2 frame, negative values for negative dipoles. \r\n");
		printf(" The provided dipole is collinearly reduced if it is bigger than the torque that the IMTQ can produce. \r\n");
		printf(" Use command getCmdActuationDipole %#04x to retrieve the dipole that is actually being used for torquing. \r\n", IMTQ_CMDACTDIP_SIZE);
		printf(" An actuation duration in milliseconds needs to be provided, \r\n");
		printf(" or 0 can be supplied to keep the coils on indefinitely. \r\n");
		printf(" The cancel command %#04x can be used to stop actuation at any time. \r\n", ISIS_MTQ_I2CCMD_CANCELOP);

		return TRUE;
	}

	printf("\r\n Perform startMTQActuationDipole \r\n");

	printf(" Provide actuation dipole [10E-4 Am2] \r\n");
	_get_axis_inputs(&input);

	rv = IsisMTQv2_startMTQActuationDipole(0, input, &rsp_stat);
	if(rv)
	{
		TRACE_ERROR(" return value=%d \r\n", rv);		// non-zero return value means error!
		return TRUE;									// indicates we should not exit to the higher demo menu
	}

	printf(" response: \r\n");
	_parse_resp(&rsp_stat);

	return TRUE;
}
static Boolean _startMTQActuationPWM(Boolean info)
{
	imtq_statcmd_t rsp_stat;
	imtq_inputs_t input;
	unsigned int rv;

	if(info)
	{
		printf("\r\n *** Information startMTQActuationPWM *** \r\n");
		printf(" Sends the actuation start pwm command %#04x to the IMTQv2 \r\n", ISIS_MTQ_I2CCMD_STARTMTQ_ACTPWM);
		printf(" Starts coil actuation providing actuation level in on-percentage. Only available in Idle mode \r\n");
		printf(" Provide positive values for positive dipoles in IMTQv2 frame, negative values for negative dipoles. \r\n");
		printf(" An actuation duration in milliseconds needs to be provided. \r\n");
		printf(" Provide 0 for duration to keep the coils on indefinitely. \r\n");
		printf(" The cancel command %#04x can be used to stop actuation at any time. \r\n", ISIS_MTQ_I2CCMD_CANCELOP);

		return TRUE;
	}

	printf("\r\n Perform startMTQActuationDipole \r\n");

	printf(" Provide actuation [0.1%%] \r\n");
	_get_axis_inputs(&input);

	rv = IsisMTQv2_startMTQActuationPWM(0, input, &rsp_stat);
	if(rv)
	{
		TRACE_ERROR(" return value=%d \r\n", rv);		// non-zero return value means error!
		return TRUE;									// indicates we should not exit to the higher demo menu
	}

	printf(" response: \r\n");
	_parse_resp(&rsp_stat);

	return TRUE;
}

static Boolean _startSelfTest(Boolean info)
{
	imtq_statcmd_t rsp_stat;
	imtq_sel_axdir_t axdir;
	unsigned int rv;

	if(info)
	{
		printf("\r\n *** Information startSelfTest *** \r\n");
		printf(" Sends the startSelfTest command %#04x to the IMTQv2 \r\n", ISIS_MTQ_I2CCMD_SWITCH_SELFMODE);
		printf(" Switches IMTQv2 into self-test mode, actuating axes and cross-checking with measured magnetic field. \r\n");
		printf(" Can only be started from Idle mode and automatically switches back to Idle mode upon completion. \r\n");
		printf(" Provide imtq_sel_axdir_all (%d) to actuate and verify all axes-directions (i.e. a positive and a negative dipole generated per axis) \r\n", imtq_sel_axdir_all);
		printf(" sequentially using a single command. Otherwise a single axis can be verified per command. \r\n");
		printf(" After completion the result of the self-test needs to be retrieved using command getSelftestData %#04x. \r\n", ISIS_MTQ_I2CCMD_GET_SELFTESTDATA);
		printf(" NOTE: The STAT byte response is *not* the result of the self-test, it only indicates start command acceptance. \r\n");

		return TRUE;
	}

	printf("\r\n Perform startSelfTest \r\n");

	// *** get axis test user input
	{
		double value;
		char tmp[255] = {0};

		snprintf(tmp, sizeof(tmp), " Provide self-test axis (%d=all,%d=+x,%d=-x,%d=+y,%d=-y,%d=+z,%d=-z) [%d]: ",
								imtq_sel_axdir_all, imtq_sel_axdir_posx, imtq_sel_axdir_negx, imtq_sel_axdir_posy, imtq_sel_axdir_negy, imtq_sel_axdir_posz, imtq_sel_axdir_negz,
								imtq_sel_axdir_all);

		while(1)
		{
			rv = ing(tmp, &value, imtq_sel_axdir_all, imtq_sel_axdir_negz, imtq_sel_axdir_all);	// request info from the user
			if(rv == INGRV_esc) {printf("<ESC> \r\n"); return TRUE;}								// esc? exit function
			else if(rv == INGRV_val) {printf("\r\n"); break;}										// valid? continue
		}

		axdir = (imtq_sel_axdir_t) value;															// cast the supplied axis-direction value
	}

	rv = IsisMTQv2_startSelfTest(0, axdir, &rsp_stat);	// issue the command
	if(rv)
	{
		TRACE_ERROR(" return value=%d \r\n", rv);		// non-zero return value means error!
		return TRUE;									// indicates we should not exit to the higher demo menu
	}

	printf(" response: \r\n");
	_parse_resp(&rsp_stat);

	return TRUE;
}
static Boolean _startDetumble(Boolean info)
{
	imtq_statcmd_t rsp_stat;
	unsigned short duration;
	unsigned int rv;

	if(info)
	{
		printf("\r\n *** Information startDetumble *** \r\n");
		printf(" Sends the startDetumble command %#04x to the IMTQv2 \r\n", ISIS_MTQ_I2CCMD_SWITCH_DETMODE);
		printf(" Switches IMTQv2 into detumble mode, providing autonomous detumble operation. \r\n");
		printf(" Detumble mode implements the b-dot algorithm using magnetic measurements and coil actuation to reduce the spin of a satellite. \r\n");
		printf(" Can only be started from Idle mode. Returns to Idle mode after duration expires. \r\n");
		printf(" Duration can *not* be set to infinite, instead startBDOT needs to be re-issued to update its duration. \r\n");

		return TRUE;
	}

	printf("\r\n Perform startDetumble \r\n");

	// *** get axis test user input
	{
		double value;

		while(1)
		{
			rv = ing(" Provide duration in seconds [0]: ", &value, 0, 65535, 0);					// request info from the user
			if(rv == INGRV_esc) {printf("<ESC> \r\n"); return TRUE;}								// esc? exit function
			else if(rv == INGRV_val) {printf("\r\n"); break;}										// valid? continue
		}

		duration = value;																			// cast the supplied axis-direction value
	}

	rv = IsisMTQv2_startDetumble(0, duration, &rsp_stat);											// issue the command
	if(rv)
	{
		TRACE_ERROR(" return value=%d \r\n", rv);													// non-zero return value means error!
		return TRUE;																				// indicates we should not exit to the higher demo menu
	}

	printf(" response: \r\n");
	_parse_resp(&rsp_stat);

	return TRUE;
}

static Boolean _getSystemState(Boolean info)
{
	imtq_statcmd_t rsp_stat;
	imtq_systemstate_t system_state;
	unsigned int rv;

	if(info)
	{
		printf("\r\n *** Information getSystemState *** \r\n");
		printf(" Sends the getMTQSystemState command %#04x to the IMTQv2 \r\n", ISIS_MTQ_I2CCMD_GET_SYSTEMSTATE);
		printf(" Provides system state information on the IMTQv2. \r\n");
		printf(" The following information is returned: \r\n");
		printf(" mode = %s \r\n", "current mode of the IMTQv2");
		printf(" error = %s \r\n", "first internal error encountered during last control iteration");
		printf(" conf = %s \r\n", "1 when the in-memory configuration been altered by the user since start-up");
		printf(" uptime = %s \r\n", "uptime in seconds");

		return TRUE;
	}

	printf("\r\n Perform getSystemState \r\n");

	rv = IsisMTQv2_getSystemState(0, &system_state, &rsp_stat);
	if(rv)
	{
		TRACE_ERROR(" return value=%d \r\n", rv);		// non-zero return value means error!
		return TRUE;									// indicates we should not exit to the higher demo menu
	}

	printf(" response: \r\n");
	_parse_resp(&rsp_stat);

	printf(" \t mode = %d ", system_state.fields.mode);
	switch(system_state.fields.mode)
	{
	case imtq_mode_idle: printf("( idle ) \r\n"); break;
	case imtq_mode_selftest: printf("( selftest ) \r\n"); break;
	case imtq_mode_detumble: printf("( detumble ) \r\n"); break;
	default: printf("( !UNKNOWN! ) \r\n"); break;
	}

	printf(" \t error = %d \r\n", system_state.fields.err);
	printf(" \t conf = %d ", system_state.fields.conf);
	if(system_state.fields.conf) printf("( config params were changed ) \r\n"); else printf("( no config param changes ) \r\n");
	printf(" \t uptime = %d [s] \r\n", system_state.fields.uptime);

	return TRUE;
}
static Boolean _getRawMTMData(Boolean info)
{
	imtq_statcmd_t rsp_stat;
	imtq_raw_magf_t field_raw;
	unsigned int rv;

	if(info)
	{
		printf("\r\n *** Information getRawMTMData *** \r\n");
		printf(" Sends the getRawMTMData command %#04x to the IMTQv2 \r\n", ISIS_MTQ_I2CCMD_GET_MTM_RAWDATA);
		printf(" Returns MTM measurement results from a previously started measurement using startMTMMeasurement. \r\n");
		printf(" Measurement results become available after the configurable integration time completes. \r\n");
		printf(" The following information is returned: \r\n");
		printf(" raw_magf = %s \r\n", "magnetometer field strength in raw counts");
		printf(" coilact = %s \r\n", "coil actuation detected during measurement");

		return TRUE;
	}

	printf("\r\n Perform getRawMTMData \r\n");

	rv = IsisMTQv2_getRawMTMData(0, &field_raw, &rsp_stat);
	if(rv)
	{
		TRACE_ERROR(" return value=%d \r\n", rv);		// non-zero return value means error!
		return TRUE;									// indicates we should not exit to the higher demo menu
	}

	printf(" response: \r\n");
	_parse_resp(&rsp_stat);

	printf(" \t raw_magf(x,y,z) = %d, %d, %d [-] \r\n", field_raw.fields.raw_magf[0], field_raw.fields.raw_magf[1], field_raw.fields.raw_magf[2]);
	printf(" \t coilact = %d \r\n", field_raw.fields.coilact);

	return TRUE;
}
static Boolean _getCalMTMData(Boolean info)
{
	imtq_statcmd_t rsp_stat;
	imtq_cal_magf_t field_cal;
	unsigned int rv;

	if(info)
	{
		printf("\r\n *** Information getCalMTMData *** \r\n");
		printf(" Sends the getCalMTMData command %#04x to the IMTQv2 \r\n", ISIS_MTQ_I2CCMD_GET_MTM_CALDATA);
		printf(" Returns MTM measurement results from a previously started measurement using startMTMMeasurement. \r\n");
		printf(" Measurement results become available after the configurable integration time completes. \r\n");
		printf(" The following information is returned: \r\n");
		printf(" cal_magf = %s \r\n", "magnetometer field strength in nano-tesla [10E-9 T]");
		printf(" coilact = %s \r\n", "coil actuation detected during measurement");

		return TRUE;
	}

	printf("\r\n Perform getCalMTMData \r\n");

	rv = IsisMTQv2_getCalMTMData(0, &field_cal, &rsp_stat);
	if(rv)
	{
		TRACE_ERROR(" return value=%d \r\n", rv);		// non-zero return value means error!
		return TRUE;									// indicates we should not exit to the higher demo menu
	}

	printf(" response: \r\n");
	_parse_resp(&rsp_stat);

	printf(" \t cal_magf(x,y,z) = %d, %d, %d [10E-9 T] \r\n", field_cal.fields.cal_magf[0], field_cal.fields.cal_magf[1], field_cal.fields.cal_magf[2]);
	printf(" \t coilact = %d \r\n", field_cal.fields.coilact);

	return TRUE;
}

static Boolean _getCoilCurrent(Boolean info)
{
	imtq_statcmd_t rsp_stat;
	imtq_coilcurr_t coilcurr;
	unsigned int rv;

	if(info)
	{
		printf("\r\n *** Information getCoilCurrent *** \r\n");
		printf(" Sends the getCoilCurrent command %#04x to the IMTQv2 \r\n", ISIS_MTQ_I2CCMD_GET_COIL_CURRMSR);
		printf(" Returns the latest coil current measurement available. \r\n");
		printf(" Measurement results become available automatically after each millisecond. \r\n");
		printf(" The following information is returned: \r\n");
		printf(" coilcurr = %s \r\n", "coil current in [10E-4 A]");

		return TRUE;
	}

	printf("\r\n Perform getCoilCurrent \r\n");

	rv = IsisMTQv2_getCoilCurrent(0, &coilcurr, &rsp_stat);
	if(rv)
	{
		TRACE_ERROR(" return value=%d \r\n", rv);		// non-zero return value means error!
		return TRUE;									// indicates we should not exit to the higher demo menu
	}

	printf(" response: \r\n");
	_parse_resp(&rsp_stat);

	printf(" \t coilcurr(x,y,z) = %d, %d, %d [10E-4 A] \r\n", coilcurr.fields.coilcurr[0], coilcurr.fields.coilcurr[1], coilcurr.fields.coilcurr[2]);

	return TRUE;
}

static Boolean _getCoilTemperature(Boolean info)
{
	imtq_statcmd_t rsp_stat;
	imtq_coiltemp_t coiltemp;
	unsigned int rv;

	if(info)
	{
		printf("\r\n *** Information getCoilTemperature *** \r\n");
		printf(" Sends the getCoilTemperature command %#04x to the IMTQv2 \r\n", ISIS_MTQ_I2CCMD_GET_COIL_TEMPMSR);
		printf(" Returns the latest coil temperature measurement available. \r\n");
		printf(" Measurement results become available automatically after each millisecond. \r\n");
		printf(" The following information is returned: \r\n");
		printf(" coiltemp = %s \r\n", "coil temperature in degrees C");

		return TRUE;
	}

	printf("\r\n Perform getCoilTemperature \r\n");

	rv = IsisMTQv2_getCoilTemperature(0, &coiltemp, &rsp_stat);
	if(rv)
	{
		TRACE_ERROR(" return value=%d \r\n", rv);		// non-zero return value means error!
		return TRUE;									// indicates we should not exit to the higher demo menu
	}

	printf(" response: \r\n");
	_parse_resp(&rsp_stat);

	printf(" \t coiltemp(x,y,z) = %d, %d, %d [degC] \r\n", coiltemp.fields.coiltemp[0], coiltemp.fields.coiltemp[1], coiltemp.fields.coiltemp[2]);

	return TRUE;
}
static Boolean _getCmdActuationDipole(Boolean info)
{
	imtq_statcmd_t rsp_stat;
	imtq_cmdactdip_t cmdact_dip;
	unsigned int rv;

	if(info)
	{
		printf("\r\n *** Information getCmdActuationDipole *** \r\n");
		printf(" Sends the getCmdActuationDipole command %#04x to the IMTQv2 \r\n", ISIS_MTQ_I2CCMD_GET_CMDACT_DIPOLE);
		printf(" Returns the dipole that is being actuated, which might be different from the one commanded by the IMTQv2 master. \r\n");
		printf(" Differences occur due to automatic scaling of the dipole to fall within temperature dependent torque-able limits of the IMTQv2. \r\n");
		printf(" NOTE: Only available after a torque command using dipole, or while in detumble mode.\r\n");
		printf(" The following information is returned: \r\n");
		printf(" cmd_act_dip = %s \r\n", "commanded coil actuation dipole in [10E-4 Am2]");

		return TRUE;
	}

	printf("\r\n Perform getCmdActuationDipole \r\n");

	rv = IsisMTQv2_getCmdActuationDipole(0, &cmdact_dip, &rsp_stat);
	if(rv)
	{
		TRACE_ERROR(" return value=%d \r\n", rv);		// non-zero return value means error!
		return TRUE;									// indicates we should not exit to the higher demo menu
	}

	printf(" response: \r\n");
	_parse_resp(&rsp_stat);

	printf(" \t cmd_act_dip(x,y,z) = %d, %d, %d [10E-4 Am2] \r\n", cmdact_dip.fields.cmdactdip[0], cmdact_dip.fields.cmdactdip[1], cmdact_dip.fields.cmdactdip[2]);

	return TRUE;
}

static Boolean _getSelftestDataSingleAxis(Boolean info)
{
	imtq_seldata_single_axdir_t seldata;
	unsigned int rv;

	if(info)
	{
		printf("\r\n *** Information getSelftestDataSingleAxis *** \r\n");
		printf(" Sends the getSelftestData command %#04x to the IMTQv2 \r\n", ISIS_MTQ_I2CCMD_GET_SELFTESTDATA);
		printf(" Returns the result of the last completed self-test, started with the command startSelfTest %#04x. \r\n", ISIS_MTQ_I2CCMD_SWITCH_SELFMODE);
		printf(" The self-test is a mode which actuates the coils and measures the resulting field to verify hardware functionality. \r\n");
		printf(" At the end of the self-test the IMTQv2 will diagnose the gathered measurement data and indicate whether problems are detected. \r\n");
		printf(" The result data returned can be either a single-axis or all-axis data block, depending on the axis-direction parameter provided to startSelfTest(), \r\n");
		printf(" which should be retrieved using the getSelftestDataSingleAxis() or getSelftestDataAllAxis() functions respectively. \r\n");
		printf(" The following information is returned: \r\n");
		_parse_selftest_step(NULL);						// request only generic information ...

		return TRUE;
	}

	printf("\r\n Perform getSelftestDataSingleAxis \r\n");

	rv = IsisMTQv2_getSelftestDataSingleAxis(0, &seldata);
	if(rv)
	{
		TRACE_ERROR(" return value=%d \r\n", rv);		// non-zero return value means error!
		return TRUE;									// indicates we should not exit to the higher demo menu
	}

	printf("\r\n Results: \r\n");

	printf(" *** step INIT \r\n");
	_parse_selftest_step(&seldata.fields.step_init);
	printf(" *** step AXAC (actuation step results for selected axis-direction) \r\n");
	_parse_selftest_step(&seldata.fields.step_axac);
	printf(" *** step FINA \r\n");
	_parse_selftest_step(&seldata.fields.step_fina);

	return TRUE;
}

static Boolean _getSelftestDataAllAxis(Boolean info)
{
	imtq_seldata_all_axdir_t seldata;
	unsigned int rv;

	if(info)
	{
		printf("\r\n *** Information getSelftestDataSingleAxis *** \r\n");
		printf(" Sends the getSelftestData command %#04x to the IMTQv2 \r\n", ISIS_MTQ_I2CCMD_GET_SELFTESTDATA);
		printf(" Returns the result of the last completed self-test, started with the command startSelfTest %#04x. \r\n", ISIS_MTQ_I2CCMD_SWITCH_SELFMODE);
		printf(" The self-test is a mode which actuates the coils and measures the resulting field to verify hardware functionality. \r\n");
		printf(" At the end of the self-test the IMTQv2 will diagnose the gathered measurement data and indicate whether problems are detected. \r\n");
		printf(" The result data returned can be either a single-axis or all-axis data block, depending on the axis-direction \r\n");
		printf(" parameter provided to startSelfTest(), which should be retrieved using the getSelftestDataSingleAxis() or \r\n");
		printf(" getSelftestDataAllAxis() functions respectively. \r\n");
		printf(" The following information is returned: \r\n");
		_parse_selftest_step(NULL);						// request only generic information ...

		return TRUE;
	}

	printf("\r\n Perform getSelftestDataSingleAxis \r\n");

	rv = IsisMTQv2_getSelftestDataAllAxis(0, &seldata);
	if(rv)
	{
		TRACE_ERROR(" return value=%d \r\n", rv);		// non-zero return value means error!
		return TRUE;									// indicates we should not exit to the higher demo menu
	}

	printf("\r\n Results: \r\n");

	printf(" *** step INIT \r\n");
	_parse_selftest_step(&seldata.fields.step_init);
	printf(" *** step POSX \r\n");
	_parse_selftest_step(&seldata.fields.step_posx);
	printf(" *** step NEGX \r\n");
	_parse_selftest_step(&seldata.fields.step_negx);
	printf(" *** step POSY \r\n");
	_parse_selftest_step(&seldata.fields.step_posy);
	printf(" *** step NEGY \r\n");
	_parse_selftest_step(&seldata.fields.step_negy);
	printf(" *** step POSZ \r\n");
	_parse_selftest_step(&seldata.fields.step_posz);
	printf(" *** step NEGZ \r\n");
	_parse_selftest_step(&seldata.fields.step_negz);
	printf(" *** step FINA \r\n");
	_parse_selftest_step(&seldata.fields.step_fina);

	return TRUE;
}

static Boolean _getDetumbleData(Boolean info)
{
	imtq_statcmd_t rsp_stat;
	imtq_detumble_data_t detdat;
	unsigned int rv;

	if(info)
	{
		printf("\r\n *** Information getDetumbleData *** \r\n");
		printf(" Sends the getDetumbleData command %#04x to the IMTQv2 \r\n", ISIS_MTQ_I2CCMD_GET_DETUMBLEDATA);
		printf(" Returns the latest measurement and control information produced by the autonomously operating detumble mode. \r\n");
		printf(" The following information is returned: \r\n");
		printf(" cal_magf = %s \r\n", "magnetic field strength in nano-tesla [10E-9 T]");
		printf(" filt_magf = %s \r\n", "filtered magnetic field strength in nano-tesla [10E-9 T]");
		printf(" bdot = %s \r\n", "computed b-dot value in nano-tesla per second [10E-9 T/s]");
		printf(" cmdactdip = %s \r\n", "commanded actuation dipole in [10E-4 Am2]");
		printf(" cmd_coilcurr = %s \r\n", "commanded actuation current in [10E-4 A]");
		printf(" meas_coilcurr = %s \r\n", "measured actuation current in [10E-4 A]");

		return TRUE;
	}

	printf("\r\n Perform getDetumbleData \r\n");

	rv = IsisMTQv2_getDetumbleData(0, &detdat, &rsp_stat);
	if(rv)
	{
		TRACE_ERROR(" return value=%d \r\n", rv);		// non-zero return value means error!
		return TRUE;									// indicates we should not exit to the higher demo menu
	}

	printf(" response: \r\n");
	_parse_resp(&rsp_stat);

	printf(" \t cal_magf(x,y,z) = %d, %d, %d [10E-9 T] \r\n", detdat.fields.cal_magf[0], detdat.fields.cal_magf[1], detdat.fields.cal_magf[2]);
	printf(" \t filt_magf(x,y,z) = %d, %d, %d [10E-9 T] \r\n", detdat.fields.filt_magf[0], detdat.fields.filt_magf[1], detdat.fields.filt_magf[2]);
	printf(" \t bdot(x,y,z) = %d, %d, %d [10E-9 T/s] \r\n", detdat.fields.bdot[0], detdat.fields.bdot[1], detdat.fields.bdot[2]);
	printf(" \t cmdactdip(x,y,z) = %d, %d, %d [10E-4 Am2] \r\n", detdat.fields.cmdactdip[0], detdat.fields.cmdactdip[1], detdat.fields.cmdactdip[2]);
	printf(" \t coilcurr_cmd(x,y,z) = %d, %d, %d [10E-4 A] \r\n", detdat.fields.coilcurr_cmd[0], detdat.fields.coilcurr_cmd[1], detdat.fields.coilcurr_cmd[2]);
	printf(" \t coilcurr_meas(x,y,z) = %d, %d, %d [10E-4 A] \r\n", detdat.fields.coilcurr_meas[0], detdat.fields.coilcurr_meas[1], detdat.fields.coilcurr_meas[2]);

	return TRUE;
}
static Boolean _getRawHKData(Boolean info)
{
	imtq_statcmd_t rsp_stat;
	imtq_rawhk_data_t rawhk;
	unsigned int rv;

	if(info)
	{
		printf("\r\n *** Information getRawHKData *** \r\n");
		printf(" Sends the getRawHKData command %#04x to the IMTQv2 \r\n", ISIS_MTQ_I2CCMD_GET_HK_RAWDATA);
		printf(" Returns the latest house-keeping data in raw form from the IMTQv2. \r\n");
		printf(" The following information is returned: \r\n");
		printf(" digit_volt = %s \r\n", "measured digital supply current in raw counts");
		printf(" anl_volt = %s \r\n", "measured analog supply current in raw counts");
		printf(" digit_curr = %s \r\n", "measured digital supply current in raw counts");
		printf(" anl_curr = %s \r\n", "measured analog supply current in raw counts");
		printf(" coilcurr = %s \r\n", "measured coil currents in raw counts");
		printf(" coiltemp = %s \r\n", "measured coil temperature in raw counts");
		printf(" mcu_temp = %s \r\n", "measured micro controller unit internal temperature in raw counts");

		return TRUE;
	}

	printf("\r\n Perform _getRawHKData \r\n");

	rv = IsisMTQv2_getRawHKData(0, &rawhk, &rsp_stat);
	if(rv)
	{
		TRACE_ERROR(" return value=%d \r\n", rv);		// non-zero return value means error!
		return TRUE;									// indicates we should not exit to the higher demo menu
	}

	printf(" response: \r\n");
	_parse_resp(&rsp_stat);

	printf(" \t digit_volt = %d [-] \r\n", rawhk.fields.digit_volt);
	printf(" \t anl_volt = %d [-] \r\n", rawhk.fields.anl_volt);
	printf(" \t digit_curr = %d [-] \r\n", rawhk.fields.digit_curr);
	printf(" \t anl_curr = %d [-] \r\n", rawhk.fields.anl_curr);
	printf(" \t coilcurr(x,y,z) = %d, %d, %d [-] \r\n", rawhk.fields.coilcurr[0] , rawhk.fields.coilcurr[1], rawhk.fields.coilcurr[2]);
	printf(" \t coiltemp(x,y,z) = %d, %d, %d [-] \r\n", rawhk.fields.coiltemp[0] , rawhk.fields.coiltemp[1], rawhk.fields.coiltemp[2]);
	printf(" \t mcu_temp = %d [-] \r\n", rawhk.fields.mcu_temp);

	return TRUE;
}

static Boolean _getEngHKData(Boolean info)
{
	imtq_statcmd_t rsp_stat;
	imtq_enghk_data_t enghk;
	unsigned int rv;

	if(info)
	{
		printf("\r\n *** Information getEngHKData *** \r\n");
		printf(" Sends the getEngHKData command %#04x to the IMTQv2 \r\n", ISIS_MTQ_I2CCMD_GET_HK_ENGDATA);
		printf(" Returns the latest house-keeping data as engineering values from the IMTQv2. \r\n");
		printf(" The following information is returned: \r\n");
		printf(" digit_volt = %s \r\n", "measured digital supply current in [mV]");
		printf(" anl_volt = %s \r\n", "measured analog supply current in [mV]");
		printf(" digit_curr = %s \r\n", "measured digital supply current in [10E-4 A]");
		printf(" anl_curr = %s \r\n", "measured analog supply current in [10E-4 A]");
		printf(" coilcurr = %s \r\n", "measured coil currents in [10E-4 A]");
		printf(" coiltemp = %s \r\n", "measured coil temperature in degrees Celcius");
		printf(" mcu_temp = %s \r\n", "measured micro controller unit internal temperature in degrees Celcius");

		return TRUE;
	}

	printf("\r\n Perform _getEngHKData \r\n");

	rv = IsisMTQv2_getEngHKData(0, &enghk, &rsp_stat);
	if(rv)
	{
		TRACE_ERROR(" return value=%d \r\n", rv);		// non-zero return value means error!
		return TRUE;									// indicates we should not exit to the higher demo menu
	}

	printf(" response: \r\n");
	_parse_resp(&rsp_stat);

	printf(" \t digit_volt = %d [mV] \r\n", enghk.fields.digit_volt);
	printf(" \t anl_volt = %d [mV] \r\n", enghk.fields.anl_volt);
	printf(" \t digit_curr = %d [10E-4 A] \r\n", enghk.fields.digit_curr);
	printf(" \t anl_curr = %d [10E-4 A] \r\n", enghk.fields.anl_curr);
	printf(" \t coilcurr(x,y,z) = %d, %d, %d [10E-4 A] \r\n", enghk.fields.coilcurr[0] , enghk.fields.coilcurr[1], enghk.fields.coilcurr[2]);
	printf(" \t coiltemp(x,y,z) = %d, %d, %d [degC] \r\n", enghk.fields.coiltemp[0] , enghk.fields.coiltemp[1], enghk.fields.coiltemp[2]);
	printf(" \t mcu_temp = %d [degC] \r\n", enghk.fields.mcu_temp);

	return TRUE;
}


static Boolean _getParameter(Boolean info)
{
	unsigned short par_id;							// storage for the param-id
	unsigned char par_data[8];						// byte array for storing our parameter data
	imtq_statcmd_t rsp_stat;						// storage for the command response
	unsigned int rv;

	if(info)
	{
		printf("\r\n Information getParameter \r\n");
		printf(" Sends the getParameter command %#04x to the IMTQv2 \r\n", ISIS_MTQ_I2CCMD_GET_PARAMETER);
		printf(" Used to get configuration parameter values from the IMTQv2. \r\n");
		printf(" Execution is performed and completed immediately. \r\n");
		printf(" The following information is returned: \r\n");
		printf(" param-id = %s \r\n", "parameter-id of the parameter under consideration.");
		printf(" param-value = %s \r\n", "value of the parameter under consideration. Between 1 and 8 bytes.");

		return TRUE;
	}

	printf("\r\n Perform getParameter \r\n");

	if(!config_param_info(CONFIG_PARAM_OP_ask_parid, &par_id, NULL)) return TRUE;	// get the param-id from the user

	rv = IsisMTQv2_getParameter(0, par_id, par_data, &rsp_stat);	// get the parameter from the IMTQv2
	if(rv)
	{
		TRACE_ERROR("return value=%d \r\n", rv);		// non-zero return value means error!
		return TRUE;								// indicates we should not exit to the higher demo menu
	}

	printf(" response: \r\n");
	_parse_resp(&rsp_stat);							// parse the command response and show that to the user

	config_param_info(CONFIG_PARAM_OP_print, &par_id, &par_data);	// show the param-id and corresponding value that we received back

	return TRUE;
}


static Boolean _setParameter(Boolean info)
{
	unsigned short par_id;							// storage for the param-id
	unsigned char par_data[8];						// byte array for storing the parameter data we want to set; our maximum size is 8 bytes (e.g. double, long ...)
	unsigned char par_data_out[8];					// byte array for storing the parameter data that was actually set; our maximum size is 8 bytes (e.g. double, long ...)
	imtq_statcmd_t rsp_stat;						// storage for the command response
	unsigned int rv;

	if(info)
	{
		printf("\r\n Information setParameter \r\n");
		printf("Sends the setParameter command %#04x to the IMTQv2 \r\n", ISIS_MTQ_I2CCMD_SET_PARAMETER);
		printf("Used to set configuration parameter values within the IMTQv2. \r\n");
		printf("Execution is performed and completed immediately. \r\n");
		printf("The following information is returned: \r\n");
		printf("param-id = %s \r\n", "parameter-id of the parameter under consideration.");
		printf("param-value = %s \r\n", "new value of the parameter under consideration. Between 1 and 8 bytes.");

		return TRUE;
	}

	printf("\r\n Perform setParameter \r\n");

	if(!config_param_info(CONFIG_PARAM_OP_ask_parid_and_data, &par_id, &par_data)) return TRUE;	// get the param-id and new value from the user

	rv = IsisMTQv2_setParameter(0, par_id, par_data, par_data_out, &rsp_stat);		// send the new parameter data to the IMTQv2
	if(rv)
	{
		TRACE_ERROR("return value=%d \r\n", rv);		// non-zero return value means error!
		return TRUE;								// indicates we should not exit to the higher demo menu
	}

	printf(" response: \r\n");
	_parse_resp(&rsp_stat);							// parse the command response and show that to the user

	if(!config_param_info(CONFIG_PARAM_OP_print, &par_id, &par_data_out)) return TRUE;	// show the param-id and corresponding value that we received back

	if(memcmp((char *) par_data, (char *) par_data_out, sizeof(par_data)) != 0)		// is the returned parameter value the same?
	{
		unsigned int i;

		printf("Warning: the resulting parameter value differs from the one send in! \r\n");
		printf("this is generally caused because the supplied variable was not in the range \r\n");
		printf("of valid values and has been set to closest valid value by the IMTQv2. \r\n");
		printf("I2C bus noise might be another possible culprit. \r\n");
		printf("Consult the manual and test the I2C bus to determine the cause. \r\n");

		printf("send     = %#04x",  par_data[0]);
		for(i = 1; i < sizeof(par_data); i++)
		{
			printf(", %#04x",  par_data[i]);
		}
		printf("\r\n");
		printf("received = %#04x",  par_data_out[0]);
		for(i = 1; i < sizeof(par_data_out); i++)
		{
			printf(", %#04x",  par_data_out[i]);
		}
		printf("\r\n");
	}

	return TRUE;
}


static Boolean _resetParameter(Boolean info)
{
	unsigned short par_id;							// storage for the param-id
	unsigned char par_data[8];						// byte array for storing our parameter data
	imtq_statcmd_t rsp_stat;						// storage for the command response
	unsigned int rv;

	if(info)
	{
		printf("\r\n Information resetParameter \r\n");
		printf(" Sends the resetParameter command %#04x to the IMTQv2 \r\n", ISIS_MTQ_I2CCMD_RESET_PARAMETER);
		printf(" Used to reset configuration parameter values back to its hard coded default within the IMTQv2. \r\n");
		printf(" note: this is different from the value stored in non-volatile memory; if required use loadConfig instead. \r\n");
		printf(" Execution is performed and completed immediately. \r\n");
		printf(" The following information is returned: \r\n");
		printf(" param-id = %s \r\n", "parameter-id of the parameter under consideration.");
		printf(" param-value = %s \r\n", "value of the parameter under consideration. Between 1 and 8 bytes.");

		return TRUE;
	}

	printf("\r\n Perform resetParameter \r\n");

	if(!config_param_info(CONFIG_PARAM_OP_ask_parid, &par_id, NULL)) return TRUE;	// get the param-id from the user

	rv = IsisMTQv2_resetParameter(0, par_id, &par_data, &rsp_stat);	// command the ISIS-MTQv2 to reset the parameter, and receive the value that it was rest to
	if(rv)
	{
		TRACE_ERROR("return value=%d \r\n", rv);		// non-zero return value means error!
		return TRUE;								// indicates we should not exit to the higher demo menu
	}

	printf(" response: \r\n");
	_parse_resp(&rsp_stat);							// parse the command response and show that to the user

	config_param_info(CONFIG_PARAM_OP_print, &par_id, &par_data);	// show the param-id and corresponding value that we received back

	return TRUE;
}

static Boolean _selectAndExecuteDemoTest(void)
{
	double value;
	unsigned int selection = 0;
	Boolean offerMoreTests = TRUE;
	static Boolean toggle_is_info = FALSE;

	if(toggle_is_info)
	{
		printf("\n\r ******************* Information Mode ******************* \n\r");
		printf("\n\r While in this mode you can select commands about which \n\r");
		printf(    " you would like to get information without issuing \n\r");
		printf(    " the actual command. To exit information mode select \n\r");
		printf(    " option 1. \n\r");
		printf("\n\r Choose which command to show information for: \n\r");
	}
	else
	{
		printf( "\n\r Select a test to perform: \n\r");
	}

	printf("\t  0) Return to main menu \n\r");

	if(toggle_is_info)	printf("\t  1) Switch back to test mode \n\r");
	else 				printf("\t  1) Switch to information mode \n\r");

	printf("\t  2) softReset \n\r");
	printf("\t  3) noOperation \n\r");
	printf("\t  4) cancelOperation \n\r");
	printf("\t  5) startMTMMeasurement \n\r");
	printf("\t  6) startMTQActuationCurrent \n\r");
	printf("\t  7) startMTQActuationDipole \n\r");
	printf("\t  8) startMTQActuationPWM \n\r");
	printf("\t  9) startSelfTest \n\r");
	printf("\t 10) startDetumble \n\r");
	printf("\t 11) getSystemState \n\r");
	printf("\t 12) getRawMTMData \n\r");
	printf("\t 13) getCalMTMData \n\r");
	printf("\t 14) getCoilCurrent \n\r");;
	printf("\t 15) getCoilTemperature \n\r");
	printf("\t 16) getCmdActuationDipole \n\r");
	printf("\t 17) getSelftestDataSingleAxis \n\r");
	printf("\t 18) getSelftestDataAllAxis \n\r");
	printf("\t 19) getDetumbleData \n\r");
	printf("\t 20) getRawHKData \n\r");
	printf("\t 21) getEngHKData \n\r");
	printf("\t 22) getParameter \n\r");
	printf("\t 23) setParameter \n\r");
	printf("\t 24) resetParameter \n\r");

	while(INGRV_val != ing("\r\n enter selection: ", &value, 0, 24, -1))
	{
		printf("\r\n");
	}

	printf("\r\n");

	selection = value;

	switch(selection)
	{
		case 0: offerMoreTests = FALSE; break;
		case 1: toggle_is_info = !toggle_is_info; offerMoreTests = TRUE; break;

		case 2: offerMoreTests = _softReset(toggle_is_info); break;
		case 3: offerMoreTests = _noOperation(toggle_is_info); break;
		case 4: offerMoreTests = _cancelOperation(toggle_is_info); break;
		case 5: offerMoreTests = _startMTMMeasurement(toggle_is_info); break;
		case 6: offerMoreTests = _startMTQActuationCurrent(toggle_is_info); break;
		case 7: offerMoreTests = _startMTQActuationDipole(toggle_is_info); break;
		case 8: offerMoreTests = _startMTQActuationPWM(toggle_is_info); break;
		case 9: offerMoreTests = _startSelfTest(toggle_is_info); break;
		case 10: offerMoreTests = _startDetumble(toggle_is_info); break;
		case 11: offerMoreTests = _getSystemState(toggle_is_info); break;
		case 12: offerMoreTests = _getRawMTMData(toggle_is_info); break;
		case 13: offerMoreTests = _getCalMTMData(toggle_is_info); break;
		case 14: offerMoreTests = _getCoilCurrent(toggle_is_info); break;
		case 15: offerMoreTests = _getCoilTemperature(toggle_is_info); break;
		case 16: offerMoreTests = _getCmdActuationDipole(toggle_is_info); break;
		case 17: offerMoreTests = _getSelftestDataSingleAxis(toggle_is_info); break;
		case 18: offerMoreTests = _getSelftestDataAllAxis(toggle_is_info); break;
		case 19: offerMoreTests = _getDetumbleData(toggle_is_info); break;
		case 20: offerMoreTests = _getRawHKData(toggle_is_info); break;
		case 21: offerMoreTests = _getEngHKData(toggle_is_info); break;
		case 22: offerMoreTests = _getParameter(toggle_is_info); break;
		case 23: offerMoreTests = _setParameter(toggle_is_info); break;
		case 24: offerMoreTests = _resetParameter(toggle_is_info); break;
		default: TRACE_ERROR("Invalid selection"); break;
	}

	return offerMoreTests;
}

/***
 * Initializes the IMTQv2 subsystem driver
 * The IMTQv2 subsystem driver is a layer that sits on top of the I2C interface driver
 * requiring the I2C interface driver to be initialized once before using any of the
 * subsystem library drivers
 */
Boolean IsisMTQv2demoInit(void)
{
    unsigned char i2c_address = 0x10;
    int rv;

	rv = IsisMTQv2_initialize(&i2c_address, 1);
	if(rv != E_NO_SS_ERR && rv != E_IS_INITIALIZED)				// rinse-repeat
	{
		// we have a problem. Indicate the error. But we'll gracefully exit to the higher menu instead of
		// hanging the code
		TRACE_ERROR("\n\r IsisMTQv2_initialize() failed; err=%d! Exiting ... \n\r", rv);
		return FALSE;
	}

	return TRUE;
}

void IsisMTQv2demoLoop(void)
{
	Boolean offerMoreTests = FALSE;

	while(1)
	{
		offerMoreTests = _selectAndExecuteDemoTest();		// show the demo command line interface and handle commands

		if(offerMoreTests == FALSE)							// was exit/back selected?
		{
			break;
		}
	}
}

Boolean IsisMTQv2demoMain(void)
{
	if(IsisMTQv2demoInit())									// initialize of I2C and IMTQv2 subsystem drivers succeeded?
	{
		IsisMTQv2demoLoop();								// show the main IMTQv2 demo interface and wait for user input
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

Boolean IsisMTQv2test(void)
{
	IsisMTQv2demoMain();
	return TRUE;
}
