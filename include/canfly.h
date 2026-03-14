#ifndef __canfly_h__
#define __canfly_h__

#include <stdio.h>

#include "sys_canfly.h"
#include "canfly_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// these routines are impmented locally to support the in-process handling
// of messages.
/**
   * @brief Create a can message sending an boolean value
   * @param msg           Message to construct
   * @param message_id    11 bit CanFly ID
   * @param data          Data to send
   *@return s_ok if created ok
   */
  extern result_t create_can_msg_bool(canmsg_t *msg, uint16_t message_id, bool value);

  /**
   * @fn const canmsg_t *create_can_msg_int8(canmsg_t *msg, uint16_t message_id, int8_t value)
   * @brief Create a can message sending an 8bit integer
   * @param msg           Message to construct
   * @param message_id    11 bit CanFly ID
   * @param data          Data to send
   *@return s_ok if created ok
   */
  extern result_t create_can_msg_int8(canmsg_t *msg, uint16_t message_id, int8_t value);
  /**
   * @fn const canmsg_t *create_can_msg_uint8(canmsg_t *msg, uint16_t message_id, uint8_t value)
   * @brief Create a can message sending an 8bit unsigned integer
   * @param msg           Message to construct
   * @param message_id    11 bit CanFly ID
   * @param value          Data to send
   *@return s_ok if created ok
   */
  extern result_t create_can_msg_uint8(canmsg_t *msg, uint16_t message_id, uint8_t value);
  /**
   * @fn const canmsg_t *create_can_msg_int16(canmsg_t *msg, uint16_t message_id, int16_t data)
   * Create a can message sending a 16bit integer
   * @param msg           Message to construct
   * @param message_id    11 bit CanFly ID
   * @param data          Data to send
   *@return s_ok if created ok
   */
  extern result_t create_can_msg_int16(canmsg_t *msg, uint16_t message_id, int16_t data);
  /**
   * @fn const canmsg_t *create_can_msg_uint16(canmsg_t *msg, uint16_t message_id, uint16_t data)
   * Create a can message sending a 16bit unsigned integer
   * @param msg           Message to construct
   * @param message_id    11 bit CanFly ID
   * @param data          Data to send
   *@return s_ok if created ok
   */
  extern result_t create_can_msg_uint16(canmsg_t *msg, uint16_t message_id, uint16_t data);
  /**
   * @fn create_can_msg_int32(canmsg_t *msg, uint16_t message_id, int32_t data)
   * Create a can message sending a 32bit integer
   * @param msg           Message to construct
   * @param message_id    11 bit CanFly ID
   * @param data          Data to send
   *@return s_ok if created ok
   */
  extern result_t create_can_msg_int32(canmsg_t *msg, uint16_t message_id, int32_t data);
  /**
   * @fn create_can_msg_uint32(canmsg_t *msg, uint16_t message_id, uint32_t data)
   * Create a can message sending a 32bit unsigned integer
   * @param msg           Message to construct
   * @param message_id    11 bit CanFly ID
   * @param data          Data to send
   *@return s_ok if created ok
   */
  extern result_t create_can_msg_uint32(canmsg_t *msg, uint16_t message_id, uint32_t data);
  /**
   * @fn create_can_msg_float(canmsg_t *msg, uint16_t message_id, float data)
   * Create a can message sending a floating point value
   * @param msg           Message to construct
   * @param message_id    11 bit CanFly ID
   * @param data          Data to send
   *@return s_ok if created ok
   */
  extern result_t create_can_msg_float(canmsg_t *msg, uint16_t message_id, float data);
  /**
   * @fn create_can_msg_utc(canmsg_t *msg, uint16_t message_id, const tm_t *value)
   * @brief Create a can message sending a utc time
   * @param msg           Message to construct
   * @param message_id    11 bit CanFly ID
   * @param data          Data to send
   *@return s_ok if created ok
   */
  extern result_t create_can_msg_utc(canmsg_t *msg, uint16_t message_id, const tm_t *value);
  /**
   * @brief create an empty message
   * @param msg Message to constuct
   * @param message_id 11 bit CanFly ID
   * @return s_ok if created ok
   */
  extern result_t create_can_msg_nodata(canmsg_t *msg, uint16_t message_id);
  /**
   * @brief Create a can message with an error code
   * @param msg           Message to construct
   * @param message_id    11 bit CanFly ID
   * @param error        Error to send
   * @return  s_ok if created ok
   */
  extern result_t create_can_msg_error(canmsg_t *msg, uint16_t message_id, uint32_t error);
  /**
   * @breif create a short string can msg
   * @param msg           Message to construct
   * @param message_id    11 bit CanFly ID
   * @param str  Message to send
   * @param len  length of message, max 7 chars or 0 to have code size it.
   * @return s_ok if created ok
   */
  extern result_t create_can_msg_chars(canmsg_t *msg, uint16_t message_id, const char *str, uint32_t len);
  /**
   * @fn get_param_int8(const canmsg_t *msg, int8_t *value)
   * @param msg         Message to extract parameter from
   * @param value       value of the message
   * @return s_ok if the message can be coerced to an int8, and the data is returned
   */
  extern result_t get_param_bool(const canmsg_t *msg, bool *value);
  /**
   * @fn get_param_int8(const canmsg_t *msg, int8_t *value)
   * @param msg         Message to extract parameter from
   * @param value       value of the message
   * @return s_ok if the message can be coerced to an int8, and the data is returned
   */
  extern result_t get_param_int8(const canmsg_t *msg, int8_t *value);
  /**
   * @fn get_param_uint8(const canmsg_t *msg, uint8_t *value)
   * @param msg         Message to extract parameter from
   * @param value       extracted value
   * @return s_ok if the message can be coerced to an uint8, and the data is returned
   */
  extern result_t get_param_uint8(const canmsg_t *msg, uint8_t *value);
  /**
   * @fn get_param_int16(const canmsg_t *msg, int16_t *value)
   * @param msg         Message to extract parameter from
   * @param value       extracted value
   * @return s_ok if the message can be coerced to an int16, and the data is returned
   */
  extern result_t get_param_int16(const canmsg_t *msg, int16_t *value);
  /**
   * @fn get_param_uint16(const canmsg_t *msg, uint16_t *value)
   * @param msg         Message to extract parameter from
   * @param index       Index of the value (0..1)
   * @param value       extracted value
   * @return s_ok if the message can be coerced to an int, and the data is returned
   */
  extern result_t get_param_uint16(const canmsg_t *msg, uint16_t *value);
  /**
   * @fn get_param_int32(const canmsg_t *msg, int32_t *value)
   * @param msg         Message to extract parameter from
   * @param value       extracted value
   * @return s_ok if the message can be coerced to an int32, and the data is returned
   */
  extern result_t get_param_int32(const canmsg_t *msg, int32_t *value);
  /**
   * @fn get_param_uint32(const canmsg_t *msg, uint32_t *value)
   * @param msg         Message to extract parameter from
   * @param value       extracted value
   * @return s_ok if the message can be coerced to an uint32, and the data is returned
   */
  extern result_t get_param_uint32(const canmsg_t *msg, uint32_t *value);
  /**
   * @brief Unpack the UTC message into a time structure
   * @param msg   message to unpack
   * @param value time values, note milliseconds is set to 0
   * @return s_ok if the message was a valid utc message
   */
  extern result_t get_param_utc(const canmsg_t *msg, tm_t *value);
  /**
   * @fn get_param_float(const canmsg_t *msg, float *value)
   * @param msg         Message to extract parameter from
   * @param value       extracted value
   * @return s_ok if the message can be coerced to a float, and the data is returned
   */
  extern result_t get_param_float(const canmsg_t *msg, float *value);

