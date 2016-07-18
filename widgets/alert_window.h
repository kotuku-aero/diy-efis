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
#ifndef __alert_window_h__
#define __alert_window_h__

#include "widget.h"

namespace kotuku {
enum alert_state
  {
  alert_off,
  alert_on,
  alert_on_dim,
  alert_acknowledged
  };

class alert_window_t : public widget_t
  {
public:
  alert_window_t(widget_t &, const char *);

  static const interval_t heartbeat_timeout = interval_per_second * 2;

private:
  virtual bool ev_msg(const msg_t &data);
  void update_window();

  canvas_t _battery_on_bitmap;
  canvas_t _battery_on_dim_bitmap;
  canvas_t _battery_off_bitmap;
  canvas_t _edu_on_bitmap;
  canvas_t _edu_on_dim_bitmap;
  canvas_t _edu_off_bitmap;
  canvas_t _imu_on_bitmap;
  canvas_t _imu_on_dim_bitmap;
  canvas_t _imu_off_bitmap;
  canvas_t _nav_on_bitmap;
  canvas_t _nav_on_dim_bitmap;
  canvas_t _nav_off_bitmap;

  alert_state _battery_alert;
  alert_state _edu_alert;
  alert_state _imu_alert;
  alert_state _nav_alert;

  date_time_t _last_edu_heartbeat;
  date_time_t _last_nav_heartbeat;
  date_time_t _last_imu_heartbeat;

  bool _on_battery;

  uint16_t _timer;
  uint16_t _timer_delay;

  canvas_t _background_canvas;
  };
  };

#endif
