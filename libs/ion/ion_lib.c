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
#include "ion.h"

static duk_ret_t lib_ticks(duk_context *ctx)
  {
  duk_push_int(ctx, ticks());
  return 1;
  }

static duk_ret_t lib_yield(duk_context *ctx)
  {
  yield();

  return 0;
  }

static duk_ret_t lib_get_int8(duk_context *ctx)
  {
  canmsg_t *msg = (canmsg_t *)duk_get_pointer(ctx, 0);
  uint16_t index = (uint16_t)duk_get_uint(ctx, 1);

  int8_t v;
  if (failed(get_param_int8(msg, index, &v)))
    return DUK_RET_TYPE_ERROR;       // not found

  duk_push_int(ctx, (duk_int_t)v);
  return 1;
  }

static duk_ret_t lib_get_uint8(duk_context *ctx)
  {
  canmsg_t *msg = (canmsg_t *)duk_get_pointer(ctx, 0);
  uint16_t index = (uint16_t)duk_get_uint(ctx, 1);

  uint8_t v;
  if (failed(get_param_uint8(msg, index, &v)))
    return DUK_RET_TYPE_ERROR;

  duk_push_uint(ctx, v);
  return 1;
  }

static duk_ret_t lib_get_int16(duk_context *ctx)
  {
  canmsg_t *msg = (canmsg_t *)duk_get_pointer(ctx, 0);
  uint16_t index = (uint16_t)duk_get_uint(ctx, 1);

  int16_t v;
  if (failed(get_param_int16(msg, index, &v)))
    return DUK_RET_TYPE_ERROR;

  duk_push_int(ctx, v);
  return 1;
  }

static duk_ret_t lib_get_uint16(duk_context *ctx)
  {
  canmsg_t *msg = (canmsg_t *)duk_get_pointer(ctx, 0);
  uint16_t index = (uint16_t)duk_get_uint(ctx, 1);

  uint16_t v;
  if (failed(get_param_uint16(msg, index, &v)))
    return DUK_RET_TYPE_ERROR;

  duk_push_uint(ctx, v);
  return 1;
  }

static duk_ret_t lib_get_int32(duk_context *ctx)
  {
  canmsg_t *msg = (canmsg_t *)duk_get_pointer(ctx, 0);

  int32_t v;
  if (failed(get_param_int32(msg, &v)))
    return DUK_RET_TYPE_ERROR;

  duk_push_int(ctx, v);
  return 1;
  }

static duk_ret_t lib_get_uint32(duk_context *ctx)
  {
  canmsg_t *msg = (canmsg_t *)duk_get_pointer(ctx, 0);

  uint32_t v;
  if (failed(get_param_uint32(msg, &v)))
    return DUK_RET_TYPE_ERROR;

  duk_push_uint(ctx, v);
  return 1;
  }

static duk_ret_t lib_get_float(duk_context *ctx)
  {
  canmsg_t *msg = (canmsg_t *)duk_get_pointer(ctx, 0);

  float v;
  if (failed(get_param_float(msg, &v)))
    return DUK_RET_TYPE_ERROR;

  duk_push_number(ctx, v);
  return 1;
  }

static duk_ret_t lib_get_string(duk_context *ctx)
  {
  canmsg_t *msg = (canmsg_t *)duk_get_pointer(ctx, 0);
  char str[5];

  switch (msg->canas.data_type)
    {
    case CANAS_DATATYPE_CHAR:
      str[0] = (char)msg->canas.data[0];
      str[1] = 0;
      break;
    case CANAS_DATATYPE_CHAR2:
      str[0] = (char)msg->canas.data[0];
      str[1] = (char)msg->canas.data[1];
      str[2] = 0;
      break;
    case CANAS_DATATYPE_CHAR3:
      str[0] = (char)msg->canas.data[0];
      str[1] = (char)msg->canas.data[1];
      str[2] = (char)msg->canas.data[2];
      str[3] = 0;
      break;
    case CANAS_DATATYPE_CHAR4:
      str[0] = (char)msg->canas.data[0];
      str[1] = (char)msg->canas.data[1];
      str[2] = (char)msg->canas.data[2];
      str[3] = (char)msg->canas.data[3];
      str[4] = 0;
      break;
    default:
      return 0;
    }

  duk_push_string(ctx, str);

  return 1;
  }

static duk_ret_t lib_get_message_type(duk_context *ctx)
  {
  canmsg_t *msg = (canmsg_t *)duk_get_pointer(ctx, 0);

  duk_push_uint(ctx, msg->canas.data_type);

  return 1;
  }

static duk_ret_t lib_get_message_id(duk_context *ctx)
  {
  canmsg_t *msg = (canmsg_t *)duk_get_pointer(ctx, 0);

  duk_push_uint(ctx, msg->id);

  return 1;
  }

static duk_ret_t lib_get_node_id(duk_context *ctx)
  {
  canmsg_t *msg = (canmsg_t *)duk_get_pointer(ctx, 0);

  duk_push_uint(ctx, msg->canas.node_id);

  return 1;
  }

static duk_ret_t lib_get_service_code(duk_context *ctx)
  {
  canmsg_t *msg = (canmsg_t *)duk_get_pointer(ctx, 0);

  duk_push_uint(ctx, msg->canas.service_code);

  return 1;
  }

static duk_ret_t lib_get_message_code(duk_context *ctx)
  {
  canmsg_t *msg = (canmsg_t *)duk_get_pointer(ctx, 0);

  duk_push_uint(ctx, msg->canas.message_code);

  return 1;
  }

