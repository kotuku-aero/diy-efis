#include "../../include/compass_widget.h"

#include <stdio.h>

static int32_t clip_degrees(int32_t value)
  {
  while (value < 0)
    value += 360;
  while (value > 359)
    value -= 360;

  return value;
  }

static int32_t round_up(int32_t value, uint32_t multiple)
  {
  bool negative = false;
  if (value < 0)
    {
    value = 0 - value;
    negative = true;
    }

  if (value % multiple == 0)
    return negative ? 0 - value : value;

  value = ((value / multiple) + 1) * multiple;

  if (negative)
    value = 0 - value;

  return value;
  }

static gdi_dim_t calculate_position(compass_widget_t* wnd, const extent_t* ex, int marker_degrees, gdi_dim_t pixels_per_degree)
  {
  int32_t marker = clip_degrees(marker_degrees);
  gdi_dim_t x = ((marker - wnd->magnetic_heading) * pixels_per_degree) + (wnd->hdg_offset);

  if (x > ex->dx)
    x -= 360 * pixels_per_degree;

  if (x < 0)
    x = 0;
  else if (x > ex->dx)
    x = ex->dx - 1;

  return x;
  }

static void on_paint(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnddata)
  {
  compass_widget_t* wnd = (compass_widget_t*)wnddata;

  extent_t ex;
  rect_extents(wnd_rect, &ex);

  on_paint_widget_background(canvas, wnd_rect, msg, &wnd->base);

  // center the display to the assigned center
  gdi_dim_t cx = wnd->hdg_offset;
  gdi_dim_t pixels_per_degree = 4;

  int32_t total_degrees = (ex.dx / pixels_per_degree);
  // calculate the left and right degrees
  // complicated by the fact that the display can be slightly asymmetrical
  // depending on the hdg_offset
  // calculate the ratio for pixels.
  int32_t left_degrees = wnd->magnetic_heading - (cx / pixels_per_degree);
  int32_t right_degrees = wnd->magnetic_heading + total_degrees - (cx / pixels_per_degree);

  // draw the compass based on 80 degrees
  // should always be a window of an 80 multiple

  for (int32_t marker = round_up(left_degrees, 5); marker <= right_degrees; marker += 5)
    {
    gdi_dim_t x = calculate_position(wnd, &ex, marker, pixels_per_degree);
    // draw the tick mark
    bool index = (marker % 10) == 0;

    point_t pt[2] = {
      {x, index ? ex.dy - 10 : ex.dy - 5},
      {x, ex.dy }
      };

    // draw the tick
    polyline(canvas, wnd_rect, color_white, 2, pt);

    if (index)
      {
      // draw the text
      char text[8];
      snprintf(text, sizeof(text), "%d", clip_degrees(marker));
      uint16_t len = (uint16_t)strlen(text);
      extent_t txt_ex;
      text_extent(wnd->base.name_font, len, text, &txt_ex);
      point_t text_pt = { x - (txt_ex.dx >> 1), 0 };

      draw_text(canvas, wnd_rect, wnd->base.name_font, color_white, color_hollow, len, text, &text_pt, wnd_rect, eto_none, 0);
      }
    }

  // calculate the heading bug and draw it
  gdi_dim_t bug_half_width = 25;
  gdi_dim_t bug_x = 0;  // assume left of the screen
  point_t bug_pts[6];

  bug_x = calculate_position(wnd, &ex, wnd->heading_bug, pixels_per_degree);

  // draw the heading bug in purple

  bug_pts[0].x = bug_x; bug_pts[0].y = ex.dy - 1;
  bug_pts[1].x = bug_x - 25; bug_pts[1].y = ex.dy - 10;
  bug_pts[2].x = bug_pts[1].x; bug_pts[2].y = bug_pts[0].y;
  bug_pts[3].x = bug_pts[0].x; bug_pts[3].y = bug_pts[0].y;

  polygon(canvas, wnd_rect, color_purple, color_purple, 4, bug_pts);

  bug_pts[1].x += 50;
  bug_pts[2].x += 50;
  polygon(canvas, wnd_rect, color_purple, color_purple, 4, bug_pts);

  // draw the track bug in green
  bug_x = calculate_position(wnd, &ex, wnd->track, pixels_per_degree);

  bug_pts[0].x = bug_x; bug_pts[0].y = ex.dy - 1;
  bug_pts[1].x = bug_x - 25; bug_pts[1].y = ex.dy - 10;
  bug_pts[2].x = bug_pts[1].x; bug_pts[2].y = bug_pts[0].y;
  bug_pts[3].x = bug_pts[0].x; bug_pts[3].y = bug_pts[0].y;

  // calculate the track bug and draw it
  polygon(canvas, wnd_rect, color_green, color_green, 4, bug_pts);

  bug_pts[1].x += 50;
  bug_pts[2].x += 50;
  polygon(canvas, wnd_rect, color_green, color_green, 4, bug_pts);

  //// draw the wind direction
  //uint32_t wind_x = (wnd->wind_direction - left_degrees) * pixels_per_degree;
  //point_t wind_pt[2] = {
  //  {wind_x, ex.dy - 10},
  //  {wind_x, ex.dy }
  //  };
  //polyline(canvas, wnd_rect, color_blue, 3, wind_pt);

  //// draw the wind speed
  //char wind_text[8];
  //snprintf(wind_text, sizeof(wind_text), "%d", wnd->wind_speed);
  //uint16_t wind_len = strlen(wind_text);
  //extent_t wind_ex;
  //text_extent(wnd->base.name_font, wind_len, wind_text, &wind_ex);
  //point_t wind_text_pt = { wind_x - (wind_ex.dx >> 1), 0 };
  //draw_text(canvas, wnd_rect, wnd->base.name_font, color_blue, color_hollow, wind_len, wind_text, &wind_text_pt, wnd_rect, eto_none, 0);


  char heading_text[8];
  snprintf(heading_text, sizeof(heading_text), "%d", wnd->magnetic_heading);
  uint16_t heading_len = (uint16_t)strlen(heading_text);
  extent_t heading_ex;
  text_extent(wnd->base.name_font, heading_len, heading_text, &heading_ex);

  bug_half_width = 25;

  // draw the heading background
  bug_pts[0].x = cx; bug_pts[0].y = ex.dy;
  bug_pts[1].x = cx - bug_half_width; bug_pts[1].y = ex.dy - 10;
  bug_pts[2].x = cx - bug_half_width; bug_pts[2].y = ex.dy - 20;
  bug_pts[3].x = cx + bug_half_width; bug_pts[3].y = ex.dy - 20;
  bug_pts[4].x = cx + bug_half_width; bug_pts[4].y = ex.dy - 10;
  bug_pts[5].x = cx; bug_pts[5].y = ex.dy;

  polygon(canvas, wnd_rect, color_white, color_white, 6, bug_pts);

  point_t heading_text_pt = { cx - (heading_ex.dx >> 1), 1 };
  draw_text(canvas, wnd_rect, wnd->base.name_font, color_black, color_white, heading_len, heading_text, &heading_text_pt, wnd_rect, eto_none, 0);
  }

