#ifndef __ahrs_h__
#define __ahrs_h__
#include <xc.h>

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "../../libs/neutron/neutron.h"
#include "../../libs/atom/analog_devices.h"
#include "../../libs/atom/input_capture_devices.h"
#include "../../libs/atom/uart_device.h"
#include "../../libs/atom/microkernel.h"
#include "../../libs/atom/eeprom.h"
#include "../../libs/atom/i2c.h"

#define strtof(a, b) (float) strtod(a, b)
   
extern result_t gps_init(bool init_eeprom);
extern result_t imu_init(bool init_eeprom);

typedef struct _date_t {
  uint8_t second;
  uint8_t minute;
  uint8_t hour;
  uint8_t day;
  uint8_t month;
  uint16_t year;
  } date_time_t;


typedef struct {
  // these are read from the world magnetic model
  xyz_t mag_flux;
  int16_t magnetic_variation; // computed horizontal magnetic variation (degrees)
  
  // stores gps position as lla (lat, lng, alt)
  lla_t gps_position;
  float gps_ground_speed;           // in m/sec
  bool gps_ground_speed_valid;
  float gps_track;
  bool gps_track_valid;
  date_time_t date;
  float wind_speed;
  float wind_direction;
  bool gps_position_valid;
  
  // computed from the barometric sensors
  float true_airspeed;
  float indicated_airspeed;
  // barometeric pressure.
  float baro_corrected_altitude;
  float pressure_altitude;
  float vertical_speed;
  
  // signalled when a gps update is received.
  semaphore_p gps_position_updated;
  } ahrs_data_t;

extern ahrs_data_t ahrs_state;

// this is the external form of a lat/long tuple.
// always in radians & meters.

// return true if the point is within the coordinates_t
// described by the vector

static inline bool point_in_polygon(const lla_t *poly, size_t len, const lla_t *pt)
  {
  size_t i, j;
  bool c = false;
  for (i = 0, j = 1; i < len; j = i++)
    {
    if ((((poly[i].lat <= pt->lat) && (pt->lat < poly[j].lat))
         || ((poly[j].lat <= pt->lat) && (pt->lat < poly[i].lat)))
        && (pt->lng
            < (poly[j].lng - poly[i].lng) * (pt->lat - poly[i].lat)
            / (poly[j].lat - poly[i].lat) + poly[i].lng))
      c = !c;
    }
  return c;
  }

static inline void intersect_type(double *r, double *s, const lla_t *a,
                           const lla_t *b, const lla_t *c, const lla_t *d)
  {
  //    Let A,B,C,D be 2-space position vectors.  Then the directed line
  //    segments AB & CD are given by:
  //
  //        AB=A+r(B-A), r in [0,1]
  //        CD=C+s(D-C), s in [0,1]
  //
  //    If AB & CD intersect, then
  //
  //        A+r(B-A)=C+s(D-C), or
  //
  //        XA+r(XB-XA)=XC+s(XD-XC)
  //        YA+r(YB-YA)=YC+s(YD-YC)  for some r,s in [0,1]
  //
  //    Solving the above for r and s yields
  //
  //            (YA-YC)(XD-XC)-(XA-XC)(YD-YC)
  //        r = -----------------------------  (eqn 1)
  //            (XB-XA)(YD-YC)-(YB-YA)(XD-XC)
  //
  //            (YA-YC)(XB-XA)-(XA-XC)(YB-YA)
  //        s = -----------------------------  (eqn 2)
  //            (XB-XA)(YD-YC)-(YB-YA)(XD-XC)
  //
  //    Let I be the position vector of the intersection point, then
  //
  //        I=A+r(B-A) or
  //
  //        XI=XA+r(XB-XA)
  //        YI=YA+r(YB-YA)
  //
  //    By examining the values of r & s, you can also determine some
  //    other limiting conditions:
  //
  //        If 0<=r<=1 & 0<=s<=1, intersection exists
  //            r<0 or r>1 or s<0 or s>1 line segments do not intersect
  //
  //        If the denominator in eqn 1 is zero, AB & CD are parallel
  //        If the numerator in eqn 1 is also zero, AB & CD are coincident
  //
  //    If the intersection point of the 2 lines are needed (lines in this
  //    context mean infinite lines) regardless whether the two line
  //    segments intersect, then
  //
  //        If r>1, I is located on extension of AB
  //        If r<0, I is located on extension of BA
  //        If s>1, I is located on extension of CD
  //        If s<0, I is located on extension of DC
  //
  //    Also note that the denominators of eqn 1 & 2 are identical.

  *r = (((a->lat - c->lat) * (d->lng - c->lng))
       - ((a->lng - c->lng) * (d->lat - c->lat)))
    / (((b->lng - a->lng) * (d->lat - c->lat))
       - ((b->lat - a->lat) * (d->lng - c->lng)));
  *s = (((a->lat - c->lat) * (b->lng - a->lng))
       - ((a->lng - c->lng) * (b->lat - a->lat)))
    / (((b->lng - a->lng) * (d->lat - c->lat))
       - ((b->lat - a->lat) * (d->lng - c->lng)));
  }

