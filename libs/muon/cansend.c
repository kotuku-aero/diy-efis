/*
diy-efis
Copyright (C) 2016-2022 Kotuku Aerospace Limited

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
then the original copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.

If you wish to use any of this code in a commercial application then
you must obtain a licence from the copyright holder.  Contact
support@kotuku.aero for information on the commercial licences.
*/
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "neutron_cli.h"
#include "cli_enumerations.h"

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

inline uint8_t extract_uint(uint32_t val, uint16_t byte)
  {
  return (uint8_t) (val >> (byte << 3));
  }

inline uint8_t extract_int(int32_t val, uint16_t byte)
  {
  return extract_uint(*((uint32_t *)&val), byte);
  }

inline uint8_t extract_float(float val, uint16_t byte)
  {
  return extract_uint(*((uint32_t *)&val), byte);
  }

result_t create_can_msg(canmsg_t *msg, uint16_t can_id, uint16_t type, const char * val1, const char * val2, const char * val3, const char * val4)
  {
  int32_t int_val;
  uint32_t uint_val;
  float float_val;

  if (msg == 0 ||
    can_id == 0 ||
    type == 0)
    return e_bad_parameter;

  memset(msg, 0, sizeof(canmsg_t));
  set_can_id(msg, can_id);
  set_can_len(msg, 4);

  // char,char2,char3,char4,uchar,uchar2,uchar3,uchar4,short,short2,ushort,ushort2,long,ulong,float,nodata
  switch (type)
    {
    case CANAS_DATATYPE_INT8:    // char
      if (val4 != 0 ||
        val3 != 0 ||
        val2 != 0 ||
        val1 == 0)
        return e_bad_parameter;

      *(int8_t *)(&msg->data[1]) = (int8_t)get_int32(val1);
      set_can_len(msg, 2);
      msg->data[0] = CANFLY_INT8;
        break;
    case CANAS_DATATYPE_UINT8 :
      if (val4 != 0 ||
        val3 != 0 ||
        val2 != 0 ||
        val1 == 0)
        return e_bad_parameter;

      *(int8_t *)(&msg->data[1]) = (int8_t)get_int32(val1);
      set_can_len(msg, 2);
      msg->data[0] = CANFLY_UINT8;
      break;
    case CANAS_DATATYPE_INT16:    // short
      if (val4 != 0 ||
        val3 != 0 ||
        val2 != 0 ||
        val1 == 0)
        return e_bad_parameter;

      int_val = get_int32(val1);

      msg->data[1] = (uint8_t)(int_val >> 8);
      msg->data[2] = (uint8_t)int_val;
      set_can_len(msg, 3);
      msg->data[0] = CANFLY_INT16;
      break;
    case CANAS_DATATYPE_UINT16:    // ushort
      if (val4 != 0 ||
        val3 != 0 ||
        val2 != 0 ||
        val1 == 0)
        return e_bad_parameter;

      uint_val = get_uint32(val1);

      msg->data[1] = (uint8_t)(uint_val >> 8);
      msg->data[2] = (uint8_t)uint_val;
      set_can_len(msg, 3);
      msg->data[0] = CANFLY_UINT16;
      break;
    case CANAS_DATATYPE_INT32:    // long
      if (val4 != 0 ||
        val3 != 0 ||
        val2 != 0 ||
        val1 == 0)
        return e_bad_parameter;

      int_val = get_int32(val1);

      msg->data[1] = (uint8_t)(int_val >> 24);
      msg->data[2] = (uint8_t)(int_val >> 16);
      msg->data[3] = (uint8_t)(int_val >> 8);
      msg->data[4] = (uint8_t)int_val;
      set_can_len(msg, 5);
      msg->data[0] = CANFLY_INT32;
      break;
    case CANAS_DATATYPE_UINT32:    // ulong
      if (val4 != 0 ||
        val3 != 0 ||
        val2 != 0 ||
        val1 == 0)
        return e_bad_parameter;

      uint_val = get_uint32(val1);

      msg->data[1] = (uint8_t)(uint_val >> 24);
      msg->data[2] = (uint8_t)(uint_val >> 16);
      msg->data[3] = (uint8_t)(uint_val >> 8);
      msg->data[4] = (uint8_t)uint_val;
      set_can_len(msg, 5);
      msg->data[0] = CANFLY_UINT32;
      break;
    case CANAS_DATATYPE_FLOAT:    // float
      if (val4 != 0 ||
        val3 != 0 ||
        val2 != 0 ||
        val1 == 0)
        return e_bad_parameter;

      float_val = get_float(val1);

      msg->data[1] = (uint8_t)((*((uint32_t *)&float_val)) >> 24);
      msg->data[2] = (uint8_t)((*((uint32_t *)&float_val)) >> 16);
      msg->data[3] = (uint8_t)((*((uint32_t *)&float_val)) >> 8);
      msg->data[4] = (uint8_t)(*((uint32_t *)&float_val));
      set_can_len(msg, 5);
      msg->data[0] = CANFLY_FLOAT;
      break;
    default:
    case CANAS_DATATYPE_NODATA:    // nodata
      if (val4 != 0 ||
        val3 != 0 ||
        val2 != 0 ||
        val1 != 0)
        return e_bad_parameter;

      set_can_len(msg, 1);
      msg->data[0] = CANFLY_NODATA;
      break;
    }

    return s_ok;
  }


result_t send_can_id_type_val1_val2_val3_val4_action(cli_t *context,
  uint16_t can_id, uint16_t type, const char * val1, const char * val2, const char * val3, const char * val4)
  {
  canmsg_t msg;
  result_t result;

  if (failed(result = create_can_msg(&msg, can_id, type, val1, val2, val3, val4)))
    return result;

  // enqueue the message onto the can bus.
  return can_send(&msg, DEFAULT_CAN_TIMEOUT);
  }
