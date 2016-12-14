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
#include "annunciator_window.h"
#include "application.h"
#include "fonts.h"
#include "pens.h"
#include "application.h"

static const kotuku::point_t clock_pt(1, 10);
static const kotuku::point_t hrs_pt(1, 55);
static const kotuku::point_t qnh_pt(1, 100);
static const kotuku::point_t oat_pt(1, 145);
static const kotuku::point_t cas_pt(1, 190);

static const char *hours_value = "TTS";
static const char *qnh_value = "QNH";

kotuku::annunciator_window_t::annunciator_window_t(widget_t &parent, const char *section)
: widget_t(parent, section),
  _background_canvas(window_rect().extents()),
  _oat(0),
  _cas(0),
  _clock(720),
  _hours(0),
  _qnh(1013)
  {
	rect_t r(_background_canvas.window_rect());
  _background_canvas.clipping_rectangle(r);

  _background_canvas.fill_rect(r, color_black);

	if(draw_border())
	  _background_canvas.round_rect(r, extent_t(12, 12));

  draw_annunciator_background(clock_pt, "UTC");
  draw_annunciator_background(hrs_pt, "Hrs");
  draw_annunciator_background(qnh_pt, "QNH");
  draw_annunciator_background(oat_pt, "OAT");
  draw_annunciator_background(cas_pt, "CAS");
  _background_canvas.clipping_rectangle(r);

  subscribe(id_qnh);
  subscribe(id_air_time);
  subscribe(id_outside_air_temperature);
  subscribe(id_true_airspeed);
  subscribe(id_def_utc);
  }

bool kotuku::annunciator_window_t::ev_msg(const msg_t &data)
	{
	bool changed = false;

  switch(data.message_id())
    {
    case id_def_utc :
      {
      short minutes;
      minutes = data.msg_data0() * 60;
      minutes += data.msg_data1();
      changed = assign_msg(minutes, _clock);
      }
      break;
    case id_qnh :
      changed = assign_msg(data.value<short>(), _qnh);
      break;
    case id_true_airspeed :
      changed = assign_msg(data.value<short>(), _cas);
      break;
    case id_outside_air_temperature :
      changed = assign_msg(data.value<short>(), _oat);
      break;
    case id_air_time :
      changed = assign_msg(data.value<uint32_t>(), _hours);
    }

	if(changed)
	  invalidate();

	return true;
	}

uint32_t kotuku::annunciator_window_t::hours() const
  {
  return _hours;
  }

short kotuku::annunciator_window_t::qnh() const
  {
  return _qnh;
  }

void kotuku::annunciator_window_t::update_window()
  {
	rect_t window_size(0, 0, window_rect().width(), window_rect().height());
	clipping_rectangle(window_size);

  bit_blt(window_size, _background_canvas, point_t(0, 0));

  char msg[10];

  sprintf(msg, "%02.2d:%02.2d", _clock / 60, _clock % 60);
  draw_annunciator_detail(clock_pt, msg, strlen(msg));

  double hrs = ((double)hours()) / 100;

  sprintf(msg, "%06.1f", hrs);
  draw_annunciator_detail(hrs_pt, msg, strlen(msg));

  sprintf(msg, "%d", qnh());
  draw_annunciator_detail(qnh_pt, msg, strlen(msg));

  sprintf(msg, "%d", _oat);
  draw_annunciator_detail(oat_pt, msg, strlen(msg));

  sprintf(msg, "%d", _cas);
  draw_annunciator_detail(cas_pt, msg, strlen(msg));
  }

void kotuku::annunciator_window_t::draw_annunciator_background(const point_t &pt, const char *label)
{
  // calculate the size of the label
  extent_t display_area = extent_t(75, 50);
  _background_canvas.clipping_rectangle(rect_t(pt, display_area));
  _background_canvas.font(&arial_9_font);
  size_t text_len = strlen(label);
  extent_t label_size = _background_canvas.text_extent(label, text_len);

  _background_canvas.background_color(color_gray);

  int width = display_area.cx;
  int text_start = pt.x + width -(label_size.cx + 3);
  int right = pt.x + width;

  _background_canvas.fill_rect(rect_t(pt.x, pt.y+1, right, pt.y + 4), color_gray);
  _background_canvas.fill_rect(rect_t(pt.x, pt.y+4, pt.x + 3, pt.y + 28), color_gray);
  _background_canvas.fill_rect(rect_t(right -3, pt.y+4, right, pt.y + 45), color_gray);
  _background_canvas.fill_rect(rect_t(pt.x, pt.y+28, right, pt.y + 31), color_gray);
  _background_canvas.fill_rect(rect_t(text_start - 6, pt.y + 31, right, pt.y + 45), color_gray);

  _background_canvas.fill_rect(rect_t(pt.x + 3, pt.y + 4, right -3, pt.y + 28), color_black);

  _background_canvas.background_color(color_hollow);

  point_t text_origin(text_start, pt.y + 30);
  _background_canvas.clipping_rectangle(rect_t(text_origin, label_size));
  _background_canvas.pen(&white_pen);
  _background_canvas.draw_text(label, text_len, text_origin);
}

void kotuku::annunciator_window_t::draw_annunciator_detail(const point_t &pt, const char *value, size_t len)
  {
  background_color(color_hollow);
  text_color(color_lightblue);
  font(&arial_15_font);

  extent_t text_size = text_extent(value, len);

  // center of the text is 37, 16
  point_t origin(pt.x + 37 -(text_size.cx >> 1), pt.y + 16 -(text_size.cy >> 1));

  draw_text(value, len, origin);
  }
