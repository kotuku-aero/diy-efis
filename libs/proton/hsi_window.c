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
#include "../photon/widget.h"
#include "pens.h"
#include "spatial.h"

const pen_t track_pen = { color_gray, 1, ps_dot };

enum {
  ds_course,
  ds_heading,
  };

typedef struct _hsi_window_t {
  uint16_t version;

  int16_t direction;
  int16_t course;
  int16_t deviation;
  int16_t track;
  int16_t distance_to_waypoint;
  int16_t time_to_waypoint;
  int16_t wind_direction; // always true
  int16_t magnetic_variation;
  int16_t wind_speed;
  char waypoint_name[17];
  int16_t heading_bug;
  int16_t heading;
  color_t background_color;
  bool draw_border;
  handle_t  font;
  } hsi_window_t;

static result_t widget_wndproc(handle_t hwnd, const canmsg_t *data);

result_t create_hsi_window(handle_t parent, memid_t key, handle_t *hwnd)
  {
  result_t result;

  // create our window
  if (failed(result = create_child_widget(parent, key, widget_wndproc, hwnd)))
    return result;

  // create the window data.
  hsi_window_t *wnd = (hsi_window_t *)neutron_malloc(sizeof(hsi_window_t));
  memset(wnd, 0, sizeof(hsi_window_t));

  wnd->version = sizeof(hsi_window_t);

  if (failed(lookup_color(key, "background-color", &wnd->background_color)))
    wnd->background_color = color_black;

  reg_get_bool(key, "draw-border", &wnd->draw_border);

  if (failed(lookup_font(key, "font", &wnd->font)))
    {
    // we always have the neo font.
    if (failed(result = open_font("neo", 9, &wnd->font)))
      return result;
    }

  // store the parameters for the window
  set_wnddata(*hwnd, wnd);

  rect_t rect;
  get_window_rect(*hwnd, &rect);
  invalidate_rect(*hwnd, &rect);

  return s_ok;
  }

