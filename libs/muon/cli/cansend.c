#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "neutron_cli.h"


static uint32_t get_uint32(string_t str)
  {
  uint32_t result = 0;

  if(string_length(str) > 0)
    result = strtoul(str, 0, 10);

  return result;
  }

static int32_t get_int32(string_t str)
  {
  int32_t result = 0;

  if(string_length(str) > 0)
    result = strtol(str, 0, 10);

  return result;
  }

static float get_float(string_t str)
  {
  float result = 0;

  if(string_length(str)> 0)
    result = strtof(str, 0);

  return result;
  }

static inline uint8_t extract_uint(uint32_t val, uint16_t byte)
  {
  return (uint8_t) (val >> (byte << 3));
  }

static inline uint8_t extract_int(int32_t val, uint16_t byte)
  {
  return extract_uint(*((uint32_t *)&val), byte);
  }

static inline uint8_t extract_float(float val, uint16_t byte)
  {
  return extract_uint(*((uint32_t *)&val), byte);
  }

result_t create_can_msg(canmsg_t *msg, uint16_t can_id, uint16_t type, uint16_t session, string_t val1, string_t val2, string_t val3, string_t val4)
  {
  int32_t int_val;
  uint32_t uint_val;
  float float_val;

  if (msg == 0 ||
    can_id == 0 ||
    type == 0 ||
    session == 0)
    return e_bad_parameter;

  memset(msg, 0, sizeof(canmsg_t));
  msg->id = can_id;
  msg->length = 4;

  msg->canas.service_code = (uint8_t)session;

  // char,char2,char3,char4,uchar,uchar2,uchar3,uchar4,short,short2,ushort,ushort2,long,ulong,float,nodata
  switch (type)
    {
    case 0:    // char
      if (val4 != 0 ||
        val3 != 0 ||
        val2 != 0 ||
        val1 == 0)
        return e_bad_parameter;

      msg->canas.data_type = CANAS_DATATYPE_UCHAR;
      *(char *)(&msg->canas.data[0]) = (char)get_int32(val1);
      msg->length = 5;
      break;
    case 1:    // char2
      if (val4 != 0 ||
        val3 != 0 ||
        val2 == 0 ||
        val1 == 0)
        return e_bad_parameter;

      msg->canas.data_type = CANAS_DATATYPE_CHAR2;
      *(char *)(&msg->canas.data[0]) = (char)get_int32(val1);
      *(char *)(&msg->canas.data[1]) = (char)get_int32(val2);
      msg->length = 6;
      break;
    case 2:    // char3
      if (val4 != 0 ||
        val3 == 0 ||
        val2 == 0 ||
        val1 == 0)
        return e_bad_parameter;

      msg->canas.data_type = CANAS_DATATYPE_CHAR3;
      *(char *)(&msg->canas.data[0]) = (char)get_int32(val1);
      *(char *)(&msg->canas.data[1]) = (char)get_int32(val2);
      *(char *)(&msg->canas.data[2]) = (char)get_int32(val3);
      msg->length = 7;
      break;
    case 3:    // char4
      if (val4 == 0 ||
        val3 == 0 ||
        val2 == 0 ||
        val1 == 0)
        return e_bad_parameter;

      msg->canas.data_type = CANAS_DATATYPE_CHAR4;
      *(char *)(&msg->canas.data[0]) = (char)get_int32(val1);
      *(char *)(&msg->canas.data[1]) = (char)get_int32(val2);
      *(char *)(&msg->canas.data[2]) = (char)get_int32(val3);
      *(char *)(&msg->canas.data[3]) = (char)get_int32(val4);
      msg->length = 8;
      break;
    case 4:    // uchar
      if (val4 != 0 ||
        val3 != 0 ||
        val2 != 0 ||
        val1 == 0)
        return e_bad_parameter;

      msg->canas.data_type = CANAS_DATATYPE_UCHAR;
      msg->canas.data[0] = (uint8_t)get_uint32(val1);
      msg->length = 5;
      break;
    case 5:    // uchar2
      if (val4 != 0 ||
        val3 != 0 ||
        val2 == 0 ||
        val1 == 0)
        return e_bad_parameter;

      msg->canas.data_type = CANAS_DATATYPE_UCHAR2;
      msg->canas.data[0] = (uint8_t)get_uint32(val1);
      msg->canas.data[1] = (uint8_t)get_uint32(val2);
      msg->length = 6;
      break;
    case 6:    // uchar3
      if (val4 != 0 ||
        val3 == 0 ||
        val2 == 0 ||
        val1 == 0)
        return e_bad_parameter;

      msg->canas.data_type = CANAS_DATATYPE_UCHAR3;
      msg->canas.data[0] = (uint8_t)get_uint32(val1);
      msg->canas.data[1] = (uint8_t)get_uint32(val2);
      msg->canas.data[2] = (uint8_t)get_uint32(val3);
      msg->length = 7;
      break;
    case 7:    // uchar4
      if (val4 == 0 ||
        val3 == 0 ||
        val2 == 0 ||
        val1 == 0)
        return e_bad_parameter;

      msg->canas.data_type = CANAS_DATATYPE_UCHAR4;
      msg->canas.data[0] = (uint8_t)get_uint32(val1);
      msg->canas.data[1] = (uint8_t)get_uint32(val2);
      msg->canas.data[2] = (uint8_t)get_uint32(val3);
      msg->canas.data[3] = (uint8_t)get_uint32(val4);
      msg->length = 8;
      break;
    case 8:    // short
      if (val4 != 0 ||
        val3 != 0 ||
        val2 != 0 ||
        val1 == 0)
        return e_bad_parameter;

      int_val = get_int32(val1);

      msg->canas.data_type = CANAS_DATATYPE_SHORT;
      msg->canas.data[0] = extract_int(int_val, 1);
      msg->canas.data[1] = extract_int(int_val, 0);
      msg->length = 6;
      break;
    case 9:    // short2
      if (val4 != 0 ||
        val3 != 0 ||
        val2 == 0 ||
        val1 == 0)
        return e_bad_parameter;

      int_val = get_int32(val1);

      msg->canas.data_type = CANAS_DATATYPE_SHORT2;
      msg->canas.data[0] = extract_int(int_val, 1);
      msg->canas.data[1] = extract_int(int_val, 0);

      int_val = get_int32(val2);
      msg->canas.data[2] = extract_int(int_val, 1);
      msg->canas.data[3] = extract_int(int_val, 0);
      msg->length = 8;
      break;
    case 10:    // ushort
      if (val4 != 0 ||
        val3 != 0 ||
        val2 != 0 ||
        val1 == 0)
        return e_bad_parameter;

      uint_val = get_uint32(val1);

      msg->canas.data_type = CANAS_DATATYPE_USHORT;
      msg->canas.data[0] = extract_uint(uint_val, 1);
      msg->canas.data[1] = extract_uint(uint_val, 0);
      msg->length = 6;
      break;
    case 11:    // ushort2
      if (val4 != 0 ||
        val3 != 0 ||
        val2 == 0 ||
        val1 == 0)
        return e_bad_parameter;

      uint_val = get_uint32(val1);

      msg->canas.data_type = CANAS_DATATYPE_USHORT2;
      msg->canas.data[0] = extract_uint(uint_val, 1);
      msg->canas.data[1] = extract_uint(uint_val, 0);

      int_val = get_int32(val2);
      msg->canas.data[2] = extract_uint(uint_val, 1);
      msg->canas.data[3] = extract_uint(uint_val, 0);
      msg->length = 8;
      break;
    case 12:    // long
      if (val4 != 0 ||
        val3 != 0 ||
        val2 != 0 ||
        val1 == 0)
        return e_bad_parameter;

      int_val = get_int32(val1);

      msg->canas.data_type = CANAS_DATATYPE_INT32;
      msg->canas.data[0] = extract_int(int_val, 3);
      msg->canas.data[1] = extract_int(int_val, 2);
      msg->canas.data[2] = extract_int(int_val, 1);
      msg->canas.data[3] = extract_int(int_val, 0);
      msg->length = 8;
      break;
    case 13:    // ulong
      if (val4 != 0 ||
        val3 != 0 ||
        val2 != 0 ||
        val1 == 0)
        return e_bad_parameter;

      uint_val = get_uint32(val1);

      msg->canas.data_type = CANAS_DATATYPE_UINT32;
      msg->canas.data[0] = extract_uint(uint_val, 3);
      msg->canas.data[1] = extract_uint(uint_val, 2);
      msg->canas.data[2] = extract_uint(uint_val, 1);
      msg->canas.data[3] = extract_uint(uint_val, 0);
      msg->length = 8;
      break;
    case 14:    // float
      if (val4 != 0 ||
        val3 != 0 ||
        val2 != 0 ||
        val1 == 0)
        return e_bad_parameter;

      float_val = get_float(val1);

      msg->canas.data_type = CANAS_DATATYPE_FLOAT;
      msg->canas.data[0] = extract_float(float_val, 3);
      msg->canas.data[1] = extract_float(float_val, 2);
      msg->canas.data[2] = extract_float(float_val, 1);
      msg->canas.data[3] = extract_float(float_val, 0);
      msg->length = 8;
      break;
    default:
    case 15:    // nodata
      if (val4 != 0 ||
        val3 != 0 ||
        val2 != 0 ||
        val1 != 0)
        return e_bad_parameter;

      msg->canas.data_type = CANAS_DATATYPE_NODATA;
      break;
    }

    return s_ok;
  }


result_t send_can_id_type_session_val1_val2_val3_val4_action(cli_t *context,
  uint16_t can_id, uint16_t type, uint16_t session, string_t val1, string_t val2, string_t val3, string_t val4)
  {
  canmsg_t msg;
  result_t result;

  if (failed(result = create_can_msg(&msg, can_id, type, session, val1, val2, val3, val4)))
    return result;

  // enqueue the message onto the can bus.
  return can_send(&msg);
  }
