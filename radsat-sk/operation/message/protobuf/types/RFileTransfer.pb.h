/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.4 */

#ifndef PB_RFILETRANSFER_PB_H_INCLUDED
#define PB_RFILETRANSFER_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
typedef PB_BYTES_ARRAY_T(200) FileTransferPacket_data_t;
typedef struct _FileTransferPacket {
    int32_t packetType;
    int32_t packetNumber;
    FileTransferPacket_data_t data;
} FileTransferPacket;

typedef struct _FileTransferResponse {
    int32_t packetType;
    int32_t packetNumber;
    int32_t response;
} FileTransferResponse;

typedef struct _FileTransferMessage {
    pb_size_t which_message;
    union {
        FileTransferResponse fileTransferResponse;
        FileTransferPacket fileTransferPacket;
    } message;
} FileTransferMessage;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define FileTransferMessage_init_default         {0, {FileTransferResponse_init_default}}
#define FileTransferResponse_init_default        {0, 0, 0}
#define FileTransferPacket_init_default          {0, 0, {0, {0}}}
#define FileTransferMessage_init_zero            {0, {FileTransferResponse_init_zero}}
#define FileTransferResponse_init_zero           {0, 0, 0}
#define FileTransferPacket_init_zero             {0, 0, {0, {0}}}

/* Field tags (for use in manual encoding/decoding) */
#define FileTransferPacket_packetType_tag        1
#define FileTransferPacket_packetNumber_tag      2
#define FileTransferPacket_data_tag              3
#define FileTransferResponse_packetType_tag      1
#define FileTransferResponse_packetNumber_tag    2
#define FileTransferResponse_response_tag        3
#define FileTransferMessage_fileTransferResponse_tag 1
#define FileTransferMessage_fileTransferPacket_tag 2

/* Struct field encoding specification for nanopb */
#define FileTransferMessage_FIELDLIST(X, a) \
X(a, STATIC,   ONEOF,    MESSAGE,  (message,fileTransferResponse,message.fileTransferResponse),   1) \
X(a, STATIC,   ONEOF,    MESSAGE,  (message,fileTransferPacket,message.fileTransferPacket),   2)
#define FileTransferMessage_CALLBACK NULL
#define FileTransferMessage_DEFAULT NULL
#define FileTransferMessage_message_fileTransferResponse_MSGTYPE FileTransferResponse
#define FileTransferMessage_message_fileTransferPacket_MSGTYPE FileTransferPacket

#define FileTransferResponse_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, INT32,    packetType,        1) \
X(a, STATIC,   SINGULAR, INT32,    packetNumber,      2) \
X(a, STATIC,   SINGULAR, INT32,    response,          3)
#define FileTransferResponse_CALLBACK NULL
#define FileTransferResponse_DEFAULT NULL

#define FileTransferPacket_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, INT32,    packetType,        1) \
X(a, STATIC,   SINGULAR, INT32,    packetNumber,      2) \
X(a, STATIC,   SINGULAR, BYTES,    data,              3)
#define FileTransferPacket_CALLBACK NULL
#define FileTransferPacket_DEFAULT NULL

extern const pb_msgdesc_t FileTransferMessage_msg;
extern const pb_msgdesc_t FileTransferResponse_msg;
extern const pb_msgdesc_t FileTransferPacket_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define FileTransferMessage_fields &FileTransferMessage_msg
#define FileTransferResponse_fields &FileTransferResponse_msg
#define FileTransferPacket_fields &FileTransferPacket_msg

/* Maximum encoded size of messages (where known) */
#define FileTransferMessage_size                 228
#define FileTransferResponse_size                33
#define FileTransferPacket_size                  225

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