static duk_ret_t lib_publish_float(duk_context *ctx)
  {
  uint16_t id = duk_get_uint(ctx, 0);
  float v = (float)duk_get_number(ctx, 1);

  publish_float(id, v);
  return 0;
  }

static duk_ret_t lib_publish_int8(duk_context *ctx)
  {
  uint16_t id = duk_get_uint(ctx, 0);
  int8_t v = (int8_t)duk_get_int(ctx, 1);

  publish_int8(id, &v, 1);
  return 0;
  }

static duk_ret_t lib_publish_uint8(duk_context *ctx)
  {
  uint16_t id = duk_get_uint(ctx, 0);
  uint8_t v = (uint8_t)duk_get_uint(ctx, 1);

  publish_uint8(id, &v, 1);
  return 0;
  }

static duk_ret_t lib_publish_int16(duk_context *ctx)
  {
  uint16_t id = duk_get_uint(ctx, 0);
  int16_t v = (int16_t)duk_get_int(ctx, 1);

  publish_int16(id, &v, 1);
  return 0;
  }

static duk_ret_t lib_publish_uint16(duk_context *ctx)
  {
  uint16_t id = duk_get_uint(ctx, 0);
  uint16_t v = (uint16_t)duk_get_uint(ctx, 1);

  publish_uint16(id, &v, 1);
  return 0;
  }

static duk_ret_t lib_publish_int32(duk_context *ctx)
  {
  uint16_t id = duk_get_uint(ctx, 0);
  int32_t v = (int32_t)duk_get_int(ctx, 1);

  publish_int32(id, v);
  return 0;
  }

static duk_ret_t lib_publish_uint32(duk_context *ctx)
  {
  uint16_t id = duk_get_uint(ctx, 0);
  int32_t v = (int32_t)duk_get_int(ctx, 1);

  publish_int32(id, v);
  return 0;
  }

static duk_ret_t lib_publish_string(duk_context *ctx)
  {
  uint16_t id = duk_get_uint(ctx, 0);
  const char *v = duk_get_string(ctx, 1);
  if (v == 0)
    return;

  // we only ever publish 4 characters
  uint16_t len = min(4, strlen(v));

  publish_int8(id, v, len);
  return 0;
  }

static duk_ret_t lib_publish_array(duk_context *ctx)
  {
  uint16_t id = duk_get_uint(ctx, 0);
  uint16_t i;
  uint16_t n;

  if (!duk_is_array(ctx, 1)) {
    /* not an array */
    return -1;
    }

  n = duk_get_length(ctx, 1);
  if (n > 4)
    return -1;

  uint8_t msg[4];
  for (i = 0; i < n; i++)
    {
    uint8_t val;
    if (duk_get_prop_index(ctx, 1, i))
      {
      val = (uint8_t)duk_get_uint(ctx, -1);
      }
    else
      {
      val = 0;
      }

    msg[i] = val;

    /* ... */
    duk_pop(ctx);
    }

  // publish an array
  publish_uint8(id, msg, n);
  }

static duk_ret_t lib_get_published_int8(duk_context *ctx)
  {
  uint16_t id = duk_get_uint(ctx, 0);
  int8_t v;

  uint16_t len = 1;
  if (failed(get_datapoint_int8(id, &v, &len)))
    return 0;
  
  duk_push_int(ctx, v);

  return 1;
  }

static duk_ret_t lib_get_published_uint8(duk_context *ctx)
  {
  uint16_t id = duk_get_uint(ctx, 0);
  uint8_t v;

  uint16_t len = 1;
  if (failed(get_datapoint_uint8(id, &v, &len)))
    return 0;
  
  duk_push_uint(ctx, v);

  return 1;
  }

static duk_ret_t lib_get_published_int16(duk_context *ctx)
  {
  uint16_t id = duk_get_uint(ctx, 0);
  int16_t v;
  uint16_t len = 1;
  if (failed(get_datapoint_int16(id, &v, &len)))
    return 0;

  duk_push_int(ctx, v);

  return 1;
  }

static duk_ret_t lib_get_published_uint16(duk_context *ctx)
  {
  uint16_t id = duk_get_uint(ctx, 0);
  uint16_t v;
  uint16_t len = 1;
  if (failed(get_datapoint_uint16(id, &v, &len)))
    return 0;

  duk_push_uint(ctx, v);

  return 1;
  }

static duk_ret_t lib_get_published_int32(duk_context *ctx)
  {
  uint16_t id = duk_get_uint(ctx, 0);
  int32_t v;

  if (failed(get_datapoint_int32(id, &v)))
    return 0;

  duk_push_int(ctx, v);

  return 1;
  }

static duk_ret_t lib_get_published_uint32(duk_context *ctx)
  {
  uint16_t id = duk_get_uint(ctx, 0);
  uint32_t v;

  if (failed(get_datapoint_uint32(id, &v)))
    return 0;

  duk_push_int(ctx, v);

  return 1;
  }

static duk_ret_t lib_get_published_string(duk_context *ctx)
  {
  uint16_t id = duk_get_uint(ctx, 0);
  char v[5] = { 0, 0, 0, 0, 0 };
  uint16_t len = 4;
  if (failed(get_datapoint_int8(id, v, &len)))
    return 0;

  duk_push_string(ctx, v);

  return 1;
  }

static duk_ret_t lib_get_published_array(duk_context *ctx)
  {
  uint16_t id = duk_get_uint(ctx, 0);
  uint8_t v[4];
  uint16_t len = 0;

  if (failed(get_datapoint_uint8(id, v, &len)))
    return 0;

  duk_idx_t arr_idx;

  arr_idx = duk_push_array(ctx);
  for (arr_idx = 0; arr_idx < len; arr_idx++)
    {
    duk_push_uint(ctx, v[arr_idx]);
    duk_put_prop_index(ctx, arr_idx, 0);
    }

  return 1;
  }