static result_t get_param_as_degrees(const canmsg_t* msg, uint16_t* out)
  {
  result_t result;
  float flt;
  if (succeeded(result = get_param_float(msg, &flt)))
    {
    int16_t heading = radians_to_degrees(flt);

    while (heading < 0)
      heading += 360;

    while (heading > 359)
      heading -= 360;

    *out = (uint16_t)heading;
    return s_ok;
    }
  return result;
  }


static result_t compass_wndproc(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  bool changed = false;
  compass_widget_t* wnd = (compass_widget_t*)wnddata;

  uint16_t u16;

  switch (get_can_id(msg))
    {
    case id_paint:
      on_paint_widget(hwnd, msg, wnddata);
      break;

    case id_magnetic_heading:
      // this is the magnet
      if (succeeded(get_param_as_degrees(msg, &u16)))
        {
        changed = wnd->magnetic_heading != u16;
        wnd->magnetic_heading = u16;
        }
      break;
    case id_heading:
      // this is the heading bug that is set by the pilot
      // and published by the ADAHRS
      if (succeeded(get_param_uint16(msg, &u16)))
        {
        changed = wnd->heading_bug != u16;
        wnd->heading_bug = u16;
        }
      break;
    case id_true_heading:
      if (succeeded(get_param_as_degrees(msg, &u16)))
        {
        changed = wnd->true_heading != u16;
        wnd->true_heading = u16;
        }
      break;
    case id_selected_course:
      if (succeeded(get_param_uint16(msg, &u16)))
        {
        changed = wnd->course != u16;
        wnd->course = u16;
        }
      break;
    case id_track:
      if (succeeded(get_param_as_degrees(msg, &u16)))
        {
        changed = wnd->track != u16;
        wnd->track = u16;
        }
      break;
    case id_wind_speed:
      {
      float v;
      if (succeeded(get_param_float(msg, &v)))
        {
        int16_t value = (int16_t)meters_per_second_to_knots(v);

        if (value < 0)
          value = 0;

        changed = wnd->wind_speed != value;
        wnd->wind_speed = value;
        }
      }
      break;
    case id_wind_direction:
      if (succeeded(get_param_as_degrees(msg, &u16)))
        {
        changed = wnd->wind_direction != u16;
        wnd->wind_direction = u16;
        }
      break;
    }

  if (changed)
    invalidate(hwnd);

  return widget_wndproc(hwnd, msg, wnddata);
  }


result_t create_compass_widget(handle_t parent, uint32_t flags, compass_widget_t* wnd, handle_t* out)
  {
  result_t result;
  handle_t hndl;
  if (failed(result = create_widget(parent, flags, compass_wndproc, &wnd->base, &hndl)))
    return result;

  wnd->base.on_paint = on_paint;

  if (out != 0)
    *out = hndl;

  return s_ok;
  }