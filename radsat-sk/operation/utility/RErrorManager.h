/**
 * @file RErrorManager.h
 * @date April 30, 2022
 * @author Tyrel Kostyk
 */

#ifndef RERRORMANAGER_H_
#define RERRORMANAGER_H_

#include <stdint.h>
#include <hal/errors.h>


/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

/** Unique IDs for every internal RADSAT-SK software module. */
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
	moduleCount
} module_t;


/** Unique IDs for every external component (HAL drivers, satellite components, etc.). */
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
	componentHalRtc			= 11,
	componentHalWdogTask	= 12,
	componentHalFreeRtos	= 13,

	// Satellite Subsystem Interface (SSI) Drivers
	componentSsiTransceiver	= 14,
	componentSsiAntenna		= 15,

	// Number of External Components
	componentCount
} component_t;


// TODO: define telemetry struct/proto message for error telemetry (i.e. a summary using the error records)

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

void errorReportInternal(module_t module, int error);
void errorReportExternal(component_t component, int error);

void errorTelemetry(void* errors);

#endif /* RERROR_H_ */