static duk_ret_t lib_get_published_float(duk_context *ctx)
  {
  uint16_t id = duk_get_uint(ctx, 0);
  float v;

  if (failed(get_datapoint_float(id, &v)))
    return 0;

  duk_push_number(ctx, v);

  return 1;
  }

static duk_ret_t lib_reg_create_key(duk_context *ctx)
  {
  memid_t parent = duk_get_uint(ctx, 0);
  const char *name = duk_get_string(ctx, 1);

  memid_t child;

  if (failed(reg_create_key(parent, name, &child)))
    return 0;

  duk_push_uint(ctx, child);
  return 1;
  }

static duk_ret_t lib_reg_open_key(duk_context *ctx)
  {
  memid_t parent = duk_get_uint(ctx, 0);
  const char *name = duk_get_string(ctx, 1);

  memid_t child;

  if (failed(reg_open_key(parent, name, &child)))
    return 0;

  duk_push_uint(ctx, child);
  return 1;
  }

static duk_ret_t lib_reg_delete_key(duk_context *ctx)
  {
  memid_t memid = duk_get_uint(ctx, 0);

  reg_delete_key(memid);
  return 0;
  }

static duk_ret_t lib_reg_delete_value(duk_context *ctx)
  {
  memid_t parent = duk_get_uint(ctx, 0);
  const char *name = duk_get_string(ctx, 1);

  reg_delete_value(parent, name);
  return 0;
  }

static duk_ret_t lib_reg_rename_value(duk_context *ctx)
  {
  memid_t memid = duk_get_uint(ctx, 0);
  const char *old_name = duk_get_string(ctx, 1);
  const char *new_name = duk_get_string(ctx, 2);

  reg_rename_value(memid, old_name, new_name);
  return 0;
  }

static duk_ret_t lib_reg_get_int16(duk_context *ctx)
  {
  memid_t memid = duk_get_uint(ctx, 0);
  const char *name = duk_get_string(ctx, 1);

  int16_t value;
  if (failed(reg_get_int16(memid, name, &value)))
    return DUK_RET_TYPE_ERROR;

  duk_push_int(ctx, value);
  return 1;
  }

static duk_ret_t lib_reg_set_int16(duk_context *ctx)
  {
  memid_t memid = duk_get_uint(ctx, 0);
  const char *name = duk_get_string(ctx, 1);
  int16_t value = (int16_t)duk_get_uint(ctx, 2);

  if (failed(reg_set_int16(memid, name, value)))
    return DUK_RET_TYPE_ERROR;

  return 0;
  }

static duk_ret_t lib_reg_get_uint16(duk_context *ctx)
  {
  memid_t memid = duk_get_uint(ctx, 0);
  const char *name = duk_get_string(ctx, 1);

  uint16_t value;
  if (failed(reg_get_uint16(memid, name, &value)))
    return DUK_RET_TYPE_ERROR;

  duk_push_uint(ctx, value);
  return 1;
  }

static duk_ret_t lib_reg_set_uint16(duk_context *ctx)
  {
  memid_t memid = duk_get_uint(ctx, 0);
  const char *name = duk_get_string(ctx, 1);
  uint16_t value = (uint16_t)duk_get_uint(ctx, 2);

  if (failed(reg_set_uint16(memid, name, value)))
    return DUK_RET_TYPE_ERROR;

  return 0;
  }

static duk_ret_t lib_reg_get_int32(duk_context *ctx)
  {
  memid_t memid = duk_get_uint(ctx, 0);
  const char *name = duk_get_string(ctx, 1);

  int32_t value;
  if (failed(reg_get_int32(memid, name, &value)))
    return DUK_RET_TYPE_ERROR;

  duk_push_int(ctx, value);
  return 1;
  }

static duk_ret_t lib_reg_set_int32(duk_context *ctx)
  {
  memid_t memid = duk_get_uint(ctx, 0);
  const char *name = duk_get_string(ctx, 1);
  int32_t value = (int32_t)duk_get_uint(ctx, 2);

  if (failed(reg_set_int32(memid, name, value)))
    return DUK_RET_TYPE_ERROR;

  return 0;
  }

static duk_ret_t lib_reg_get_uint32(duk_context *ctx)
  {
  memid_t memid = duk_get_uint(ctx, 0);
  const char *name = duk_get_string(ctx, 1);

  uint32_t value;
  if (failed(reg_get_uint32(memid, name, &value)))
    return DUK_RET_TYPE_ERROR;

  duk_push_uint(ctx, value);
  return 1;
  }

static duk_ret_t lib_reg_set_uint32(duk_context *ctx)
  {
  memid_t memid = duk_get_uint(ctx, 0);
  const char *name = duk_get_string(ctx, 1);
  uint32_t value = (uint32_t)duk_get_uint(ctx, 2);

  if (failed(reg_set_uint32(memid, name, value)))
    return DUK_RET_TYPE_ERROR;

  return 0;
  }

static duk_ret_t lib_reg_get_lla(duk_context *ctx)
  {
  memid_t memid = duk_get_uint(ctx, 0);
  const char *name = duk_get_string(ctx, 1);

  lla_t value;
  if (failed(reg_get_lla(memid, name, &value)))
    return DUK_RET_TYPE_ERROR;

  //json encode it.
  duk_push_sprintf(ctx, "\"lat\":\"%f\",\"lng\":\"%f\",\"alt\":\"%f\"", value.lat, value.lng, value.alt);

  return 1;
  }

