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
#ifndef __attitude_window_h__
#define __attitude_window_h__

#include "widget.h"

namespace kotuku {
class attitude_window_t : public widget_t
  {
public:
  attitude_window_t(widget_t &, const char *section);

  short critical_aoa() const;
  void critical_aoa(short);

  short approach_aoa() const;
  void approach_aoa(short);

  short climb_aoa() const;
  void climb_aoa(short);

  short cruise_aoa() const;
  void cruise_aoa(short);
private:
  virtual bool ev_msg(const msg_t &);
  canvas_t _image_canvas;

  void update_window();

  double _pitch;
  short _pitch_degrees;
  double _roll;
  short _roll_degrees;
  double _abs_roll;
  short _yaw_degrees;

  short _aoa_degrees;

  double _aoa_pixels_per_degree;
  double _aoa_degrees_per_mark;

  long _glideslope;
  long _localizer;
  bool _glideslope_aquired;
  bool _localizer_aquired;

  short _critical_aoa;
  short _approach_aoa;
  short _climb_aoa;
  short _cruise_aoa;
  short _yaw_max;
  bool _show_aoa;
  bool _show_glideslope;
  };

inline short attitude_window_t::critical_aoa() const
  {
  return _critical_aoa;
  }

inline void attitude_window_t::critical_aoa(short value)
  {
  _critical_aoa = value;
  }

inline short attitude_window_t::approach_aoa() const
  {
  return _approach_aoa;
  }

inline void attitude_window_t::approach_aoa(short value)
  {
  _approach_aoa = value;
  }

inline short attitude_window_t::climb_aoa() const
  {
  return _climb_aoa;
  }

inline void attitude_window_t::climb_aoa(short value)
  {
  _climb_aoa = value;
  }

inline short attitude_window_t::cruise_aoa() const
  {
  return _cruise_aoa;
  }

inline void attitude_window_t::cruise_aoa(short value)
  {
  _cruise_aoa = value;
  }
  };

#endif
