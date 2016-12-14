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
#include "application.h"
#include "fuel_totalizer_window.h"
#include "pens.h"
#include "fonts.h"

kotuku::fuel_totalizer_window_t::fuel_totalizer_window_t(widget_t &parent, const char *section)
: widget_t(parent, section),
  _background_canvas(window_rect().extents())
  {
	rect_t r(_background_canvas.window_rect());

  _background_canvas.background_color(color_black);
  _background_canvas.pen(&light_gray_pen);
  //_background_canvas.round_rect(_background_canvas.window_rect(), extent_t(12, 12));

  // the totalizer shows two boxes:
  //
  // ********************************
  // * Endur *      * Range *       *
  // ********************************

  gdi_dim_t mid_point = (r.width() >> 1)+ r.left;
  rect_t endurance_box(mid_point - 44, r.top + 1, mid_point -3, r.bottom -1);
  _background_canvas.rectangle(endurance_box);

  rect_t range_box(r.right - 44, r.top + 1, r.right -3, r.bottom -1);
  _background_canvas.rectangle(range_box);

  static const char *endurance_txt = "End";
  static const char *range_txt = "Rng";

	_background_canvas.font(&arial_9_font);
	_background_canvas.background_color(color_black);
	_background_canvas.text_color(color_white);

	extent_t sz = _background_canvas.text_extent(endurance_txt, size_t(-1));
	point_t pt(r.bottom_left());

  pt.x += 2;
	pt.y -= sz.cy + 2;

	_background_canvas.draw_text(endurance_txt, size_t(-1), pt);

  pt.x = mid_point;

	_background_canvas.draw_text(range_txt, size_t(-1), pt);
  }

kotuku::fuel_totalizer_window_t::~fuel_totalizer_window_t()
  {
  }


bool kotuku::fuel_totalizer_window_t::ev_msg(const msg_t &msg)
  {
  assert_valid(this);

  bool changed = false;

  if(changed)
    invalidate();

  return false;
  }

void kotuku::fuel_totalizer_window_t::update_window()
  {
	rect_t window_size(0, 0, window_rect().width(), window_rect().height());
  clipping_rectangle(window_size);

  bit_blt(window_size, _background_canvas, point_t(0, 0));
  }
