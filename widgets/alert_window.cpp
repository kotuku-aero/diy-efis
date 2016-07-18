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
#include "pfd_application.h"
#include "alert_window.h"
#include "bitmaps.h"

static const kotuku::rect_t battery_rect(2, 2, 26, 59);
static const kotuku::rect_t ahrs_rect(2, 62, 26, 119);
static const kotuku::rect_t edu_rect(2, 122, 26, 179);
static const kotuku::rect_t gps_rect(2, 182, 26, 239);

kotuku::alert_window_t::alert_window_t(widget_t &parent, const char *section)
: widget_t(parent, section),
  _background_canvas(*this, window_rect().extents()),
  _battery_alert(alert_off),
  _edu_alert(alert_on),
  _imu_alert(alert_on),
  _nav_alert(alert_on),
  _timer(0),
  _battery_on_bitmap(parent, battery_on_bitmap),
  _battery_on_dim_bitmap(parent, battery_on_dim_bitmap),
  _battery_off_bitmap(parent, battery_off_bitmap),
  _edu_on_bitmap(parent, edu_fail_bitmap),
  _edu_on_dim_bitmap(parent, edu_fail_dim_bitmap),
  _edu_off_bitmap(parent, edu_ok_bitmap),
  _imu_on_bitmap(parent, imu_fail_bitmap),
  _imu_on_dim_bitmap(parent, imu_fail_dim_bitmap),
  _imu_off_bitmap(parent, imu_ok_bitmap),
  _nav_on_bitmap(parent, nav_fail_bitmap),
  _nav_on_dim_bitmap(parent, nav_fail_dim_bitmap),
  _nav_off_bitmap(parent, nav_ok_bitmap)
  {
  if(draw_border())
    _background_canvas.round_rect(window_rect(), extent_t(12, 12));

  _timer_delay = 20; //(uint16_t)get_config_value("HighlightDelay", 20);
  subscribe(id_timer);
  subscribe(id_buttonpress);
  subscribe(id_nav_valid);
  subscribe(id_imu_valid);
  subscribe(id_aux_battery);
  subscribe(id_edu_valid);
  }

bool kotuku::alert_window_t::ev_msg(const msg_t &data)
  {
  date_time_t now;

	bool changed = false;
	switch(data.message_id())
		{
    case id_timer :
      {
      uint16_t new_timer = _timer + data.value<uint16_t>();
      if(new_timer < _timer)
        {
        // wrapped around
        _timer = 65535 - _timer;
        }
      if((new_timer - _timer) >= _timer_delay)
        {
        if(_battery_alert == alert_on)
          _battery_alert = alert_on_dim;
        else if(_battery_alert == alert_on_dim)
          _battery_alert = alert_on;

        if(_edu_alert == alert_on)
          _edu_alert = alert_on_dim;
        else if(_edu_alert == alert_on_dim)
          _edu_alert = alert_on;

        if(_imu_alert == alert_on)
          _imu_alert = alert_on_dim;
        else if(_imu_alert == alert_on_dim)
          _imu_alert = alert_on;

        if(_nav_alert == alert_on)
          _nav_alert = alert_on_dim;
        else if(_nav_alert == alert_on_dim)
          _nav_alert = alert_on;

        _timer = new_timer;
        changed = true;
        }

      interval_t timer_elapsed = now - _last_edu_heartbeat;
      // check each timer
      if(timer_elapsed >= heartbeat_timeout && _edu_alert == alert_off)
        _edu_alert = alert_on;

      timer_elapsed = now - _last_nav_heartbeat;
      if(timer_elapsed >= heartbeat_timeout && _nav_alert == alert_off)
        _nav_alert = alert_on;

      timer_elapsed = now - _last_imu_heartbeat;
      if(timer_elapsed > heartbeat_timeout && _imu_alert == alert_off)
        _imu_alert = alert_on;
      }
      break;
    case id_buttonpress :
      if(_battery_alert != alert_off)
        _battery_alert = alert_acknowledged;

      if(_edu_alert != alert_off)
        _edu_alert = alert_acknowledged;

      if(_imu_alert != alert_off)
        _imu_alert = alert_acknowledged;

      if(_nav_alert != alert_off)
        _nav_alert = alert_acknowledged;

      changed = true;
      break;
    case id_nav_valid :
      if(_nav_alert != alert_off)
        changed = true;

      _nav_alert = alert_off;
      _last_nav_heartbeat = now;
      break;
    case id_imu_valid :
      if(_imu_alert != alert_off)
        changed = true;

      _imu_alert = alert_off;
      _last_imu_heartbeat = now;
      break;
    case id_aux_battery :
      break;
    case id_edu_valid :
      if(_edu_alert != alert_off)
        changed = true;

      _edu_alert = alert_off;
      _last_edu_heartbeat = now;
      break;
		default :
			return false;
		}

	if(changed)
	  invalidate();

	return true;
  }

void kotuku::alert_window_t::update_window()
  {
	rect_t window_size(0, 0, window_rect().width(), window_rect().height());
  clipping_rectangle(window_size);

  bit_blt(window_size, _background_canvas, point_t(0, 0), rop_srccopy);

  switch(_battery_alert)
    {
    case alert_off :
      bit_blt(battery_rect, _battery_off_bitmap, point_t(0, 0), rop_srccopy);
      break;
    case alert_acknowledged :
    case alert_on :
      bit_blt(battery_rect, _battery_on_bitmap, point_t(0, 0), rop_srccopy);
      break;
    case alert_on_dim :
      bit_blt(battery_rect, _battery_on_dim_bitmap, point_t(0, 0), rop_srccopy);
      break;
    }

  switch(_edu_alert)
    {
    case alert_off :
      bit_blt(edu_rect, _edu_off_bitmap, point_t(0, 0), rop_srccopy);
      break;
    case alert_acknowledged :
    case alert_on :
      bit_blt(edu_rect, _edu_on_bitmap, point_t(0, 0), rop_srccopy);
      break;
    case alert_on_dim :
      bit_blt(edu_rect, _edu_on_dim_bitmap, point_t(0, 0), rop_srccopy);
      break;
    }

  switch(_imu_alert)
    {
    case alert_off :
      bit_blt(ahrs_rect, _imu_off_bitmap, point_t(0, 0), rop_srccopy);
      break;
    case alert_acknowledged :
    case alert_on :
      bit_blt(ahrs_rect, _imu_on_bitmap, point_t(0, 0), rop_srccopy);
      break;
    case alert_on_dim :
      bit_blt(ahrs_rect, _imu_on_dim_bitmap, point_t(0, 0), rop_srccopy);
      break;
    }

  switch(_nav_alert)
    {
    case alert_off :
      bit_blt(gps_rect, _nav_off_bitmap, point_t(0, 0), rop_srccopy);
      break;
    case alert_acknowledged :
    case alert_on :
      bit_blt(gps_rect, _nav_on_bitmap, point_t(0, 0), rop_srccopy);
      break;
    case alert_on_dim :
      bit_blt(gps_rect, _nav_on_dim_bitmap, point_t(0, 0), rop_srccopy);
      break;
    }
  }
