#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "neutron_cli.h"

result_t open_key(memid_t current, const char *path, bool create, memid_t *memid)
  {
  result_t result;
  if (path == 0 || memid == 0)
    return e_bad_parameter;

  memid_t parent = 0;

  vector_t parts = string_split(path, '/');

  uint16_t num_parts;
  vector_count(parts, &num_parts);

  if (num_parts == 0)
    {
    *memid = current;
    return s_ok;
    }

  const char * s;
  if (failed(result = vector_at(parts, 0, &s)))
    {
    kfree_split(parts);
    return result;
    }

  if (strlen(s) == 0)
    current = 0;        // start from the root as the part starts with /

  uint16_t i;
  for (i = 0; i < num_parts; i++)
    {
    if (failed(result = vector_at(parts, i, &s)))
      {
      kfree_split(parts);

      return result;
      }

    // handle special cases
    if (strcmp(s, ".") == 0)      // means current
      {
      *memid = current;
      continue;
      }
    else if (strcmp(s, "..") == 0)
      {
      if (failed(result = reg_query_memid(current, 0, 0, 0, memid)))
        break;
      }
    else if (failed(result = reg_open_key(current, s, memid)))
      {
      if (create)
        {
        if (failed(result = reg_create_key(current, s, memid)))
          break;
        }
      else
        break;
      }

    current = *memid;
    }

  kfree_split(parts);

  return result;
  }

char * get_full_path(memid_t key)
  {
  char name[REG_NAME_MAX+1];
  char * path = 0;
  memid_t parent;
  if (failed(reg_query_memid(key, 0, name, 0, &parent)))
    return 0;

  if (parent != 0)
    path = get_full_path(parent);

  uint16_t path_length = (path != 0) ? strlen(path) : 0;
  uint16_t name_length = strlen(name) + 1;    // assume a trailing '/'

  char *result = (char *)kmalloc(path_length + name_length + 1);

  if (path != 0)
    {
    strcpy(result, path);

    if (path_length > 0 &&
      (path_length > 1 || strcmp(path, "/") != 0))
      strcat(result, "/");

    kfree(path);
    }
  else
    result[0] = 0;

  strcat(result, name);

  return result;
  }

result_t edit_script(cli_t *context, const char *title, handle_t stream)
  {
  // get the stream
  result_t result;

  if (failed(result = muon_edit(context->cfg.console_in,
    context->cfg.console_out,
    title,
    stream)))
    {
    stream_close(stream);
    return result;
    }

  return stream_close(stream);
  }

result_t find_enum_name(const enum_t *enums, uint16_t value, const enum_t **name)
  {
  if (name == 0 || enums == 0)
    return e_bad_parameter;
  *name = 0;
  while (enums->name != 0)
    {
    if (enums->value == value)
      {
      *name = enums;
      return s_ok;
      }
    enums++;
    }

  return e_not_found;
  }

void do_indent(handle_t dest, uint16_t indent)
  {
  while (indent--)
    stream_putc(dest, ' ');
  }

result_t show_value(handle_t dest, memid_t key, field_datatype type, const char *name, uint16_t *indent, const enum_t *enumerations)
  {
  if (name == 0)
    return e_bad_parameter;

  switch (type)
    {
    case field_bool:
      {
      bool value;
      reg_get_bool(key, name, &value);
      do_indent(dest, *indent);
      stream_printf(dest, "bool   %s %s\r\n", name, value ? "true" : "false");
      }
      break;
    case field_int16:
      {
      int16_t value;
      reg_get_int16(key, name, &value);
      do_indent(dest, *indent);
      stream_printf(dest, "int16  %s %d\r\n", name, value);
      }
      break;
    case field_uint16:
      {
      uint16_t value;
      reg_get_uint16(key, name, &value);
      do_indent(dest, *indent);

      const enum_t *enum_value;
      if (enumerations != 0 &&
        succeeded(find_enum_name(enumerations, value, &enum_value)))
        stream_printf(dest, "uint16 %s %s\r\n", name, enum_value->name);
      else
        stream_printf(dest, "uint16 %s %ud\r\n", name, value);
      }
      break;
    case field_int32:
      {
      int32_t value;
      reg_get_int32(key, name, &value);
      do_indent(dest, *indent);
      stream_printf(dest, "int32  %s %d\r\n", name, value);
      }
      break;
    case field_uint32:
      {
      uint32_t value;
      reg_get_uint32(key, name, &value);
      do_indent(dest, *indent);

      const enum_t *enum_value;
      if (enumerations != 0 &&
        succeeded(find_enum_name(enumerations, value, &enum_value)))
        stream_printf(dest, "uint32 %s %s\r\n", name, enum_value->name);
      else
        stream_printf(dest, "uint32 %s %d\r\n", name, value);
      }
      break;
    case field_float:
      {
      float value;
      reg_get_float(key, name, &value);
      do_indent(dest, *indent);
      stream_printf(dest, "float  %s %f\r\n", name, (double)value);
      }
      break;
    case field_xyz:
      {
      xyz_t value;
      reg_get_xyz(key, name, &value);
      do_indent(dest, *indent);
      stream_printf(dest, "xyz    %s [%f,%f,%f]\r\n", name, (double)value.x, (double)value.y, (double)value.z);
      }
      break;
    case field_matrix:
      {
      matrix_t value;
      reg_get_matrix(key, name, &value);
      do_indent(dest, *indent);
      stream_printf(dest, "matrix %s [%f,%f,%f][%f,%f,%f][%f,%f,%f]\r\n", name,
        (double)value.v[0][0], (double)value.v[0][1], (double)value.v[0][2],
        (double)value.v[1][0], (double)value.v[1][1], (double)value.v[1][2],
        (double)value.v[2][0], (double)value.v[2][1], (double)value.v[2][2]);
      }
      break;
    case field_string:
      {
      char value[REG_STRING_MAX + 1];
      reg_get_string(key, name, value, 0);
      do_indent(dest, *indent);
      stream_printf(dest, "string %s \"%s\"\r\n", name, value);
      }
      break;
    case field_qtn:
      {
      qtn_t value;
      reg_get_qtn(key, name, &value);
      do_indent(dest, *indent);
      stream_printf(dest, "qtn    %s [%f,%f,%f,%f]\r\n", name, (double)value.q0, (double)value.q1, (double)value.q2, (double)value.q3);
      }
      break;
    case field_lla:
    {
    lla_t value;
    reg_get_lla(key, name, &value);
    do_indent(dest, *indent);
    stream_printf(dest, "lla    %s [%f,%f,%f]\r\n", name, (double)value.lat, (double)value.lng, (double)value.alt);
    }
    break;
    case field_stream:
      do_indent(dest, *indent);
      // TODO: handle this..
      stream_printf(dest, "script %s\r\n", name);
      break;
    }

  return s_ok;
  }