static duk_ret_t lib_reg_set_lla(duk_context *ctx)
  {
  memid_t memid = duk_get_uint(ctx, 0);
  const char *name = duk_get_string(ctx, 1);

  // decode the json value
  duk_json_decode(ctx, 2);
  lla_t value;
  duk_get_prop_string(ctx, -1, "lat");
  value.lat = (float)duk_get_number(ctx, -2);
  duk_pop(ctx);
  duk_get_prop_string(ctx, -1, "lng");
  value.lng = (float)duk_get_number(ctx, -2);
  duk_pop(ctx);
  duk_get_prop_string(ctx, -1, "alt");
  value.alt = (float)duk_get_number(ctx, -2);
  duk_pop_2(ctx);

  if (failed(reg_set_lla(memid, name, &value)))
    return DUK_RET_TYPE_ERROR;

  return 0;
  }

static duk_ret_t lib_reg_get_xyz(duk_context *ctx)
  {
  memid_t memid = duk_get_uint(ctx, 0);
  const char *name = duk_get_string(ctx, 1);

  xyz_t value;
  if (failed(reg_get_xyz(memid, name, &value)))
    return DUK_RET_TYPE_ERROR;

  //json encode it.
  duk_push_sprintf(ctx, "\"x\":\"%f\",\"y\":\"%f\",\"z\":\"%f\"", value.x, value.y, value.z);

  return 1;
  }

static duk_ret_t lib_reg_set_xyz(duk_context *ctx)
  {
  memid_t memid = duk_get_uint(ctx, 0);
  const char *name = duk_get_string(ctx, 1);

  // decode the json value
  duk_json_decode(ctx, 2);
  xyz_t value;
  duk_get_prop_string(ctx, -1, "x");
  value.x = (float)duk_get_number(ctx, -2);
  duk_pop(ctx);
  duk_get_prop_string(ctx, -1, "y");
  value.y = (float)duk_get_number(ctx, -2);
  duk_pop(ctx);
  duk_get_prop_string(ctx, -1, "z");
  value.z = (float)duk_get_number(ctx, -2);
  duk_pop_2(ctx);

  if (failed(reg_set_xyz(memid, name, &value)))
    return DUK_RET_TYPE_ERROR;

  return 0;
  }

static duk_ret_t lib_reg_get_matrix(duk_context *ctx)
  {
  memid_t memid = duk_get_uint(ctx, 0);
  const char *name = duk_get_string(ctx, 1);

  matrix_t value;
  if (failed(reg_get_matrix(memid, name, &value)))
    return DUK_RET_TYPE_ERROR;
  duk_push_sprintf(ctx, "[[%f,%f,%f][%f,%f,%f],[%f,%f,%f]]",
    value.v[0][0], value.v[0][1], value.v[0][2],
    value.v[1][0], value.v[1][1], value.v[1][2],
    value.v[1][0], value.v[1][1], value.v[1][2]);

  return 1;
  }

static duk_ret_t lib_reg_set_matrix(duk_context *ctx)
  {
  memid_t memid = duk_get_uint(ctx, 0);
  const char *name = duk_get_string(ctx, 1);

  // decode the json value
  duk_json_decode(ctx, 2);
  matrix_t value;
  duk_get_prop_index(ctx, -1, 0);
  duk_get_prop_index(ctx, -2, 0);
  value.v[0][0] = (float)duk_get_number(ctx, -3);
  duk_pop(ctx);
  duk_get_prop_index(ctx, -2, 1);
  value.v[0][1] = (float)duk_get_number(ctx, -3);
  duk_pop(ctx);
  duk_get_prop_index(ctx, -2, 2);
  value.v[0][2] = (float)duk_get_number(ctx, -3);
  duk_pop_2(ctx);

  duk_get_prop_index(ctx, -1, 1);
  duk_get_prop_index(ctx, -2, 0);
  value.v[1][0] = (float)duk_get_number(ctx, -3);
  duk_pop(ctx);
  duk_get_prop_index(ctx, -2, 1);
  value.v[1][1] = (float)duk_get_number(ctx, -3);
  duk_pop(ctx);
  duk_get_prop_index(ctx, -2, 2);
  value.v[1][2] = (float)duk_get_number(ctx, -3);
  duk_pop_2(ctx);

  duk_get_prop_index(ctx, -1, 2);
  duk_get_prop_index(ctx, -2, 0);
  value.v[2][0] = (float)duk_get_number(ctx, -3);
  duk_pop(ctx);
  duk_get_prop_index(ctx, -2, 1);
  value.v[2][1] = (float)duk_get_number(ctx, -3);
  duk_pop(ctx);
  duk_get_prop_index(ctx, -2, 2);
  value.v[2][2] = (float)duk_get_number(ctx, -3);
  duk_pop_3(ctx);

  if (failed(reg_set_matrix(memid, name, &value)))
    return DUK_RET_TYPE_ERROR;

  return 0;
  }

static duk_ret_t lib_reg_get_qtn(duk_context *ctx)
  {
  memid_t memid = duk_get_uint(ctx, 0);
  const char *name = duk_get_string(ctx, 1);


  qtn_t value;
  if (failed(reg_get_qtn(memid, name, &value)))
    return DUK_RET_TYPE_ERROR;

  //json encode it.
  duk_push_sprintf(ctx, "\"q0\":\"%f\",\"q1\":\"%f\",\"q2\":\"%f\",\"q3\":\"%f\"", value.q0, value.q1, value.q2, value.q3);

  return 1;
  }

