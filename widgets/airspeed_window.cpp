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
#include "airspeed_window.h"
#include "pfd_application.h"
#include "fonts.h"
#include "pens.h"

kotuku::airspeed_window_t::airspeed_window_t(widget_t &parent, const char *section_name)
: widget_t(parent, section_name),
  _background_canvas(*this, window_rect().extents()),
  _airspeed(0)
  {

  the_app()->get_config_value(section_name, "vs0", _vs0);
  the_app()->get_config_value(section_name, "vs1", _vs1);
  the_app()->get_config_value(section_name, "vfe", _vfe);
  the_app()->get_config_value(section_name, "vno", _vno);
  the_app()->get_config_value(section_name, "vne", _vne);
  the_app()->get_config_value(section_name, "va", _va);
  the_app()->get_config_value(section_name, "vx", _vx);
  the_app()->get_config_value(section_name, "vy", _vy);

  _background_canvas.clipping_rectangle(_background_canvas.window_rect());
  _background_canvas.fill_rect(rect_t(0, 0, 80, 8), color_black);
  _background_canvas.fill_rect(rect_t(0, 8, 8, 240), color_black);
  _background_canvas.fill_rect(rect_t(0, 232, 80, 240), color_black);
  _background_canvas.fill_rect(rect_t(71, 8, 80, 232), color_black);
  _background_canvas.fill_rect(rect_t(8, 8, 71, 232), color_gray);

  // get our data feed running
  subscribe(id_indicated_airspeed);
  }

bool kotuku::airspeed_window_t::ev_msg(const msg_t &data)
  {
  bool changed = false;
  switch(data.message_id())
    {
  case id_indicated_airspeed :
    {
    // airspeed is in m/s convert to knots
    float airspeed = data.value<float>() / 0.5144444445610519f;
    changed = assign_msg<long>(long(airspeed), _airspeed);
    }
    break;
  default :
    return false;
    }

  if(changed)
    invalidate();

  return true;
  }

void kotuku::airspeed_window_t::update_window()
  {
	rect_t window_size(0, 0, window_rect().width(), window_rect().height());
  clipping_rectangle(window_size);

  bit_blt(window_size, _background_canvas, point_t(0, 0), rop_srccopy);
  // create the background bitmap
  pen(&white_pen);
  background_color(color_gray);
  text_color(color_white);
  font(&arial_12_font);

  // the vertical tape displays 28 knots around the current position
  // as there are 240 pixels.  We calc the upper one first
  gdi_dim_t top_asi = gdi_dim_t(_airspeed * 10) + 110;

  clipping_rectangle(rect_t(8, 8, 92, 232));

  // assign the first line airspeed
  gdi_dim_t asi_line =(top_asi / 25) * 25;
  // work out how many lines to the next lowest marker
  for(gdi_dim_t marker_line =(top_asi - asi_line)+ 10;
    marker_line < 240; marker_line += 25)
    {
    // draw a line from 10 pixels to 30 pixels then the text.
    // lines at 25 are shorter
    point_t pts[2] = {
      point_t(asi_line ==((asi_line / 50) * 50) ? 50 : 55, marker_line),
      point_t(65, marker_line)
      };

    polyline(pts, 2);

    if(asi_line ==((asi_line / 100) * 100))
      {
      static const size_t buf_len = 64;
      char str[buf_len];
      sprintf(str, "%d",(int)asi_line / 10);

      size_t len = strlen(str);
      extent_t size = text_extent(str, len);

      draw_text(str, len, point_t(47 - size.dx, marker_line -(size.dy >> 1)));
      }

    asi_line -= 25;

    if(asi_line < 0)
      break;
    }

  clipping_rectangle(rect_t(0, 0, 100, 240));

  static point_t roller[8] = 
    {
    point_t(47,  120),
    point_t(40,  127),
    point_t(40,  140),
    point_t(0,  140),
    point_t(0,  100),
    point_t(40,  100),
    point_t(40,  113),
    point_t(47,  120)
    };

  background_color(color_black);
  polygon(roller, 8);

  // now we draw the roller
  display_roller(*this, rect_t(1, 101, 39, 139), _airspeed, 1);

  // finally draw the markers that indicate the v-speeds
  // each knot is 10 pixels in the y direction
  // the scale is 240 pixels high. 

  gdi_dim_t vne_pixels = top_asi - _vne + 10;
  gdi_dim_t vno_pixels = top_asi - _vno + 10;
  gdi_dim_t va_pixels = top_asi - _va + 10;
  gdi_dim_t vfe_pixels = top_asi - _vfe + 10;
  gdi_dim_t vs0_pixels = top_asi - _vs0 + 10;      // stall flaps extended
  gdi_dim_t vs1_pixels = top_asi - _vs1 + 10;      // stall flaps up
  gdi_dim_t vx_pixels = top_asi - _vx + 10;        // best angle of climb
  gdi_dim_t vy_pixels = top_asi - _vy + 10;        // best rate of climb

  // draw vne exceeded
  if(vne_pixels >= 8)
    fill_rect(rect_t(75, 8, 79, std::min((gdi_dim_t)232, vne_pixels)), color_red);

  // draw vne->vno
  if(vno_pixels >= (gdi_dim_t)8 && vne_pixels < (gdi_dim_t)232)
    fill_rect(rect_t(75, std::max((gdi_dim_t)8, vne_pixels), 79, std::min((gdi_dim_t)232, vno_pixels)), color_yellow);

  // draw vno->vs1
  if(vs1_pixels >= 8 && vno_pixels < 232)
    fill_rect(rect_t(75, std::max((gdi_dim_t)8, vno_pixels), 79, std::min((gdi_dim_t)232, vs1_pixels)), color_green);

  // draw vfe->vs0
  if(vs0_pixels >= 8 && vfe_pixels < 232)
    fill_rect(rect_t(71, std::max((gdi_dim_t)8, vfe_pixels), 75, std::min((gdi_dim_t)232, vs0_pixels)), color_white);

  // draw vy -> vx
  if(vx_pixels >= 8 && vy_pixels < 232)
    fill_rect(rect_t(67, std::max((gdi_dim_t)8, vy_pixels), 71, std::min((gdi_dim_t)232, vx_pixels)), color_blue);
  }

