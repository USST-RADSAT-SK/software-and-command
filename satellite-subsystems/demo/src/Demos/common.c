/*
 * common.c
 *
 * Contains functions that are used in across multiple demo's
 *
 *  Created on: 5 okt. 2015
 *      Author: lrot
 */

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

#include <stddef.h>			// for size_t
#include <stdlib.h>
#include <stdio.h>
#include <string.h>			// used for memset/memcpy/strncpy functions
#include <stdbool.h>		// 'normal' boolean values; note: these are incompatible with the ones defined by Boolean.h in the hal library

#include <math.h>			// for the exp function used in the user input parser function ing()
#include <float.h>			// floating point min and max values
#include <stdint.h>			// integer data type min and max values
#include <inttypes.h>		// printf format codes

#include <Demos/common.h>

// ***
// input number generic functionality
//

void ing_debug_show_input_test()
{
	INGRV_e ingrv;
	double value = 0;
	printf(" \r\nDEBUG: voiding_debug_show_input_test commence! \r\n");
	while(1)
	{
		ingrv = ing("provide test value: ", &value, -32768, 32767, 0);

		switch(ingrv)
		{
		case INGRV_val: printf("\r\nvalid; value = %lf \r\n", value); break;
		case INGRV_inv: printf("\r\ninvalid; value = %lf \r\n", value); break;
		case INGRV_rng: printf("\r\nout of range; value = %lf \r\n", value); break;
		case INGRV_esc: printf("\r\nescape; value =  %lf; ... exiting ... \r\n", value); break;
		default: printf("unknow rv %d", ingrv); break;
		}

		if(INGRV_esc == ingrv) break;	// exit while
	}
}


void ing_debug_show_input_ascii()
{
	char key; char _tmp[10]; unsigned int i;
	printf(" \r\nDEBUG: ing_debug_show_input_ascii commence! \r\n");
	while(1)
	{
		while( !DBGU_IsRxReady() )		// while no character provided over the UART ...
		{
			WDT_forceKickEveryNms(10);	// kick the WDG every 10 ms; too fast will trigger it, too slow as well
		}

		key = DBGU_GetChar();

		memset(_tmp, 0, sizeof(_tmp));
		snprintf(_tmp, sizeof(_tmp), "%#04x ", key);
		for(i=0; i < sizeof(_tmp); i++)
		{
			if(_tmp[i] == '\0') break;
			DBGU_PutChar(_tmp[i]);				// print the ascii value(s) of the input
		}
	}
}