static duk_ret_t lib_reg_set_qtn(duk_context *ctx)
  {
  memid_t memid = duk_get_uint(ctx, 0);
  const char *name = duk_get_string(ctx, 1);

  // decode the json value
  duk_json_decode(ctx, 2);
  qtn_t value;
  duk_get_prop_string(ctx, -1, "q0");
  value.q0 = (float)duk_get_number(ctx, -2);
  duk_pop(ctx);
  duk_get_prop_string(ctx, -1, "q1");
  value.q1 = (float)duk_get_number(ctx, -2);
  duk_pop(ctx);
  duk_get_prop_string(ctx, -1, "q2");
  value.q2 = (float)duk_get_number(ctx, -2);
  duk_pop(ctx);
  duk_get_prop_string(ctx, -1, "q3");
  value.q3 = (float)duk_get_number(ctx, -2);
  duk_pop_2(ctx);

  if (failed(reg_set_qtn(memid, name, &value)))
    return DUK_RET_TYPE_ERROR;

  return 0;
  }

static duk_ret_t lib_reg_get_string(duk_context *ctx)
  {
  memid_t memid = duk_get_uint(ctx, 0);
  const char *name = duk_get_string(ctx, 1);


  char str[REG_STRING_MAX + 1];

  if (failed(reg_get_string(memid, name, str, 0)))
    return DUK_RET_TYPE_ERROR;

  str[REG_STRING_MAX] = 0;

  duk_push_string(ctx, str);

  return 1;
  }

static duk_ret_t lib_reg_set_string(duk_context *ctx)
  {
  memid_t memid = duk_get_uint(ctx, 0);
  const char *name = duk_get_string(ctx, 1);
  const char *value = duk_get_string(ctx, 2);

  if (failed(reg_set_string(memid, name, value)))
    return DUK_RET_TYPE_ERROR;

  return 0;
  }

static duk_ret_t lib_reg_get_float(duk_context *ctx)
  {
  memid_t memid = duk_get_uint(ctx, 0);
  const char *name = duk_get_string(ctx, 1);

  float value;
  if (failed(reg_get_float(memid, name, &value)))
    return DUK_RET_TYPE_ERROR;

  duk_push_number(ctx, value);
  return 1;
  }

static duk_ret_t lib_reg_set_float(duk_context *ctx)
  {
  result_t result;
  memid_t memid = duk_get_uint(ctx, 0);
  const char *name = duk_get_string(ctx, 1);
  float value = (float)duk_get_number(ctx, 2);

  if (failed(reg_set_float(memid, name, value)))
    return DUK_RET_TYPE_ERROR;

  return 0;
  }

static duk_ret_t lib_reg_get_bool(duk_context *ctx)
  {
  memid_t memid = duk_get_uint(ctx, 0);
  const char *name = duk_get_string(ctx, 1);

  bool value;
  if (failed(reg_get_bool(memid, name, &value)))
    return DUK_RET_TYPE_ERROR;

  duk_push_boolean(ctx, value);
  return 1;
  }

static duk_ret_t lib_reg_set_bool(duk_context *ctx)
  {
  memid_t memid = duk_get_uint(ctx, 0);
  const char *name = duk_get_string(ctx, 1);
  bool value = (bool)duk_get_boolean(ctx, 2);

  if (failed(reg_set_bool(memid, name, value)))
    return DUK_RET_TYPE_ERROR;

  return 0;
  }

static duk_ret_t lib_stream_create(duk_context *ctx)
  {
  memid_t memid = duk_get_uint(ctx, 0);
  const char *name = duk_get_string(ctx, 1);

  stream_p value;
  if (failed(stream_create(memid, name, &value)))
    return DUK_RET_TYPE_ERROR;

  duk_push_pointer(ctx, value);
  return 1;
  }

static duk_ret_t lib_stream_close(duk_context *ctx)
  {
  handle_t stream = duk_get_pointer(ctx, 0);

  if (failed(stream_close(stream)))
    return DUK_RET_TYPE_ERROR;

  return 0;
  }

static duk_ret_t lib_stream_delete(duk_context *ctx)
  {
  handle_t stream = duk_get_pointer(ctx, 0);

  if (failed(stream_delete(stream)))
    return DUK_RET_TYPE_ERROR;

  return 0;
  }

static duk_ret_t lib_stream_eof(duk_context *ctx)
  {
  handle_t stream = duk_get_pointer(ctx, 0);

  result_t result = stream_eof(stream);

  if (result != s_ok && result != s_false)
    return DUK_RET_TYPE_ERROR;

  duk_push_boolean(ctx, result == s_ok);

  return 1;
  }

// array_t stream_read(handle_t, type_t, uint16_t)
static duk_ret_t lib_stream_read(duk_context *ctx)
  {
  handle_t stream = duk_get_pointer(ctx, 0);
  duk_size_t size;
  void *buffer = duk_get_buffer_data(ctx, 1, &size);

  uint16_t num_read;
  uint16_t bytes_to_read = (uint16_t)size;

  // expand the array to the size requested
  if (failed(stream_read(stream, buffer, bytes_to_read, &num_read)))
    return DUK_RET_TYPE_ERROR;

  // assign the array to the result.
  duk_push_uint(ctx, num_read);
  return 1;
  }

// void stream_write(handle_t, array_t)
static duk_ret_t lib_stream_write(duk_context *ctx)
  {
  handle_t stream = duk_get_pointer(ctx, 0);
  duk_size_t size;
  void *buffer = duk_get_buffer_data(ctx, 1, &size);


  if (failed(stream_write(stream, buffer, (uint16_t)size)))
    return DUK_RET_TYPE_ERROR;

  return 0;
  }

