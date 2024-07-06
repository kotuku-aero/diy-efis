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
#include "neutron.h"

const variant_t* create_variant_nodata(variant_t* v)
  {
  v->vt = v_none;

  return v;
  }

const variant_t* create_variant_bool(bool value, variant_t* v)
  {
  v->value.boolean = value;
  v->vt = v_bool;

  return v;
  }

const variant_t* create_variant_int8(int8_t value, variant_t* v)
  {
  v->value.int8 = value;
  v->vt = v_int8;

  return v;
  }

const variant_t* create_variant_uint8(uint8_t value, variant_t* v)
  {
  v->value.uint8 = value;
  v->vt = v_uint8;

  return v;
  }

const variant_t* create_variant_int16(int16_t value, variant_t* v)
  {
  v->value.int16 = value;
  v->vt = v_int16;

  return v;
  }

const variant_t* create_variant_uint16(uint16_t value, variant_t* v)
  {
  v->value.uint16 = value;
  v->vt = v_uint16;

  return v;
  }

const variant_t* create_variant_int32(int32_t value, variant_t* v)
  {
  v->value.int32 = value;
  v->vt = v_int32;

  return v;
  }

const variant_t* create_variant_uint32(uint32_t value, variant_t* v)
  {
  v->value.uint32 = value;
  v->vt = v_uint32;

  return v;
  }

const variant_t* create_variant_float(float value, variant_t* v)
  {
  v->value.flt = value;
  v->vt = v_float;

  return v;
  }

const variant_t* create_variant_utc(const tm_t* value, variant_t* v)
  {
  if (v == 0 || value == 0)
    return 0;

  memcpy(&v->value.utc, value, sizeof(tm_t));
  v->vt = v_utc;

  return v;
  }

static float get_float(const canmsg_t* msg)
  {
  uint32_t value = (((uint32_t)msg->data[1]) << 24) |
    (((uint32_t)msg->data[2]) << 16) |
    (((uint32_t)msg->data[3]) << 8) |
    ((uint32_t)msg->data[4]);
  return  *(float*)(&value);
  }

static uint16_t get_uint16(const canmsg_t* msg)
  {
  return  ((((uint16_t)msg->data[1]) << 8) |
    ((uint16_t)msg->data[2]));
  }

static uint32_t get_uint32(const canmsg_t* msg)
  {
  return (((uint32_t)msg->data[1]) << 24) |
    (((uint32_t)msg->data[2]) << 16) |
    (((uint32_t)msg->data[3]) << 8) |
    ((uint32_t)msg->data[4]);
  }

uint16_t can_type_from_variant(variant_type vt)
  {
  switch (vt)
    {
    case   v_none:
    default:
      return CANFLY_NODATA;
    case  v_bool:
      return CANFLY_BOOL;
    case v_int8:
      return CANFLY_INT8;
    case v_uint8:
      return CANFLY_UINT8;
    case v_int16:
      return CANFLY_INT16;
    case v_uint16:
      return CANFLY_UINT16;
    case v_int32:
      return CANFLY_INT32;
    case v_uint32:
      return CANFLY_UINT32;
    case v_float:
      return CANFLY_FLOAT;
    case v_utc:
      return CANFLY_UTC;
    }
  }

result_t msg_to_variant(const canmsg_t* msg, variant_t* v)
  {
  if (msg == 0 || v == 0)
    return e_bad_parameter;

  result_t result;
  tm_t utc;

  uint8_t len = get_can_len(msg);
  if (len < 1)
    return e_bad_parameter;

  switch (msg->data[0])
    {
    case CANFLY_NODATA:
      if (len != 1)
        return e_bad_parameter;

      create_variant_nodata(v);
      break;
    case CANFLY_BOOL_TRUE:
      if (len != 1)
        return e_bad_parameter;

      create_variant_bool(true, v);
      break;
    case CANFLY_BOOL_FALSE:
      if (len != 1)
        return e_bad_parameter;

      create_variant_bool(false, v);
      break;
    case CANFLY_INT8:
      if (len != 2)
        return e_bad_parameter;

      create_variant_int8((int8_t)msg->data[1], v);
      break;
    case CANFLY_UINT8:
      if (len != 2)
        return e_bad_parameter;

      create_variant_uint8(msg->data[1], v);
      break;
    case CANFLY_INT16:
      if (len != 3)
        return e_bad_parameter;

      create_variant_int16((int16_t)get_uint16(msg), v);
      break;
    case CANFLY_UINT16:
      if (len != 3)
        return e_bad_parameter;

      create_variant_uint16(get_uint16(msg), v);
      break;
    case CANFLY_INT32:
      if (len != 5)
        return e_bad_parameter;

      create_variant_int32((int32_t)get_uint32(msg), v);
      break;
    case CANFLY_ERROR:
    case CANFLY_UINT32:
      if (len != 5)
        return e_bad_parameter;

      create_variant_uint32(get_uint32(msg), v);
      break;
    case CANFLY_FLOAT:
      if (len != 5)
        return e_bad_parameter;

      create_variant_float(get_float(msg), v);
      break;
    case CANFLY_UTC:
      if (failed(result = get_param_utc(msg, &utc)))
        return result;

      create_variant_utc(&utc, v);
      break;
    default:
      return e_bad_type;
    }

  return s_ok;
  }

