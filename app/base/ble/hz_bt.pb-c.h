/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: hz_bt.proto */

#ifndef PROTOBUF_C_hz_5fbt_2eproto__INCLUDED
#define PROTOBUF_C_hz_5fbt_2eproto__INCLUDED

#include <protobuf-c/protobuf-c.h>

PROTOBUF_C__BEGIN_DECLS

#if PROTOBUF_C_VERSION_NUMBER < 1003000
# error This file was generated by a newer version of protoc-c which is incompatible with your libprotobuf-c headers. Please update your headers.
#elif 1003002 < PROTOBUF_C_MIN_COMPILER_VERSION
# error This file was generated by an older version of protoc-c which is incompatible with your libprotobuf-c headers. Please regenerate this file with a newer version of protoc-c.
#endif


typedef struct _AuthenticationInfo AuthenticationInfo;
typedef struct _AuthenticationResponse AuthenticationResponse;
typedef struct _TimeStamp TimeStamp;
typedef struct _UserInfo UserInfo;
typedef struct _ACK ACK;
typedef struct _VehicleDoor VehicleDoor;
typedef struct _PanoramicSunroof PanoramicSunroof;
typedef struct _ElectricDoor ElectricDoor;
typedef struct _RemoteFineCar RemoteFineCar;
typedef struct _Charge Charge;
typedef struct _PowerControl PowerControl;
typedef struct _VehicleStatus VehicleStatus;
typedef struct _VehicleInfor VehicleInfor;
typedef struct _ApplicationHeader ApplicationHeader;
typedef struct _ProtocolHeader ProtocolHeader;


/* --- enums --- */

typedef enum _ApplicationHeader__MessageType {
  APPLICATION_HEADER__MESSAGE_TYPE__INIT_VALUE = 0,
  APPLICATION_HEADER__MESSAGE_TYPE__VEHICLE_SECURITY_FUNC = 1,
  APPLICATION_HEADER__MESSAGE_TYPE__SECURITY_FUNC_RESPONSE = 2,
  APPLICATION_HEADER__MESSAGE_TYPE__VEHICLE_DOOR = 3,
  APPLICATION_HEADER__MESSAGE_TYPE__PANORAMIC_SUNROOF = 4,
  APPLICATION_HEADER__MESSAGE_TYPE__ELECTRIC_DOOR = 5,
  APPLICATION_HEADER__MESSAGE_TYPE__REMOTE_FINE_CAR = 6,
  APPLICATION_HEADER__MESSAGE_TYPE__CHARGE = 7,
  APPLICATION_HEADER__MESSAGE_TYPE__POWER_CONTRO = 8,
  APPLICATION_HEADER__MESSAGE_TYPE__ACK = 9,
  APPLICATION_HEADER__MESSAGE_TYPE__Vehicle_Status = 10,
  APPLICATION_HEADER__MESSAGE_TYPE__Vehicle_Infor = 11
    PROTOBUF_C__FORCE_ENUM_TO_BE_INT_SIZE(APPLICATION_HEADER__MESSAGE_TYPE)
} ApplicationHeader__MessageType;

/* --- messages --- */

struct  _AuthenticationInfo
{
  ProtobufCMessage base;
  int32_t authentication_type;
  ProtobufCBinaryData authentication_data;
};
#define AUTHENTICATION_INFO__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&authentication_info__descriptor) \
    , 0, {0,NULL} }


struct  _AuthenticationResponse
{
  ProtobufCMessage base;
  protobuf_c_boolean auth_result;
  int32_t failure_reasons;
  ProtobufCBinaryData ver_data;
};
#define AUTHENTICATION_RESPONSE__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&authentication_response__descriptor) \
    , 0, 0, {0,NULL} }


struct  _TimeStamp
{
  ProtobufCMessage base;
  int32_t year;
  int32_t month;
  int32_t day;
  int32_t hour;
  int32_t minute;
  int32_t second;
};
#define TIME_STAMP__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&time_stamp__descriptor) \
    , 0, 0, 0, 0, 0, 0 }


struct  _UserInfo
{
  ProtobufCMessage base;
  char *bt_addr;
  char *bt_name;
  char *imsi;
};
#define USER_INFO__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&user_info__descriptor) \
    , (char *)protobuf_c_empty_string, (char *)protobuf_c_empty_string, (char *)protobuf_c_empty_string }


struct  _ACK
{
  ProtobufCMessage base;
  int32_t msg_type;
  int32_t ack_state;
  int32_t execution_result;
};
#define ACK__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&ack__descriptor) \
    , 0, 0, 0 }