INGRV_e ing_interact(char *p_head, char *p_buf, size_t buf_sz, size_t *p_gotten_sz)
{
	#define HEAD_STR_MAX_SZ		255				// max head string length
	#define KEY_RETURN			0x0D			// ASCII carriage return (i.e. 'enter')
	#define KEY_BACK			0x08			// ASCII backspace
	#define KEY_DEL				0x7F			// ASCII del control character
	#define KEY_ESC				0x1B			// ASCII escape control character

	typedef enum _valt_enum
	{
		_valt_dec,						// by default decimal values are assumed
		_valt_bin,						// binary starts with 0b or 0B
		_valt_oct,						// octal starts with 0o or 0O
		_valt_hex,						// hexadecimal starts with 0x or 0X
	} _valt_e;

	_valt_e valt = _valt_dec;
	unsigned int sz, i;

	unsigned char ach;					// current and previous ascii character codes received
	bool escm = false;					// after a single escape char we'll be entering 'escape mode'

	memset(p_buf, 0, buf_sz);			// clear up the buffer receiving the resulting string
	*p_gotten_sz = 0;					// reset the gotten size to 0

	if(p_head != NULL)					// was a head provided?
	{
		for(i = 0; i < HEAD_STR_MAX_SZ; i++)	// push the head string out until either \0 or max length reached
		{
			if(p_head[i] == 0) break;

			DBGU_PutChar(p_head[i]);

			WDT_forceKickEveryNms(10);	// kick the WDG every 10 ms; too fast will trigger it, too slow as well
		}

		if(i >= HEAD_STR_MAX_SZ)		// maximum reached? probably forgot the NUL character ... error!
		{
			TRACE_ERROR("\r\n\r\n p_head too long (> 255)! Forgot \\0 character? \r\n\r\n");
			return INGRV_inv;
		}
	}

	sz = 0;
	while(1)
	{
		while( !DBGU_IsRxReady() )		// while no character provided over the UART ...
		{
			WDT_forceKickEveryNms(10);	// kick the WDG every 10 ms; too fast will trigger it, too slow as well
		}

		ach = DBGU_GetChar();			// get the new character

		if(ach == KEY_BACK || ach == KEY_DEL)	// is it the backspace or delete char?
		{
			if(sz > 0)
			{
				sz--;					// remove the last character from the buffer by decrementing the size
				DBGU_PutChar(ach);		// put the control character to the terminal so it is properly updated
			}

			if(sz < 2) valt = _valt_dec;// less than two characters received? revert to value type 'decimal'
		}
		else if(ach == KEY_RETURN)		// is it the return key?
		{
			break;						// start processing the value
		}
		else if(ach == KEY_ESC)			// escape char?
		{
			if(escm)					// already in escape mode? (i.e. escape pressed twice in a row)
			{
				return INGRV_esc;		// escape!
			}

			escm = !escm;				// toggle escape mode
		}
		else if(escm)					// when escape mode is entered we'll be capturing all key presses instead of the regular input
		{
			// nothing here yet but we could start doing some arrow key processing
			// hence any key other than ESC will cause exiting escape mode again
			if(1) escm = false;
		}
		else if(sz < (buf_sz-1))		// do we have buffer space to store the char? if not we'll simply ignore that char; the -1 makes sure we keep the last char '\0'
		{
			if(sz == 1 && p_buf[0] == '0')		// is a special format entered?
			{
				if((ach == 'b' || ach == 'B'))
				{
					valt = _valt_bin;			// update value-type
					p_buf[sz++] = ach;			// add the character to the buffer
					DBGU_PutChar(ach);			// echo the character to the terminal
				}
				else if((ach == 'o' || ach == 'O'))
				{
					valt = _valt_oct;			// update value-type
					p_buf[sz++] = ach;			// add the character to the buffer
					DBGU_PutChar(ach);			// echo the character to the terminal
				}
				else if((ach == 'x' || ach == 'X'))
				{
					valt = _valt_hex;			// update value-type
					p_buf[sz++] = ach;			// add the character to the buffer
					DBGU_PutChar(ach);			// echo the character to the terminal
				}
			}
			else if(	((valt == _valt_bin) && ((ach >= '0') && (ach <= '1')))
					|| 	((valt == _valt_oct) && ((ach >= '0') && (ach <= '7')))
					|| 	((valt == _valt_hex) && (((ach >= '0') && (ach <= '9')) || ((ach >= 'a') && (ach <= 'f')) || ((ach >= 'A') && (ach <= 'F'))))
					|| 	((valt == _valt_dec) && (((ach >= '0') && (ach <= '9')) || (ach == '+') || (ach == '-') || (ach == '.') || (ach == 'e') || (ach == 'E'))))
			{
				p_buf[sz++] = ach;			// add the character to the buffer

				DBGU_PutChar(ach);			// echo the character to the terminal; any backspace will be processed properly as well
			}
		}
	}

	*p_gotten_sz = sz;			// return th eresult count to the output

	return INGRV_val;
}

