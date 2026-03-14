#include "../../include/canfly.h"
#include <memory.h>

#ifdef _DEBUG
char trace_buffer[4096];
#endif

result_t create_can_msg_nodata(canmsg_t* msg, uint16_t message_id)
  {
  memset(msg, 0, sizeof(canmsg_t));
  set_can_len(msg, 1);
  set_can_id(msg, message_id);
  msg->data[0] = CANFLY_NODATA;

  return s_ok;
  }

result_t create_can_msg_error(canmsg_t* msg, uint16_t message_id, uint32_t data)
  {
  memset(msg, 0, sizeof(canmsg_t));
  set_can_len(msg, 5);
  set_can_id(msg, message_id);
  msg->data[0] = CANFLY_ERROR;
  msg->data[1] = (uint8_t)(data >> 24);
  msg->data[2] = (uint8_t)(data >> 16);
  msg->data[3] = (uint8_t)(data >> 8);
  msg->data[4] = (uint8_t)data;

  return s_ok;
  }

result_t create_can_msg_float(canmsg_t* msg, uint16_t message_id, float value)
  {
  memset(msg, 0, sizeof(canmsg_t));
  set_can_len(msg, 5);
  set_can_id(msg, message_id);

  uint32_t ulvalue = *((uint32_t*)&value);
  msg->data[0] = CANFLY_FLOAT;
  msg->data[1] = (uint8_t)(ulvalue >> 24);
  msg->data[2] = (uint8_t)(ulvalue >> 16);
  msg->data[3] = (uint8_t)(ulvalue >> 8);
  msg->data[4] = (uint8_t)ulvalue;

  return s_ok;
  }

result_t create_can_msg_int16(canmsg_t* msg, uint16_t message_id, int16_t data)
  {
  memset(msg, 0, sizeof(canmsg_t));
  set_can_len(msg, 3);
  set_can_id(msg, message_id);
  msg->data[0] = CANFLY_INT16;
  msg->data[1] = (uint8_t)(data >> 8);
  msg->data[2] = (uint8_t)data;

  return s_ok;
  }

result_t create_can_msg_int32(canmsg_t* msg, uint16_t message_id, int32_t data)
  {
  memset(msg, 0, sizeof(canmsg_t));
  set_can_len(msg, 5);
  set_can_id(msg, message_id);
  msg->data[0] = CANFLY_INT32;
  msg->data[1] = (uint8_t)(data >> 24);
  msg->data[2] = (uint8_t)(data >> 16);
  msg->data[3] = (uint8_t)(data >> 8);
  msg->data[4] = (uint8_t)data;

  return s_ok;
  }

result_t create_can_msg_uint16(canmsg_t* msg, uint16_t message_id, uint16_t data)
  {
  memset(msg, 0, sizeof(canmsg_t));
  set_can_len(msg, 3);
  set_can_id(msg, message_id);
  msg->data[0] = CANFLY_UINT16;
  msg->data[1] = (uint8_t)(data >> 8);
  msg->data[2] = (uint8_t)data;

  return s_ok;
  }

result_t create_can_msg_uint32(canmsg_t* msg, uint16_t message_id, uint32_t data)
  {
  memset(msg, 0, sizeof(canmsg_t));
  set_can_len(msg, 5);
  set_can_id(msg, message_id);
  msg->data[0] = CANFLY_UINT32;
  msg->data[1] = (uint8_t)(data >> 24);
  msg->data[2] = (uint8_t)(data >> 16);
  msg->data[3] = (uint8_t)(data >> 8);
  msg->data[4] = (uint8_t)data;

  return s_ok;
  }

result_t create_can_msg_bool(canmsg_t* msg, uint16_t message_id, bool value)
  {
  memset(msg, 0, sizeof(canmsg_t));
  set_can_len(msg, 1);
  set_can_id(msg, message_id);
  msg->data[0] = value ? CANFLY_BOOL_TRUE : CANFLY_BOOL_FALSE;

  return s_ok;
  }

