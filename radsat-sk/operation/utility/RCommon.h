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

/** SETTINGS */
/** Main Settings */
#define OBC_WDOG_KICK_PERIOD_MS	(15 / portTICK_RATE_MS)	// How often the internal OBC Watchdog is kicked (i.e. pet, i.e. reset) in ms
#define DEFAULT_TASK_STACK_SIZE	(4096)					// Default stack size (in bytes) allotted to each FreeRTOS Task.

/**  */
#define COMMS_MIN_DELAY 	(10)
#define COMMS_QUIET_DELAY	(500 - COMMS_MIN_DELAY)
#define MAX_PASS_MODE_DURATION	((portTickType)(15*60*1000*portTICK_RATE_MS))	// Maximum possible duration of a pass is 15 minutes; value set in ms.

/** Camera Settings */
#define IMAGE_CAPTURE_DELAY_MS				(1000)		// Delay in milliseconds to allow image capture to be completed
#define DOWNLOAD_TASK_STACK_SIZE			(configMINIMAL_STACK_SIZE + 50)	// Stack size (in bytes) allotted to the image download FreeRTOS Task
#define VALID_IMAGE_RETRY_COUNT				(10)		// Number of attempts for image capture with successful detection results

#define IMAGE_CAPTURES_PER_DAY				(1)			// How many images to capture per day.
#define IMAGE_CAPTURE_TASK_NORMAL_DELAY_MS	(MS_PER_DAY / IMAGE_CAPTURES_PER_DAY)	// Image Capture Task normal delay (in ms).
#define IMAGE_CAPTURE_TASK_SHORT_DELAY_MS	(10000)		// Image Capture Task short delay (in ms), used when task couldn't execute because CubeSense is in use.

/** ADCS Settings */
#define ADCS_CAPTURES_PER_HOUR				(1)			// How many ADCS readings to capture per hour.
#define ADCS_CAPTURE_TASK_NORMAL_DELAY_MS	(MS_PER_HOUR / ADCS_CAPTURES_PER_HOUR)	// ADCS Capture Task normal delay (in ms).
#define ADCS_CAPTURE_TASK_SHORT_DELAY_MS	(10000)		// ADCS Capture Task short delay (in ms).

/** Dosimeter Settings */
#define DOSIMETER_READINGS_PER_DAY			(6)			// How many dosimeter payload readings to collect per day.
#define DOSIMETER_COLLECTION_TASK_DELAY_MS	(1000)		// Dosimeter Collection Task delay (in ms).

/** Watchdog Settings */
#define SATELLITE_WATCHDOG_TASK_DELAY_MS	(100)		// Satellite Watchdog Task delay (in ms)

/** Telemetry Settings */
#define TELEMETRY_READINGS_PER_HOUR			(10)		// How many telemetry readings to collect per hour.
#define TELEMETRY_COLLECTION_TASK_DELAY_MS	(MS_PER_HOUR / TELEMETRY_READINGS_PER_HOUR)	// Telemetry Collection Task delay (in ms).

#endif /* RCOMMON_H_ */