result_t variant_to_msg(const variant_t* v, uint16_t id, uint16_t type, canmsg_t* msg)
  {
  result_t result;
  set_can_id(msg, id);

  switch (type)
    {
    case CANFLY_NODATA:
      return create_can_msg_nodata(msg, id);
    case CANFLY_ERROR:
    {
    uint32_t error;
    if (failed(result = coerce_to_uint32(v, &error)))
      return result;

    return create_can_msg_error(msg, id, error);
    }
    case CANFLY_UINT8:
    {
    uint8_t value;
    if (failed(result = coerce_to_uint8(v, &value)))
      return result;

    return create_can_msg_uint8(msg, id, value);
    }
    break;
    case CANFLY_INT8:
    {
    int8_t value;
    if (failed(result = coerce_to_int8(v, &value)))
      return result;

    return create_can_msg_int8(msg, id, value);
    }
    break;
    case CANFLY_UINT16:
    {
    uint16_t value;
    if (failed(result = coerce_to_uint16(v, &value)))
      return result;

    return create_can_msg_uint16(msg, id, value);
    }
    break;
    case CANFLY_INT16:
    {
    int16_t value;
    if (failed(result = coerce_to_int16(v, &value)))
      return result;

    return create_can_msg_int16(msg, id, value);
    }
    break;
    case CANFLY_UINT32:
    {
    uint32_t value;
    if (failed(result = coerce_to_uint32(v, &value)))
      return result;

    return create_can_msg_uint32(msg, id, value);
    }
    break;
    case CANFLY_INT32:
    {
    int32_t value;
    if (failed(result = coerce_to_int32(v, &value)))
      return result;

    return create_can_msg_int32(msg, id, value);
    }
    break;
    case CANFLY_BOOL:
    {
    bool value;
    if (failed(result = coerce_to_bool(v, &value)))
      return result;

    return create_can_msg_bool(msg, id, value);
    }
    break;
    case CANFLY_FLOAT:
    {
    float value;
    if (failed(result = coerce_to_float(v, &value)))
      return result;

    return create_can_msg_float(msg, id, value);
    }
    break;
    case CANFLY_UTC:
    {
    tm_t value;
    if (failed(result = coerce_to_utc(v, &value)))
      return result;

    return create_can_msg_utc(msg, id, &value);
    }
    break;
    default:
      return e_bad_type;
    }

  return s_ok;
  }

result_t coerce_to_bool(const variant_t* src, bool* value)
  {
  switch (src->vt)
    {
    case v_bool:
      *value = src->value.boolean;
      break;
    case v_int8:
      *value = src->value.int8 != 0;
      break;
    case v_uint8:
      *value = src->value.uint8 != 0;
      break;
    case v_int16:
      *value = src->value.int16 != 0;
      break;
    case v_uint16:
      *value = src->value.uint16 != 0;
      break;
    case v_int32:
      *value = src->value.int32 != 0;
      break;
    case v_uint32:
      *value = src->value.uint32 != 0;
      break;
    case v_float:
      *value = src->value.flt != 0.0f;
      break;
    default:
      return e_bad_type;
    }

  return s_ok;
  }

