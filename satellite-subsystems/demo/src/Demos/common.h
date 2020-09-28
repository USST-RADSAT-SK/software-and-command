/*
 * common.h
 *
 * Contains functions that are used in across multiple demo's
 *
 *  Created on: 5 okt. 2015
 *      Author: lrot
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <hal/boolean.h>

#include <stddef.h>

typedef enum INGRV_enum	// input number generic return values
{
	 INGRV_val = 0,		// input number valid
	 INGRV_inv = 1,		// input number invalid
	 INGRV_rng = 2,		// input number out of specified range
	 INGRV_esc = 3		// user pressed escape twice to indicate aborting input
} INGRV_e;

typedef enum CONFIG_PARAM_OP_enum				//!< configuration param printing an getting user input function
{
	CONFIG_PARAM_OP_ask_parid = 0,				///!< ask user for the parameter-id
	CONFIG_PARAM_OP_ask_parid_and_data = 1,		///!< ask user for param-id and its data
	CONFIG_PARAM_OP_print = 2					///!< show parameter id and data to the user
} CONFIG_PARAM_OP_e;


/***
 * input number generic (ING) user input request functions
 * provides a scanf style input function that includes watchdog petting in the data request loop
 * it is used to get user input.
 */
INGRV_e ing_interact(char *p_head, char *p_buf, size_t buf_sz, size_t *p_gotten_sz);
INGRV_e ing_parse(double *p_value, char *p_buf, size_t buf_sz, double min, double max, double def);
INGRV_e ing(char *p_head, double *p_value, double min, double max, double def);

/***
 * prints typed configuration parameters and gets parameter id and values from the user
 * depending on the requested operation.
 * Used with the generic configuration parameter system of the IMTQ and IEPS.
 */
Boolean config_param_info(CONFIG_PARAM_OP_e op, unsigned short* p_param_id, void* vp_param_data);

/***
 * prints an error message if the error code differs from 0
 */
void print_error(int error);

#endif /* COMMON_H_ */
