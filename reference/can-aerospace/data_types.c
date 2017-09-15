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
#include "data_types.h"
#include "parameter_definitions.h"
#include "can_msg.h"

#include <string.h>

void add_days(int days, date_t *date)
  {

  }

void add_months(int months, date_t *date)
  {

  }

const matrix_t identity_matrix = {
  { 1.0, 0.0, 0.0 },
  { 0.0, 1.0, 0.0 },
  { 0.0, 0.0, 1.0 }
  };

interval_t add_interval(interval_t current, interval_t interval, date_t *update)
  {
  current += interval;

  if(update != 0)
    add_days(current / SECS_PER_DAY, update);

  return current % SECS_PER_DAY;
  }

void matrix_multiply(matrix_t mat, const matrix_t a, const matrix_t b)
  {
  int x;
  int y;
  int w;
  vector_t op;

  for(x = 0; x < 3; x++)
    {
    for(y = 0; y < 3; y++)
      {
      for(w = 0; w < 3; w++)
        op[w] = a[x][w] * b[w][y];

      mat[x][y] = 0;
      mat[x][y] = op[0] + op[1] + op[2];
      }
    }
  }

void transform(xyz_t *vec, matrix_t mat)
  {
  vec->x = mat[0][0] * vec->x + mat[0][1] * vec->x + mat[0][2] * vec->x;
  vec->y = mat[0][0] * vec->y + mat[0][1] * vec->y + mat[0][2] * vec->y;
  vec->z = mat[0][0] * vec->z + mat[0][1] * vec->z + mat[0][2] * vec->z;
  }

void xyz_add(xyz_t *vec, const xyz_t *v1)
  {
  vec->x += v1->x;
  vec->y += v1->y;
  vec->z += v1->z;
  }

void vector_cross_product(vector_t v3, const vector_t v1, const vector_t v2)
  {
  v3[0]= (v1[1]*v2[2]) - (v1[2]*v2[1]);
  v3[1]= (v1[2]*v2[0]) - (v1[0]*v2[2]);
  v3[2]= (v1[0]*v2[1]) - (v1[1]*v2[0]);
  }

void vector_scale(vector_t v3, const vector_t v1, float scale)
  {
  v3[0] = v1[0] * scale;
  v3[1] = v1[1] * scale;
  v3[2] = v1[2] * scale;
  }

void vector_add(vector_t v3, const vector_t v1, const vector_t v2)
  {
  v3[0] = v1[0] + v2[0];
  v3[1] = v1[1] + v2[1];
  v3[2] = v1[2] + v2[2];
  }

uint32_t to_big_endian32(uint32_t value)
  {
  return ((value >> 24) & 0x000000ff) |
         ((value >> 8)  & 0x0000ff00) |
         ((value << 8)  & 0x00ff0000) |
         ((value << 24) & 0xff000000);
  }

uint16_t to_big_endian16(uint16_t value)
  {
  return ((value >> 8)  & 0x00ff) |
         ((value << 8)  & 0xff00);
  }

uint8_t build_parameter1_short(can_parameter_type_1_t *param, short value)
  {
  param->hdr.data_type = p1t_short;
  param->hdr.length = sizeof(can_parameter_definition_t) + sizeof(int16_t);
  param->value.SHORT = (int16_t)value;
  
  return param->hdr.length;
  }

uint8_t build_parameter1_uint16(can_parameter_type_1_t *param, uint16_t value)
  {
  param->hdr.data_type = p1t_ushort;
  param->hdr.length = sizeof(can_parameter_definition_t) + sizeof(uint16_t);
  param->value.USHORT = value;
  
  return param->hdr.length;
  }

uint8_t build_parameter1_float(can_parameter_type_1_t *param, float value)
  {
  param->hdr.data_type = p1t_float;
  param->hdr.length = sizeof(can_parameter_definition_t) + sizeof(float);
  param->value.FLOAT = value;
  
  return param->hdr.length;
  }

uint8_t build_parameter1_matrix(can_parameter_type_1_t *param, const matrix_t *value)
  {
  param->hdr.data_type = p1t_matrix;
  param->hdr.length = sizeof(can_parameter_definition_t) + sizeof(matrix_t);
  
  memcpy(&param->value.MATRIX, value, sizeof(matrix_t));
  
  return param->hdr.length;
  }
