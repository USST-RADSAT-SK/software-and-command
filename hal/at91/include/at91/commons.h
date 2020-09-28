/*!
 * @file	commons.h
 * @date	June, 2012
 * @author	Akhil Piplani
 *
 * @section DESCRIPTION
 * Some commonly used preprocessor directives.
 */

#ifndef COMMONS_H_
#define COMMONS_H_

/*!
 * Used by drivers to tell the callbacks if they are being called from
 * within an ISR or from a regular task. This is required because FreeRTOS
 * has different functions for handling semaphores from within an ISR and task.
 */
typedef enum _SystemContext {
	task_context = 0x00,//!< task_context
	isr_context = 0xFF  //!< isr_context
} SystemContext;

#define STORAGE_RESTART_ON_SEMAPHORE_ERRORS	0

/*!
 * Can be used to enable/disable debug functionalities across the project.
 */
#define FLIGHT_CONFIG	0

/*!
 * Setting this to 0 will lead to standard stdio.h and string.h functions
 * like printf being used instead of the ones implemented here.
 * Using AT91LIB implementations reduces code size at the expense of features.
 *
 *
 * A good tradeoff between using using newlib and AT91LIB implementations
 * is newlib-nano which can be used in-place of newlib by
 * making sure USE_AT91LIB_STDIO_AND_STRING is 0 and adding the following
 * options to the linker :
 * @verbatim
 * --specs=nano.specs -lc -u _printf_float -u _scanf_float
 * @endverbatim
 *
 * Further, not including the following linker options will reduce code size
 * even more at the expense of floating point I/O.
 * @verbatim
 * -u _printf_float -u _scanf_float
 * @endverbatim
 *
 *
 * It is required to have this as a configuration in the project properties
 * in the compiler section when setting this to 1.
 *
 * @note If you set this to 1, be sure to use functions implemented in util.c
 * instead of scanf etc to get input from the user over the debug port.
 *
 * @see util.c stdio.c string.c
 */
#ifndef USE_AT91LIB_STDIO_AND_STRING
	#define USE_AT91LIB_STDIO_AND_STRING	0
#endif

#endif /* COMMONS_H_ */