struct  _VehicleDoor
{
  ProtobufCMessage base;
  int32_t door_state;
};
#define VEHICLE_DOOR__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&vehicle_door__descriptor) \
    , 0 }


struct  _PanoramicSunroof
{
  ProtobufCMessage base;
  int32_t panoramic_sunroof_state;
};
#define PANORAMIC_SUNROOF__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&panoramic_sunroof__descriptor) \
    , 0 }


struct  _ElectricDoor
{
  ProtobufCMessage base;
  int32_t electric_door_state;
};
#define ELECTRIC_DOOR__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&electric_door__descriptor) \
    , 0 }


struct  _RemoteFineCar
{
  ProtobufCMessage base;
  int32_t fine_car_state;
};
#define REMOTE_FINE_CAR__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&remote_fine_car__descriptor) \
    , 0 }


struct  _Charge
{
  ProtobufCMessage base;
  int32_t charge_state;
};
#define CHARGE__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&charge__descriptor) \
    , 0 }


struct  _PowerControl
{
  ProtobufCMessage base;
  int32_t power_state;
};
#define POWER_CONTROL__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&power_control__descriptor) \
    , 0 }


struct  _VehicleStatus
{
  ProtobufCMessage base;
  int32_t vehicle_state;
};
#define VEHICLE_STATUS__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&vehicle_status__descriptor) \
    , 0 }


struct  _VehicleInfor
{
  ProtobufCMessage base;
  int32_t vehiclie_door_state;
  int32_t sunroof_state;
  int32_t electric_door_state;
  int32_t fine_car_state;
  int32_t charge_state;
  int32_t power_state;
};
#define VEHICLE_INFOR__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&vehicle_infor__descriptor) \
    , 0, 0, 0, 0, 0, 0 }


struct  _ApplicationHeader
{
  ProtobufCMessage base;
  int32_t protocol_version;
  ApplicationHeader__MessageType msg_type;
};
#define APPLICATION_HEADER__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&application_header__descriptor) \
    , 0, APPLICATION_HEADER__MESSAGE_TYPE__INIT_VALUE }


struct  _ProtocolHeader
{
  ProtobufCMessage base;
  ApplicationHeader *head;
  TimeStamp *timestamp;
  int32_t msgcarrierlen;
  ProtobufCBinaryData msgcarrier;
};
#define PROTOCOL_HEADER__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&protocol_header__descriptor) \
    , NULL, NULL, 0, {0,NULL} }


/* AuthenticationInfo methods */
void   authentication_info__init
                     (AuthenticationInfo         *message);
size_t authentication_info__get_packed_size
                     (const AuthenticationInfo   *message);
size_t authentication_info__pack
                     (const AuthenticationInfo   *message,
                      uint8_t             *out);
size_t authentication_info__pack_to_buffer
                     (const AuthenticationInfo   *message,
                      ProtobufCBuffer     *buffer);
AuthenticationInfo *
       authentication_info__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   authentication_info__free_unpacked
                     (AuthenticationInfo *message,
                      ProtobufCAllocator *allocator);
/* AuthenticationResponse methods */
void   authentication_response__init
                     (AuthenticationResponse         *message);
size_t authentication_response__get_packed_size
                     (const AuthenticationResponse   *message);
size_t authentication_response__pack
                     (const AuthenticationResponse   *message,
                      uint8_t             *out);
size_t authentication_response__pack_to_buffer
                     (const AuthenticationResponse   *message,
                      ProtobufCBuffer     *buffer);
AuthenticationResponse *
       authentication_response__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   authentication_response__free_unpacked
                     (AuthenticationResponse *message,
                      ProtobufCAllocator *allocator);
/* TimeStamp methods */
void   time_stamp__init
                     (TimeStamp         *message);
size_t time_stamp__get_packed_size
                     (const TimeStamp   *message);
size_t time_stamp__pack
                     (const TimeStamp   *message,
                      uint8_t             *out);
size_t time_stamp__pack_to_buffer
                     (const TimeStamp   *message,
                      ProtobufCBuffer     *buffer);
TimeStamp *
       time_stamp__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   time_stamp__free_unpacked
                     (TimeStamp *message,
                      ProtobufCAllocator *allocator);
/* UserInfo methods */
void   user_info__init
                     (UserInfo         *message);
size_t user_info__get_packed_size
                     (const UserInfo   *message);
size_t user_info__pack
                     (const UserInfo   *message,
                      uint8_t             *out);
size_t user_info__pack_to_buffer
                     (const UserInfo   *message,
                      ProtobufCBuffer     *buffer);
