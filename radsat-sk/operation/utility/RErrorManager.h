/**
 * @file RErrorManager.h
 * @date April 30, 2022
 * @author Tyrel Kostyk (tck290)
 */

#ifndef RERRORMANAGER_H_
#define RERRORMANAGER_H_

#include <stdint.h>
#include <hal/errors.h>
#include <RFileTransfer.pb.h>


/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

/** Success flag. Used to indicate success, in the absence of errors. */
#define SUCCESS			(0)
/** Generic Error flag. Used to indicate a general error, when further elaboration is not necessary. */
#define E_GENERIC		(-1)

/**
 * Unique IDs for every internal RADSAT-SK software module.
 * NOTE: Ensure that "error_report_summary.modules" has "max_count" equal to "moduleCount" below.
 */
typedef enum _module_t {
	// Application Modules
	moduleMain 			= 0,
	moduleDosimeterTask	= 1,
	moduleRxTask 		= 2,
	moduleTxTask		= 3,
	moduleImageTask		= 4,
	moduleAdcsTask		= 5,
	moduleTelemetryTask = 6,
	moduleWatchdogTask	= 7,

	// Operation Modules
	moduleProtocolService		= 8,
	moduleTelecommandService	= 9,
	moduleFileTransferService	= 10,
	moduleMessage				= 11,
	moduleXorCipher				= 12,
	moduleProtobuf				= 13,
	moduleKey					= 14,
	moduleImage					= 15,
	moduleSunSensor				= 16,
	moduleDebug					= 17,
	moduleError					= 18,

	moduleObc			= 19,
	moduleDosimeter		= 20,
	moduleTransceiver	= 21,
	moduleCamera		= 22,
	modulePdb			= 23,
	moduleBattery		= 24,
	moduleAntenna		= 25,

	// Framework Modules
	moduleI2c	= 26,
	moduleFram	= 27,
	moduleUart	= 28,

	// Number of Internal Software Modules
	moduleCount = 29
} module_t;


/**
 * Unique IDs for every external component (HAL drivers, satellite components, etc.).
 * NOTE: Ensure that "error_report_summary.components" has "max_count" equal to "componentCount" below.
 */
typedef enum _component_t {
	// Physical Satellite Components
	componentObc				= 0,
	componentDosimeterTop		= 1,
	componentDosimeterBottom	= 2,
	componentTransceiver		= 3,
	componentCamera				= 4,
	componentPdb				= 5,
	componentBattery			= 6,
	componentAntenna			= 7,

	// Hardware Abstraction Library (HAL) Drivers
	componentHalI2c			= 8,
	componentHalFram		= 9,
	componentHalUart		= 10,
	componentHalRtc			= 11,	// Also accounts for RTT module
	componentHalTime		= 12,
	componentHalSupervisor	= 13,
	componentHalOther		= 14,	// Misc uncommon HAL features
	componentHalWdogTask	= 15,
	componentHalFreeRtos	= 16,

	// Satellite Subsystem Interface (SSI) Drivers
	componentSsiTransceiver	= 17,
	componentSsiAntenna		= 18,

	// Number of External Components
	componentCount = 19
} component_t;


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int errorReportModule(module_t module, int errorReported);
int errorReportComponent(component_t component, int errorReported);

int errorTelemetry(error_report_summary* summary);

#endif /* RERROR_H_ */