static inline bool intersects(const lla_t *p1, const lla_t *p2, const lla_t *p3,
                       const lla_t *p4)
  {
  double r;
  double s;
  intersect_type(&r, &s, p1, p2, p3, p4);

  return (r >= 0 && r <= 1) && (s >= 0 && s <= 1);
  }

#define pi 3.1415926535897932384626433832795

static inline double degrees_to_radians(double degrees)
  {
  return degrees * (pi / 180);
  }

static inline double radians_to_degrees(double radians)
  {
  double retval = radians / (pi / 180);
  return retval;
  }

#define rad_deg (pi / 180)
#define rad_45 (pi / 4)
#define rad_90 (pi / 2)
#define rad_180 pi;
#define rad_270 (pi * 1.5)
#define rad_360 (pi * 2)

#define feet_per_meter 3.2808398950131233595800524934383

static inline double meters_to_feet(double meters)
  {
  return meters * feet_per_meter;
  }

static inline double feet_to_meters(double feet)
  {
  return feet / feet_per_meter;
  }

static inline double feet_per_minute_to_meters_per_second(double n)
  {
  return (n / (60 * feet_per_meter));
  }

static inline double meters_per_second_to_feet_per_minute(double n)
  {
  return n * 60 * feet_per_meter;
  }

#define meters_per_nm (6076.11549 / feet_per_meter);

static inline double nm_to_meters(double nm)
  {
  return nm * meters_per_nm;
  }

static inline double meters_to_nm(double meters)
  {
  return meters / meters_per_nm;
  }

static inline double knots_to_meters_per_second(double knots)
  {
  return nm_to_meters(knots) / 3600;
  }

static inline double meters_per_second_to_knots(double mps)
  {
  return meters_to_nm(mps * 3600);
  }

static inline double meters_to_degrees(double meters)
  {
  return meters_to_nm(meters) / 60;
  }

static inline double meters_to_degrees_ex(double meters, double declination,
                                bool declination_is_radians)
  {
  if (declination == 0.0)
    return meters_to_nm(meters) / 60;

  return (meters_to_nm(meters) / 60)
    / cos(
          declination_is_radians ?
          declination : degrees_to_radians(declination));
  }

static inline double degrees_to_meters(double d)
  {
  return nm_to_meters(d) * 60;
  }

static inline double degrees_to_meters_ex(double d, double declination,
                                bool declination_is_radians)
  {
  if (declination == 0.0)
    return nm_to_meters(d) * 60;

  return (nm_to_meters(d) * 60)
    * cos(
          declination_is_radians ?
          declination : degrees_to_radians(declination));
  }

static inline double meters_to_radians(double meters)
  {
  return degrees_to_radians(meters_to_degrees(meters));
  }

static inline double meters_to_radians_ex(double meters, double declination,
                                bool decl_is_radians)
  {
  return degrees_to_radians(meters_to_degrees_ex(meters, declination, decl_is_radians));
  }

static inline double radians_to_meters(double d)
  {
  return degrees_to_meters(radians_to_degrees(d));
  }

static inline double radians_to_meters_ex(double d, double declination,
                                bool decl_is_radians)
  {
  return degrees_to_meters_ex(radians_to_degrees(d), declination,
                           decl_is_radians);
  }

// this will calculate the horizontal distance between two points.

static inline double lng_separation(const lla_t *p1, const lla_t *p2)
  {
  // first correct for the spherical conversion
  double h1 = cos(p1->lat) * radians_to_degrees(p1->lng);
  double h2 = cos(p2->lat) * radians_to_degrees(p2->lng);

  if (h1 > h2)
    return degrees_to_meters(h1) - degrees_to_meters(h2);

  return degrees_to_meters(h2) - degrees_to_meters(h1);
  }

// this will calculate the horizontal distance between two points.

static inline double lat_separation(const lla_t *p1, const lla_t *p2)
  {
  if (p1->lat > p2->lat)
    return degrees_to_meters(radians_to_degrees(p1->lat))
    - degrees_to_meters(radians_to_degrees(p2->lat));

  return degrees_to_meters(radians_to_degrees(p1->lat))
    - degrees_to_meters(radians_to_degrees(p2->lat));
  }

// this method will take two ltln coordinates_t and return the number of meters between
// them.

static inline float distance(const lla_t *p1, const lla_t *p2)
  {
  double x = lng_separation(p1, p2);
  double y = lat_separation(p1, p2);

  return sqrt(x * x + y * y);
  }

#endif