UserInfo *
       user_info__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   user_info__free_unpacked
                     (UserInfo *message,
                      ProtobufCAllocator *allocator);
/* ACK methods */
void   ack__init
                     (ACK         *message);
size_t ack__get_packed_size
                     (const ACK   *message);
size_t ack__pack
                     (const ACK   *message,
                      uint8_t             *out);
size_t ack__pack_to_buffer
                     (const ACK   *message,
                      ProtobufCBuffer     *buffer);
ACK *
       ack__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   ack__free_unpacked
                     (ACK *message,
                      ProtobufCAllocator *allocator);
/* VehicleDoor methods */
void   vehicle_door__init
                     (VehicleDoor         *message);
size_t vehicle_door__get_packed_size
                     (const VehicleDoor   *message);
size_t vehicle_door__pack
                     (const VehicleDoor   *message,
                      uint8_t             *out);
size_t vehicle_door__pack_to_buffer
                     (const VehicleDoor   *message,
                      ProtobufCBuffer     *buffer);
VehicleDoor *
       vehicle_door__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   vehicle_door__free_unpacked
                     (VehicleDoor *message,
                      ProtobufCAllocator *allocator);
/* PanoramicSunroof methods */
void   panoramic_sunroof__init
                     (PanoramicSunroof         *message);
size_t panoramic_sunroof__get_packed_size
                     (const PanoramicSunroof   *message);
size_t panoramic_sunroof__pack
                     (const PanoramicSunroof   *message,
                      uint8_t             *out);
size_t panoramic_sunroof__pack_to_buffer
                     (const PanoramicSunroof   *message,
                      ProtobufCBuffer     *buffer);
PanoramicSunroof *
       panoramic_sunroof__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   panoramic_sunroof__free_unpacked
                     (PanoramicSunroof *message,
                      ProtobufCAllocator *allocator);
/* ElectricDoor methods */
void   electric_door__init
                     (ElectricDoor         *message);
size_t electric_door__get_packed_size
                     (const ElectricDoor   *message);
size_t electric_door__pack
                     (const ElectricDoor   *message,
                      uint8_t             *out);
size_t electric_door__pack_to_buffer
                     (const ElectricDoor   *message,
                      ProtobufCBuffer     *buffer);
ElectricDoor *
       electric_door__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   electric_door__free_unpacked
                     (ElectricDoor *message,
                      ProtobufCAllocator *allocator);
/* RemoteFineCar methods */
void   remote_fine_car__init
                     (RemoteFineCar         *message);
size_t remote_fine_car__get_packed_size
                     (const RemoteFineCar   *message);
size_t remote_fine_car__pack
                     (const RemoteFineCar   *message,
                      uint8_t             *out);
size_t remote_fine_car__pack_to_buffer
                     (const RemoteFineCar   *message,
                      ProtobufCBuffer     *buffer);
RemoteFineCar *
       remote_fine_car__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   remote_fine_car__free_unpacked
                     (RemoteFineCar *message,
                      ProtobufCAllocator *allocator);
/* Charge methods */
void   charge__init
                     (Charge         *message);
size_t charge__get_packed_size
                     (const Charge   *message);
size_t charge__pack
                     (const Charge   *message,
                      uint8_t             *out);
size_t charge__pack_to_buffer
                     (const Charge   *message,
                      ProtobufCBuffer     *buffer);
Charge *
       charge__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   charge__free_unpacked
                     (Charge *message,
                      ProtobufCAllocator *allocator);
/* PowerControl methods */
void   power_control__init
                     (PowerControl         *message);
size_t power_control__get_packed_size
                     (const PowerControl   *message);
size_t power_control__pack
                     (const PowerControl   *message,
                      uint8_t             *out);
size_t power_control__pack_to_buffer
                     (const PowerControl   *message,
                      ProtobufCBuffer     *buffer);
PowerControl *
       power_control__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   power_control__free_unpacked
                     (PowerControl *message,
                      ProtobufCAllocator *allocator);
/* VehicleStatus methods */
void   vehicle_status__init
                     (VehicleStatus         *message);
size_t vehicle_status__get_packed_size
                     (const VehicleStatus   *message);
size_t vehicle_status__pack
                     (const VehicleStatus   *message,
                      uint8_t             *out);
size_t vehicle_status__pack_to_buffer
                     (const VehicleStatus   *message,
                      ProtobufCBuffer     *buffer);
VehicleStatus *
       vehicle_status__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   vehicle_status__free_unpacked
                     (VehicleStatus *message,
                      ProtobufCAllocator *allocator);
/* VehicleInfor methods */
void   vehicle_infor__init
                     (VehicleInfor         *message);
