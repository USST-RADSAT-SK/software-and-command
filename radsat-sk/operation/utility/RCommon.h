/**
 * @file RCommon.h
 * @date February 27, 2022
 * @author Tyrel Kostyk (tck290)
 */

#ifndef RCOMMON_H_
#define RCOMMON_H_

#include <RErrorManager.h>	// Error Reporting, Handling
#include <RProtobuf.h>		// Message Struct Definitions (telemetry, etc.)
#include <RDebug.h>			// Debugging Printing


/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

/** How many ms in a second. */
#define MS_PER_SECOND		(1000)
/** How many ms in a minute. */
#define MS_PER_MINUTE		(1000 * 60)
/** How many ms in an hour. */
#define MS_PER_HOUR			(1000 * 60 * 60)
/** How many ms in a day. */
#define MS_PER_DAY			(1000 * 60 * 60 * 24)
/** How many ms in a week. */
#define MS_PER_WEEK			(1000 * 60 * 60 * 24 * 7)

#endif /* RCOMMON_H_ */