INGRV_e ing_parse(double *p_value, char *p_buf, size_t buf_sz, double min, double max, double def)
{
	typedef enum _valt_enum
	{
		_valt_dec,												// by default decimal values are assumed
		_valt_bin,												// binary starts with 0b or 0B
		_valt_oct,												// octal starts with 0o or 0O
		_valt_hex,												// hexadecimal starts with 0x or 0X
	} _valt_e;

	_valt_e valt = _valt_dec;
	unsigned int i;
	double val = 0.0;

	if(buf_sz == 0)												// nothing typed? return default
	{
		*p_value = def;

		if(def < min) return INGRV_rng;						// we still perform a range check; this way the callee can easily force entering a value
		if(def > max) return INGRV_rng;						// by providing a default that is outside the valid range

		return INGRV_val;
	}

	if(p_buf[0] == '0')											// check for special formats
	{
		if(p_buf[1] == 'b' || p_buf[1] == 'B') valt = _valt_bin;
		if(p_buf[1] == 'o' || p_buf[1] == 'O') valt = _valt_oct;
		if(p_buf[1] == 'x' || p_buf[1] == 'X') valt = _valt_hex;
	}

	switch(valt)
	{
	case _valt_dec:
	{
		double cw = 1., expv = 1.0;
		bool frac = false, expo = false, sign = false;
		i = buf_sz-1;											// move back from the last entered character
		while(1)
		{
			if(p_buf[i] == '.')									// fractional dot?
			{
				if(frac) return INGRV_inv;						// already processed a dot before? invalid number!
				frac = true;									// we've encountered our only possible fractional dot
				val = val / cw;									// correct the value for the character weight offset
				cw = 1.;										// reset character weight
			}
			else if(p_buf[i] == 'e' || p_buf[i] == 'E')			// exponential?
			{
				if(frac) return INGRV_inv;						// already processed a dot before this exponential? invalid number!
				if(expo) return INGRV_inv;						// already processed an exponential indicator? invalid number!
				if(i == 0) return INGRV_inv;					// exponential indicator at the beginning? invalid number!
				expo = true;									// we've encountered our only possible exponential indicator
				sign = false;									// reset sign flag, we are allowed another at the start of the value
				expv = exp(val*M_LN10);							// store provided exponential as double
				cw = 1.;										// reset character weight
				val = 0.;										// reset value
			}
			else if(p_buf[i] == '+')							// plus?
			{
				if(sign) return INGRV_inv;						// already processed a sign before? invalid number!
				if(i > 0 && p_buf[i-1] != 'e' && p_buf[i-1] != 'E') return INGRV_inv;	// sign only allowed at the start or after an exponential
				sign = true;
			}
			else if(p_buf[i] == '-')							// minus?
			{
				if(sign) return INGRV_inv;						// already processed a sign before? invalid number!
				if(i > 0 && p_buf[i-1] != 'e' && p_buf[i-1] != 'E') return INGRV_inv;	// sign only allowed at the start or after an exponential
				sign = true;
				val *= -1.0;									// implement the sign onto the value
			}
			else if((p_buf[i] >= '0') || (p_buf[i] <= '9'))
			{
				val += (((double)(p_buf[i] - '0')) * expv) * cw;
				cw *= 10.;										// increment character weight
			}
			else return INGRV_inv;								// invalid character received

			if(i == 0) break;									// end of value?
			i--;												// move to next character
		}
		break;
	}
	case _valt_bin:
		for(i = 2; i < buf_sz; i++)								// move through the characters from the start towards the back
		{
			if((p_buf[i] >= '0') && (p_buf[i] <= '1'))
			{
				val = (val * 2.) + (p_buf[i] - '0');			// promote compounded value to the next weight, and add the next character
			}
			else return INGRV_inv;								// invalid character received
		}
		break;
	case _valt_oct:
		for(i = 2; i < buf_sz; i++)								// move through the characters from the start towards the back
		{
			if((p_buf[i] >= '0') && (p_buf[i] <= '7'))
			{
				val = (val * 8.) + (p_buf[i] - '0');			// promote compounded value to the next weight, and add the next character
			}
			else return INGRV_inv;								// invalid character received
		}
		break;
	case _valt_hex:
		for(i = 2; i < buf_sz; i++)								// move through the characters from the start towards the back
		{
			if(p_buf[i] >= '0' && p_buf[i] <= '9')
			{
				val = (val * 16.) + (p_buf[i] - '0');			// promote compounded value to the next base, and add the next character
			}
			else if(p_buf[i] >= 'a' && p_buf[i] <= 'f')
			{
				val = (val * 16.) + (10. + p_buf[i] - 'a');		// promote compounded value to the next base, and add the next character
			}
			else if(p_buf[i] >= 'A' && p_buf[i] <= 'F')
			{
				val = (val * 16.) + (10. + p_buf[i] - 'A');		// promote compounded value to the next base, and add the next character
			}
			else return INGRV_inv;								// invalid character received
		}
		break;
	}

	*p_value = val;

	if(val < min) return INGRV_rng;							// we do the range check at the end; indication only, so the callee can decide what to do
	if(val > max) return INGRV_rng;

	return INGRV_val;
}