result_t create_can_msg_int8(canmsg_t* msg, uint16_t message_id, int8_t value)
  {
  memset(msg, 0, sizeof(canmsg_t));
  set_can_len(msg, 2);
  set_can_id(msg, message_id);
  msg->data[0] = CANFLY_INT8;
  msg->data[1] = (uint8_t)value;

  return s_ok;
  }

result_t create_can_msg_uint8(canmsg_t* msg, uint16_t message_id, uint8_t value)
  {
  memset(msg, 0, sizeof(canmsg_t));
  set_can_len(msg, 2);
  set_can_id(msg, message_id);

  msg->data[0] = CANFLY_UINT8;
  msg->data[1] = value;

  return s_ok;
  }

result_t create_can_msg_utc(canmsg_t* msg, uint16_t message_id, const tm_t* value)
  {
  set_can_len(msg, 8);
  set_can_id(msg, message_id);

  msg->data[0] = CANFLY_TM;
  msg->data[1] = (uint8_t)(value->year >> 8);
  msg->data[2] = (uint8_t)value->year;
  msg->data[3] = (uint8_t)value->month;
  msg->data[4] = (uint8_t)value->day;
  msg->data[5] = (uint8_t)value->hour;
  msg->data[6] = (uint8_t)value->minute;
  msg->data[7] = (uint8_t)value->second;

  return s_ok;
  }

result_t get_param_float(const canmsg_t* msg, float* v)
  {
  if (msg == 0 || v == 0)
    return e_bad_parameter;

  variant_t var;

  result_t result;
  if (failed(result = msg_to_variant(msg, &var)))
    return result;

  return coerce_to_float(&var, v);
  }

result_t get_param_bool(const canmsg_t* msg, bool* v)
  {
  if (msg == 0 || v == 0)
    return e_bad_parameter;

  variant_t var;

  result_t result;
  if (failed(result = msg_to_variant(msg, &var)))
    return result;

  return coerce_to_bool(&var, v);
  }

result_t get_param_int8(const canmsg_t* msg, int8_t* v)
  {
  if (msg == 0 || v == 0)
    return e_bad_parameter;

  variant_t var;

  result_t result;
  if (failed(result = msg_to_variant(msg, &var)))
    return result;

  return coerce_to_int8(&var, v);
  }

result_t get_param_uint8(const canmsg_t* msg, uint8_t* v)
  {
  if (msg == 0 || v == 0)
    return e_bad_parameter;

  variant_t var;

  result_t result;
  if (failed(result = msg_to_variant(msg, &var)))
    return result;

  return coerce_to_uint8(&var, v);
  }

result_t get_param_int16(const canmsg_t* msg, int16_t* v)
  {
  if (msg == 0 || v == 0)
    return e_bad_parameter;

  variant_t var;

  result_t result;
  if (failed(result = msg_to_variant(msg, &var)))
    return result;

  return coerce_to_int16(&var, v);
  }

result_t get_param_uint16(const canmsg_t* msg, uint16_t* v)
  {
  if (msg == 0 || v == 0)
    return e_bad_parameter;

  variant_t var;

  result_t result;
  if (failed(result = msg_to_variant(msg, &var)))
    return result;

  return coerce_to_uint16(&var, v);
  }

result_t get_param_int32(const canmsg_t* msg, int32_t* v)
  {
  if (msg == 0 || v == 0)
    return e_bad_parameter;

  variant_t var;

  result_t result;
  if (failed(result = msg_to_variant(msg, &var)))
    return result;

  return coerce_to_int32(&var, v);
  }

result_t get_param_uint32(const canmsg_t* msg, uint32_t* v)
  {
  if (msg == 0 || v == 0)
    return e_bad_parameter;

  variant_t var;

  result_t result;
  if (failed(result = msg_to_variant(msg, &var)))
    return result;

  return coerce_to_uint32(&var, v);
  }

result_t get_param_utc(const canmsg_t* msg, tm_t* value)
  {
  if (msg == 0 || value == 0 || get_can_len(msg) != 8)
    return e_bad_parameter;

  value->year = msg->data[1] * 256;
  value->year += msg->data[2];
  value->month = msg->data[3];
  value->day = msg->data[4];
  value->hour = msg->data[5];
  value->minute = msg->data[6];
  value->second = msg->data[7];

  return s_ok;
  }
