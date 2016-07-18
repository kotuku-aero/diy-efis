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
#ifndef __rect_h__
#define __rect_h__

#include "stddef.h"
#include "point.h"

#include <algorithm>

namespace kotuku {
class rect_t : public ::rect_t
  {
public:
  rect_t();
  rect_t(const rect_t &r);
  rect_t(const gdi_dim_t &_left, const gdi_dim_t &_top, const gdi_dim_t &_right, const gdi_dim_t &_bottom);
  rect_t(const point_t &p, const extent_t &s);
  rect_t(const point_t &p1, const point_t &p2);
  const rect_t &operator=(const rect_t &r);
  bool operator!() const;
  operator bool() const;
  int width() const;
  int height() const;
  extent_t extents() const;
  point_t top_left() const;
  point_t bottom_left() const;
  point_t top_right() const;
  point_t bottom_right() const;

  const rect_t &operator=(const point_t &_pt);
  const rect_t &operator=(const extent_t &_ex);
  const rect_t &operator+=(const extent_t &_ex);
  const rect_t &operator-=(const extent_t &_ex);

  const rect_t &operator|=(const rect_t &r);
  const rect_t &operator&=(const rect_t &r);

  void normalize();
  };

inline void rect_t::normalize()
  {
  if (left > right)
    std::swap(left, right);

  if (top > bottom)
    std::swap(top, bottom);
  }


inline rect_t::rect_t()
  {
  }


inline rect_t::rect_t(const rect_t &r)
  {
  left = r.left;
  right = r.right;
  top = r.top;
  bottom = r.bottom;
  }


inline rect_t::rect_t(const gdi_dim_t &_left, const gdi_dim_t &_top,
                                     const gdi_dim_t &_right, const gdi_dim_t &_bottom)
  {
  left = _left;
  right = _right;
  top = _top;
  bottom = _bottom;
  }


inline rect_t::rect_t(const point_t &p,const extent_t &s)
  {
  left = p.x;
  top = p.y;
  right = p.x + s.dx;
  bottom = p.y + s.dy;
  }


inline rect_t::rect_t(const point_t &p1, const point_t &p2)
  {
  left = p1.x;
  top = p1.y;
  right = p2.x;
  bottom = p2.y;
  }


inline const rect_t &rect_t::operator=(const rect_t &r)
  {
  left = r.left;
  top = r.top;
  right = r.right;
  bottom = r.bottom;
  return *this;
  }


inline const rect_t &rect_t::operator&=(const rect_t &r)
  {
  left = std::max(left, r.left);
  top = std::max(top, r.top);
  right = std::min(right, r.right);
  bottom = std::min(bottom, r.bottom);
  if (left >= right || top >= bottom)
    left = right = top = bottom = 0;

  return *this;
  }


inline const rect_t &rect_t::operator|=(const rect_t &r)
  {
  left = std::min(left, r.left);
  top = std::min(top, r.top);
  right = std::max(right, r.right);
  bottom = std::max(bottom, r.bottom);

  return *this;
  }


inline bool rect_t::operator!() const
  {
  return left == 0 && top == 0 && right == 0 && bottom == 0;
  }


inline rect_t::operator bool() const
  {
  return !operator!();
  }


inline int rect_t::width() const
  {
  return right - left;
  }


inline int rect_t::height() const
  {
  return bottom - top;
  }


inline extent_t rect_t::extents() const
  {
  return extent_t(width(), height());
  }


inline point_t rect_t::top_left() const
  {
  return point_t(left, top);
  }


inline point_t rect_t::bottom_left() const
  {
  return point_t(left, bottom);
  }


inline point_t rect_t::top_right() const
  {
  return point_t(right, top);
  }


inline point_t rect_t::bottom_right() const
  {
  return point_t(right, bottom);
  }


inline const rect_t &rect_t::operator=(const point_t &_pt)
  {
  left = _pt.x;
  top = _pt.y;
  normalize();
  return *this;
  }


inline const rect_t &rect_t::operator=(const extent_t &_ex)
  {
  right += _ex.dx;
  bottom += _ex.dy;
  normalize();
  return *this;
  }


inline const rect_t &rect_t::operator+=(const extent_t &_ex)
  {
  right += _ex.dx;
  bottom += _ex.dy;
  normalize();
  return *this;
  }


inline const rect_t &rect_t::operator-=(const extent_t &_ex)
  {
  right -= _ex.dx;
  bottom -= _ex.dy;
  normalize();
  return *this;
  }

// return true if the point is within the rectangle
inline bool operator&&(const rect_t &r, const point_t &p)
  {
  return r.left <= p.x && r.right > p.x && r.top <= p.y && r.bottom > p.y;
  }

// return true is the rectangles overlap

inline bool operator||(const rect_t &r1, const rect_t &r2)
  {
  return r1.left <= r2.right && r1.right >= r2.left && r1.top <= r1.bottom
    && r1.bottom >= r1.top;
  }


inline rect_t operator+(const rect_t &r,
                                 const extent_t &extents)
  {
  return rect_t(r.top_left(), r.extents() + extents);
  }


inline rect_t operator-(const rect_t &r,
                                 const extent_t &extents)
  {
  return rect_t(r.top_left(), r.extents() - extents);
  }


inline rect_t operator+(const rect_t &r,
                                 const point_t &pt)
  {
  return rect_t(point_t(r.left + pt.x, r.top + pt.y),
                         r.extents());
  }


inline rect_t operator-(const rect_t &r,
                                 const point_t &pt)
  {
  return rect_t(point_t(r.left - pt.x, r.top - pt.y),
                         r.extents());
  }
  };

#endif