static void update_window(handle_t hwnd, canvas_t *canvas, hsi_window_t *wnd)
  {
  rect_t wnd_rect;
  get_window_rect(hwnd, &wnd_rect);

  extent_t ex;
  rect_extents(&wnd_rect, &ex);

  point_t pt;
  rect_t rect;
  // fill without a border
  rectangle(canvas, &wnd_rect, 0, wnd->background_color, &wnd_rect);

  if (wnd->draw_border)
    round_rect(canvas, &wnd_rect, &gray_pen, color_hollow, &wnd_rect, 12);

  /////////////////////////////////////////////////////////////////////////////
  //
  // Draw the HSI Indicator

  const gdi_dim_t mark_start = 12;
  const gdi_dim_t center_x = ex.dx >> 1;
  const gdi_dim_t center_y = ex.dy >> 1;
  const gdi_dim_t window_x = ex.dx;
  const gdi_dim_t window_y = ex.dy;
  const gdi_dim_t border = 10;
  const gdi_dim_t pixels_per_nm_cdi = 6;
  const point_t median = { center_x, center_y };
  const gdi_dim_t major_mark = mark_start + 16;
  const gdi_dim_t minor_mark = mark_start + 8;
  const gdi_dim_t font_x_y = 19;
  const gdi_dim_t font_center = (font_x_y >> 1) + 1;
  const gdi_dim_t font_ordinal = major_mark + font_center;

  // start at 0
  gdi_dim_t i = 0;
  gdi_dim_t index;
  for (index = -wnd->direction; i < 12; index += 30, i++)
    {
    while (index > 359)
      index -= 360;

    while (index < 0)
      index += 360;

    // draw the marker
    point_t pts[2];

    pts[0].x = center_x; pts[0].y = mark_start;
    pts[1].x = center_x; pts[1].y = major_mark;
    rotate_point(&median, &pts[0], index);
    rotate_point(&median, &pts[1], index);

    polyline(canvas, &wnd_rect, &white_pen, 2, pts);

    bool do_minor_mark = false;
    int16_t minor_index;
    for (minor_index = 0; minor_index < 30; minor_index += 5)
      {
      pts[0].x = center_x; pts[0].y = mark_start;
      pts[1].x = center_x; pts[1].y = do_minor_mark ? minor_mark : major_mark;

      rotate_point(&median, &pts[0], index + minor_index);
      rotate_point(&median, &pts[1], index + minor_index);

      polyline(canvas, &wnd_rect, &white_pen, 2, pts);

      do_minor_mark = !do_minor_mark;
      }

    // we now draw the text onto the canvas.  The text has a 23x23 pixel
    // block so the center is 12, 12.
    pts[0].x = center_x; pts[0].y = font_ordinal;
    int16_t rotn = (index < 0) ? index + 360 : index;
    rotate_point(&median, &pts[0], rotn);

    draw_text(canvas, &wnd_rect, wnd->font, color_white, color_black,
      (char *)&i, 1,
      make_point(pts[0].x - font_center, pts[0].y - font_center, &pt),
      0, 0, 0);
    }

  ///////////////////////////////////////////////////////////////////////////
  // Draw the Track

  int rotation = wnd->track - wnd->direction;

  // the marker is a dashed line
  point_t track_marker[4] = {
    { center_x, center_y - 88 },
    { center_x - 7, center_y - 95 },
    { center_x + 7, center_y - 95 },
    { center_x, center_y - 88 }
    };

  for (i = 0; i < 4; i++)
    rotate_point(&median, &track_marker[i], rotation);

  polygon(canvas, &wnd_rect, &gray_pen, color_hollow, 4, track_marker);

  point_t track_line[2] = {
    { center_x, center_y - 88 },
    { median.x, median.y }
    };

  rotate_point(&median, &track_line[0], rotation);

  polyline(canvas, &wnd_rect, &track_pen, 2, track_line);

  ///////////////////////////////////////////////////////////////////////////
  // Draw the CDI

  rotation = wnd->course - wnd->direction;

  gdi_dim_t dist;
  for (dist = -10; dist < 11; dist += 2)
    {
    if (dist == 0)
      continue;

    point_t pts[2] = {
      { center_x + (pixels_per_nm_cdi * dist), center_y - 5 },
      { center_x + (pixels_per_nm_cdi * dist), center_y + 5 }
      };

    rotate_point(&median, &pts[0], rotation);
    rotate_point(&median, &pts[1], rotation);

    polyline(canvas, &wnd_rect, &green_pen_3, 2, pts);
    }

  // draw the CDI Marker head next
  point_t cdi_pts[4] = {
    { center_x, center_y - 97 },
    { center_x - 6, center_y - 88 },
    { center_x + 6, center_y - 88 },
    { center_x, center_y - 97 }
    };

  for (i = 0; i < 4; i++)
    rotate_point(&median, &cdi_pts[i], rotation);

  polygon(canvas, &wnd_rect, 0, color_green, 4, cdi_pts);

  // we now convert the deviation to pixels.
  // 1 degree = 24 pixels
  double cdi_var = pixels_per_nm_cdi *((double)wnd->deviation / 10);

  gdi_dim_t cdi = (gdi_dim_t) max(-66, min(66, roundf(cdi_var)));

  point_t pts[4];
  pts[0].x = center_x; pts[0].y = center_y - 98;
  pts[1].x = center_x; pts[1].y = center_y - 50;

  rotate_point(&median, &pts[0], rotation);
  rotate_point(&median, &pts[1], rotation);

  polyline(canvas, &wnd_rect, &green_pen_3, 2, pts);

  pts[0].x = center_x; pts[0].y = center_y + 50;
  pts[1].x = center_x; pts[1].y = center_y + 98;

  rotate_point(&median, &pts[0], rotation);
  rotate_point(&median, &pts[1], rotation);

  polyline(canvas, &wnd_rect, &green_pen_3, 2, pts);

  pts[0].x = center_x + cdi; pts[0].y = center_y - 48;
  pts[1].x = pts[0].x; pts[1].y = center_y + 48;

  rotate_point(&median, &pts[0], rotation);
  rotate_point(&median, &pts[1], rotation);

  polyline(canvas, &wnd_rect, &green_pen_3, 2, pts);

  /////////////////////////////////////////////////////////////////////////////
  //	Draw the heading bug.

  int hdg = wnd->heading - wnd->direction;

  point_t heading_points[8] = {
    { center_x - 15, 3 },
    { center_x - 5, 3 },
    { center_x, 10 },
    { center_x + 5, 3 },
    { center_x + 15, 3 },
    { center_x + 15, 12 },
    { center_x - 15, 12 },
    { center_x - 15, 3 }
    };

  for (i = 0; i < 8; i++)
    rotate_point(&median, &heading_points[i], hdg);

  polyline(canvas, &wnd_rect, &magenta_pen, 8, heading_points);

  heading_points[0].x = center_x - 5; heading_points[0].y = 0;
  heading_points[1].x = center_x + 5; heading_points[1].y = 0;
  heading_points[2].x = center_x; heading_points[2].y = 10;
  heading_points[3].x = center_x - 5; heading_points[3].y = 0;

  polygon(canvas, &wnd_rect, &white_pen, color_white, 4, heading_points);

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

  int16_t relative_wind = wnd->direction + wnd->magnetic_variation - wnd->direction;
  while (relative_wind < 0)
    relative_wind += 360;

  // draw the wind first
  point_t wind_bug[4] =
    {
    { center_x - 15, 2 },
    { center_x + 15, 2 },
    { center_x, 12 },
    { center_x - 15, 2 }
    };

  for (i = 0; i < 4; i++)
    rotate_point(&median, &wind_bug[i], relative_wind);

  polyline(canvas, &wnd_rect, &yellow_pen, 4, wind_bug);

  // now the text in upper left

  char msg[32];
  sprintf(msg, "%03.3d", wnd->direction + wnd->magnetic_variation);
  size_t length = strlen(msg);

  extent_t pixels;
  text_extent(canvas, wnd->font, msg, length, &pixels);
  draw_text(canvas, &wnd_rect, wnd->font, color_yellow, color_hollow,
    msg, length, make_point(25 - (pixels.dx >> 1), 2, &pt), 0, 0, 0);

  sprintf(msg, "%d", wnd->wind_speed);
  length = strlen(msg);
  text_extent(canvas, wnd->font, msg, length, &pixels);
  draw_text(canvas, &wnd_rect, wnd->font, color_yellow, color_hollow,
    msg, length, make_point(25 - (pixels.dx >> 1), 13, &pt), 0, 0, 0);

  /////////////////////////////////////////////////////////////////////////////
  // Draw the estimated time to waypoint.
  // drawn in top right as distance/time
  sprintf(msg, "%d", wnd->distance_to_waypoint);
  length = strlen(msg);
  text_extent(canvas, wnd->font, msg, length, &pixels);
  draw_text(canvas, &wnd_rect, wnd->font, color_yellow, color_hollow,
    msg, length, make_point(window_x - 25 - (pixels.dx >> 1), 2, &pt), 0, 0, 0);

  sprintf(msg, "%02.2d:%02.2d", wnd->time_to_waypoint / 60, wnd->time_to_waypoint % 60);
  length = strlen(msg);
  text_extent(canvas, wnd->font, msg, length, &pixels);
  draw_text(canvas, &wnd_rect, wnd->font, color_yellow, color_hollow,
    msg, length, make_point(window_x - 25 - (pixels.dx >> 1), 13, &pt), 0, 0, 0);

  sprintf(msg, "%s", wnd->waypoint_name);
  length = strlen(msg);
  text_extent(canvas, wnd->font, msg, length, &pixels);
  draw_text(canvas, &wnd_rect, wnd->font, color_yellow, color_hollow,
    msg, length, make_point(window_x - 25 - (pixels.dx >> 1), 24, &pt), 0, 0, 0);
  }


