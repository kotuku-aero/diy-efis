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
#ifndef __spatial_h__
#define __spatial_h__

#include "canfly.h"

#include <vector>
#include <math.h>

// this is the external form of a lat/long tuple.
// always in radians & meters.

class ltln_t
  {
public:
  double lat;
  double lng;

  ltln_t()
    {
    }

  ltln_t(double lt, double ln) :
  lat(lt), lng(ln)
    {
    }

  ltln_t(const ltln_t &o) :
  lat(o.lat), lng(o.lng)
    {
    }

  ltln_t &operator=(const ltln_t &o)
    {
    lat = o.lat;
    lng = o.lng;

    return *this;
    }

  uint16_t lat_degrees() const;
  uint16_t lat_minutes() const;
  uint16_t lat_seconds() const;

  uint16_t lng_degrees() const;
  uint16_t lng_minutes() const;
  uint16_t lng_seconds() const;
  };
typedef std::vector<ltln_t> coordinates_t;

// return true if the point is within the coordinates_t
// described by the vector

inline bool point_in_polygon(const coordinates_t &poly, const ltln_t &pt)
  {
  size_t i, j;
  bool c = false;
  for (i = 0, j = poly.size() - 1; i < poly.size(); j = i++)
    {
    if ((((poly[i].lat <= pt.lat) && (pt.lat < poly[j].lat))
         || ((poly[j].lat <= pt.lat) && (pt.lat < poly[i].lat)))
        && (pt.lng
            < (poly[j].lng - poly[i].lng) * (pt.lat - poly[i].lat)
            / (poly[j].lat - poly[i].lat) + poly[i].lng))
      c = !c;
    }
  return c;
  }

inline void intersect_type(double &r, double &s, const ltln_t &a,
                           const ltln_t &b, const ltln_t &c, const ltln_t &d)
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

  r = (((a.lat - c.lat) * (d.lng - c.lng))
       - ((a.lng - c.lng) * (d.lat - c.lat)))
    / (((b.lng - a.lng) * (d.lat - c.lat))
       - ((b.lat - a.lat) * (d.lng - c.lng)));
  s = (((a.lat - c.lat) * (b.lng - a.lng))
       - ((a.lng - c.lng) * (b.lat - a.lat)))
    / (((b.lng - a.lng) * (d.lat - c.lat))
       - ((b.lat - a.lat) * (d.lng - c.lng)));
  }

inline bool intersects(const ltln_t &p1, const ltln_t &p2, const ltln_t &p3,
                       const ltln_t &p4)
  {
  double r;
  double s;
  intersect_type(r, s, p1, p2, p3, p4);

  return (r >= 0 && r <= 1) && (s >= 0 && s <= 1);
  }

const double pi = 3.1415926535897932384626433832795;

inline double degrees_to_radians(double degrees)
  {
  return degrees * (pi / 180);
  }

inline double radians_to_degrees(double radians)
  {
  double retval = radians / (pi / 180);
  return retval;
  }

const double rad_deg = pi / 180;
const double rad_45 = pi / 4;
const double rad_90 = pi / 2; // !!sr rad_deg / 2;
const double rad_180 = pi;
const double rad_270 = pi * 1.5;
const double rad_360 = pi * 2;

const double feet_per_meter = 3.2808398950131233595800524934383;

inline double meters_to_feet(double meters)
  {
  return meters * feet_per_meter;
  }

inline double feet_to_meters(double feet)
  {
  return feet / feet_per_meter;
  }

inline double feet_per_minute_to_meters_per_second(double n)
  {
  return (n / (60 * feet_per_meter));
  }

inline double meters_per_second_to_feet_per_minute(double n)
  {
  return n * 60 * feet_per_meter;
  }

const double meters_per_nm = feet_to_meters(6076.11549);

inline double nm_to_meters(double nm)
  {
  return nm * meters_per_nm;
  }

inline double meters_to_nm(double meters)
  {
  return meters / meters_per_nm;
  }

inline double knots_to_meters_per_second(double knots)
  {
  return nm_to_meters(knots) / 3600;
  }

inline double meters_per_second_to_knots(double mps)
  {
  return meters_to_nm(mps * 3600);
  }

inline double meters_to_degrees(double meters, double declination = 0.0,
                                bool declination_is_radians = false)
  {
  if (declination == 0.0)
    return meters_to_nm(meters) / 60;

  return (meters_to_nm(meters) / 60)
    / cos(
          declination_is_radians ?
          declination : degrees_to_radians(declination));
  }

inline double degrees_to_meters(double d, double declination = 0.0,
                                bool declination_is_radians = false)
  {
  if (declination == 0.0)
    return nm_to_meters(d) * 60;

  return (nm_to_meters(d) * 60)
    * cos(
          declination_is_radians ?
          declination : degrees_to_radians(declination));
  }

inline double meters_to_radians(double meters, double declination = 0.0,
                                bool decl_is_radians = true)
  {
  return degrees_to_radians(
                            meters_to_degrees(meters, declination, decl_is_radians));
  }

inline double radians_to_meters(double d, double declination = 0.0,
                                bool decl_is_radians = true)
  {
  return degrees_to_meters(radians_to_degrees(d), declination,
                           decl_is_radians);
  }

// this will calculate the horizontal distance between two points.

inline double lng_separation(const ltln_t &p1, const ltln_t &p2)
  {
  // first correct for the spherical conversion
  double h1 = cos(p1.lat) * radians_to_degrees(p1.lng);
  double h2 = cos(p2.lat) * radians_to_degrees(p2.lng);

  if (h1 > h2)
    return degrees_to_meters(h1) - degrees_to_meters(h2);

  return degrees_to_meters(h2) - degrees_to_meters(h1);
  }

// this will calculate the horizontal distance between two points.

inline double lat_separation(const ltln_t &p1, const ltln_t &p2)
  {
  if (p1.lat > p2.lat)
    return degrees_to_meters(radians_to_degrees(p1.lat))
    - degrees_to_meters(radians_to_degrees(p2.lat));

  return degrees_to_meters(radians_to_degrees(p1.lat))
    - degrees_to_meters(radians_to_degrees(p2.lat));
  }

// this method will take two ltln coordinates_t and return the number of meters between
// them.

inline double distance(const ltln_t &p1, const ltln_t &p2)
  {
  double x = lng_separation(p1, p2);
  double y = lat_separation(p1, p2);

  return sqrt(x * x + y * y);
  }

#endif