static duk_ret_t lib_stream_getpos(duk_context *ctx)
  {
  handle_t stream = duk_get_pointer(ctx, 0);

  uint16_t pos;
  if (failed(stream_getpos(stream, &pos)))
    return DUK_RET_TYPE_ERROR;

  duk_push_uint(ctx, pos);
  return 1;
  }

static duk_ret_t lib_stream_setpos(duk_context *ctx)
  {
  handle_t stream = duk_get_pointer(ctx, 0);
  uint16_t pos = (uint16_t)duk_get_uint(ctx, 1);

  if (failed(stream_setpos(stream, pos)))
    return DUK_RET_TYPE_ERROR;

  return 0;
  }

static duk_ret_t lib_stream_length(duk_context *ctx)
  {
  handle_t stream = duk_get_pointer(ctx, 0);
  uint16_t pos;
  if (failed(stream_length(stream, &pos)))
    return DUK_RET_TYPE_ERROR;

  duk_push_uint(ctx, pos);
  return 1;
  }

static duk_ret_t lib_stream_truncate(duk_context *ctx)
  {
  handle_t stream = duk_get_pointer(ctx, 0);
  uint16_t pos = (uint16_t)duk_get_uint(ctx, 1);

  if (failed(stream_truncate(stream, pos)))
    return DUK_RET_TYPE_ERROR;

  return 0;
  }

static duk_ret_t lib_stream_copy(duk_context *ctx)
  {
  handle_t stream1 = duk_get_pointer(ctx, 0);
  handle_t stream2 = duk_get_pointer(ctx, 1);

  if (failed(stream_copy(stream1, stream2)))
    return DUK_RET_TYPE_ERROR;

  return 0;
  }

static duk_ret_t lib_stream_path(duk_context *ctx)
  {
  handle_t stream = duk_get_pointer(ctx, 0);
  bool full_path = duk_get_boolean(ctx, 1);

  char path[256];
  if (failed(stream_path(stream, full_path, 256, path)))
    return DUK_RET_TYPE_ERROR;

  duk_push_string(ctx, path);
  return 1;
  }

static void add_function(duk_context *ctx, duk_c_function fn, const char *name, duk_idx_t nargs)
  {
  duk_push_c_function(ctx, fn, nargs);
  duk_put_global_string(ctx, name);
  }

result_t ion_split_path(const char *id, memid_t *parent, char *filename)
  {
  size_t path_len = strlen(id);
  char *path = (char *)neutron_malloc(path_len + 1);

  if (path == 0)
    return e_not_enough_memory;

  memcpy(path, id, path_len + 1);

  // split the path
  char *s = path;
  char *name = path;
  // scan and split the path
  while (*s != 0)
    {
    if (*s == '/')
      {
      *s = 0;
      if (failed(reg_open_key(*parent, name, parent)))
        {
        neutron_free(path);
        return DUK_RET_TYPE_ERROR;
        }

      s++;
      name = s;
      }
    else
      s++;
    }

  strncpy(filename, name, REG_NAME_MAX);
  filename[REG_NAME_MAX] = 0;

  neutron_free(path);

  return s_ok;
  }

static duk_ret_t ion_load_script(ion_context_t *ion, const char *id)
  {
  // pick up the root key
  memid_t parent = ion->home;

  char name[REG_NAME_MAX + 1];

  if (failed(ion_split_path(id, &parent, name)))
    return DUK_RET_TYPE_ERROR;

  stream_p stream;
  if (failed(stream_open(parent, name, &stream)))
    return DUK_RET_TYPE_ERROR;

  uint16_t len;
  if (failed(stream_length(stream, &len)))
    {
    stream_close(stream);
    return DUK_RET_TYPE_ERROR;
    }

  char *script = (char *)neutron_malloc(len + 1);

  if (script == 0)
    {
    stream_close(stream);
    return DUK_RET_TYPE_ERROR;
    }

  if (failed(stream_read(stream, script, len, &len)))
    {
    neutron_free(script);
    stream_close(stream);
    return DUK_RET_TYPE_ERROR;
    }

  script[len] = 0;

  duk_push_lstring(ion->ctx, script, len);
  neutron_free(script);
  stream_close(stream);

  return 1;
  }

static duk_ret_t neutron_mod_search(duk_context *ctx)
  {
  /* Nargs was given as 4 and we get the following stack arguments:
  *   index 0: id
  *   index 1: require
  *   index 2: exports
  *   index 3: module
  */

  const char *id = duk_require_string(ctx, 0);

  duk_memory_functions out_funcs;
  duk_get_memory_functions(ctx, &out_funcs);
  
  ion_context_t *ion = (ion_context_t *)out_funcs.udata;

  return ion_load_script(ion, id);
  }

/* Use a proxy wrapper to make undefined methods (console.foo()) no-ops. */
#define DUK_CONSOLE_PROXY_WRAPPER  (1 << 0)

/* XXX: Add some form of log level filtering. */

/* XXX: For now logs everything to stdout, V8/Node.js logs debug/info level
* to stdout, warn and above to stderr.  Should this extra do the same?
*/

/* XXX: Should all output be written via e.g. console.write(formattedMsg)?
* This would make it easier for user code to redirect all console output
* to a custom backend.
*/

/* XXX: Init console object using duk_def_prop() when that call is available. */