extern const variant_t *create_variant_nodata(variant_t *v);
extern const variant_t *create_variant_bool(bool value, variant_t *v);
extern const variant_t *create_variant_int8(int8_t value, variant_t *v);
extern const variant_t *create_variant_uint8(uint8_t value, variant_t *v);
extern const variant_t *create_variant_int16(int16_t value, variant_t *v);
extern const variant_t *create_variant_uint16(uint16_t value, variant_t *v);
extern const variant_t *create_variant_int32(int32_t value, variant_t *v);
extern const variant_t *create_variant_uint32(uint32_t value, variant_t *v);
extern const variant_t *create_variant_float(float value, variant_t *v);
extern const variant_t *create_variant_utc(const tm_t *value, variant_t *v);
extern result_t msg_to_variant(const canmsg_t *msg, variant_t *v);
extern result_t variant_to_msg(const variant_t *v, uint16_t id, uint16_t type, canmsg_t *msg);
extern result_t coerce_to_bool(const variant_t *src, bool *value);
extern result_t coerce_to_int8(const variant_t *src, int8_t *value);
extern result_t coerce_to_uint8(const variant_t *src, uint8_t *value);
extern result_t coerce_to_int16(const variant_t *src, int16_t *value);
extern result_t coerce_to_uint16(const variant_t *src, uint16_t *value);
extern result_t coerce_to_int32(const variant_t *src, int32_t *value);
extern result_t coerce_to_uint32(const variant_t *src, uint32_t *value);
extern result_t coerce_to_float(const variant_t *src, float *value);
extern result_t coerce_to_utc(const variant_t *src, tm_t *value);
extern result_t coerce_variant(const variant_t *src, variant_t *dst, variant_type to_type);
extern const variant_t *copy_variant(const variant_t *src, variant_t *dst);
extern int compare_variant(const variant_t *v1, const variant_t *v2);
extern uint16_t can_type_from_variant(variant_type vt);


#ifdef trace
#undef trace
#endif

#ifdef _DEBUG
#include <stdarg.h>
extern char trace_buffer[4096];
// trace writes messages to a stream in the registry that is limited to 4kbytes.
// used for startup logging
static inline void trace(uint16_t level, const char *msg, ...)
{
  va_list list;
  va_start(list, msg);

  (void)level;
  vsnprintf(trace_buffer, sizeof(trace_buffer), msg, list);
  trace_message(level, trace_buffer);
}
#ifdef platform_trace
#undef platform_trace
#endif
#else
#define trace(...)
#endif

#ifndef TRACE_LEVEL
#define TRACE_LEVEL 7
#endif

#define trace_emergency(...)  trace(0, __VA_ARGS__)

#if TRACE_LEVEL > 0
#define trace_alert(...)      trace(1, __VA_ARGS__)
#else
#define trace_alert(...)
#endif

#if TRACE_LEVEL > 1
#define trace_critical(...)   trace(2, __VA_ARGS__)
#else
#define trace_critical(...)
#endif

#if TRACE_LEVEL > 2
#define trace_error(...)      trace(3, __VA_ARGS__)
#else
#define trace_error(...)
#endif

#if TRACE_LEVEL > 3
#define trace_warning(...)    trace(4, __VA_ARGS__)
#else
#define trace_warning(...)
#endif

#if TRACE_LEVEL > 4
#define trace_notice(...)     trace(5, __VA_ARGS__)
#else
#define trace_notice(...)
#endif

#if TRACE_LEVEL > 5
#define trace_info(...)       trace(6, __VA_ARGS__)
#else
#define trace_info(...)
#endif

#if TRACE_LEVEL > 6
#define trace_debug(...)      trace(7, __VA_ARGS__)
#else
#define trace_debug(...)
#endif


#ifdef __cplusplus
}
#endif

#endif