result_t coerce_to_int8(const variant_t* src, int8_t* value)
  {
  switch (src->vt)
    {
    case v_bool:
      *value = src->value.boolean ? 1 : 0;
      break;
    case v_int8:
      *value = src->value.int8;
      break;
    case v_uint8:
      if (src->value.uint8 > 127)
        return e_bad_parameter;

      *value = (int8_t)src->value.uint8;
      break;
    case v_int16:
      if (src->value.int16 < -128 || src->value.int16 > 127)
        return e_bad_parameter;

      *value = (int8_t)src->value.int16;
      break;
    case v_uint16:
      if (src->value.uint16 > 127)
        return e_bad_parameter;

      *value = (int8_t)src->value.uint16;
      break;
    case v_int32:
      if (src->value.int32 < -128 || src->value.int32 > 127)
        return e_bad_parameter;

      *value = (int8_t)src->value.int32;
      break;
    case v_uint32:
      if (src->value.uint32 > 127)
        return e_bad_parameter;

      *value = (int8_t)src->value.uint32;
      break;
    case v_float:
      if (src->value.flt < -128 || src->value.flt > 127)
        return e_bad_parameter;

      *value = (int8_t)src->value.flt;
      break;
    default:
      return e_bad_type;
    }

  return s_ok;
  }

result_t coerce_to_uint8(const variant_t* src, uint8_t* value)
  {
  switch (src->vt)
    {
    case v_bool:
      *value = src->value.boolean ? 1 : 0;
      break;
    case v_int8:
      if (src->value.int8 < 0)
        return e_bad_parameter;

      *value = src->value.int8;
      break;
    case v_uint8:
      *value = src->value.uint8;
      break;
    case v_int16:
      if (src->value.int16 < 0 || src->value.int16 > 255)
        return e_bad_parameter;

      *value = (uint8_t)src->value.int16;
      break;
    case v_uint16:
      if (src->value.uint16 > 255)
        return e_bad_parameter;

      *value = (uint8_t)src->value.uint16;
      break;
    case v_int32:
      if (src->value.int32 < 0 || src->value.int32 > 255)
        return e_bad_parameter;

      *value = (uint8_t)src->value.int32;
      break;
    case v_uint32:
      if (src->value.uint32 > 255)
        return e_bad_parameter;

      *value = (uint8_t)src->value.uint32;
      break;
    case v_float:
      if (src->value.flt < 0 || src->value.flt > 255)
        return e_bad_parameter;

      *value = (uint8_t)src->value.flt;
      break;
    default:
      return e_bad_type;
    }

  return s_ok;
  }

result_t coerce_to_int16(const variant_t* src, int16_t* value)
  {
  switch (src->vt)
    {
    case v_bool:
      *value = src->value.boolean ? 1 : 0;
      break;
    case v_int8:
      *value = src->value.int8;
      break;
    case v_uint8:
      *value = src->value.uint8;
      break;
    case v_int16:
      *value = src->value.int16;
      break;
    case v_uint16:
      if (src->value.uint16 > 32767)
        return e_bad_parameter;

      *value = (int16_t)src->value.uint16;
      break;
    case v_int32:
      if (src->value.int32 < -32768 || src->value.int32 > 32767)
        return e_bad_parameter;

      *value = (int16_t)src->value.int32;
      break;
    case v_uint32:
      if (src->value.uint32 > 32767)
        return e_bad_parameter;

      *value = (int16_t)src->value.uint32;
      break;
    case v_float:
      if (src->value.flt < -32768 || src->value.flt > 32767)
        return e_bad_parameter;

      *value = (int16_t)src->value.flt;
      break;
    default:
      return e_bad_type;
    }

  return s_ok;
  }

result_t coerce_to_uint16(const variant_t* src, uint16_t* value)
  {
  switch (src->vt)
    {
    case v_bool:
      *value = src->value.boolean ? 1 : 0;
      break;
    case v_int8:
      if (src->value.int8 < 0)
        *value = 0;
      else
        *value = src->value.int8;
      break;
    case v_uint8:
      *value = src->value.uint8;
      break;
    case v_int16:
      if (src->value.int16 < 0)
        *value = 0;
      else
        *value = (uint16_t)src->value.int16;
      break;
    case v_uint16:
      *value = src->value.uint16;
      break;
    case v_int32:
      if (src->value.int32 < 0)
        *value = 0;
      else if (src->value.int32 > 65535)
        *value = 65535;
      else
        *value = (uint16_t)src->value.int32;
      break;
    case v_uint32:
      if (src->value.uint32 > 65535)
        *value = 65535;
      else
        *value = (uint16_t)src->value.uint32;
      break;
    case v_float:
      if (src->value.flt < 0)
        *value = 0;
      else if (src->value.flt > 65535)
        *value = 65535;
      else
        *value = (uint16_t)src->value.flt;
      break;
    default:
      return e_bad_type;
    }

  return s_ok;
  }

