/**
 * @file RCommon.h
 * @date February 27, 2022
 * @author Tyrel Kostyk (tck290)
 */

#ifndef RCOMMON_H_
#define RCOMMON_H_

#include <RErrorManager.h>
#include <RDebug.h>


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

/** Success flag. Used to indicate success, in the absence of errors. */
#define SUCCESS			(0)
/** Generic Error flag. Used to indicate a general error, when further elaboration is not necessary. */
#define E_GENERIC		(-1)

#endif /* RCOMMON_H_ */