static duk_ret_t ion_console_log_helper(duk_context *ctx, const char *error_name)
  {
  duk_memory_functions mem_funcs;
  duk_get_memory_functions(ctx, &mem_funcs);

  ion_context_t *ion = (ion_context_t *)mem_funcs.udata;
  // quick test for no logging.
  if (ion->console_out == 0)
    return 0;

  duk_idx_t i, n;
  duk_uint_t flags;

  flags = (duk_uint_t)duk_get_current_magic(ctx);

  n = duk_get_top(ctx);

  duk_get_global_string(ctx, "console");
  duk_get_prop_string(ctx, -1, "format");

  for (i = 0; i < n; i++) {
    if (duk_check_type_mask(ctx, i, DUK_TYPE_MASK_OBJECT)) {
      /* Slow path formatting. */
      duk_dup(ctx, -1);  /* console.format */
      duk_dup(ctx, i);
      duk_call(ctx, 1);
      duk_replace(ctx, i);  /* arg[i] = console.format(arg[i]); */
      }
    }

  duk_pop_2(ctx);

  duk_push_string(ctx, " ");
  duk_insert(ctx, 0);
  duk_join(ctx, n);

  if (error_name) {
    duk_push_error_object(ctx, DUK_ERR_ERROR, "%s", duk_require_string(ctx, -1));
    duk_push_string(ctx, "name");
    duk_push_string(ctx, error_name);
    duk_def_prop(ctx, -3, DUK_DEFPROP_FORCE | DUK_DEFPROP_HAVE_VALUE);  /* to get e.g. 'Trace: 1 2 3' */
    duk_get_prop_string(ctx, -1, "stack");
    }

  stream_printf(ion->console_out, "%s\n", duk_to_string(ctx, -1));

  return 0;
  }

static duk_ret_t ion_console_assert(duk_context *ctx) {
  if (duk_to_boolean(ctx, 0)) {
    return 0;
    }
  duk_remove(ctx, 0);

  return ion_console_log_helper(ctx, "AssertionError");
  }

static duk_ret_t ion_console_log(duk_context *ctx) {
  return ion_console_log_helper(ctx, NULL);
  }

static duk_ret_t ion_console_trace(duk_context *ctx) {
  return ion_console_log_helper(ctx, "Trace");
  }

static duk_ret_t ion_console_info(duk_context *ctx) {
  return ion_console_log_helper(ctx, NULL);
  }

static duk_ret_t ion_console_warn(duk_context *ctx) {
  return ion_console_log_helper(ctx, NULL);
  }

static duk_ret_t ion_console_error(duk_context *ctx) {
  return ion_console_log_helper(ctx, "Error");
  }

static duk_ret_t ion_console_dir(duk_context *ctx) {
  /* For now, just share the formatting of .log() */
  return ion_console_log_helper(ctx, 0);
  }

static void ion_console_reg_vararg_func(duk_context *ctx, duk_c_function func, const char *name, duk_uint_t flags)
  {
  duk_push_c_function(ctx, func, DUK_VARARGS);
  duk_push_string(ctx, "name");
  duk_push_string(ctx, name);
  duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_FORCE);  /* Improve stacktraces by displaying function name */
  duk_set_magic(ctx, -1, (duk_int_t)flags);
  duk_put_prop_string(ctx, -2, name);
  }

static void duk_console_init(duk_context *ctx, duk_uint_t flags)
  {
  duk_push_object(ctx);

  /* Custom function to format objects; user can replace.
  * For now, try JX-formatting and if that fails, fall back
  * to ToString(v).
  */
  duk_eval_string(ctx,
    "(function (E) {"
    "return function format(v){"
    "try{"
    "return E('jx',v);"
    "}catch(e){"
    "return String(v);"  /* String() allows symbols, ToString() internal algorithm doesn't. */
    "}"
    "};"
    "})(Duktape.enc)");
  duk_put_prop_string(ctx, -2, "format");

  ion_console_reg_vararg_func(ctx, ion_console_assert, "assert", flags);
  ion_console_reg_vararg_func(ctx, ion_console_log, "log", flags);
  ion_console_reg_vararg_func(ctx, ion_console_log, "debug", flags);  /* alias to console.log */
  ion_console_reg_vararg_func(ctx, ion_console_trace, "trace", flags);
  ion_console_reg_vararg_func(ctx, ion_console_info, "info", flags);
  ion_console_reg_vararg_func(ctx, ion_console_warn, "warn", flags);
  ion_console_reg_vararg_func(ctx, ion_console_error, "error", flags);
  ion_console_reg_vararg_func(ctx, ion_console_error, "exception", flags);  /* alias to console.error */
  ion_console_reg_vararg_func(ctx, ion_console_dir, "dir", flags);

  duk_put_global_string(ctx, "console");

  /* Proxy wrapping: ensures any undefined console method calls are
  * ignored silently.  This is required specifically by the
  * DeveloperToolsWG proposal (and is implemented also by Firefox:
  * https://bugzilla.mozilla.org/show_bug.cgi?id=629607).
  */

  if (flags & DUK_CONSOLE_PROXY_WRAPPER) {
    /* Tolerate errors: Proxy may be disabled. */
    duk_peval_string_noresult(ctx,
      "(function(){"
      "var D=function(){};"
      "console=new Proxy(console,{"
      "get:function(t,k){"
      "var v=t[k];"
      "return typeof v==='function'?v:D;"
      "}"
      "});"
      "})();"
    );
    }
  }

