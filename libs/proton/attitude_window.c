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
#include "widget.h"
#include "spatial.h"
#include "pens.h"

static const gdi_dim_t pixels_per_degree = 49;

typedef struct _attitude_window_t {
  uint16_t version;

  int16_t pitch;
  int16_t roll;
  int16_t yaw;

  uint16_t aoa_degrees;

  float aoa_pixels_per_degree;
  float aoa_degrees_per_mark;

  int16_t glideslope;
  int16_t localizer;
  bool glideslope_aquired;
  bool localizer_aquired;

  uint16_t critical_aoa;
  uint16_t approach_aoa;
  uint16_t climb_aoa;
  uint16_t cruise_aoa;
  uint16_t yaw_max;
  bool show_aoa;
  bool show_glideslope;

  point_t median;

  handle_t  font;
  } attitude_window_t;


static result_t on_paint(handle_t hwnd, event_proxy_t *proxy, const canmsg_t *msg)
  {
  begin_paint(hwnd);

  attitude_window_t *wnd = (attitude_window_t *)proxy->parg;
  rect_t wnd_rect;
  get_window_rect(hwnd, &wnd_rect);

  extent_t ex;
  rect_extents(&wnd_rect, &ex);

  rect_t rect;
  // first step is to draw the background.  Then we can rotate it correctly
  // the background is 240x240 pixels with 20 pixels being the wnd->median line

  // the display is +/- 21.2132 degrees so we round to +/- 20 degrees
  // or 400 pixels
  int16_t pitch = wnd->pitch;

  // pitch is now +/- 90
  // limit to 25 degrees as we don't need any more than that
  pitch = min(25, max(-25, pitch));

  // make this == pixels that is the azimuth line
  pitch *= 10;

  // draw the upper area
  point_t pts[5];
  pts[0].x = -500; pts[0].y = wnd->median.y - 500 + pitch;
  pts[1].x = 500;  pts[1].y = wnd->median.y - 500 + pitch;
  pts[2].x = 500;  pts[2].y = wnd->median.y + pitch;
  pts[3].x = -500; pts[3].y = wnd->median.y + pitch;
  pts[4].x = -500; pts[4].y = wnd->median.y - 500 + pitch;

  // rotate the upper rect
  int pt;
  for (pt = 0; pt < 5; pt++)
    rotate_point(&wnd->median, &pts[pt], wnd->roll);

  pts[4] = pts[0];

  polygon(hwnd, &wnd_rect, 0, color_lightblue, 5, pts);

  pts[0].x = -500; pts[0].y = wnd->median.y + pitch;
  pts[1].x = 500;  pts[1].y = wnd->median.y + pitch;
  pts[2].x = 500;  pts[2].y = wnd->median.y + pitch + 500;
  pts[3].x = -500; pts[3].y = wnd->median.y + pitch + 500;
  pts[4].x = -500; pts[4].y = wnd->median.y + pitch;

  // rotate the brown rect
  for (pt = 0; pt < 5; pt++)
    rotate_point(&wnd->median, &pts[pt], wnd->roll);
  pts[4] = pts[0];

  polygon(hwnd, &wnd_rect, 0, color_brown, 5, pts);

  /////////////////////////////////////////////////////////////////////////////
  //	Draw the pitch indicator

  //background_color(color_black);
  //pen(&white_pen);

  /////////////////////////////////////////////////////////////////////////////
  // we now draw the image of the bank angle marks
  point_t slip_indicator[18] = {
    { wnd->median.x + 104,  60 }, { wnd->median.x + 94,  66 },
    { wnd->median.x + 60,  17 }, { wnd->median.x + 54,  27 },
    { wnd->median.x + 40,  11 }, { wnd->median.x + 37,  19 },
    { wnd->median.x + 20,   6 }, { wnd->median.x + 18,  14 },
    { wnd->median.x - 20,   6 }, { wnd->median.x - 18,  14 },
    { wnd->median.x - 39,  11 }, { wnd->median.x - 36,  19 },
    { wnd->median.x - 59,  17 }, { wnd->median.x - 53,  27 },
    { wnd->median.x - 103,  60 }, { wnd->median.x - 93,  66 },
    { wnd->median.x,   0 }, { wnd->median.x,  12 },
    };


  int i;
  for (i = 0; i < 18; i += 2)
    {
    rotate_point(&wnd->median, &slip_indicator[i], wnd->roll);
    rotate_point(&wnd->median, &slip_indicator[i + 1], wnd->roll);

    polyline(hwnd, &wnd_rect, i == 16 ? &green_pen_3 : &green_pen,
      2, slip_indicator + i);
    }

  /////////////////////////////////////////////////////////////////////////////
  //	Draw the rotated roll/pitch indicator

  // The window height is 240 pixels, and each 25 deg of pitch
  // is 20 pixels.  So the display is +/- 150 degrees (120/20)*25
  //
  gdi_dim_t pitch_range = (wnd->median.y / 20) * 25;
  gdi_dim_t pitch_angle = ((gdi_dim_t)(wnd->pitch * 10)) + pitch_range;
  gdi_dim_t line = 0;

  // now we draw the pitch line(s)
  if (pitch_angle % 25)
    {
    gdi_dim_t new_pitch = ((pitch_angle / 25) + 1) * 25;
    line = pitch_angle - new_pitch;
    pitch_angle = new_pitch;
    }

  while (line < wnd_rect.bottom)
    {
    if (pitch_angle == 3600 || pitch_angle == 0)
      {
      pitch_angle -= 25;
      line += 20;
      }
    else if ((pitch_angle % 100) == 0)
      {
      point_t pts[2] =
        {
        { wnd->median.x - 40, line },
        { wnd->median.x + 40, line }
        };

      rotate_point(&wnd->median, &pts[0], wnd->roll);
      rotate_point(&wnd->median, &pts[1], wnd->roll);

      polyline(hwnd, &wnd_rect, &white_pen, 2, pts);

      // we have a bitmap which is the text to draw.  We then select the bitmap
      // from the text angle and the rotation angle.
      // the text is 19x19 pixels

      // calc the text angle as 10, 20..90
      char text_angle[2] = { (pitch_angle / 100), 0 };
      text_angle[0] = text_angle[0] < 0 ? -text_angle[0] : text_angle[0];

      if (text_angle > 0)
        {

        // calc the left/right center point
        point_t pt_left = { wnd->median.x - 48, line };
        point_t pt_right = { wnd->median.x + 47, line };

        rotate_point(&wnd->median, &pt_left, wnd->roll);
        rotate_point(&wnd->median, &pt_right, wnd->roll);

        // we now calc the left and right points to write the text to
        pt_left.x -= 9; pt_left.y -= 9;
        pt_right.x -= 9; pt_right.y -= 9;

        text_angle[0] += '0';

        draw_text(hwnd, &wnd_rect, wnd->font, color_white, color_hollow,
          text_angle, 1, &pt_left, 0, 0, 0);

        draw_text(hwnd, &wnd_rect, wnd->font, color_white, color_hollow,
          text_angle, 1, &pt_right, 0, 0, 0);
        }
      pitch_angle -= 25;
      line += 20;
      }
    else if ((pitch_angle % 50) == 0)
      {
      point_t pts[2] =
        {
        { wnd->median.x - 26, line },
        { wnd->median.x + 25, line }
        };

      rotate_point(&wnd->median, &pts[0], wnd->roll);
      rotate_point(&wnd->median, &pts[1], wnd->roll);

      polyline(hwnd, &wnd_rect, &white_pen, 2, pts);

      pitch_angle -= 25;
      line += 20;
      }
    else
      {
      point_t pts[2] =
        {
        { wnd->median.x - 12, line },
        { wnd->median.x + 12, line }
        };

      rotate_point(&wnd->median, &pts[0], wnd->roll);
      rotate_point(&wnd->median, &pts[1], wnd->roll);

      polyline(hwnd, &wnd_rect, &white_pen, 2, pts);

      pitch_angle -= 25;
      line += 20;
      }
    }

  /////////////////////////////////////////////////////////////////////////////
  // Draw the angle-of-attack indicator
  //
  // this is 40 pixels up/down

  if (wnd->show_aoa)
    {
    // calc the effective AOA
    int16_t aoa = min(wnd->critical_aoa, max(wnd->cruise_aoa, wnd->aoa_degrees));
    aoa -= wnd->cruise_aoa;
    int16_t pixels = (int16_t)(aoa * wnd->aoa_pixels_per_degree);

    float aoa_marker = wnd->critical_aoa;
    int16_t offset;
    for (offset = 60; offset > 0; offset -= 6)
      {
      if (aoa_marker > wnd->approach_aoa)
        {
        // draw red chevron.
        point_t chevron[3] =
          {
            { wnd->median.x - 15, pixels + wnd->median.y - offset },
            { wnd->median.x, pixels + wnd->median.y - offset + 4 },
            { wnd->median.x + 15, pixels + wnd->median.y - offset }
          };

        polyline(hwnd, &wnd_rect, &red_pen_3, 3, chevron);
        }
      else if (aoa_marker > wnd->climb_aoa)
        {
        point_t marker[2] =
          {
            { wnd->median.x - 15, pixels + wnd->median.y - offset },
            { wnd->median.x + 15, pixels + wnd->median.y - offset }
          };

        polyline(hwnd, &wnd_rect, &yellow_pen_3, 2, marker);
        }
      else
        {
        point_t marker[2] =
          {
            { wnd->median.x - 15, pixels + wnd->median.y - offset },
            { wnd->median.x + 15, pixels + wnd->median.y - offset }
          };

        polyline(hwnd, &wnd_rect, &green_pen_3, 2, marker);
        }

      aoa_marker -= wnd->aoa_degrees_per_mark;
      }
    }

  /////////////////////////////////////////////////////////////////////////////
  // draw the aircraft image
  point_t aircraft_points[2];

  aircraft_points[0].x = wnd->median.x - 7; aircraft_points[0].y = wnd->median.y;
  aircraft_points[1].x = wnd->median.x - 22; aircraft_points[1].y = wnd->median.y;

  polyline(hwnd, &wnd_rect, &white_pen, 2, aircraft_points);

  aircraft_points[0].x = wnd->median.x + 7; aircraft_points[0].y = wnd->median.y;
  aircraft_points[1].x = wnd->median.x + 22; aircraft_points[1].y = wnd->median.y;

  polyline(hwnd, &wnd_rect, &white_pen, 2, aircraft_points);

  aircraft_points[0].x = wnd->median.x; aircraft_points[0].y = wnd->median.y - 7;
  aircraft_points[1].x = wnd->median.x; aircraft_points[1].y = wnd->median.y - 15;

  polyline(hwnd, &wnd_rect, &white_pen, 2, aircraft_points);

  ellipse(hwnd, &wnd_rect, &white_pen, color_hollow,
    make_rect(wnd->median.x - 7, wnd->median.y - 7,
      wnd->median.x + 7, wnd->median.y + 7, &rect));

  /////////////////////////////////////////////////////////////////////////////
  // draw the glide slope and localizer indicators
  if (wnd->glideslope_aquired && wnd->show_glideslope)
    {
    // draw the marker, 0.7 degrees = 59 pixels

    float deviation = max(-1.2, min(1.2, wnd->glideslope / 100.0));

    gdi_dim_t pixels = (gdi_dim_t)(deviation / (1.0 / pixels_per_degree));

    pixels += 120;

    point_t pts[2] = {
      { wnd_rect.right - 12, wnd->median.y },
      { wnd_rect.right, wnd->median.y }
      };

    polyline(hwnd, &wnd_rect, &white_pen, 2, pts);

    static rect_t glideslope[4] =
      {
        { 230,  57, 238,  65 },
        { 230,  86, 238,  94 },
        { 230, 146, 238, 154 },
        { 230, 175, 238, 183 }
      };

    // rest are hollow
    ellipse(hwnd, &wnd_rect, &white_pen, color_hollow, &glideslope[0]);
    ellipse(hwnd, &wnd_rect, &white_pen, color_hollow, &glideslope[1]);
    ellipse(hwnd, &wnd_rect, &white_pen, color_hollow, &glideslope[2]);
    ellipse(hwnd, &wnd_rect, &white_pen, color_hollow, &glideslope[3]);

    // black filled ellipse
    ellipse(hwnd, &wnd_rect, &white_pen, color_black,
      make_rect(230, pixels - 4, 238, pixels + 4, &rect));

    }

  if (wnd->localizer_aquired && wnd->show_glideslope)
    {
    // draw the marker, 1.0 degrees = 74 pixels

    float deviation = max(-1.2, min(1.2, wnd->localizer / 100.0));

    gdi_dim_t pixels = (gdi_dim_t)(deviation / (1.0 / pixels_per_degree));

    pixels += wnd->median.x;

    point_t pts[2] = {
      { wnd->median.x, wnd->median.y - 15 },
      { wnd->median.x, wnd->median.y }
      };

    polyline(hwnd, &wnd_rect, &white_pen_3, 2, pts);

    static rect_t localizer[4] = {
      {  57, 230,  65, 238 },
      {  86, 230,  94, 238 },
      { 146, 230, 154, 238 },
      { 175, 230, 183, 238 }
      };

    // rest are hollow
    ellipse(hwnd, &wnd_rect, &white_pen, color_hollow, &localizer[0]);
    ellipse(hwnd, &wnd_rect, &white_pen, color_hollow, &localizer[1]);
    ellipse(hwnd, &wnd_rect, &white_pen, color_hollow, &localizer[2]);
    ellipse(hwnd, &wnd_rect, &white_pen, color_hollow, &localizer[3]);

    // black filled ellipse

    ellipse(hwnd, &wnd_rect, &white_pen, color_black,
      make_rect(pixels - 4, 230, pixels + 4, 238, &rect));

    }

  /////////////////////////////////////////////////////////////////////////////
  // Draw the roll indicator
  // Draw the aircraft pointer at the top of the window.
  point_t roll_points[4] = {
    { wnd->median.x, 12 },
    { wnd->median.x - 6, 23 },
    { wnd->median.x + 6, 23 },
    { wnd->median.x, 12 }
    };

  // the roll indicator is shifted left/right by the yaw angle,  1degree = 2pix
  int16_t offset = min(wnd->yaw_max, max(-wnd->yaw_max, wnd->yaw << 1));

  roll_points[0].x += offset;
  roll_points[1].x += offset;
  roll_points[2].x += offset;
  roll_points[3].x += offset;

  polygon(hwnd, &wnd_rect, &white_pen, color_hollow, 4, roll_points);

  point_t roll_points_base[5] = {
    { wnd->median.x - 6, 23 },
    { wnd->median.x + 6, 23 },
    { wnd->median.x + 9, 27 },
    { wnd->median.x - 9, 27 },
    { wnd->median.x - 6, 23 }
    };

  polygon(hwnd, &wnd_rect, &white_pen, color_hollow, 5, roll_points_base);
  end_paint(hwnd);

  return s_ok;
  }

