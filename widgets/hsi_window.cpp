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
#include "hsi_window.h"
#include "pfd_application.h"

static const gdi_dim_t mark_start = 12;
static const gdi_dim_t center_x = 120;
static const gdi_dim_t center_y = 120;
static const gdi_dim_t window_x = 240;
static const gdi_dim_t window_y = 240;
static const gdi_dim_t border = 10;
static const gdi_dim_t pixels_per_nm_cdi = 6;
static const kotuku::point_t median(center_x, center_y);
const pen_t track_pen = { color_gray, 1, ps_dot };

enum {
  ds_course,
  ds_heading,
  };

kotuku::hsi_window_t::hsi_window_t(widget_t &parent, const char *section)
: widget_t(parent, section),
  _background_canvas(*this, window_rect().extents()),
  _direction(15),
  _course(30),
  _deviation(5),
  _track(22),
  _wind_speed(0),
  _wind_direction(0),
  _distance_to_waypoint(0),
  _time_to_waypoint(0),
  _magnetic_variation(0)
  {
  memset(_waypoint_name, 0, sizeof(_waypoint_name));

  _background_canvas.pen(&light_gray_pen);

  _background_canvas.fill_rect(_background_canvas.window_rect(), color_black);

  if(draw_border())
    _background_canvas.round_rect(_background_canvas.window_rect(), extent_t(12, 12));

  subscribe(id_heading_angle);
  subscribe(id_magnetic_heading);
  subscribe(id_track);
  subscribe(id_deviation);
  subscribe(id_desired_track_angle);
  subscribe(id_selected_course);
  subscribe(id_wind_speed);
  subscribe(id_wind_direction);
  subscribe(id_magnetic_variation);
  subscribe(id_estimated_time_to_next);
  subscribe(id_distance_to_next);
  subscribe(id_heading);
  }

bool kotuku::hsi_window_t::ev_msg(const msg_t &data)
  {
  assert_valid(this);
  bool changed = false;

  switch(data.message_id())
    {
    case id_magnetic_heading :
      {
      short direction = data.value<short>();
      while(direction < 0)
        direction += 360;
      while(direction > 359)
        direction -= 360;

      changed = assign_msg(direction, _direction);
      }
      break;
    case id_heading :
      changed = assign_msg(data.value<short>(), _heading_bug);
      break;
    case id_heading_angle :
      changed = assign_msg(data.value<short>(), _heading);
      break;
    case id_deviation :
      // the deviation is +/- * 10
      changed = assign_msg(data.value<short>(), _deviation);
      break;
    case id_selected_course :
      changed = assign_msg(data.value<short>(), _course);
      break;
    case id_track :
      changed = assign_msg((short)radians_to_degrees(data.value<double>()), _track);
      break;
    case id_wind_speed :
      changed = assign_msg((short)meters_per_second_to_knots(data.value<double>()), _wind_speed);
      break;
    case id_wind_direction :
      changed = assign_msg((short)radians_to_degrees(data.value<double>()), _wind_direction);
      break;
    case id_distance_to_next :
      changed = assign_msg((short)meters_to_nm(data.value<double>()), _distance_to_waypoint);
      break;
    case id_magnetic_variation :
      changed = assign_msg((short)radians_to_degrees(data.value<double>()), _magnetic_variation);
       break;
    case id_estimated_time_to_next :
      changed = assign_msg(data.value<short>(), _time_to_waypoint);
      break;
    }

  if(changed)
    invalidate();

  return true;
  }