void register_ion_functions(duk_context *ctx, handle_t co)
  {
  add_function(ctx, lib_publish_float, "publish_float", 2);
  add_function(ctx, lib_publish_int8, " publish_int8", 2);
  add_function(ctx, lib_publish_uint8, "publish_uint8", 2);
  add_function(ctx, lib_publish_int16, "publish_int16", 2);
  add_function(ctx, lib_publish_uint16, "publish_uint16", 2);
  add_function(ctx, lib_publish_int32, "publish_int32", 2);
  add_function(ctx, lib_publish_uint32, "publish_uint32", 2);
  add_function(ctx, lib_publish_string, "publish_string", 2);
  add_function(ctx, lib_publish_array, "publish_array", 2);
  add_function(ctx, lib_get_published_int8, "get_published_int8", 1);
  add_function(ctx, lib_get_published_uint8, "get_published_uint8", 1);
  add_function(ctx, lib_get_published_int16, "get_published_int16", 1);
  add_function(ctx, lib_get_published_uint16, "get_published_uint16", 1);
  add_function(ctx, lib_get_published_int32, "get_published_int32", 1);
  add_function(ctx, lib_get_published_uint32, "get_published_uint32", 1);
  add_function(ctx, lib_get_published_float, "get_published_float", 1);
  add_function(ctx, lib_get_published_string, "get_published_string", 1);
  add_function(ctx, lib_get_published_array, "get_published_array", 1);
  add_function(ctx, lib_get_int8, "get_int8", 2);
  add_function(ctx, lib_get_uint8, "get_uint8", 2);
  add_function(ctx, lib_get_int16, "get_int16", 2);
  add_function(ctx, lib_get_uint16, "get_uint16", 2);
  add_function(ctx, lib_get_int32, "get_int32", 1);
  add_function(ctx, lib_get_uint32, "get_uint32", 1);
  add_function(ctx, lib_get_float, "get_float", 1);
  add_function(ctx, lib_get_string, "get_string", 1);
  add_function(ctx, lib_get_node_id, "get_node_id", 1);
  add_function(ctx, lib_get_message_type, "get_message_type", 1);
  add_function(ctx, lib_get_service_code, "get_service_code", 1);
  add_function(ctx, lib_get_message_code, "get_message_code", 1);
  add_function(ctx, lib_get_message_id, "get_message_id", 1);
  add_function(ctx, lib_reg_create_key, "reg_create_key", 2);
  add_function(ctx, lib_reg_open_key, "reg_open_key", 2);
  add_function(ctx, lib_reg_delete_key, "reg_delete_key", 1);
  add_function(ctx, lib_reg_delete_value, "reg_delete_value", 2);
  add_function(ctx, lib_reg_rename_value, "reg_rename_value", 3);
  add_function(ctx, lib_reg_get_int16, "reg_get_int16", 1);
  add_function(ctx, lib_reg_set_int16, " reg_set_int16", 3);
  add_function(ctx, lib_reg_get_uint16, "reg_get_uint16", 2);
  add_function(ctx, lib_reg_set_uint16, "reg_set_uint16", 3);
  add_function(ctx, lib_reg_get_int32, "reg_get_int32", 2);
  add_function(ctx, lib_reg_set_int32, "reg_set_int32", 3);
  add_function(ctx, lib_reg_get_uint32, "reg_get_uint32", 2);
  add_function(ctx, lib_reg_set_uint32, "reg_set_uint32", 3);
  add_function(ctx, lib_reg_get_lla, "reg_get_lla", 2);
  add_function(ctx, lib_reg_set_lla, "reg_set_lla", 3);
  add_function(ctx, lib_reg_get_xyz, "reg_get_xyz", 2);
  add_function(ctx, lib_reg_set_xyz, "reg_set_xyz", 3);
  add_function(ctx, lib_reg_get_matrix, "reg_get_matrix", 2);
  add_function(ctx, lib_reg_set_matrix, "reg_set_matrix", 3);
  add_function(ctx, lib_reg_get_qtn, "reg_get_qtn", 2);
  add_function(ctx, lib_reg_set_qtn, "reg_set_qtn", 3);
  add_function(ctx, lib_reg_get_string, "reg_get_string", 2);
  add_function(ctx, lib_reg_set_string, "reg_set_string", 3);
  add_function(ctx, lib_reg_get_float, "reg_get_float", 2);
  add_function(ctx, lib_reg_set_float, "reg_set_float", 3);
  add_function(ctx, lib_reg_get_bool, "reg_get_bool", 2);
  add_function(ctx, lib_reg_set_bool, "reg_set_bool", 3);
  add_function(ctx, lib_stream_create, "stream_create", 2);
  add_function(ctx, lib_stream_close, "stream_close", 1);
  add_function(ctx, lib_stream_delete, "stream_delete", 1);
  add_function(ctx, lib_stream_eof, "stream_eof", 1);
  add_function(ctx, lib_stream_read, "stream_read", 2);
  add_function(ctx, lib_stream_write, "stream_write", 2);
  add_function(ctx, lib_stream_getpos, "stream_getpos", 1);
  add_function(ctx, lib_stream_setpos, "stream_setpo", 2);
  add_function(ctx, lib_stream_length, "stream_length", 1);
  add_function(ctx, lib_stream_truncate, "stream_truncate", 2);
  add_function(ctx, lib_stream_copy, "stream_copy", 2);
  add_function(ctx, lib_stream_path, "stream_path", 2);
  add_function(ctx, lib_stream_path, "stream_path", 2);
  add_function(ctx, lib_ticks, "ticks", 0);
  add_function(ctx, lib_yield, "yield", 0);

  // init the module loader
  duk_get_global_string(ctx, "Duktape");
  duk_push_c_function(ctx, neutron_mod_search, 4 /*nargs*/);
  duk_put_prop_string(ctx, -2, "modSearch");
  duk_pop(ctx);

  if (co != 0)
    duk_console_init(ctx, DUK_CONSOLE_PROXY_WRAPPER);
  }
