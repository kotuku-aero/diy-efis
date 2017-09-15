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
#ifndef __data_types_h__
#define	__data_types_h__

#include <stdint.h>

#ifdef	__cplusplus
extern "C"
  {
#endif

typedef struct _lla_t {
  float lat;
  float lng;
  float alt;
  } lla_t;
  
typedef struct _xyz_t {
  float x;
  float y;
  float z;
  } xyz_t;
  
typedef struct _qtn_t {
  float q0;
  float q1;
  float q2;
  float q3;
  } qtn_t;

typedef float matrix_t[3][3];

extern const matrix_t identity_matrix;

typedef float vector_t[3];

#define lla_to_vector(lla)  ((float *)lla)

#define xyz_to_vector(xyz) ((float *)xyz)
  
typedef struct
  {
  uint16_t year;
  uint16_t month;
  uint16_t day;
  } date_t;
  
#define SECS_PER_HOUR 3600
#define SECS_PER_DAY 86400
  
extern void add_days(int days, date_t *date);
extern void add_months(int months, date_t *date);

typedef int32_t interval_t;

extern interval_t add_interval(interval_t current, interval_t interval, date_t *update);

// Generic float math constants
#define M_E          2.71828182845904523536028747135f      /* e */
#define M_LOG2E      1.44269504088896340735992468100f      /* log_2 (e) */
#define M_LOG10E     0.43429448190325182765112891892f      /* log_10 (e) */
#define M_SQRT2      1.41421356237309504880168872421f      /* sqrt(2) */
#define M_SQRT1_2    0.70710678118654752440084436210f      /* sqrt(1/2) */
#define M_SQRT3      1.73205080756887729352744634151f      /* sqrt(3) */
#define M_PI         3.14159265358979323846264338328f      /* pi */
#define M_PI_2       1.57079632679489661923132169164f      /* pi/2 */
#define M_PI_4       0.78539816339744830961566084582f      /* pi/4 */
#define M_SQRTPI     1.77245385090551602729816748334f      /* sqrt(pi) */
#define M_2_SQRTPI   1.12837916709551257389615890312f /* 2/sqrt(pi) */
#define M_1_PI       0.31830988618379067153776752675f      /* 1/pi */
#define M_2_PI       0.63661977236758134307553505349f      /* 2/pi */
#define M_2PI        6.28318530717958647692528676656f      /* 2pi  */
#define M_LN10       2.30258509299404568401799145468f      /* ln(10) */
#define M_LN2        0.69314718055994530941723212146f      /* ln(2) */
#define M_LNPI       1.14472988584940017414342735135f      /* ln(pi) */
#define M_EULER      0.57721566490153286060651209008f      /* Euler constant */
#define M_PI         3.14159265358979323846264338328f
#define M_PI_180     0.01745329251994329576923690769f
//#define X            0.00106526443603169529841533860f

#define degrees_to_radians(value) (((float)value) * M_PI_180)
#define knots_to_meters(value) (((float)value) * 0.514444444444444)
#define radians_to_degrees(value) ((short)(value / M_PI_180))
#define meters_to_knots(value) ((short)(value / 0.514444444444444))
#define feet_to_meters(value) (value * 0.3048)

extern char *float_to_scientific(float value, int precision, char *buffer, unsigned int length);

#define vector_dot_product(v1, v2) (v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2])
extern void vector_cross_product(vector_t v3, const vector_t v1, const vector_t v2);
extern void vector_scale(vector_t v3, const vector_t v1, float scale);
extern void vector_add(vector_t v3, const vector_t v1, const vector_t v2);
extern void matrix_multiply(matrix_t mat, const matrix_t a, const matrix_t b);
extern void transform(xyz_t *vec, matrix_t mat);
extern void xyz_add(xyz_t *vec, const xyz_t *v1);

#define numelements(a) (sizeof(a) / sizeof(a[0]))

#ifdef	__cplusplus
  }
#endif

#endif	/* SPATIAL_H */

