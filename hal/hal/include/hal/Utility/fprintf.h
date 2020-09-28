/*!
 * @file	fprintf.h
 * @brief	Implements fprintf(...) like functionality.
 * @date	Apr 06, 2015
 * @author	Pieter Botma
 */

#ifndef FPRINTF_H_
#define FPRINTF_H_

#include <hcc/api_fat.h>

/*!
 * @brief
 *   Mimics fprintf(...) functionality from <stdio.h> but writing it to the
 *   specified file.
 * @param [in] fp
 *   Pointer to the file object
 * @param [in] str
 *   Pointer to the format string
 * @param [in] ...
 *   Additional arguments
 * @return
 *   EOF if unsuccessful,
 *   number of character written if successful.
 */
int f_printf ( F_FILE* fp, const char* str,	... );

#endif /* FPRINTF_H_ */