INGRV_e ing(char *p_head, double *p_value, double min, double max, double def)
{
	// the 'input number generic' (ING) function handles numeric user input
	// it accepts multiple number formats and returns these as a double
	// * p_head is a string of up to 255 chars and is pushed out first without alteration or delays (hence no \n required for it to be pushed out).
	//   set to NULL if no header is required.
	// * the value provided by the user is returned in p_value
	// * control characters:
	//		DEL or BACKSPACE		remove a character
	//		ESC 					exit value entry function without committing
	//		RETURN					commits the text entered
	// * echos typed numbers
	// * number entry styles:
	//		0b010101				starting with 0b or 0B sets binary mode. Only 0 and 1 are allowed characters. Always unsigned.
	//		0o010101				starting with 0o or 0O sets octal mode. Only 0 through 7 are allowed characters. Always unsigned.
	//		0x010101				starting with 0x or 0X sets hexadecimal mode. Only 0 through F are allowed characters. Always unsigned.
	//		-123.456E-05			any other starting chars are interpreted as (floating point) decimal.
	//		123.456E05				another decimal floating point example. +/- signs are optional on both start and exponential
	//		123.456					another decimal floating point example. exponential is optional
	//		-123					another decimal integer example. it also understands simple stuff
	// * the output stream is left sitting after the last echo-ed character; add return characters if desired
	// note: 	the 8-byte double value range encompasses the entire int32 value range
	//			and can therefore represent any 32-bit int without loss of precision
	// 			the callee can simply cast the double to whichever integer/float type required
	// note2:	platform independent except for the WDG kick and character read/write functions (i.e. easily portable)
	// written: LBM Rotthier for ISISpace


	char buf[255];
	INGRV_e rv;
	size_t gotten;

	rv = ing_interact(p_head, buf, sizeof(buf), &gotten);
	if(rv) return rv;

	rv = ing_parse(p_value, buf, gotten, min, max, def);
	return rv;
}


/**
 *  Enumeration of all parameter types.
 *  we're using a locally defined version which equals the ieps and imtq defined lists
 *  For production code imtq_gnlparam_t and ieps_gnlparam_t enumerations should be used instead
 */
typedef enum __attribute__ ((__packed__)) _gnlparam_struct
{
    _int8_val = 0x01, ///< The parameter is an integer type signed 8 bits value
    _uint8_val = 0x02, ///< The parameter is an integer type unsigned 8 bits value
    _int16_val = 0x03, ///< The parameter is an integer type signed 16 bits value
    _uint16_val = 0x04, ///< The parameter is an integer type unsigned 16 bits value
    _int32_val = 0x05, ///< The parameter is an integer type signed 32 bits value
    _uint32_val = 0x06, ///< The parameter is an integer type unsigned 32 bits value
    _float_val = 0x07, ///< The parameter is a float type value
    _int64_val = 0x08, ///< The parameter is an integer type signed 8 bits value
    _uint64_val = 0x09, ///< The parameter is an integer type signed 8 bits value
    _double_val = 0x0A ///< The parameter is a double type value
} _gnlparam_t;

