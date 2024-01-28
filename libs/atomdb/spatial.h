#ifndef __spatial_h__
#define __spatial_h__

#include <math.h>
#include "../neutron/neutron.h"
#include "../neutron/type_vector.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief A pixel is stored in the terrain database
 * and is hillside shaded
*/

#define FIXED_POINT

#ifdef FIXED_POINT
#include "../fixedpoint/fixmath.h"
typedef fix16_t fixed_t;

// #define SPATIAL_MINUS_ONE_DEGREE fix16_from_float(-1.0f)
#define SPATIAL_MINUS_ONE_DEGREE (0xFFFF0000)
// #define SPATIAL_ONE_DEGREE fix16_from_float(1.0f)
#define SPATIAL_ONE_DEGREE (65536)
// #define SPATIAL_ONE_MINUTE fix16_from_float(1.0f / 30.0f)
#define SPATIAL_30_MINUTES (16384)
// #define SPATIAL_ONE_MINUTE fix16_from_float(1.0f / 60.0f)
#define SPATIAL_ONE_MINUTE (1092)
// #define SPATIAL_30_SECOND fix16_from_float(1.0f / 120.0f)
#define SPATIAL_30_SECOND (546)
// #define SPATIAL_ONE_SECOND (1.0f / 3600.0f)
#define SPATIAL_ONE_SECOND (18)

#else
typedef  float fixed_t;
#define SPATIAL_MINUS_ONE_DEGREE (-1.0f)
#define SPATIAL_30_MINUTES (0.5f)
#define SPATIAL_ONE_MINUTE (1.0f / 60.0f)
#define SPATIAL_30_SECOND (1.0f / 120.0f)
#define SPATIAL_ONE_SECOND (1.0f / 3600.0f)
#define SPATIAL_ONE_DEGREE (1.0f)
#endif

/**
 * @brief Return the fixed-point cosine for an angle
 * @param angle angle of latitude -90..90
 * @return clamped cosine
*/
extern fixed_t fixed_cos(int angle);

static inline fixed_t make_fixed(int deg, int min, int sec)
  {
#ifdef FIXED_POINT
  // rounding errors here....
  return fix16_from_int(deg) + (SPATIAL_ONE_MINUTE * min) + (SPATIAL_ONE_SECOND * sec);
#else
  return (min / 60.0f) + (sec / 3600.0f) + ((float)deg);
#endif
  }
  
  /**
 * @brief convert a spatial position to a floating point approximation
 * @param value
 * @return
*/
static inline float fixed_to_float(fixed_t value)
  {
#ifdef FIXED_POINT
  return fix16_to_float(value);
#else
  return value;
#endif
  }

static inline fixed_t float_to_fixed(float value)
  {
#ifdef FIXED_POINT
  return fix16_from_float(value);
#else
  return value;
#endif
  }

static inline fixed_t int_to_fixed(int value)
  {
#ifdef FIXED_POINT
  return fix16_from_int(value);
#else
  return (fixed_t)value;
#endif
  }
/**
 * @brief Compare 2 spatial points
 * @param left  left point
 * @param right right point
 * @return 0 if same, -1 if left < right, 1 if right > left
*/
static inline int compare_fixed(fixed_t left, fixed_t right)
  {
  return left == right ? 0 : (left > right ? 1 : -1);
  }

static inline int fixed_to_int(fixed_t deg)
  {
#ifdef FIXED_POINT
  if (deg < 0)
    {
    deg = 0-deg;
    deg >>= 16;
    deg *= -1;
    return deg;
    }
  return deg >> 16;
#else
  return (int)deg;
#endif
  }

static inline int spatial_min(fixed_t deg)
  {
#ifdef FIXED_POINT
  deg &= 0x0000FFFF;    // mask part
  deg *= 60;
  return deg >> 16;
#else
  return abs(((int)(deg * 60.0f)) % 60);
#endif
  }

static inline int spatial_sec(fixed_t deg)
  {
#ifdef FIXED_POINT
  deg &= 0x0000FFFF;    // mask part
  deg *= 60;            // remove min
  deg &= 0x0000FFFF;    // mask part
  deg *= 60;
  return deg >> 16;     // return seconds
#else
  return abs(((int)(deg * 3600.0f)) % 60);
#endif
  }

static inline fixed_t spatial_abs(fixed_t value)
  {
#ifdef FIXED_POINT
  return fix16_abs(value);
#else
  return (float) fabs(value);
#endif
  }
/**
 * @brief Converted a compressed altitude (
 * @param alt
 * @return
*/
static inline float alt_to_float(uint32_t alt)
  {
  return alt * 0.01f;
  }

static inline fixed_t* copy_fixed(const fixed_t* src, fixed_t* dst)
  {
  *dst = *src;
  return dst;
  }

