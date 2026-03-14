#include "../../include/attitude_widget.h"
#include "../../include/proton.h"
#include "../mfdlib/mfd.h"

static const char *AOA_warning_str = "PUSH";

static void on_paint(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnddata)
  {
  attitude_widget_t* wnd = (attitude_widget_t*)wnddata;
  extent_t ex;
  rect_extents(wnd_rect, &ex);

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
    rotate_point(&wnd->median, wnd->roll, &pts[pt]);

  pts[4] = pts[0];

  polygon(canvas, wnd_rect, 0, color_lightblue, 5, pts);

  pts[0].x = -500; pts[0].y = wnd->median.y + pitch;
  pts[1].x = 500;  pts[1].y = wnd->median.y + pitch;
  pts[2].x = 500;  pts[2].y = wnd->median.y + pitch + 500;
  pts[3].x = -500; pts[3].y = wnd->median.y + pitch + 500;
  pts[4].x = -500; pts[4].y = wnd->median.y + pitch;

  // rotate the brown rect
  for (pt = 0; pt < 5; pt++)
    rotate_point(&wnd->median, wnd->roll, &pts[pt]);
  pts[4] = pts[0];

  polygon(canvas, wnd_rect, 0, color_brown, 5, pts);

  /////////////////////////////////////////////////////////////////////////////
  //	Draw the pitch indicator

  //background_color(color_black);
  //pen(color_white);

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
    rotate_point(&wnd->median, wnd->roll, &slip_indicator[i]);
    rotate_point(&wnd->median, wnd->roll, &slip_indicator[i + 1]);

    polyline(canvas, wnd_rect, color_green, 2, slip_indicator + i);
    }

  /////////////////////////////////////////////////////////////////////////////
  //	Draw the rotated roll/pitch indicator

  // The window height is 240 pixels, and each 25 deg of pitch
  // is 20 pixels.  So the display is +/- 150 degrees (120/20)*25
  //
  int32_t pitch_range = (wnd->median.y / 20) * 25;
  int32_t pitch_angle = ((int32_t)(wnd->pitch * 10)) + pitch_range;
  int32_t line = 0;

  // now we draw the pitch line(s)
  if (pitch_angle % 25)
    {
    int32_t new_pitch = ((pitch_angle / 25) + 1) * 25;
    line = pitch_angle - new_pitch;
    pitch_angle = new_pitch;
    }

  while (line < wnd_rect->bottom)
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

      rotate_point(&wnd->median, wnd->roll, &pts[0]);
      rotate_point(&wnd->median, wnd->roll, &pts[1]);

      polyline(canvas, wnd_rect, color_white, 2, pts);
      // calc the text angle as 10, 20..90
      char text_angle[3] =
        {
        (abs(pitch_angle) / 100) + '0',
        '0',
        0
        };

      if (text_angle > 0)
        {

        // calc the left/right center point
        point_t pt_left = { wnd->median.x - 48, line };
        point_t pt_right = { wnd->median.x + 47, line };

        rotate_point(&wnd->median, wnd->roll, &pt_left);
        rotate_point(&wnd->median, wnd->roll, &pt_right);

        // we now calc the left and right points to write the text to
        pt_left.x -= 9; pt_left.y -= 9;
        pt_right.x -= 9; pt_right.y -= 9;

        draw_text(canvas, wnd_rect, wnd->base.name_font, color_white, color_hollow,
          2, text_angle, &pt_left, 0, 0, 0);

        draw_text(canvas, wnd_rect, wnd->base.name_font, color_white, color_hollow,
          2, text_angle, &pt_right, 0, 0, 0);
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

      rotate_point(&wnd->median, wnd->roll, &pts[0]);
      rotate_point(&wnd->median, wnd->roll, &pts[1]);

      polyline(canvas, wnd_rect, color_white, 2, pts);

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

      rotate_point(&wnd->median, wnd->roll, &pts[0]);
      rotate_point(&wnd->median, wnd->roll, &pts[1]);

      polyline(canvas, wnd_rect, color_white, 2, pts);

      pitch_angle -= 25;
      line += 20;
      }
    }

  /////////////////////////////////////////////////////////////////////////////
  // Draw the angle-of-attack indicator
  //
  // this is 60 pixels up/down

  if (wnd->show_aoa)
    {
    bool show_alarm_bars = false;
    bool show_warning_bars = false;
    // calc the effective AOA
    int16_t aoa = min(mfd.aircraft.critical_aoa, wnd->aoa_degrees);
    int16_t pixels = 0;
    if (aoa >= mfd.aircraft.critical_aoa)
      {
      pixels = 66;        // show the chevrons for red
      show_alarm_bars = true;
      show_warning_bars = true;
      }
    else if (aoa > mfd.aircraft.approach_aoa)
      {
      int16_t range_span = mfd.aircraft.critical_aoa - mfd.aircraft.approach_aoa;
      range_span <<= 8;
      int16_t range = aoa - mfd.aircraft.approach_aoa;
      range <<= 8;

      pixels = 42 + ((range * ((24 * 256) / range_span)) >> 8);
      show_warning_bars = true;
      }
    else
      {
      int16_t range_span = mfd.aircraft.approach_aoa;
      range_span <<= 8;
      int16_t range = max(aoa, 0);
      range <<= 8;

      pixels = 0 + ((range * ((36 * 256) / range_span)) >> 8);
      }



    int16_t offset;
    // this will draw 13 lines
    // 2 red chevrons, 3 yellow lines, and 7 green lines
    if (show_alarm_bars)
      {
      // draw the PUSH annunciator
      point_t pt = 
        { 
        wnd->median.x - (wnd->aoa_warning_extent.dx >> 1),
        wnd->median.y - 72 + pixels - (wnd->aoa_warning_extent.dy +1)
        };

      draw_text(canvas, wnd_rect, wnd->base.name_font, color_red, color_hollow,
        sizeof(AOA_warning_str), AOA_warning_str, &pt, 0, 0, 0);

      for (offset = 72; offset > 60; offset -= 6)
        {
        // draw red chevron.
        point_t chevron[3] =
          {
            { wnd->median.x - 15, wnd->median.y - offset + pixels },
            { wnd->median.x, wnd->median.y - offset + 4 + pixels },
            { wnd->median.x + 15, wnd->median.y - offset + pixels }
          };

        polyline(canvas, wnd_rect, color_red, 3, chevron);
        }
      }
    else
      offset = 60;

    // split the yellow line into 2 sections
    point_t marker[2];

    if (show_warning_bars)
      {
      for (; offset > 48; offset -= 6)
        {
        // draw red chevron.
        point_t chevron[3] =
          {
            { wnd->median.x - 15, wnd->median.y - offset + pixels },
            { wnd->median.x, wnd->median.y - offset + 4 + pixels },
            { wnd->median.x + 15, wnd->median.y - offset + pixels }
          };

        polyline(canvas, wnd_rect, color_yellow, 3, chevron);
        }

      marker[0].x = wnd->median.x - 15;
      marker[0].y = wnd->median.y - offset + pixels;

      marker[1].x = wnd->median.x - 5;
      marker[1].y = wnd->median.y - offset + pixels;      

      polyline(canvas, wnd_rect, color_yellow, 2, marker);

      marker[0].x = wnd->median.x + 5;
      marker[1].x = wnd->median.x + 15;
      polyline(canvas, wnd_rect, color_yellow, 2, marker);

      offset -= 6;

      marker[0].x = wnd->median.x - 15;
      marker[0].y = wnd->median.y - offset + pixels;
      marker[1].y = wnd->median.y - offset + pixels;

      polyline(canvas, wnd_rect, color_yellow, 2, marker);

      offset -= 6;

      marker[0].y = wnd->median.y - offset + pixels;
      marker[1].y = wnd->median.y - offset + pixels;

      polyline(canvas, wnd_rect, color_yellow, 2, marker);

      offset -= 6;
      }
    else
      offset = 36;

    // draw the target approach AOA

    marker[0].x = wnd->median.x - 15;
    marker[0].y = wnd->median.y - offset + pixels;

    marker[1].x = wnd->median.x - 8;
    marker[1].y = wnd->median.y - offset + pixels;

    polyline(canvas, wnd_rect, color_green, 2, marker);

    marker[0].x = wnd->median.x + 8;
    marker[1].x = wnd->median.x + 15;
    polyline(canvas, wnd_rect, color_green, 2, marker);

    // if the AOA is +- 1 from the approach aoa draw the target
    bool fill_target = aoa >= mfd.aircraft.approach_aoa - 1 &&
      aoa <= mfd.aircraft.approach_aoa;

    // target approach AOA Circle
    ellipse(canvas, wnd_rect, color_green, fill_target ? color_green : color_hollow,
      rect_create(wnd->median.x - 5, wnd->median.y - offset - 5 + pixels,
        wnd->median.x + 5, wnd->median.y - offset + 5 + pixels, &rect));

    marker[0].x = wnd->median.x - 15;
    marker[1].x = wnd->median.x + 15;

    for (offset -= 6; offset > 0; offset -= 6)
      {
      marker[0].y = wnd->median.y - offset + pixels;
      marker[1].y = wnd->median.y - offset + pixels;

      polyline(canvas, wnd_rect, color_green, 2, marker);
      }
    }

  /////////////////////////////////////////////////////////////////////////////
  // draw the aircraft image
  point_t aircraft_points[2];

  aircraft_points[0].x = wnd->median.x - 7; aircraft_points[0].y = wnd->median.y;
  aircraft_points[1].x = wnd->median.x - 22; aircraft_points[1].y = wnd->median.y;

  polyline(canvas, wnd_rect, color_white, 2, aircraft_points);

  aircraft_points[0].x = wnd->median.x + 7; aircraft_points[0].y = wnd->median.y;
  aircraft_points[1].x = wnd->median.x + 22; aircraft_points[1].y = wnd->median.y;

  polyline(canvas, wnd_rect, color_white, 2, aircraft_points);

  aircraft_points[0].x = wnd->median.x; aircraft_points[0].y = wnd->median.y - 7;
  aircraft_points[1].x = wnd->median.x; aircraft_points[1].y = wnd->median.y - 15;

  polyline(canvas, wnd_rect, color_white, 2, aircraft_points);

  ellipse(canvas, wnd_rect, color_white, color_hollow,
    rect_create(wnd->median.x - 7, wnd->median.y - 7,
      wnd->median.x + 7, wnd->median.y + 7, &rect));

  /////////////////////////////////////////////////////////////////////////////
  // draw the glide slope and localizer indicators
  if (wnd->glideslope_aquired && wnd->show_glideslope)
    {
    // draw the marker, 0.7 degrees = 59 pixels

    float deviation = (float)(max(-1.2, min(1.2, wnd->glideslope / 100.0)));

    int32_t pixels = (int32_t)(deviation / (1.0 / pixels_per_degree));

    pixels += 120;

    point_t pts[2] = {
      { wnd_rect->right - 12, wnd->median.y },
      { wnd_rect->right, wnd->median.y }
      };

    polyline(canvas, wnd_rect, color_white, 2, pts);

    static rect_t glideslope[4] =
      {
        { 230,  57, 238,  65 },
        { 230,  86, 238,  94 },
        { 230, 146, 238, 154 },
        { 230, 175, 238, 183 }
      };

    // rest are hollow
    ellipse(canvas, wnd_rect, color_white, color_hollow, &glideslope[0]);
    ellipse(canvas, wnd_rect, color_white, color_hollow, &glideslope[1]);
    ellipse(canvas, wnd_rect, color_white, color_hollow, &glideslope[2]);
    ellipse(canvas, wnd_rect, color_white, color_hollow, &glideslope[3]);

    // black filled ellipse
    ellipse(canvas, wnd_rect, color_white, color_black,
      rect_create(230, pixels - 4, 238, pixels + 4, &rect));

    }

  if (wnd->localizer_aquired && wnd->show_glideslope)
    {
    // draw the marker, 1.0 degrees = 74 pixels

    float deviation = (float)(max(-1.2, min(1.2, wnd->localizer / 100.0)));

    int32_t pixels = (int32_t)(deviation / (1.0 / pixels_per_degree));

    pixels += wnd->median.x;

    point_t pts[2] = {
      { wnd->median.x, wnd->median.y - 15 },
      { wnd->median.x, wnd->median.y }
      };

    polyline(canvas, wnd_rect, color_white, 2, pts);

    static rect_t localizer[4] = {
      {  57, 230,  65, 238 },
      {  86, 230,  94, 238 },
      { 146, 230, 154, 238 },
      { 175, 230, 183, 238 }
      };

    // rest are hollow
    ellipse(canvas, wnd_rect, color_white, color_hollow, &localizer[0]);
    ellipse(canvas, wnd_rect, color_white, color_hollow, &localizer[1]);
    ellipse(canvas, wnd_rect, color_white, color_hollow, &localizer[2]);
    ellipse(canvas, wnd_rect, color_white, color_hollow, &localizer[3]);

    // black filled ellipse

    ellipse(canvas, wnd_rect, color_white, color_black,
      rect_create(pixels - 4, 230, pixels + 4, 238, &rect));

    }

  /////////////////////////////////////////////////////////////////////////////
  // Draw the roll indicator
  // Draw the aircraft pointer at the top of the window.

  rectangle(canvas, wnd_rect, color_white, color_black, &wnd->skid_indicator);

  gdi_dim_t balance_height = wnd->skid_indicator.bottom - wnd->skid_indicator.top;

  // verticalbars
  point_t balance_points[2] = {
    { wnd->median.x - (balance_height >>1) - 1, wnd->skid_indicator.top },
    { wnd->median.x - (balance_height >> 1) - 1, wnd->skid_indicator.bottom },
  };

  polyline(canvas, wnd_rect, color_white, 2, balance_points);

  balance_points[0].x = wnd->median.x + (balance_height >> 1) + 1;
  balance_points[1].x = wnd->median.x + (balance_height >> 1) + 1;

  polyline(canvas, wnd_rect, color_white, 2, balance_points);

  // the roll indicator is shifted left/right by the yaw angle
  int16_t offset = 0- min(mfd.aircraft.yaw_max, max(-mfd.aircraft.yaw_max, wnd->yaw));

  offset *= wnd->yaw_scale;

  // draw the white ball on a black background
  ellipse(canvas, wnd_rect, color_white, color_white,
    rect_create(wnd->median.x - (balance_height >> 1) + offset,
                wnd->skid_indicator.top,
                wnd->median.x + (balance_height >> 1) + offset,
                wnd->skid_indicator.bottom, &rect));

  }

result_t attitude_wndproc(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  attitude_widget_t* wnd = (attitude_widget_t*)wnddata;
  bool changed = false;
  float v;
  int16_t angle;

  switch (get_can_id(msg))
    {
    case id_paint:
      on_paint_widget(hwnd, msg, wnddata);
      break;
    case id_yaw_rate:
      get_param_float(msg, &v);
      angle = (int16_t)radians_to_degrees(v);
      while (angle > 179)
        angle -= 360;

      while (angle < -180)
        angle += 360;

      changed = wnd->yaw != angle;
      wnd->yaw = angle;

      break;
    case id_roll_angle:
      get_param_float(msg, &v);
      angle = (int16_t)radians_to_degrees(0 - v);
      while (angle > 179)
        angle -= 360;

      while (angle < -180)
        angle += 360;

      if (angle > 90 || angle < -90)
        angle -= 180;

      while (angle < -180)
        angle += 360;

      angle = min(90, max(-90, angle));

      changed = wnd->roll != angle;
      wnd->roll = angle;
      break;
    case id_pitch_angle:
      get_param_float(msg, &v);
      angle = (int16_t)radians_to_degrees(v);

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
      break;
    case id_angle_of_attack:
      get_param_int16(msg, &angle);
      changed = wnd->aoa_degrees != angle;
      wnd->aoa_degrees = angle;

      break;
    }

  if (changed)
    invalidate(hwnd);

  // pass to default
  return defwndproc(hwnd, msg, wnddata);
  }


result_t create_attitude_widget(handle_t parent, uint32_t flags, attitude_widget_t* wnd, handle_t* out)
  {
  result_t result;
  handle_t hndl;
  if (failed(result = create_widget(parent, flags, attitude_wndproc, &wnd->base, &hndl)))
    return result;

  wnd->base.on_paint = on_paint;

  text_extent(wnd->base.name_font, sizeof(AOA_warning_str), AOA_warning_str, &wnd->aoa_warning_extent);
  
  // calculate once when the aircraft performance is known
  wnd->yaw_scale = (rect_width(&wnd->skid_indicator) - rect_height(&wnd->skid_indicator)) / (2 * mfd.aircraft.yaw_max);

  if (out != 0)
    *out = hndl;

  return s_ok;
  }