Boolean config_param_info(CONFIG_PARAM_OP_e op, unsigned short* p_param_id, void* vp_param_data)
{
	// this demo helper function is used with the configuration parameter commands
	// and handles user interaction
	// it has three functionalities:
	// 1) ask for param-id
	// 2) ask for param-id and accompanying data
	// 3) simply show the parameter information to the user

	unsigned char ptype;
	double value;
	int rv;

	if(p_param_id == NULL)														// check param-id pointer is provided
	{
		TRACE_ERROR(" p_param_id is NULL!");
		return FALSE;
	}

	if(op != CONFIG_PARAM_OP_ask_parid && vp_param_data == NULL)				// check param-data is provided
	{
		TRACE_ERROR(" vp_param_data is NULL!");
		return FALSE;
	}

	if(op == CONFIG_PARAM_OP_print)												// need to show the param-id to the user?
	{
		printf(" param-id = %#06x \r\n", *p_param_id);							// show it
	}
	else																		// need to get the param-id from the user!
	{
		*p_param_id = 0;														// clear variable receiving input data

		rv = ing(" provide param-id [0]: ", &value, 0, 65535, 0); 				// get parameter id from the user
		printf("\r\n");
		if(rv)
		{
			TRACE_ERROR("\r\n invalid number!\r\n");
			return FALSE;
		}

		*p_param_id = (unsigned short) value;									// drop it into the output variable

		if(op == CONFIG_PARAM_OP_ask_parid) return TRUE;						// only getting param_id? then we're done!
	}

	ptype = (unsigned char) (((*p_param_id) >> 12) & 0x000F);					// get the high nibble of the high byte; its the param type

	switch(ptype)																// apply the format depending on the parameter type
	{
	case _int8_val: {
		int8_t val = 0;
		if(op == CONFIG_PARAM_OP_print)
		{
			val = *((int8_t *) vp_param_data);
		}
		else if(op == CONFIG_PARAM_OP_ask_parid_and_data)
		{
			rv = ing(" provide int8 param-value [0]: ", &value, INT8_MIN, INT8_MAX, 0); printf("\r\n"); if(rv) return FALSE;
			val = (int8_t) value;
			memcpy(vp_param_data, &val, sizeof(val));							// copy provided values to the output
		}
		printf(" param-value = %+03" PRId8 " (%#04" PRIx8 ") \r\n", val, val);
		break;
	}
	case _uint8_val: {
		uint8_t val = 0;
		if(op == CONFIG_PARAM_OP_print)
		{
			val = *((uint8_t *) vp_param_data);
		}
		else if(op == CONFIG_PARAM_OP_ask_parid_and_data)
		{
			rv = ing(" provide uint8 param-value [0]: ", &value, 0, UINT8_MAX, 0); printf("\r\n"); if(rv) return FALSE;
			val = (uint8_t) value;
			memcpy(vp_param_data, &val, sizeof(val));							// copy provided values to the output
		}
		printf(" param-value = %03" PRIu8 " (%#04" PRIx8 ") \r\n", val, val);
		break;
	}
	case _int16_val: {
		int16_t val = 0;
		if(op == CONFIG_PARAM_OP_print)
		{
			val = *((int16_t *) vp_param_data);
		}
		else if(op == CONFIG_PARAM_OP_ask_parid_and_data)
		{
			rv = ing(" provide int16 param-value [0]: ", &value, INT16_MIN, INT16_MAX, 0); printf("\r\n"); if(rv) return FALSE;
			val = (int16_t) value;
			memcpy(vp_param_data, &val, sizeof(val));							// copy provided values to the output
		}
		printf(" param-value = %+06" PRId16 " (%#06" PRIx16 ") \r\n", val, val);
		break;
	}
	case _uint16_val: {
		uint16_t val = 0;
		if(op == CONFIG_PARAM_OP_print)
		{
			val = *((unsigned short *) vp_param_data);
		}
		else if(op == CONFIG_PARAM_OP_ask_parid_and_data)
		{
			rv = ing(" provide uint16 param-value [0]: ", &value, 0, UINT16_MAX, 0); printf("\r\n"); if(rv) return FALSE;
			val = (uint16_t) value;
			memcpy(vp_param_data, &val, sizeof(val));							// copy provided values to the output
		}
		printf(" param-value = %05" PRIu16 " (%#06" PRIx16 ") \r\n", val, val);
		break;
	}
	case _int32_val: {
		int32_t val = 0;
		if(op == CONFIG_PARAM_OP_print)
		{
			val = *((int32_t *) vp_param_data);
		}
		else if(op == CONFIG_PARAM_OP_ask_parid_and_data)
		{
			rv = ing(" provide int32 param-value [0]: ", &value, INT32_MIN, INT32_MAX, 0); printf("\r\n"); if(rv) return FALSE;
			val = (int32_t) value;
			memcpy(vp_param_data, &val, sizeof(val));							// copy provided values to the output
		}
		printf(" param-value = %+011" PRId32 " (%#010" PRIx32 ") \r\n", val, val);
		break;
	}
	case _uint32_val: {
		uint32_t val = 0;
		if(op == CONFIG_PARAM_OP_print)
		{
			val = *((uint32_t *) vp_param_data);
		}
		else if(op == CONFIG_PARAM_OP_ask_parid_and_data)
		{
			rv = ing(" provide uint32 param-value [0]: ", &value, 0, UINT32_MAX, 0); printf("\r\n"); if(rv) return FALSE;
			val = (uint32_t) value;
			memcpy(vp_param_data, &val, sizeof(val));							// copy provided values to the output
		}
		printf(" param-value = %010" PRIu32 " (%#010" PRIx32 ") \r\n", val, val);
		break;
	}
	case _float_val: {
		float val = 0;
		if(op == CONFIG_PARAM_OP_print)
		{
			val = *((float *) vp_param_data);
		}
		else if(op == CONFIG_PARAM_OP_ask_parid_and_data)
		{
			rv = ing(" provide float param-value [0]: ", &value, FLT_MIN, FLT_MAX, 0); printf("\r\n"); if(rv) return FALSE;
			val = (float) value;
			memcpy(vp_param_data, &val, sizeof(val));							// copy provided values to the output
		}
		printf(" param-value = %+0.4E \r\n", val);
		break;
	}
	case _int64_val: {
		int64_t val = 0;
		if(op == CONFIG_PARAM_OP_print)
		{
			val = *((int64_t *) vp_param_data);
		}
		else if(op == CONFIG_PARAM_OP_ask_parid_and_data)
		{
			// double range is insufficient to represent an 64 bit integer
			// hence we'll ask for separate bytes instead
			unsigned int i;
			char tmp[128];
			uint8_t *p_chr = (uint8_t *) &val;									// get a pointer to the memory where the long long is located
			printf(" provide int64 param-value as 8 bytes (little-endian, i.e. low to high): \r\n");
			for(i = 0; i < 8; i++)
			{
				snprintf(tmp, sizeof(tmp), " byte[%d] [0] = ", i);				// build the string that asks for the bytes from the user
				do {
					rv = ing(tmp, &value, 0, 255, 0); printf("\r\n");			// get the byte from the user
					if(rv == INGRV_esc) return FALSE;							// escape? exit
				} while(rv != INGRV_val);										// with anything other than valid we'll retry
				p_chr[i] = (uint8_t) value;										// put the char provided by the user in the long long
			}
			memcpy(vp_param_data, &val, sizeof(val));							// copy provided values to the output
		}
		printf(" param-value = %020" PRId64 " (%#018" PRIx64 ") \r\n", val, val);
		break;
	}
	case _uint64_val: {
		uint64_t val = 0;
		if(op == CONFIG_PARAM_OP_print)
		{
			val = *((uint64_t *) vp_param_data);
		}
		else if(op == CONFIG_PARAM_OP_ask_parid_and_data)
		{
			// double range is insufficient to represent an 64 bit integer
			// hence we'll ask for separate bytes instead
			unsigned int i;
			char tmp[128];
			uint8_t *p_chr = (uint8_t *) &val;									// get a pointer to the memory where the long long is located
			printf(" provide uint64 param-value as 8 bytes (little-endian, i.e. low to high): \r\n");
			for(i = 0; i < 8; i++)
			{
				snprintf(tmp, sizeof(tmp), " byte[%d] [0] = ", i);				// build the string that asks for the bytes from the user
				do {
					rv = ing(tmp, &value, 0, 255, 0); printf("\r\n");			// get the byte from the user
					if(rv == INGRV_esc) return FALSE;							// escape? exit
				} while(rv != INGRV_val);										// with anything other than valid we'll retry
				p_chr[i] = (uint8_t) value;										// put the char provided by the user in the long long
			}
			memcpy(vp_param_data, &val, sizeof(val));							// copy provided values to the output
		}
		printf(" param-value = %020" PRIu64 " (%#018" PRIx64 ") \r\n", val, val);
		break;
	}
	case _double_val: {
		double val = 0;
		if(op == CONFIG_PARAM_OP_print)
		{
			val = *((double *) vp_param_data);
		}
		else if(op == CONFIG_PARAM_OP_ask_parid_and_data)
		{
			rv = ing(" provide double param-value [0]: ", &value, DBL_MIN, DBL_MAX, 0); printf("\r\n"); if(rv) return FALSE;
			val = (double) value;
			memcpy(vp_param_data, &val, sizeof(val));							// copy provided values to the output
		}
		printf(" param-value = %+0.8lE \r\n", val);
		break;
	}
	default: TRACE_ERROR(" parameter type invalid! \r\n"); return FALSE; break;
	}

	return TRUE;
}

void print_error(int error)
{
	if(error != E_NO_SS_ERR)
	{
	    TRACE_ERROR("ERROR %d encountered while executing the request\n\r", error);
	}
}
