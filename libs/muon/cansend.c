/*
diy-efis
Copyright (C) 2016 Kotuku Aerospace Limited

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

If a file does not contain a copyright header, either because it is incomplete
or a binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice and
the GPL3 are subservient to that copyright notice.

Portions of this repository contain code fragments from the following
providers.


If any file has a copyright notice or portions of code have been used
and the original copyright notice is not yet transcribed to the repository
then the origional copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "neutron_cli.h"


static uint32_t get_uint32(const char * str)
  {
  uint32_t result = 0;

  if(strlen(str) > 0)
    result = strtoul(str, 0, 10);

  return result;
  }

static int32_t get_int32(const char * str)
  {
  int32_t result = 0;

  if(strlen(str) > 0)
    result = strtol(str, 0, 10);

  return result;
  }

static float get_float(const char * str)
  {
  float result = 0;

  if(strlen(str)> 0)
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

result_t create_can_msg(canmsg_t *msg, uint16_t can_id, uint16_t type, uint16_t session, const char * val1, const char * val2, const char * val3, const char * val4)
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
    case CANAS_DATATYPE_CHAR:    // char
      if (val4 != 0 ||
        val3 != 0 ||
        val2 != 0 ||
        val1 == 0)
        return e_bad_parameter;

      msg->canas.data_type = CANAS_DATATYPE_CHAR;
      *(char *)(&msg->canas.data[0]) = (char)get_int32(val1);
      msg->length = 5;
      break;
    case CANAS_DATATYPE_CHAR2:    // char2
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
    case CANAS_DATATYPE_CHAR3:    // char3
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
    case CANAS_DATATYPE_CHAR4:    // char4
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
    case CANAS_DATATYPE_UCHAR:    // uchar
      if (val4 != 0 ||
        val3 != 0 ||
        val2 != 0 ||
        val1 == 0)
        return e_bad_parameter;

      msg->canas.data_type = CANAS_DATATYPE_UCHAR;
      msg->canas.data[0] = (uint8_t)get_uint32(val1);
      msg->length = 5;
      break;
    case CANAS_DATATYPE_UCHAR2:    // uchar2
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
    case CANAS_DATATYPE_UCHAR3:    // uchar3
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
    case CANAS_DATATYPE_UCHAR4:    // uchar4
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
    case CANAS_DATATYPE_SHORT:    // short
      if (val4 != 0 ||
        val3 != 0 ||
        val2 != 0 ||
        val1 == 0)
        return e_bad_parameter;

      int_val = get_int32(val1);

      msg->canas.data_type = CANAS_DATATYPE_SHORT;
      msg->canas.data[0] = (uint8_t)(int_val >> 8);
      msg->canas.data[1] = (uint8_t)int_val;
      msg->length = 6;
      break;
    case CANAS_DATATYPE_SHORT2:    // short2
      if (val4 != 0 ||
        val3 != 0 ||
        val2 == 0 ||
        val1 == 0)
        return e_bad_parameter;

      int_val = get_int32(val1);

      msg->canas.data_type = CANAS_DATATYPE_SHORT2;
      msg->canas.data[0] = (uint8_t)(int_val >> 8);
      msg->canas.data[1] = (uint8_t)int_val;

      int_val = get_int32(val2);
      msg->canas.data[2] = (uint8_t)(int_val >> 8);
      msg->canas.data[3] = (uint8_t)int_val;
      msg->length = 8;
      break;
    case CANAS_DATATYPE_USHORT:    // ushort
      if (val4 != 0 ||
        val3 != 0 ||
        val2 != 0 ||
        val1 == 0)
        return e_bad_parameter;

      uint_val = get_uint32(val1);

      msg->canas.data_type = CANAS_DATATYPE_USHORT;
      msg->canas.data[0] = (uint8_t)(int_val >> 8);
      msg->canas.data[1] = (uint8_t)int_val;
      msg->length = 6;
      break;
    case CANAS_DATATYPE_USHORT2:    // ushort2
      if (val4 != 0 ||
        val3 != 0 ||
        val2 == 0 ||
        val1 == 0)
        return e_bad_parameter;

      uint_val = get_uint32(val1);

      msg->canas.data_type = CANAS_DATATYPE_USHORT2;
      msg->canas.data[0] = (uint8_t)(int_val >> 8);
      msg->canas.data[1] = (uint8_t)int_val;

      int_val = get_int32(val2);
      msg->canas.data[2] = (uint8_t)(int_val >> 8);
      msg->canas.data[3] = (uint8_t)int_val;
      msg->length = 8;
      break;
    case CANAS_DATATYPE_INT32:    // long
      if (val4 != 0 ||
        val3 != 0 ||
        val2 != 0 ||
        val1 == 0)
        return e_bad_parameter;

      int_val = get_int32(val1);

      msg->canas.data_type = CANAS_DATATYPE_INT32;
      msg->canas.data[0] = (uint8_t)(int_val >> 24);
      msg->canas.data[1] = (uint8_t)(int_val >> 16);
      msg->canas.data[2] = (uint8_t)(int_val >> 8);
      msg->canas.data[3] = (uint8_t)int_val;
      msg->length = 8;
      break;
    case CANAS_DATATYPE_UINT32:    // ulong
      if (val4 != 0 ||
        val3 != 0 ||
        val2 != 0 ||
        val1 == 0)
        return e_bad_parameter;

      uint_val = get_uint32(val1);

      msg->canas.data_type = CANAS_DATATYPE_UINT32;
      msg->canas.data[0] = (uint8_t)(int_val >> 24);
      msg->canas.data[1] = (uint8_t)(int_val >> 16);
      msg->canas.data[2] = (uint8_t)(int_val >> 8);
      msg->canas.data[3] = (uint8_t)int_val;
      msg->length = 8;
      break;
    case CANAS_DATATYPE_FLOAT:    // float
      if (val4 != 0 ||
        val3 != 0 ||
        val2 != 0 ||
        val1 == 0)
        return e_bad_parameter;

      float_val = get_float(val1);

      msg->canas.data_type = CANAS_DATATYPE_FLOAT;
      msg->canas.data[0] = (uint8_t)((*((uint32_t *)&float_val)) >> 24);
      msg->canas.data[1] = (uint8_t)((*((uint32_t *)&float_val)) >> 16);
      msg->canas.data[2] = (uint8_t)((*((uint32_t *)&float_val)) >> 8);
      msg->canas.data[3] = (uint8_t)(*((uint32_t *)&float_val));
      msg->length = 8;
      break;
    default:
    case CANAS_DATATYPE_NODATA:    // nodata
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
  uint16_t can_id, uint16_t type, uint16_t session, const char * val1, const char * val2, const char * val3, const char * val4)
  {
  canmsg_t msg;
  result_t result;

  if (failed(result = create_can_msg(&msg, can_id, type, session, val1, val2, val3, val4)))
    return result;

  // enqueue the message onto the can bus.
  return can_send(&msg);
  }