result_t widget_wndproc(handle_t hwnd, const canmsg_t *msg)
  {
  bool changed = false;
  hsi_window_t *wnd;
  get_wnddata(hwnd, (void **)&wnd);

  switch (msg->id)
    {
    case id_magnetic_heading:
    {
    int16_t direction;
    get_param_int16(msg, 0, &direction);

    while (direction < 0)
      direction += 360;
    while (direction > 359)
      direction -= 360;

    changed = wnd->direction != direction;
    wnd->direction = direction;
    }
    break;
    case id_heading:
    {
    int16_t value;
    get_param_int16(msg, 0, &value);
    changed = wnd->heading_bug != value;
    wnd->heading_bug = value;
    }
    break;
    case id_heading_angle:
    {
    int16_t value;
    get_param_int16(msg, 0, &value);
    changed = wnd->heading != value;
    wnd->heading = value;
    }
    break;
    case id_deviation:
    {
    int16_t value;
    get_param_int16(msg, 0, &value);
    changed = wnd->deviation != value;
    wnd->deviation = value;
    // the deviation is +/- * 10
    }
    break;
    case id_selected_course:
    {
    int16_t value;
    get_param_int16(msg, 0, &value);
    changed = wnd->course != value;
    wnd->course = value;
    }
    break;
    case id_track:
    {
    int16_t value;
    get_param_int16(msg, 0, &value);
    changed = wnd->track != value;
    wnd->track = value;
    }
    break;
    case id_wind_speed:
    {
    float v;
    get_param_float(msg, &v);
    int16_t value = (int16_t)meters_per_second_to_knots(v);
    changed = wnd->wind_speed != value;
    wnd->wind_speed = value;
    }
    break;
    case id_wind_direction:
    {
    float v;
    get_param_float(msg, &v);
    int16_t value = (int16_t)radians_to_degrees(v);
    changed = wnd->wind_direction != value;
    wnd->wind_direction != value;
    }
    break;
    case id_distance_to_next:
    {
    float v;
    get_param_float(msg, &v);
    int16_t value = (int16_t)meters_to_nm(v);
    changed = wnd->distance_to_waypoint != value;
    wnd->distance_to_waypoint = value;
    }
    break;
    case id_magnetic_variation:
    {
    float v;
    get_param_float(msg, &v);
    int16_t value = (int16_t)radians_to_degrees(v);
    changed = wnd->magnetic_variation != value;
    wnd->magnetic_variation = value;
    }
    break;
    case id_estimated_time_to_next:
    {
    int16_t value;
    get_param_int16(msg, 0, &value);
    changed = wnd->time_to_waypoint != value;
    wnd->time_to_waypoint = value;
    }
    break;
    case id_paint:
      {
    canvas_t *canvas;
      begin_paint(hwnd, &canvas);
      update_window(hwnd, canvas, wnd);
      end_paint(hwnd);
      }
        break;
    default:
      return defwndproc(hwnd, msg);
    }

  if (changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }
