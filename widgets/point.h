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
#ifndef __point_h__
#define __point_h__

#include "canfly.h"
#include "extent.h"

namespace kotuku {

class point_t : public ::point_t
  {
public:
  point_t();
  point_t(const point_t &p);
  point_t(const gdi_dim_t &x, const gdi_dim_t &y);
  const point_t &operator=(const point_t &p);

  const point_t &operator+=(const extent_t &e);
  const point_t &operator-=(const extent_t &e);
  const point_t &operator+=(const std::pair<gdi_dim_t, gdi_dim_t> &p);
  const point_t &operator-=(const std::pair<gdi_dim_t, gdi_dim_t> &p);
  const point_t &operator*=(const std::pair<gdi_dim_t, gdi_dim_t> &p);
  const point_t &operator/=(const std::pair<gdi_dim_t, gdi_dim_t> &p);
  };


inline bool operator==(const point_t &p1, const point_t &p2)
  {
  return p1.x == p2.x && p1.y == p2.y;
  }


inline bool operator<(const point_t &p1, const point_t &p2)
  {
  return p1.x < p2.x || p1.y < p2.y;
  }


inline const point_t &point_t::operator+=(const extent_t &e)
  {
  x += e.cx;
  y += e.cy;
  return *this;
  }


inline const point_t &point_t::operator-=(const extent_t &e)
  {
  x -= e.cx;
  y -= e.cy;
  return *this;
  }


inline const point_t &point_t::operator+=(const std::pair<gdi_dim_t, gdi_dim_t> &p)
  {
  x += p.first;
  y += p.second;
  return *this;
  }


inline const point_t &point_t::operator-=(const std::pair<gdi_dim_t, gdi_dim_t> &p)
  {
  x -= p.first;
  y -= p.second;
  return *this;
  }


inline const point_t &point_t::operator*=(const std::pair<gdi_dim_t, gdi_dim_t> &p)
  {
  x *= p.first;
  y *= p.second;
  return *this;
  }


inline const point_t &point_t::operator/=(const std::pair<gdi_dim_t, gdi_dim_t> &p)
  {
  x /= p.first;
  y /= p.second;
  return *this;
  }


inline point_t::point_t()
  {
  }


inline point_t::point_t(const point_t &p)
  {
  x = p.x;
  y = p.y;
  }


inline point_t::point_t(const gdi_dim_t &_x, const gdi_dim_t &_y)
  {
  x = _x;
  y = _y;
  }


inline const point_t &point_t::operator=(const point_t &p)
  {
  x = p.x;
  y = p.y;
  return *this;
  }


inline extent_t operator-(const point_t &p1,
                                   const point_t &p2)
  {
  return extent_t(p2.x - p1.x, p2.y - p1.y);
  }


inline point_t operator+(const point_t &p,
                                  const extent_t &e)
  {
  return point_t(p.x + e.cx, p.y + e.cy);
  }


inline point_t operator-(const point_t &p,
                                  const extent_t &e)
  {
  return point_t(p.x - e.cx, p.y - e.cy);
  }

// utility function to rotate a point_t given an angle in radians
extern void rotate_point(const point_t &pt, point_t &, double angle);

  };
  
#endif
