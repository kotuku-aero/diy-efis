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
#include "gps_window.h"

static const kotuku::rect_t waypoint(kotuku::point_t(4, 10), kotuku::extent_t(122, 45));
static const kotuku::rect_t waypoint_text(kotuku::point_t(7, 15), kotuku::extent_t(114, 23));

static const kotuku::rect_t dist_to_waypoint(kotuku::point_t(4, 55), kotuku::extent_t(122, 45));
static const kotuku::rect_t dist_to_waypoint_text(kotuku::point_t(7, 60), kotuku::extent_t(114, 23));

static const kotuku::rect_t time_to_waypoint(kotuku::point_t(4, 100), kotuku::extent_t(122, 45));
static const kotuku::rect_t time_to_waypoint_text(kotuku::point_t(7, 105), kotuku::extent_t(114, 23));

static const kotuku::rect_t next_waypoint(kotuku::point_t(4, 145), kotuku::extent_t(122, 93));
static const kotuku::rect_t next_waypoint_clip_area(kotuku::point_t(7, 149), kotuku::extent_t(114, 74));
static const kotuku::rect_t next_waypoint_text_0(kotuku::point_t(7, 150), kotuku::extent_t(114, 23));
static const kotuku::rect_t next_waypoint_text_1(kotuku::point_t(7, 174), kotuku::extent_t(114, 23));
static const kotuku::rect_t next_waypoint_text_2(kotuku::point_t(7, 198), kotuku::extent_t(114, 23));

kotuku::gps_window_t::gps_window_t(widget_t &parent, const char *section)
: widget_t(parent, section),
  _background_canvas(*this, window_rect().extents()),
  _dist_to_waypoint(0),
  _time_to_next(-1)
  {
  memset(_waypoints, 0, sizeof(_waypoints));
  // generate the font sizes
  const font_t *old_font = font(&arial_12_font);
  _size_large_font_cell = text_extent("M", 1);
  _size_large_font_cell += extent_t(2, 0);

  font(old_font);

  point_t border[] =
    {
    _background_canvas.window_rect().top_left() + extent_t(12, 0),
    _background_canvas.window_rect().top_right() + extent_t(-1, 0),
    _background_canvas.window_rect().bottom_right() + extent_t(-1, 0)
    };

  _background_canvas.pen(&light_gray_pen);
  _background_canvas.polyline(border, 3);

  point_t arc_pt(_background_canvas.window_rect().top_left() +  extent_t(12, 12));
  _background_canvas.angle_arc(arc_pt, 12, rad_270, rad_360);

  _background_canvas.pen(&gray_pen);
  _background_canvas.background_color(color_gray);
  // fill the top area
  _background_canvas.pie(arc_pt, rad_180, rad_270, 8, 0);

  rect_t top_fill(arc_pt.x, arc_pt.y - 8, _background_canvas.window_rect().top_right().x-2, arc_pt.y);
  _background_canvas.fill_rect(top_fill, color_gray);

  draw_background(waypoint, "Waypoint");
  draw_background(dist_to_waypoint, "Distance");
  draw_background(time_to_waypoint, "Time");
  draw_background(next_waypoint, "Next");
  }

kotuku::gps_window_t::~gps_window_t()
  {
  }

bool kotuku::gps_window_t::ev_msg(const msg_t &msg)
  {
  assert_valid(this);

  bool changed = false;
  //switch(msg.message_id())
  //  {
  //  case 	id_new_route :
  //    // read the route
  //    {
  //    aio_application_t *aio_app = reinterpret_cast<aio_application_t *>(the_app());

  //    const base_gps_device_t *gps = aio_app->gps_device();

  //    size_t waypoint = 0;
  //    size_t destination_waypoint = gps->destination_waypoint();
  //    size_t waypoint_count = gps->waypoint_count() - destination_waypoint;

  //    for(waypoint = 0; waypoint < waypoint_count && waypoint < 4; waypoint++)
  //      {
  //      strncpy(&(_waypoints[waypoint][0]), gps->get_waypoint_name(waypoint + destination_waypoint), 15);
  //      _waypoints[waypoint][15] = 0;
  //      }

  //    for(; waypoint < 4; waypoint++)
  //      _waypoints[waypoint][0] = 0;

  //    changed = true;
  //    }
  //    break;
  //  case id_distance_to_destination :
  //    changed = assign_msg<short>(msg.msg_data_16(), _dist_to_waypoint);
  //    break;
  //  case id_time_to_destination :
  //    changed = assign_msg<short>(msg.msg_data_16(), _time_to_next);
  //    break;
  //  }

  if(changed)
    invalidate();

  return false;
  }