result_t coerce_to_int32(const variant_t* src, int32_t* value)
  {
  switch (src->vt)
    {
    case v_bool:
      *value = src->value.boolean ? 1 : 0;
      break;
    case v_int8:
      *value = src->value.int8;
      break;
    case v_uint8:
      *value = src->value.uint8;
      break;
    case v_int16:
      *value = src->value.int16;
      break;
    case v_uint16:
      *value = src->value.uint16;
      break;
    case v_int32:
      *value = src->value.int32;
      break;
    case v_uint32:
      if (src->value.uint32 > 2147483647)
        return e_bad_parameter;

      *value = (int32_t)src->value.uint32;
      break;
    case v_float:
      if (src->value.flt < -0.2147483648f || src->value.flt > 0.2147483647f)
        return e_bad_parameter;

      *value = (int32_t)src->value.flt;
      break;
    default:
      return e_bad_type;
    }

  return s_ok;
  }

result_t coerce_to_uint32(const variant_t* src, uint32_t* value)
  {
  switch (src->vt)
    {
    case v_bool:
      *value = src->value.boolean ? 1 : 0;
      break;
    case v_int8:
      if (src->value.int8 < 0)
        return e_bad_parameter;

      *value = src->value.int8;
      break;
    case v_uint8:
      *value = src->value.uint8;
      break;
    case v_int16:
      if (src->value.int16 < 0)
        return e_bad_parameter;

      *value = (uint32_t)src->value.int16;
      break;
    case v_uint16:
      *value = src->value.uint16;
      break;
    case v_int32:
      if (src->value.int32 < 0)
        return e_bad_parameter;

      *value = (uint32_t)src->value.int32;
      break;
    case v_uint32:

      *value = src->value.uint32;
      break;
    case v_float:
      if (src->value.flt < 0 || src->value.flt > 4294967295)
        return e_bad_parameter;

      *value = (uint32_t)src->value.flt;
      break;
    default:
      return e_bad_type;
    }

  return s_ok;
  }

result_t coerce_to_float(const variant_t* src, float* value)
  {
  switch (src->vt)
    {
    case v_bool:
      *value = src->value.boolean ? 1.0f : 0.0f;
      break;
    case v_int8:
      *value = src->value.int8;
      break;
    case v_uint8:
      *value = src->value.uint8;
      break;
    case v_int16:
      *value = src->value.int16;
      break;
    case v_uint16:
      *value = src->value.uint16;
      break;
    case v_int32:
      *value = (float)src->value.int32;
      break;
    case v_uint32:
      *value = (float)src->value.uint32;
      break;
    case v_float:
      *value = src->value.flt;
      break;
    default:
      return e_bad_type;
    }

  return s_ok;
  }

result_t coerce_to_utc(const variant_t* src, tm_t* value)
  {
  if (src->vt != v_utc)
    return e_bad_type;

  if (src == 0 || value == 0)
    return e_bad_pointer;

  memcpy(value, &src->value.utc, sizeof(tm_t));
  return s_ok;
  }

const variant_t* copy_variant(const variant_t* src, variant_t* dst)
  {
  dst->vt = src->vt;
  switch (src->vt)
    {
    case v_utc:
      memcpy(&dst->value.utc, &src->value.utc, sizeof(tm_t));
      break;
    default:
      dst->value.uint32 = src->value.uint32;
      break;
    }

  return dst;
  }