void kotuku::hsi_window_t::update_window()
  {
	rect_t window_size(0, 0, window_rect().width(), window_rect().height());

  clipping_rectangle(window_size);
  bit_blt(window_size, _background_canvas, point_t(0, 0), rop_srccopy);

  /////////////////////////////////////////////////////////////////////////////
  //
  // Draw the HSI Indicator
  static const gdi_dim_t major_mark = mark_start + 16;
  static const gdi_dim_t minor_mark = mark_start + 8;
  static const gdi_dim_t font_x_y = 19;
  static const gdi_dim_t font_center =(font_x_y >> 1) + 1;
  static const gdi_dim_t font_ordinal = major_mark + font_center;

  pen(&white_pen);

  // start at 0
  gdi_dim_t i =0;
  for(gdi_dim_t index = -_direction; i < 12; index += 30, i++)
    {
    while(index > 359)
      index -= 360;

    while(index < 0)
      index += 360;

    // draw the marker
    point_t pts[2];

    pts[0].x = center_x; pts[0].y = mark_start;
    pts[1].x = center_x; pts[1].y = major_mark;
    rotate_point(median, pts[0], degrees_to_radians(index));
    rotate_point(median, pts[1], degrees_to_radians(index));

    polyline(pts, 2);

    bool do_minor_mark = false;
    short minor_index;
    for(minor_index = 0; minor_index < 30; minor_index += 5)
      {
      pts[0].x = center_x; pts[0].y = mark_start;
      pts[1].x = center_x; pts[1].y = do_minor_mark ? minor_mark : major_mark;

      rotate_point(median, pts[0], degrees_to_radians(index + minor_index));
      rotate_point(median, pts[1], degrees_to_radians(index + minor_index));

      polyline(pts, 2);

      do_minor_mark = !do_minor_mark;
      }

    // we now draw the text onto the canvas.  The text has a 23x23 pixel
    // block so the center is 12, 12.
    pts[0].x = center_x; pts[0].y = font_ordinal;
    short rotn = short((index < 0) ? index + 360 : index);
    rotate_point(median, pts[0], degrees_to_radians(rotn));

    font(navigation_window_fonts[rotn]);

    draw_text((char *) &i, 1, point_t(pts[0].x - font_center, pts[0].y - font_center));
    }

  ///////////////////////////////////////////////////////////////////////////
  // Draw the Track

  double rotation = degrees_to_radians(_track - _direction);

  // the marker is a dashed line
  point_t track_marker[4] = {
    point_t(center_x, center_y - 88),
    point_t(center_x - 7, center_y - 95),
    point_t(center_x + 7, center_y - 95),
    point_t(center_x, center_y - 88)
    };

  for(i = 0; i < 4; i++)
    rotate_point(median, track_marker[i], rotation);

  pen(&gray_pen);
  background_color(color_hollow);
  polygon(track_marker, 4);

  pen(&track_pen);
  e_background_mode old_mode = background_mode(transparent);

  point_t track_line[2] = {
    point_t(center_x, center_y - 88),
    point_t(median)
    };

  rotate_point(median, track_line[0], rotation);

  polyline(track_line, 2);

  background_mode(old_mode);

  ///////////////////////////////////////////////////////////////////////////
  // Draw the CDI

  pen(&green_pen_3);

  rotation = degrees_to_radians(_course - _direction);
  for(gdi_dim_t dist = -10; dist < 11; dist += 2)
    {
    if(dist == 0)
      continue;

    point_t pts[2] = {
      point_t(center_x +(pixels_per_nm_cdi * dist), center_y - 5),
      point_t(center_x +(pixels_per_nm_cdi * dist), center_y + 5)
      };

    rotate_point(median, pts[0], rotation);
    rotate_point(median, pts[1], rotation);
      
    polyline(pts, 2);
    }

  // draw the CDI Marker head next
  point_t cdi_pts[4] = {
    point_t(center_x, center_y - 97),
    point_t(center_x - 6, center_y - 88),
    point_t(center_x + 6, center_y - 88),
    point_t(center_x, center_y - 97)
    };

  for(i = 0; i < 4; i++)
    rotate_point(median, cdi_pts[i], rotation);

  background_color(color_green);
  polygon(cdi_pts, 4);

  // we now convert the deviation to pixels.
  // 1 degree = 24 pixels
  double cdi_var = pixels_per_nm_cdi *((double)_deviation/10);

  gdi_dim_t cdi = std::max(gdi_dim_t(-66), std::min(gdi_dim_t(66), gdi_dim_t(round(cdi_var))));

  point_t pts[4];
  pts[0].x = center_x; pts[0].y = center_y - 98;
  pts[1].x = center_x; pts[1].y = center_y - 50;

  rotate_point(median, pts[0], rotation);
  rotate_point(median, pts[1], rotation);

  polyline(pts, 2);

  pts[0].x = center_x; pts[0].y = center_y + 50;
  pts[1].x = center_x; pts[1].y = center_y + 98;

  rotate_point(median, pts[0], rotation);
  rotate_point(median, pts[1], rotation);

  polyline(pts, 2);

  pts[0].x = center_x + cdi; pts[0].y = center_y - 48;
  pts[1].x = pts[0].x; pts[1].y = center_y + 48;
    
  rotate_point(median, pts[0], rotation);
  rotate_point(median, pts[1], rotation);

  polyline(pts, 2);

  /////////////////////////////////////////////////////////////////////////////
  //	Draw the heading bug.

  double hdg = degrees_to_radians(heading() - _direction);

  point_t heading_points[8] = {
    point_t(center_x - 15, 3),
    point_t(center_x - 5, 3),
    point_t(center_x, 10),
    point_t(center_x + 5, 3),
    point_t(center_x + 15, 3),
    point_t(center_x + 15, 12),
    point_t(center_x - 15, 12),
    point_t(center_x - 15, 3)
    };

  for(i = 0; i < 8; i++)
    rotate_point(median, heading_points[i], hdg);

  pen(&magenta_pen);
  polyline(heading_points, 8);

  heading_points[0].x = center_x - 5; heading_points[0].y = 0;
  heading_points[1].x = center_x + 5; heading_points[1].y = 0;
  heading_points[2].x = center_x; heading_points[2].y = 10;
  heading_points[3].x = center_x - 5; heading_points[3].y = 0;

  pen(&white_pen);
  background_color(color_white);

  polygon(heading_points, 4);

  /////////////////////////////////////////////////////////////////////////////
  // Draw the wind direction indicator.
  // it is in the top left of the HSI and has an arrow that is
  // relative to the magnetic heading of the aircraft and the
  // speed/magnetic heading in the form deg/speed so
  // so for a wind of 15 knots at 50 degrees magnetic we would
  // show 050/15.  If the aircraft heading is 240 degrees magnetic we
  // would see a wind vector on the tail of 40 degrees toward the aircraft
  // the wind direction is shown as a yellow triangle around the HSI indicator
  // the text allows for 3 characters with an maximum width of 23 pixels each
  // so the allowance is 69 by 64 pixels
  
  short relative_wind_degrees = _wind_direction + _magnetic_variation - _direction;
  while(relative_wind_degrees < 0)
    relative_wind_degrees += 360;
  double relative_wind = degrees_to_radians(relative_wind_degrees);
  // draw the wind first
  point_t wind_bug[4] =
    {
    point_t(center_x - 15, 2),
    point_t(center_x + 15, 2),
    point_t(center_x, 12),
    point_t(center_x - 15, 2)
    };

  for(size_t i = 0; i < 4; i++)
    rotate_point(median, wind_bug[i], relative_wind);

  pen(&yellow_pen);
  polyline(wind_bug, 4);

  // now the text in upper left
  text_color(color_yellow);
  font(&arial_9_font);

  char msg[32];
  sprintf(msg, "%03.3d", _wind_direction + _magnetic_variation);
  size_t length = strlen(msg);

  extent_t pixels = text_extent(msg, length);
  draw_text(msg, length, point_t(25 - (pixels.dx >> 1), 2));

  sprintf(msg, "%d", _wind_speed);
  length = strlen(msg);
  pixels = text_extent(msg, length);
  draw_text(msg, length, point_t(25 - (pixels.dx >> 1), 13));

  /////////////////////////////////////////////////////////////////////////////
  // Draw the estimated time to waypoint.
  // drawn in top right as distance/time
  sprintf(msg, "%d", _distance_to_waypoint);
  length = strlen(msg);
  pixels = text_extent(msg, length);
  draw_text(msg, length, point_t(window_size.width() - 25 - (pixels.dx >> 1), 2));

  sprintf(msg, "%02.2d:%02.2d", _time_to_waypoint / 60, _time_to_waypoint % 60);
  length = strlen(msg);
  pixels = text_extent(msg, length);
  draw_text(msg, length, point_t(window_size.width() - 25 - (pixels.dx >> 1), 13));

  sprintf(msg, "%s", _waypoint_name);
  length = strlen(msg);
  pixels = text_extent(msg, length);
  draw_text(msg, length, point_t(window_size.width() - 25 - (pixels.dx >> 1), 24));
  }