static result_t on_yaw_angle(handle_t hwnd, event_proxy_t *proxy, const canmsg_t *msg)
  {
  attitude_window_t *wnd = (attitude_window_t *)proxy->parg;
  bool changed = false;

  float v;
  get_param_float(msg, &v);
  int16_t value = (int16_t)radians_to_degrees(v);
  while (value > 179)
    value -= 360;

  while (value < -180)
    value += 360;

  changed = wnd->yaw != value;
  wnd->yaw = value;

  if (changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

static result_t on_roll_angle(handle_t hwnd, event_proxy_t *proxy, const canmsg_t *msg)
  {
  attitude_window_t *wnd = (attitude_window_t *)proxy->parg;
  bool changed = false;

  float v;
  get_param_float(msg, &v);
  int16_t value = (int16_t)radians_to_degrees(v);
  while (value > 179)
    value -= 360;

  while (value < -180)
    value += 360;

  if (value > 90 || value < -90)
    value -= 180;

  while (value < -180)
    value += 360;

  value = min(90, max(-90, value));

  changed = wnd->roll != value;
  wnd->roll = value;

  if (changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

static result_t on_pitch_angle(handle_t hwnd, event_proxy_t *proxy, const canmsg_t *msg)
  {
  attitude_window_t *wnd = (attitude_window_t *)proxy->parg;
  bool changed = false;

  float v;
  get_param_float(msg, &v);
  int16_t angle = (int16_t)radians_to_degrees(v);

  while (angle < -180)
    angle += 180;

  while (angle > 180)
    angle -= 180;

  // angle is now within range
  if (angle > 90)
    angle = 180 - angle;

  else if (angle < -90)
    angle = -180 - angle;

  changed = wnd->pitch != angle;
  wnd->pitch = angle;

  if (changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

  result_t create_attitude_window(handle_t parent, memid_t key, handle_t *hwnd)
    {
    result_t result;

    // create our window
    if (failed(create_child_widget(parent, key, defwndproc, hwnd)))
      return result;

    // create the window data.
    attitude_window_t *wnd = (attitude_window_t *)neutron_malloc(sizeof(attitude_window_t));
    memset(wnd, 0, sizeof(attitude_window_t));

    wnd->version = sizeof(attitude_window_t);

    reg_get_uint16(key, "critical-aoa", &wnd->critical_aoa);
    reg_get_uint16(key, "approach-aoa", &wnd->approach_aoa);
    reg_get_uint16(key, "climb-aoa", &wnd->climb_aoa);
    reg_get_uint16(key, "cruise-aoa", &wnd->cruise_aoa);
    if (failed(reg_get_uint16(key, "yaw-max", &wnd->yaw_max)))
      wnd->yaw_max = 45;

    reg_get_bool(key, "show-aoa", &wnd->show_aoa);
    reg_get_bool(key, "show-gs", &wnd->show_glideslope);
    wnd->aoa_degrees = wnd->cruise_aoa;

    // aoa is 40 pixels
    wnd->aoa_pixels_per_degree = 40.0 / (wnd->critical_aoa - wnd->cruise_aoa);
    wnd->aoa_degrees_per_mark = (wnd->critical_aoa - wnd->cruise_aoa) / 8.0;

    rect_t wnd_rect;
    get_window_rect(*hwnd, &wnd_rect);

    int16_t value;
    if (succeeded(reg_get_int16(key, "center-x", &value)))
      wnd->median.x = (gdi_dim_t)value;
    else
      wnd->median.x = rect_width(&wnd_rect) >> 1;

    if (succeeded(reg_get_int16(key, "center-y", &value)))
      wnd->median.y = (gdi_dim_t)value;
    else
      wnd->median.y = rect_height(&wnd_rect) >> 1;

    if (failed(lookup_font(key, "font", &wnd->font)))
      {
      // we always have the neo font.
      if (failed(result = open_font("neo", 9, &wnd->font)))
        return result;
      }

    // store the parameters for the window
    set_wnddata(*hwnd, wnd);

    add_event(*hwnd, id_paint, wnd, 0, on_paint);
    add_event(*hwnd, id_yaw_angle, wnd, 0, on_yaw_angle);
    add_event(*hwnd, id_roll_angle, wnd, 0, on_roll_angle);
    add_event(*hwnd, id_pitch_angle, wnd, 0, on_pitch_angle);

    invalidate_rect(*hwnd, &wnd_rect);

    return s_ok;
    }
