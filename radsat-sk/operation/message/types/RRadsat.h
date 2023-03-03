

#ifndef PB_RRADSAT_PB_H_INCLUDED
#define PB_RRADSAT_PB_H_INCLUDED

#include <stdint.h>
#include <RProtocol.h>
#include <RTelecommands.h>
#include <RFileTransfer.h>

typedef uint8_t radsat_tag_t;

#define file_transfer_ObcTelemetry_tag          1 // Telemetry
#define file_transfer_TransceiverTelemetry_tag  2 // Telemetry
#define file_transfer_CameraTelemetry_tag       3
#define file_transfer_EpsTelemetry_tag          4 // Telemetry
#define file_transfer_BatteryTelemetry_tag      5 // Telemetry
#define file_transfer_AntennaTelemetry_tag      6 // Telemetry
#define file_transfer_DosimeterData_tag         7
#define file_transfer_ImagePacket_tag           8
#define file_transfer_ModuleErrorReport_tag     9
#define file_transfer_ComponentErrorReport_tag  10
#define file_transfer_ErrorReportSummary_tag    11
#define protocol_Ack_tag                        12
#define protocol_Nack_tag                       13
#define telecommand_BeginPass_tag               14
#define telecommand_BeginFileTransfer_tag       15
#define telecommand_CeaseTransmission_tag       16
#define telecommand_UpdateTime_tag              17
#define telecommand_Reset_tag                   18
#define file_transfer_adcs_detection_tag		19


#define file_transfer_ObcTelemetry_size         sizeof(obc_telemetry) + 1
#define file_transfer_TransceiverTelemetry_size sizeof(transceiver_telemetry) + 1
#define file_transfer_CameraTelemetry_size      sizeof(camera_telemetry) + 1
#define file_transfer_EpsTelemetry_size         sizeof(eps_telemetry) + 1
#define file_transfer_BatteryTelemetry_size     sizeof(battery_telemetry) + 1
#define file_transfer_AntennaTelemetry_size     sizeof(antenna_telemetry) + 1
#define file_transfer_DosimeterData_size        sizeof(dosimeter_data) + 1
#define file_transfer_ImagePacket_size          sizeof(image_packet) + 1
#define file_transfer_ModuleErrorReport_size    sizeof(module_error_report) + 1
#define file_transfer_ComponentErrorReport_size sizeof(component_error_report) + 1
#define file_transfer_ErrorReportSummary_size   sizeof(error_report_summary) + 1
#define protocol_Ack_size                       sizeof(ack) + 1
#define protocol_Nack_size                      sizeof(nack) + 1
#define telecommand_BeginPass_size              sizeof(begin_pass) + 1
#define telecommand_BeginFileTransfer_size      sizeof(begin_file_transfer) + 1
#define telecommand_CeaseTransmission_size      sizeof(cease_transmission) + 1
#define telecommand_ResumeTransmission_size     sizeof(resume_transmission) + 1
#define telecommand_UpdateTime_size             sizeof(update_time) + 1
#define telecommand_Reset_size                  sizeof(reset) + 1
#define file_transfer_adcs_detection_size		sizeof(adcs_detection) + 1

/* Struct definitions */
typedef struct __attribute__ ((__packed__)) _radsat_message {
    radsat_tag_t which_service;
    union {
        protocol_message ProtocolMessage;
        file_transfer_message FileTransferMessage;
        telecommand_message TelecommandMessage;
    };
} radsat_message;

#define radsat_message_size sizeof(radsat_message)
/* Field tags (for use in manual encoding/decoding) */

#endif
