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
#ifndef __extent_h__
#define __extent_h__

#include "canfly.h"
#include <utility>

namespace kotuku {
// return the operating system type as string
///////////////////////////////////////////////////////////////////////////////
// basic geometric types rect, point, extent_t

class extent_t : public ::extent_t
  {
public:
  extent_t();
  extent_t(const extent_t &s);
  extent_t(gdi_dim_t _dx, gdi_dim_t _dy);
  extent_t &operator=(const extent_t &s);

  const extent_t &operator+=(const extent_t &e);
  const extent_t &operator-=(const extent_t &e);

  const extent_t &operator+=(const std::pair<gdi_dim_t, gdi_dim_t> &p);
  const extent_t &operator-=(const std::pair<gdi_dim_t, gdi_dim_t> &p);
  const extent_t &operator*=(const std::pair<gdi_dim_t, gdi_dim_t> &p);
  const extent_t &operator/=(const std::pair<gdi_dim_t, gdi_dim_t> &p);
  };


inline extent_t::extent_t()
  {
  }

inline extent_t::extent_t(const extent_t &s) 
  {
  cx = s.cx;
  cy = s.cy;
  }


inline extent_t &extent_t::operator=(const extent_t &s)
  {
  cx = s.cx;
  cy = s.cy;

  return *this;
  }


inline extent_t::extent_t(gdi_dim_t _dx, gdi_dim_t _dy)
  {
  cx = _dx;
  cy = _dy;
  }


inline extent_t operator+(const extent_t &e1,
                                   const extent_t &e2)
  {
  return extent_t(e1.cx + e2.cx, e1.cy + e2.cy);
  }


inline extent_t operator-(const extent_t &e1,
                                   const extent_t &e2)
  {
  return extent_t(e1.cx - e2.cx, e1.cy - e2.cy);
  }


inline const extent_t &extent_t::operator+=(const std::pair<gdi_dim_t, gdi_dim_t> &p)
  {
  cx += p.first;
  cy += p.second;
  return *this;
  }


inline const extent_t &extent_t::operator-=(const std::pair<gdi_dim_t, gdi_dim_t> &p)
  {
  cx -= p.first;
  cy -= p.second;
  return *this;
  }


inline const extent_t &extent_t::operator*=(const std::pair<gdi_dim_t, gdi_dim_t> &p)
  {
  cx *= p.first;
  cy *= p.second;
  return *this;
  }


inline const extent_t &extent_t::operator/=(const std::pair<gdi_dim_t, gdi_dim_t> &p)
  {
  cx /= p.first;
  cy /= p.second;
  return *this;
  }


inline extent_t operator+=(const extent_t &e, const std::pair<gdi_dim_t, gdi_dim_t> &p)
  {
  return extent_t(e.cx + p.first, e.cy + p.second);
  }


inline extent_t operator-=(const extent_t &e, const std::pair<gdi_dim_t, gdi_dim_t> &p)
  {
  return extent_t(e.cx - p.first, e.cy - p.second);
  }


inline extent_t operator*=(const extent_t &e, const std::pair<gdi_dim_t, gdi_dim_t> &p)
  {
  return extent_t(e.cx * p.first, e.cy * p.second);
  }


inline extent_t operator/=(const extent_t &e, const std::pair<gdi_dim_t, gdi_dim_t> &p)
  {
  return extent_t(e.cx / p.first, e.cy / p.second);
  }


inline const extent_t &extent_t::operator+=(const extent_t &e)
  {
  cx += e.cx;
  cy += e.cy;
  return *this;
  }


inline const extent_t &extent_t::operator-=(const extent_t &e)
  {
  cx -= e.cx;
  cy -= e.cy;
  return *this;
  }

  };

#endif