int compare_variant(const variant_t* v1, const variant_t* v2)
  {
  if (v1 == 0 || v2 == 0)
    return e_bad_parameter;

  result_t result;

  variant_type comp_type;

  // coerce to best type
  switch (v1->vt)
    {
    case v_none:
      return -1;       // never equal
    case v_int8:
    case v_int16:
    case v_int32:
      comp_type = v_int32;
      break;
    case v_bool:
    case v_uint8:
    case v_uint16:
    case v_uint32:
      comp_type = v_uint32;
      break;
    case v_float:
      comp_type = v_float;
      break;
    default:
      return -1;
    }

  switch (v2->vt)
    {
    case v_none:
      return s_false;       // never equal
    case v_int8:
    case v_int16:
    case v_int32:
    {
    int32_t value2;
    if (failed(result = coerce_to_int32(v2, &value2)))
      return -1;

    switch (comp_type)
      {
      case v_int32:
      {
      int32_t value1;
      if (failed(result = coerce_to_int32(v1, &value1)))
        return -1;

      return value1 > value2 ? 1 : value1 == value2 ? 0 : -1;
      }
      case v_uint32:
      {
      uint32_t value1;
      if (failed(result = coerce_to_uint32(v1, &value1)))
        return -1;

      return value1 > value2 ? 1 : value1 == value2 ? 0 : -1;
      }
      case v_float:
      {
      float value1;
      if (failed(result = coerce_to_float(v1, &value1)))
        return -1;

      float flt_value2 = (float)value2;
      if (value1 > flt_value2)
        return 1;

      if (value1 < flt_value2)
        return -1;

      return 0;
      }
      }
    }
    case v_bool:
    case v_uint8:
    case v_uint16:
    case v_uint32:
    {
    uint32_t value2;
    if (failed(result = coerce_to_uint32(v2, &value2)))
      return -1;
    switch (comp_type)
      {
      case v_int32:
      {
      int32_t value1;
      if (failed(result = coerce_to_int32(v1, &value1)))
        return -1;

      if (value1 < 0 || value1 > 0x7fffffff)
        return 1;       // always greater

      return value1 > value2 ? 1 : value1 == value2 ? 0 : -1;
      }
      case v_uint32:
      {
      uint32_t value1;
      if (failed(result = coerce_to_uint32(v1, &value1)))
        return -1;

      return value1 > value2 ? 1 : value1 == value2 ? 0 : -1;
      }
      case v_float:
      {
      float value1;
      if (failed(result = coerce_to_float(v1, &value1)))
        return -1;

      float flt_value2 = (float)value2;
      if (value1 > flt_value2)
        return 1;

      if (value1 < flt_value2)
        return -1;

      return 0;
      }
      }
    }

    case v_float:
    {
    float value2;
    if (failed(result = coerce_to_float(v2, &value2)))
      return -1;

    float value1;
    if (failed(result = coerce_to_float(v1, &value1)))
      return -1;

    if (value1 > value2)
      return 1;

    if (value1 < value2)
      return -1;

    return 0;
    }
    default:
      return -1;
    }

  return -1;
  }

result_t coerce_variant(const variant_t* src, variant_t* dst, variant_type to_type)
  {
  if (src == 0 || dst == 0)
    return e_bad_parameter;

  result_t result;

  switch (to_type)
    {
    case v_none:
      return e_bad_parameter;
    case v_bool:
    {
    bool value;
    if (failed(result = coerce_to_bool(src, &value)))
      return result;

    create_variant_bool(value, dst);
    }
    break;
    case v_int8:
    {
    int8_t value;
    if (failed(result = coerce_to_int8(src, &value)))
      return result;

    create_variant_int8(value, dst);
    }
    break;
    case v_uint8:
    {
    uint8_t value;
    if (failed(result = coerce_to_uint8(src, &value)))
      return result;
    create_variant_uint8(value, dst);
    }
    break;
    case v_int16:
    {
    int16_t value;
    if (failed(result = coerce_to_int16(src, &value)))
      return result;
    create_variant_int16(value, dst);
    }
    break;
    case v_uint16:
    {
    uint16_t value;
    if (failed(result = coerce_to_uint16(src, &value)))
      return result;
    create_variant_uint16(value, dst);
    }
    break;
    case v_int32:
    {
    int32_t value;
    if (failed(result = coerce_to_int32(src, &value)))
      return result;
    create_variant_int32(value, dst);
    }
    break;
    case v_uint32:
    {
    uint32_t value;
    if (failed(result = coerce_to_uint32(src, &value)))
      return result;
    create_variant_uint32(value, dst);
    }
    break;
    case v_float:
    {
    float value;
    if (failed(result = coerce_to_float(src, &value)))
      return result;
    create_variant_float(value, dst);
    }
    break;
    default:
      return e_bad_parameter;
    }

  return s_ok;
  }