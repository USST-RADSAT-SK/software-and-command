/**
 * @file RErrorManager.c
 * @date April 30, 2022
 * @author Tyrel Kostyk (tck290)
 */

#include <RErrorManager.h>
#include <RFileTransferService.h>
#include <RTransceiver.h>

#include <hal/supervisor.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


/***************************************************************************************************
                                   DEFINITIONS & PRIVATE VARIABLES
***************************************************************************************************/

/** Times that a module/componenent must be error-free before having its error count cleared. */
#define	MODULE_ERROR_RESET_TIME_MS		((uint32_t)5*60*1000)	///> 5 minutes
#define	COMPONENT_ERROR_RESET_TIME_MS	((uint32_t)30*60*1000)	///> 30 minutes

/** Error count thresholds before restorative action is taken (i.e. reset). */
#define MODULE_ERROR_THRESHOLD		((uint8_t)10)	///> 10 internal errors allowed every 5 minutes
#define COMPONENT_ERROR_THRESHOLD	((uint8_t)2)	///> 2 component errors allowed every 30 minutes

/** Local arrays for tracking error records for all modules and components. */
static error_record moduleErrors[moduleCount] = { 0 };
static error_record componentErrors[componentCount] = { 0 };


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Report an error that occured on an internal software module.
 *
 * Records error counts, and if a threshold is exceeded, active measures are taken to remedy the
 * issue (resets, etc.). Error counts are reset if no errors are recorded for a certain duration
 * of time. When an error occurs, a single error report is sent to the File Transfer Service. A
 * cohesive summary report is also available for all modules/components via @sa errorTelemetry().
 *
 * @param module The unique ID of the module which experienced an error (returned with error code).
 * @param errorReported The exact error code returned from the module.
 * @return 0 for Success, otherwise failure.
 */
int errorReportModule(module_t module, int errorReported) {

	int error = SUCCESS;

	// record time of previous error occurance
	uint32_t oldTime = moduleErrors[module].timeRecorded;

	// obtain current time
	uint32_t currentTime = xTaskGetTickCount();

	// record time of new error occurance
	moduleErrors[module].timeRecorded = currentTime;

	// if enough time has passed; clear previous error counts
	if ((currentTime - oldTime) > MODULE_ERROR_RESET_TIME_MS)
		moduleErrors[module].count = 0;

	// increment the error counter for this module
	moduleErrors[module].count++;

	// prepare error report
	module_error_report report = { 0 };
	report.error = errorReported;
	report.module = module;

	// send error report to the file transfer service (unless the error came from there)
	if (module != moduleFileTransferService) {
		error = fileTransferAddMessage(&report, sizeof(report), file_transfer_message_ModuleErrorReport_tag);
		if (error != SUCCESS) {
			errorReportModule(moduleFileTransferService, error);
		}
	}

	// reset the OBC if any module exceeds the internal error threshold
	if (moduleErrors[module].count > MODULE_ERROR_THRESHOLD) {
		// reset the error count
		moduleErrors[module].count = 0;

		// TODO: verify that this does in fact reset the OBC; look into alternatives?
		supervisor_generic_reply_t emptyReply = { 0 };
		error = Supervisor_reset(&emptyReply, SUPERVISOR_SPI_INDEX);
		if (error != SUCCESS) {
			errorReportComponent(componentHalSupervisor, error);
		}
	}

	return error;
}


/**
 * Report an error that occured on an external component (HAL, SSI, Satellite Component, etc.).
 *
 * Records error counts, and if threshold is exceeded, active measures are taken to remedy the
 * issue (resets, etc.). Error counts are reset if no errors are recorded for a certain duration
 * of time. When an error occurs, a single error report is sent to the File Transfer Service. A
 * cohesive summary report is also available for all modules/components via @sa errorTelemetry().
 *
 * @param component The unique ID of the component which experienced an error (returned with error code).
 * @param errorReported The exact error code returned from the component.
 * @return 0 for Success, otherwise failure.
 */
int errorReportComponent(component_t component, int errorReported) {

	int error = SUCCESS;

	// record time of previous error occurance
	uint32_t oldTime = componentErrors[component].timeRecorded;

	// obtain current time
	uint32_t currentTime = xTaskGetTickCount();

	// record time of new error occurance
	componentErrors[component].timeRecorded = currentTime;

	// if enough time has passed; clear previous error counts
	if ((currentTime - oldTime) > COMPONENT_ERROR_RESET_TIME_MS)
		componentErrors[component].count = 0;

	// increment the error counter for this component
	componentErrors[component].count++;

	// prepare error report
	component_error_report report = { 0 };
	report.error = errorReported;
	report.component = component;

	// send error report to the file transfer service
	error = fileTransferAddMessage(&report, sizeof(report), file_transfer_message_ComponentErrorReport_tag);
	if (error != SUCCESS) {
		errorReportModule(moduleFileTransferService, error);
	}

	// reset the component or OBC when they exceed the error threshold
	if (componentErrors[component].count > COMPONENT_ERROR_THRESHOLD) {

		// reset the error count for the component in question
		componentErrors[component].count = 0;

		// generic reply object for
		supervisor_generic_reply_t emptyReply = { 0 };

		switch (component) {

		// Dosimeters - no reset functionality; power-cycle OBC
		case (componentDosimeterTop):
		case (componentDosimeterBottom):
		// OBC Software & Hardware
		case (componentObc):
		case (componentHalI2c):
		case (componentHalFram):
		case (componentHalUart):
		case (componentHalRtc):
		case (componentHalSupervisor):
			// power-cycle OBC board (hard reset)
			error = Supervisor_powerCycleIobc(&emptyReply, SUPERVISOR_SPI_INDEX);
			if (error != SUCCESS) {
				errorReportComponent(componentHalSupervisor, error);
			}

			break;

		// Transceiver
		case (componentTransceiver):
		case (componentSsiTransceiver):
			// reset transceiver
			transceiverPowerCycle();
			break;

		// Antenna
		case (componentAntenna):
		case (componentSsiAntenna):
			// TODO: reset Antenna
			break;

		// Camera
		case (componentCamera):
			// TODO: reset camera
			break;

		// Power Distribution Board
		case (componentPdb):
			// TODO: reset PDB
			break;

		// Battery
		case (componentBattery):
			// TODO: reset Battery
			break;

		// OBC Internal Software
		case (componentHalTime):
		case (componentHalOther):
		case (componentHalWdogTask):
		case (componentHalFreeRtos):
			// TODO: soft reset OBC processor
			break;

		// do nothing
		default:
			break;
		}
	}

	return error;
}


/**
 * Provides a summary report of the recently tracked errors on the system.
 *
 * @param summary A message struct containing (active) error counts for all modules.
 * @return 0 for Success, otherwise failure.
 */
int errorTelemetry(error_report_summary* summary) {

	// ensure input pointer is valid
	if (summary == 0) {
		errorReportModule(moduleError, E_INPUT_POINTER_NULL);
		return E_INPUT_POINTER_NULL;
	}

	// populate error report summary message
	for (int module = 0; module < moduleCount; module++)
		summary->moduleErrorCount[module] = moduleErrors[module].count;

	for (int component = 0; component < componentCount; component++)
		summary->componentErrorCount[component] = componentErrors[component].count;

	return SUCCESS;
}