static inline fixed_t fmul_fixed(float value, fixed_t src)
  {
  return float_to_fixed(fixed_to_float(src) * value);
  }

static inline fixed_t mul_fixed(fixed_t x, fixed_t y)
  {
#ifdef FIXED_POINT
  return fix16_mul(x, y);
#else
  return x * y;
#endif
  }

static inline fixed_t div_fixed(fixed_t left, fixed_t right)
  {
#ifdef FIXED_POINT
  return fix16_div(left, right);
#else
  return left / right;
#endif
  }

static inline fixed_t idiv_fixed(fixed_t left, int right)
  {
#ifdef FIXED_POINT
  return fix16_div(left, right << 16);
#else
  return left / right;
#endif
  }

static inline fixed_t add_fixed(fixed_t left, fixed_t right)
  {
#ifdef FIXED_POINT
  return fix16_add(left, right);
#else
  return left + right;
#endif
  }

static inline fixed_t sub_fixed(fixed_t left, fixed_t right)
  {
#ifdef FIXED_POINT
  return fix16_sub(left, right);
#else
  return left - right;
#endif
  }

static inline fixed_t imul_fixed(int16_t mul, fixed_t src)
  {
#ifdef FIXED_POINT
  return fix16_mul(fix16_from_int(mul), src);
#else
  return src * mul;
#endif
  }

extern fixed_t round_fixed(fixed_t value);

extern fixed_t add_spatial_lat(fixed_t left, fixed_t right);
extern fixed_t sub_spatial_lat(fixed_t left, fixed_t right);

extern fixed_t add_spatial_lng(fixed_t left, fixed_t right);
extern fixed_t sub_spatial_lng(fixed_t left, fixed_t right);

typedef struct PACKED _latlng_t {
  fixed_t lat;      // alias for y
  fixed_t lng;      // alias for x
  } latlng_t;
/**
 * @brief Approximate distance between 2 spatial points.
 * @param from first point
 * @param to   second point
 * @return approximate distance in meters
*/
extern uint32_t spatial_distance(const latlng_t *from, const latlng_t *to);

static inline latlng_t* copy_latlng(const latlng_t* src, latlng_t* dst)
  {
  dst->lat = src->lat;
  dst->lng = src->lng;
  return dst;
  }

/*  To support encoding paths in a pointset which is a collection
*   of spatial points, the z attribute is used.
*   The upper 2 bits can be used to encode an arc start, or a circle
*   start.
* 
*   If the upper 2 bits are:
*   00    This is a point, part of a polygon or polyline
*   01    This is a start of a circle, the current point is the center
*         the following point is on the circumference and the z element is
*         the elevation.
*   10    This is an arc, 2 points follow. This point is the arc start, the
*         second following is the arc end and the z element is the elevation
*   11    This is a negative elevation, part of a polygon or polyline
* 
*   This allows 2^14 meters of elevation or 49152 feet.  As a general rule
*   the encoding does not affect the ESRI shape conversion as mostly the elevations
*   are 00
*/
#define ZMASK 0x3FFF
#define CIRCLE_START 0x4000
#define ARC_START 0x8000
/**
 * @brief A spatial point is 14 bytes long and
 * holds a 3d geographic point.  The start of it
 * looks line a latlng_t
*/
typedef struct PACKED _spatial_point_t {
  latlng_t latlng;
  // see encoding above.  Allows for storage or paths
  int16_t  elevation;         // altitude in meters
  } spatial_point_t;

/**
 * @brief A spatial rectangle holds NED rctangle
*/
typedef struct PACKED _spatial_rect_t {
  fixed_t left;    // west coordinate      
  fixed_t top;   // north coordinate
  fixed_t right;    // east coordinate
  fixed_t bottom;   // south coordinates
  } spatial_rect_t;
/**
 * @brief Return true if the pt is within the rect
 * @param rect  Rectangle to test
 * @param pt    Point to sample
 * @return true if rect contains pt
*/
static inline bool spatial_rect_contains(const spatial_rect_t* rect, const latlng_t* pt)
  {
  return pt->lat <= rect->top && pt->lat >= rect->bottom &&
    pt->lng >= rect->left && pt->lng <= rect->right;
  }


#define pi 3.1415926535897932384626433832795f

static inline float degrees_to_radians(float degrees)
  {
  return degrees * (pi / 180);
  }

static inline float radians_to_degrees(float radians)
  {
  float retval = radians / (pi / 180);
  return retval;
  }

#define rad_deg (pi / 180)
#define rad_45 (pi / 4)
#define rad_90 (pi / 2)
#define rad_180 pi;
#define rad_270 (pi * 1.5)
#define rad_360 (pi * 2)

#define feet_per_meter 3.2808398950131233595800524934383f

static inline float meters_to_feet(float meters)
  {
  return meters * feet_per_meter;
  }

static inline float feet_to_meters(float feet)
  {
  return feet / feet_per_meter;
  }

