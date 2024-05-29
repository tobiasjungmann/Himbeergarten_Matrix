/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.7 */

#ifndef PB_SMART_HOME_HUMIDITYSTORAGE_PB_H_INCLUDED
#define PB_SMART_HOME_HUMIDITYSTORAGE_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
typedef struct _smart_home_StoreHumidityRequest {
    char deviceMAC[24];
    int32_t sensorId;
    int32_t humidity;
    int32_t humidityInPercent;
} smart_home_StoreHumidityRequest;

typedef struct _smart_home_StoreHumidityReply {
    char dummy_field;
} smart_home_StoreHumidityReply;

typedef struct _smart_home_GetActiveSensorsRequest {
    char deviceMAC[24];
    pb_callback_t availableSensors;
    uint32_t sensorCount;
} smart_home_GetActiveSensorsRequest;

typedef struct _smart_home_GetActiveSensorsReply {
    pb_callback_t sensors;
    uint32_t sensorCount;
} smart_home_GetActiveSensorsReply;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define smart_home_StoreHumidityRequest_init_default {"", 0, 0, 0}
#define smart_home_StoreHumidityReply_init_default {0}
#define smart_home_GetActiveSensorsRequest_init_default {"", {{NULL}, NULL}, 0}
#define smart_home_GetActiveSensorsReply_init_default {{{NULL}, NULL}, 0}
#define smart_home_StoreHumidityRequest_init_zero {"", 0, 0, 0}
#define smart_home_StoreHumidityReply_init_zero  {0}
#define smart_home_GetActiveSensorsRequest_init_zero {"", {{NULL}, NULL}, 0}
#define smart_home_GetActiveSensorsReply_init_zero {{{NULL}, NULL}, 0}

/* Field tags (for use in manual encoding/decoding) */
#define smart_home_StoreHumidityRequest_deviceMAC_tag 1
#define smart_home_StoreHumidityRequest_sensorId_tag 2
#define smart_home_StoreHumidityRequest_humidity_tag 3
#define smart_home_StoreHumidityRequest_humidityInPercent_tag 4
#define smart_home_GetActiveSensorsRequest_deviceMAC_tag 1
#define smart_home_GetActiveSensorsRequest_availableSensors_tag 2
#define smart_home_GetActiveSensorsRequest_sensorCount_tag 3
#define smart_home_GetActiveSensorsReply_sensors_tag 1
#define smart_home_GetActiveSensorsReply_sensorCount_tag 2

/* Struct field encoding specification for nanopb */
#define smart_home_StoreHumidityRequest_FIELDLIST(X, a) \
X(a, STATIC,   REQUIRED, STRING,   deviceMAC,         1) \
X(a, STATIC,   REQUIRED, INT32,    sensorId,          2) \
X(a, STATIC,   REQUIRED, INT32,    humidity,          3) \
X(a, STATIC,   REQUIRED, INT32,    humidityInPercent,   4)
#define smart_home_StoreHumidityRequest_CALLBACK NULL
#define smart_home_StoreHumidityRequest_DEFAULT NULL

#define smart_home_StoreHumidityReply_FIELDLIST(X, a) \

#define smart_home_StoreHumidityReply_CALLBACK NULL
#define smart_home_StoreHumidityReply_DEFAULT NULL

#define smart_home_GetActiveSensorsRequest_FIELDLIST(X, a) \
X(a, STATIC,   REQUIRED, STRING,   deviceMAC,         1) \
X(a, CALLBACK, REPEATED, UINT32,   availableSensors,   2) \
X(a, STATIC,   REQUIRED, UINT32,   sensorCount,       3)
#define smart_home_GetActiveSensorsRequest_CALLBACK pb_default_field_callback
#define smart_home_GetActiveSensorsRequest_DEFAULT NULL

#define smart_home_GetActiveSensorsReply_FIELDLIST(X, a) \
X(a, CALLBACK, REPEATED, UINT32,   sensors,           1) \
X(a, STATIC,   REQUIRED, UINT32,   sensorCount,       2)
#define smart_home_GetActiveSensorsReply_CALLBACK pb_default_field_callback
#define smart_home_GetActiveSensorsReply_DEFAULT NULL

extern const pb_msgdesc_t smart_home_StoreHumidityRequest_msg;
extern const pb_msgdesc_t smart_home_StoreHumidityReply_msg;
extern const pb_msgdesc_t smart_home_GetActiveSensorsRequest_msg;
extern const pb_msgdesc_t smart_home_GetActiveSensorsReply_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define smart_home_StoreHumidityRequest_fields &smart_home_StoreHumidityRequest_msg
#define smart_home_StoreHumidityReply_fields &smart_home_StoreHumidityReply_msg
#define smart_home_GetActiveSensorsRequest_fields &smart_home_GetActiveSensorsRequest_msg
#define smart_home_GetActiveSensorsReply_fields &smart_home_GetActiveSensorsReply_msg

/* Maximum encoded size of messages (where known) */
/* smart_home_GetActiveSensorsRequest_size depends on runtime parameters */
/* smart_home_GetActiveSensorsReply_size depends on runtime parameters */
#define smart_home_StoreHumidityReply_size       0
#define smart_home_StoreHumidityRequest_size     58

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif