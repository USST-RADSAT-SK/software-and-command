/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.4 */

#ifndef PB_RRADSAT_PB_H_INCLUDED
#define PB_RRADSAT_PB_H_INCLUDED
#include <pb.h>
#include <RProtocol.pb.h>
#include <RTelecommands.pb.h>
#include <RFileTransfer.pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
typedef struct _radsat_message {
    pb_size_t which_service;
    union {
        protocol_message ProtocolMessage;
        file_transfer_message FileTransferMessage;
        telecommand_message TelecommandMessage;
    };
} radsat_message;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define radsat_message_init_default              {0, {protocol_message_init_default}}
#define radsat_message_init_zero                 {0, {protocol_message_init_zero}}

/* Field tags (for use in manual encoding/decoding) */
#define radsat_message_ProtocolMessage_tag       1
#define radsat_message_FileTransferMessage_tag   2
#define radsat_message_TelecommandMessage_tag    3

/* Struct field encoding specification for nanopb */
#define radsat_message_FIELDLIST(X, a) \
X(a, STATIC,   ONEOF,    MESSAGE,  (service,ProtocolMessage,ProtocolMessage),   1) \
X(a, STATIC,   ONEOF,    MESSAGE,  (service,FileTransferMessage,FileTransferMessage),   2) \
X(a, STATIC,   ONEOF,    MESSAGE,  (service,TelecommandMessage,TelecommandMessage),   3)
#define radsat_message_CALLBACK NULL
#define radsat_message_DEFAULT NULL
#define radsat_message_service_ProtocolMessage_MSGTYPE protocol_message
#define radsat_message_service_FileTransferMessage_MSGTYPE file_transfer_message
#define radsat_message_service_TelecommandMessage_MSGTYPE telecommand_message

extern const pb_msgdesc_t radsat_message_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define radsat_message_fields &radsat_message_msg

/* Maximum encoded size of messages (where known) */
#define radsat_message_size                      217

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