void kotuku::gps_window_t::update_window()
  {
  rect_t r(point_t(0, 0), window_rect().extents());
  clipping_rectangle(r);
  bit_blt(r, _background_canvas, point_t(0, 0), rop_srccopy);

  // display the waypoint
  draw_detail(waypoint_text, _waypoints[0], color_magenta);

  // display next waypoint(s)
  draw_detail(next_waypoint_text_0, _waypoints[1], color_green);
  draw_detail(next_waypoint_text_1, _waypoints[2], color_green);
  draw_detail(next_waypoint_text_2, _waypoints[3], color_green);

  // draw the distance
  char buf[64];
  sprintf(buf, "%d",(int)_dist_to_waypoint);

  draw_detail(dist_to_waypoint_text, buf, color_magenta);

  static const char *unknown_time = "--:--";
  static const char *time_fmt = "%02.2d:%02.2d";

  // draw the time to next
  if(_time_to_next == -1)
    strcpy(buf, unknown_time);
  else
    sprintf(buf, time_fmt, _time_to_next/60, _time_to_next % 60);

  draw_detail(time_to_waypoint_text, buf, color_magenta);
  }

void kotuku::gps_window_t::draw_background(const rect_t &pt, const char *label)
{
  // calculate the size of the label
  _background_canvas.clipping_rectangle(pt);
  _background_canvas.font(&arial_9_font);
  size_t text_len = strlen(label);
  extent_t label_size = _background_canvas.text_extent(label, text_len);

  _background_canvas.background_color(color_gray);

  int width = pt.width();
  int text_start = pt.left + 3;
  int text_end = text_start + label_size.dx;
  int right = pt.left + width;

  _background_canvas.fill_rect(rect_t(pt.left, pt.top+1, right, pt.top + 4), color_gray);
  _background_canvas.fill_rect(rect_t(pt.left, pt.top+4, pt.left + 3, pt.bottom - 17), color_gray);
  _background_canvas.fill_rect(rect_t(right -3, pt.top+4, right, pt.bottom -14), color_gray);
  _background_canvas.fill_rect(rect_t(pt.left, pt.bottom - 17, right, pt.bottom - 14), color_gray);
  _background_canvas.fill_rect(rect_t(pt.left, pt.bottom - 14, text_end + 3, pt.bottom), color_gray);

  //_background_canvas.fill_rect(rect_t(pt.left + 3, pt.top + 4, right -3, pt.bottom - 17), color_black);

  _background_canvas.background_color(color_hollow);

  point_t text_origin(text_start, pt.bottom - 15);
  _background_canvas.clipping_rectangle(rect_t(text_origin, label_size));
  _background_canvas.pen(&white_pen);
  _background_canvas.draw_text(label, text_len, text_origin);
}

void kotuku::gps_window_t::draw_detail(const rect_t &rect,
                               const char *txt,
                               color_t fg,
                               color_t bg,
                               const rect_t *clip_area)
  {
  fill_rect(rect, bg);

  if(txt == 0)
    return;

  // we draw the waypoint depending on the edit mode
  const font_t *old_font = font(&arial_12_font);

  // center the string
  point_t pt = rect.top_left();

  gdi_dim_t cell_width = _size_large_font_cell.dx;

  size_t len = strlen(txt);

  pt.x += rect.width() >> 1;
  pt.x -= gdi_dim_t((cell_width * len) >> 1);

  pt.y += rect.height() >> 1;
  pt.y -= _size_large_font_cell.dy >> 1;

  color_t old_bk_color = background_color(bg);
  color_t old_text_color = text_color();

  for(size_t c = 0; c < len; c++)
    {
    text_color(fg);

    extent_t cell_size = text_extent(txt + c, 1);

    point_t cp = pt;
    cp.x += cell_width >> 1;
    cp.x -= cell_size.dx >> 1;

    draw_text(txt + c, 1, cp, rect, eto_clipped);

    pt.x += cell_width;
    }

  font(old_font);
  text_color(old_text_color);
  background_color(old_bk_color);
  }
