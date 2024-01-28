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


result_t open_key(memid_t current, const char *path, bool create, memid_t *memid)
  {
  result_t result;
  if (path == 0 || memid == 0)
    return e_bad_parameter;

  charps_t *parts = string_split(path, '/');

  uint16_t num_parts = charps_count(parts);

  if (num_parts == 0)
    {
    *memid = current;
    return s_ok;
    }

  const char * s = charps_begin(parts)[0];

  if (strlen(s) == 0)
    current = 0;        // start from the root as the part starts with /

  uint16_t i;
  for (i = 0; i < num_parts; i++)
    {
    s = charps_begin(parts)[i];

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

  close_and_free_charps(parts);

  return result;
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
      stream_printf(dest, "float  %s %f\r\n", name, (float)value);
      }
      break;
    case field_xyz:
      {
      xyz_t value;
      reg_get_xyz(key, name, &value);
      do_indent(dest, *indent);
      stream_printf(dest, "xyz    %s [%f,%f,%f]\r\n", name, (float)value.x, (float)value.y, (float)value.z);
      }
      break;
    case field_matrix:
      {
      matrix_t value;
      reg_get_matrix(key, name, &value);
      do_indent(dest, *indent);
      stream_printf(dest, "matrix %s [%f,%f,%f][%f,%f,%f][%f,%f,%f]\r\n", name,
        (float)value.v[0][0], (float)value.v[0][1], (float)value.v[0][2],
        (float)value.v[1][0], (float)value.v[1][1], (float)value.v[1][2],
        (float)value.v[2][0], (float)value.v[2][1], (float)value.v[2][2]);
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
      stream_printf(dest, "qtn    %s [%f,%f,%f,%f]\r\n", name, (float)value.w, (float)value.x, (float)value.y, (float)value.z);
      }
      break;
    case field_lla:
    {
    lla_t value;
    reg_get_lla(key, name, &value);
    do_indent(dest, *indent);
    stream_printf(dest, "lla    %s [%f,%f,%f]\r\n", name, (float)value.lat, (float)value.lng, (float)value.alt);
    }
    break;
    case field_stream:
      do_indent(dest, *indent);
      // TODO: handle this..
      stream_printf(dest, "stream %s\r\n", name);
      break;
    }

  return s_ok;
  }