static inline float feet_per_minute_to_meters_per_second(float n)
  {
  return (n / (60 * feet_per_meter));
  }

static inline float meters_per_second_to_feet_per_minute(float n)
  {
  return n * 60 * feet_per_meter;
  }

//#define meters_per_nm (6076.11549 / feet_per_meter)
#define meters_per_nm (1852.000001352f)

static inline float nm_to_meters(float nm)
  {
  return nm * meters_per_nm;
  }

static inline float meters_to_nm(float meters)
  {
  return meters / meters_per_nm;
  }

static inline float knots_to_meters_per_second(float knots)
  {
  return nm_to_meters(knots) / 3600;
  }

static inline float meters_per_second_to_knots(float mps)
  {
  return meters_to_nm(mps * 3600);
  }

static inline float meters_to_degrees(float meters)
  {
  return meters_to_nm(meters) / 60;
  }

static inline float meters_to_degrees_ex(float meters, float declination, bool declination_is_radians)
  {
  if (declination == 0.0)
    return meters_to_nm(meters) / 60;

  return (meters_to_nm(meters) / 60)/ cosf(declination_is_radians ? declination : degrees_to_radians(declination));
  }

static inline float degrees_to_meters(float d)
  {
  return nm_to_meters(d) * 60;
  }

static inline float degrees_to_meters_ex(float d, float declination, bool declination_is_radians)
  {
  if (declination == 0.0)
    return nm_to_meters(d) * 60;

  return (nm_to_meters(d) * 60) * cosf(declination_is_radians ? declination : degrees_to_radians(declination));
  }

static inline float meters_to_radians(float meters)
  {
  return degrees_to_radians(meters_to_degrees(meters));
  }

static inline float meters_to_radians_ex(float meters, float declination,
                                bool decl_is_radians)
  {
  return degrees_to_radians(meters_to_degrees_ex(meters, declination, decl_is_radians));
  }

static inline float radians_to_meters(float d)
  {
  return degrees_to_meters(radians_to_degrees(d));
  }

static inline float radians_to_meters_ex(float d, float declination,
                                bool decl_is_radians)
  {
  return degrees_to_meters_ex(radians_to_degrees(d), declination,
                           decl_is_radians);
  }

// this will calculate the horizontal distance between two points.

static inline float lng_separation(const lla_t *p1, const lla_t *p2)
  {
  // first correct for the spherical conversion
  float h1 = cosf(p1->lat) * radians_to_degrees(p1->lng);
  float h2 = cosf(p2->lat) * radians_to_degrees(p2->lng);

  if (h1 > h2)
    return degrees_to_meters(h1) - degrees_to_meters(h2);

  return degrees_to_meters(h2) - degrees_to_meters(h1);
  }

// this will calculate the horizontal distance between two points.

static inline float lat_separation(const lla_t *p1, const lla_t *p2)
  {
  if (p1->lat > p2->lat)
    return degrees_to_meters(radians_to_degrees(p1->lat))
    - degrees_to_meters(radians_to_degrees(p2->lat));

  return degrees_to_meters(radians_to_degrees(p1->lat))
    - degrees_to_meters(radians_to_degrees(p2->lat));
  }

extern result_t rotate_spatial_point(const spatial_point_t* center, int16_t angle, spatial_point_t* pt);


// this method will take two ltln coordinates_t and return the number of meters between
// them.

static inline float distance(const lla_t *p1, const lla_t *p2)
  {
  float x = lng_separation(p1, p2);
  float y = lat_separation(p1, p2);

  return sqrtf(x * x + y * y);
  }

static inline const spatial_point_t* create_point32(int32_t x, int32_t y, spatial_point_t* pt)
  {
  pt->latlng.lat = int_to_fixed(x);
  pt->latlng.lng = int_to_fixed(y);

  return pt;
  }

static inline bool point32_equal(const spatial_point_t* p1, const spatial_point_t* p2)
  {
  return p1->latlng.lat == p2->latlng.lat && p1->latlng.lng == p1->latlng.lng;
  }
/**
 * @brief copy a point32
 * @param src point to copy
 * @param dst destination
 * @return destination
*/
static inline const spatial_point_t* spatial_point_copy(const spatial_point_t* src, spatial_point_t* dst)
  {
  dst->latlng.lat = src->latlng.lat;
  dst->latlng.lng = src->latlng.lng;
  dst->elevation = src->elevation;

  return dst;
  }

type_vector_t(spatial_point);

typedef spatial_points_t polyline_t;
type_vector_t(polyline);

static inline void incarnate_spatial_points(spatial_points_t *vec)
  {
  memset(vec, 0, sizeof(spatial_points_t));
  }

// utility definitions
#define meters_per_nm_fixed float_to_fixed(meters_per_nm)


#ifdef __cplusplus
  }
#endif

#endif