size_t vehicle_infor__get_packed_size
                     (const VehicleInfor   *message);
size_t vehicle_infor__pack
                     (const VehicleInfor   *message,
                      uint8_t             *out);
size_t vehicle_infor__pack_to_buffer
                     (const VehicleInfor   *message,
                      ProtobufCBuffer     *buffer);
VehicleInfor *
       vehicle_infor__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   vehicle_infor__free_unpacked
                     (VehicleInfor *message,
                      ProtobufCAllocator *allocator);
/* ApplicationHeader methods */
void   application_header__init
                     (ApplicationHeader         *message);
size_t application_header__get_packed_size
                     (const ApplicationHeader   *message);
size_t application_header__pack
                     (const ApplicationHeader   *message,
                      uint8_t             *out);
size_t application_header__pack_to_buffer
                     (const ApplicationHeader   *message,
                      ProtobufCBuffer     *buffer);
ApplicationHeader *
       application_header__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   application_header__free_unpacked
                     (ApplicationHeader *message,
                      ProtobufCAllocator *allocator);
/* ProtocolHeader methods */
void   protocol_header__init
                     (ProtocolHeader         *message);
size_t protocol_header__get_packed_size
                     (const ProtocolHeader   *message);
size_t protocol_header__pack
                     (const ProtocolHeader   *message,
                      uint8_t             *out);
size_t protocol_header__pack_to_buffer
                     (const ProtocolHeader   *message,
                      ProtobufCBuffer     *buffer);
ProtocolHeader *
       protocol_header__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   protocol_header__free_unpacked
                     (ProtocolHeader *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*AuthenticationInfo_Closure)
                 (const AuthenticationInfo *message,
                  void *closure_data);
typedef void (*AuthenticationResponse_Closure)
                 (const AuthenticationResponse *message,
                  void *closure_data);
typedef void (*TimeStamp_Closure)
                 (const TimeStamp *message,
                  void *closure_data);
typedef void (*UserInfo_Closure)
                 (const UserInfo *message,
                  void *closure_data);
typedef void (*ACK_Closure)
                 (const ACK *message,
                  void *closure_data);
typedef void (*VehicleDoor_Closure)
                 (const VehicleDoor *message,
                  void *closure_data);
typedef void (*PanoramicSunroof_Closure)
                 (const PanoramicSunroof *message,
                  void *closure_data);
typedef void (*ElectricDoor_Closure)
                 (const ElectricDoor *message,
                  void *closure_data);
typedef void (*RemoteFineCar_Closure)
                 (const RemoteFineCar *message,
                  void *closure_data);
typedef void (*Charge_Closure)
                 (const Charge *message,
                  void *closure_data);
typedef void (*PowerControl_Closure)
                 (const PowerControl *message,
                  void *closure_data);
typedef void (*VehicleStatus_Closure)
                 (const VehicleStatus *message,
                  void *closure_data);
typedef void (*VehicleInfor_Closure)
                 (const VehicleInfor *message,
                  void *closure_data);
typedef void (*ApplicationHeader_Closure)
                 (const ApplicationHeader *message,
                  void *closure_data);
typedef void (*ProtocolHeader_Closure)
                 (const ProtocolHeader *message,
                  void *closure_data);

/* --- services --- */


/* --- descriptors --- */

extern const ProtobufCMessageDescriptor authentication_info__descriptor;
extern const ProtobufCMessageDescriptor authentication_response__descriptor;
extern const ProtobufCMessageDescriptor time_stamp__descriptor;
extern const ProtobufCMessageDescriptor user_info__descriptor;
extern const ProtobufCMessageDescriptor ack__descriptor;
extern const ProtobufCMessageDescriptor vehicle_door__descriptor;
extern const ProtobufCMessageDescriptor panoramic_sunroof__descriptor;
extern const ProtobufCMessageDescriptor electric_door__descriptor;
extern const ProtobufCMessageDescriptor remote_fine_car__descriptor;
extern const ProtobufCMessageDescriptor charge__descriptor;
extern const ProtobufCMessageDescriptor power_control__descriptor;
extern const ProtobufCMessageDescriptor vehicle_status__descriptor;
extern const ProtobufCMessageDescriptor vehicle_infor__descriptor;
extern const ProtobufCMessageDescriptor application_header__descriptor;
extern const ProtobufCEnumDescriptor    application_header__message_type__descriptor;
extern const ProtobufCMessageDescriptor protocol_header__descriptor;

PROTOBUF_C__END_DECLS


#endif  /* PROTOBUF_C_hz_5fbt_2eproto__INCLUDED */
