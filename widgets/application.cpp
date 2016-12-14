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
#include "hal.h"
#include "fonts.h"

kotuku::application_t::application_t(hal_t *the_hal)
: _hal(the_hal)
  {
  the_hal->set_can_provider(this);
  }

kotuku::application_t::~application_t()
  {

  }

result_t kotuku::application_t::initialize(const char *ini_file)
  {
  result_t result;
  if(failed(result = _hal->initialize(ini_file)))
    return result;

  int node_id = 1;

  if(succeeded(_hal->get_config_value("diy-efis", "node-id", node_id)))
    _node_id = (uint8_t)node_id;

  return s_ok;
  }
// can driver functions
result_t kotuku::application_t::publish(const can_msg_t &msg)
  {
  return _hal->publish(msg);
  }


// draw a roller gauge
void kotuku::display_roller(kotuku::canvas_t &cv,
  const kotuku::rect_t &bounds,
  double value,
  int digits,
  color_t bg_color,
  color_t fg_color)
  {
  kotuku::rect_t clip_rect = cv.clipping_rectangle(bounds);
  color_t old_bg_color = cv.background_color(bg_color);
  color_t old_text_color = cv.text_color(fg_color);

  // we need to work out the size of the roller digits first
  const font_t *old_font = cv.font(&arial_12_font);
  extent_t size_medium = cv.text_extent("00", 2);

  kotuku::point_t pt(bounds.right - (digits == 1 ? size_medium.cx >>= 1 : size_medium.cx), bounds.top);
  pt.y += (bounds.bottom - bounds.top) >> 1;
  pt.y -= size_medium.cy >> 1;

  if(digits == 1)
    value *= 10;

  // calc the interval / pixel ratio
  pt.y += gdi_dim_t((gdi_dim_t(value) % 10) *(size_medium.cy / 10.0));
  gdi_dim_t minor = (gdi_dim_t(value) / 10) * 10;

  gdi_dim_t large_value = minor / 100;
  minor %= 100;

  while(pt.y > bounds.top)
    {
    pt.y -= size_medium.cy;
    minor += 10;
    }

  char str[64];

  while(pt.y <= bounds.bottom)
    {
    // draw the text + digits first
    minor %= 100;
    if(minor < 0)
      minor += 100;

    if(minor >= 0)
      {
      if(digits == 1)
        sprintf(str, "%d", (int)minor / 10);
      else
        sprintf(str, "%02.2d", (int)minor);

      cv.draw_text(str, strlen(str), kotuku::point_t(pt.x, pt.y), bounds, eto_clipped);
      }

    minor -= 10;
    pt.y += size_medium.cy;
    }

  // now the larger value

  sprintf(str, "%d", (int)large_value);
  size_t len = strlen(str);

  // calc the size
  cv.font(&arial_15_font);
  extent_t large_size = cv.text_extent(str, len);

  pt.x -= large_size.cx;
  pt.y = bounds.top;
  pt.y += (bounds.bottom - bounds.top) >> 1;
  pt.y -= large_size.cy >> 1;

  cv.draw_text(str, len, kotuku::point_t(pt.x, pt.y), bounds, eto_clipped);

  cv.background_color(old_bg_color);
  cv.text_color(old_text_color);
  cv.font(old_font);
  cv.clipping_rectangle(clip_rect);
  }
