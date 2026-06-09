#include "../../include/hsi_widget.h"
#include "../../include/proton.h"

static void on_paint(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* _msg, void* wnddata)
  {
  hsi_widget_t* wnd = (hsi_widget_t*)wnddata;

  extent_t ex;
  rect_extents(wnd_rect, &ex);

  on_paint_widget_background(canvas, wnd_rect, _msg, wnddata);

  point_t pt = { 0, 0 };

  /////////////////////////////////////////////////////////////////////////////
  //
  // Draw the HSI Indicator

  const int32_t mark_start = 12;
  const int32_t center_x = ex.dx >> 1;
  const int32_t center_y = ex.dy >> 1;
  const int32_t window_x = ex.dx;
  const int32_t window_y = ex.dy;
  const int32_t border = 10;
  const int32_t pixels_per_nm_cdi = 6;
  const point_t median = { center_x, center_y };
  const int32_t major_mark = mark_start + 16;
  const int32_t minor_mark = mark_start + 8;
  const int32_t font_x_y = 19;
  const int32_t font_center = (font_x_y >> 1) + 1;
  const int32_t font_ordinal = major_mark + font_center;
  // draw the marker
  point_t pts[4];

  // start at 0
  int32_t i;
  int32_t index = -wnd->direction;
  for (i = 0; i < 12; index += 30, i++)
    {

    pts[0].x = center_x; pts[0].y = mark_start;
    pts[1].x = center_x; pts[1].y = major_mark;
    rotate_point(&median, index, &pts[0]);
    rotate_point(&median, index, &pts[1]);

    polyline(canvas, wnd_rect, color_white, 2, pts);

    bool do_minor_mark = false;
    int16_t minor_index;
    for (minor_index = 0; minor_index < 30; minor_index += 5)
      {
      pts[0].x = center_x; pts[0].y = mark_start;
      pts[1].x = center_x; pts[1].y = do_minor_mark ? minor_mark : major_mark;

      rotate_point(&median, index + minor_index, &pts[0]);
      rotate_point(&median, index + minor_index, &pts[1]);

      polyline(canvas, wnd_rect, color_white, 2, pts);

      do_minor_mark = !do_minor_mark;
      }
    }

  index = -wnd->direction;
  for (int i = 0; i < 12; i++, index += 30)
    {
    pts[0].x = center_x;
    pts[0].y = font_ordinal;
    int16_t rotn = (index >= 360) ? index - 360 : index;

    rotate_point(&median, rotn, &pts[0]);

    // each of these is a font for
    // N 3 6 E 12 15 S 21 24 W 30 33
    // which are requested by characters
    // 0 .. 11
    // and they contain the fonts rotated by the index offset
    // for the 12 characters
    // the fonts are stored in the hsi_fonts and are loaded
    // when the application starts  The fonts are called
    // hsi_neo_0 .. hsi_neo_359 and are 9 pt fonts

    static const char* marks[] = {
      "N",
      "3",
      "6",
      "E",
      "12",
      "15",
      "S",
      "21",
      "24",
      "W",
      "30",
      "33",
      "N"
      };

    draw_text(canvas, wnd_rect, &neo_9_bold_font, color_white, color_black,
      0, marks[i],
      point_create(pts[0].x - font_center, pts[0].y - font_center, &pt),
      0, 0, 0);

    }
  // we now draw the text onto the canvas.  The text has a 23x23 pixel
  // block so the center is 12, 12.
  // 
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
    rotate_point(&median, rotation, &track_marker[i]);

  polygon(canvas, wnd_rect, color_grey, color_hollow, 4, track_marker);

  point_t track_line[2] = {
    { center_x, center_y - 88 },
    { median.x, median.y }
    };

  rotate_point(&median, rotation, &track_line[0]);

  polyline(canvas, wnd_rect, color_grey, 2, track_line);

  ///////////////////////////////////////////////////////////////////////////
  // Draw the CDI

  rotation = wnd->course - wnd->direction;

  int32_t dist;
  for (dist = -10; dist < 11; dist += 2)
    {
    if (dist == 0)
      continue;

    point_t pts[2] = {
      { center_x + (pixels_per_nm_cdi * dist), center_y - 5 },
      { center_x + (pixels_per_nm_cdi * dist), center_y + 5 }
      };

    rotate_point(&median, rotation, &pts[0]);
    rotate_point(&median, rotation, &pts[1]);

    polyline(canvas, wnd_rect, color_green, 2, pts);
    }

  // draw the CDI Marker head next
  point_t cdi_pts[4] = {
    { center_x, center_y - 97 },
    { center_x - 6, center_y - 88 },
    { center_x + 6, center_y - 88 },
    { center_x, center_y - 97 }
    };

  for (i = 0; i < 4; i++)
    rotate_point(&median, rotation, &cdi_pts[i]);

  polygon(canvas, wnd_rect, 0, color_green, 4, cdi_pts);

  // we now convert the deviation to pixels.
  // 1 degree = 24 pixels
  float cdi_var = pixels_per_nm_cdi * ((float)wnd->deviation / 10);

  int32_t cdi = (int32_t)max(-66.0f, min(66.0f, roundf(cdi_var)));

  pts[0].x = center_x; pts[0].y = center_y - 98;
  pts[1].x = center_x; pts[1].y = center_y - 50;

  rotate_point(&median, rotation, &pts[0]);
  rotate_point(&median, rotation, &pts[1]);

  polyline(canvas, wnd_rect, color_green, 2, pts);

  pts[0].x = center_x; pts[0].y = center_y + 50;
  pts[1].x = center_x; pts[1].y = center_y + 98;

  rotate_point(&median, rotation, &pts[0]);
  rotate_point(&median, rotation, &pts[1]);

  polyline(canvas, wnd_rect, color_green, 2, pts);

  pts[0].x = center_x + cdi; pts[0].y = center_y - 48;
  pts[1].x = pts[0].x; pts[1].y = center_y + 48;

  rotate_point(&median, rotation, &pts[0]);
  rotate_point(&median, rotation, &pts[1]);

  polyline(canvas, wnd_rect, color_green, 2, pts);

  /////////////////////////////////////////////////////////////////////////////
  //	Draw the heading bug.

  int hdg = wnd->heading_bug - wnd->direction;

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
    rotate_point(&median, hdg, &heading_points[i]);

  polygon(canvas, wnd_rect, color_magenta, color_magenta, 8, heading_points);

  heading_points[0].x = center_x - 5; heading_points[0].y = 0;
  heading_points[1].x = center_x + 5; heading_points[1].y = 0;
  heading_points[2].x = center_x; heading_points[2].y = 10;
  heading_points[3].x = center_x - 5; heading_points[3].y = 0;

  polygon(canvas, wnd_rect, color_white, color_white, 4, heading_points);

  /////////////////////////////////////////////////////////////////////////////
  // Draw the wind direction indicator.
  // it is in the top left of the HSI and has an arrow that is
  // relative to the magnetic heading of the aircraft and the
  // speed/magnetic heading in the form deg/speed so
  // so for a wind of 15 knots at 50 degrees true we would
  // show 050/15.  If the aircraft heading is 240 degrees we
  // would see a wind vector on the tail of 40 degrees toward the aircraft
  // the wind direction is shown as a yellow triangle around the HSI indicator
  // the text allows for 3 characters with an maximum width of 23 pixels each
  // so the allowance is 69 by 64 pixels

  int16_t relative_wind = wnd->wind_direction - wnd->direction;
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
    rotate_point(&median, relative_wind, &wind_bug[i]);

  polygon(canvas, wnd_rect, color_yellow, color_yellow, 4, wind_bug);

  // now the text in upper left

  char msg[32];  sprintf(msg, "%03.3d", wnd->wind_direction);
  uint16_t length = (uint16_t)strlen(msg);

  extent_t pixels;
  text_extent(wnd->base.name_font, length, msg, &pixels);
  draw_text(canvas, wnd_rect, wnd->base.name_font, color_yellow, color_hollow,
    length, msg, point_create(25 - (pixels.dx >> 1), 2, &pt), 0, 0, 0);

  sprintf(msg, "%d", wnd->wind_speed);
  length = (uint16_t)strlen(msg);
  text_extent(wnd->base.name_font, length, msg, &pixels);
  draw_text(canvas, wnd_rect, wnd->base.name_font, color_yellow, color_hollow,
    length, msg, point_create(25 - (pixels.dx >> 1), 13, &pt), 0, 0, 0);

  /////////////////////////////////////////////////////////////////////////////
  // Draw the estimated time to waypoint. drawn in top right as distance/time
  sprintf(msg, "%d", wnd->distance_to_waypoint);
  length = (uint16_t)strlen(msg);
  text_extent(wnd->base.name_font, length, msg, &pixels);
  draw_text(canvas, wnd_rect, wnd->base.name_font, color_yellow, color_hollow,
    length, msg, point_create(window_x - 25 - (pixels.dx >> 1), 2, &pt), 0, 0, 0);

  sprintf(msg, "%02.2d:%02.2d", wnd->time_to_waypoint / 60, wnd->time_to_waypoint % 60);
  length = (uint16_t)strlen(msg);
  text_extent(wnd->base.name_font, length, msg, &pixels);
  draw_text(canvas, wnd_rect, wnd->base.name_font, color_yellow, color_hollow,
    length, msg, point_create(window_x - 25 - (pixels.dx >> 1), 13, &pt), 0, 0, 0);

  sprintf(msg, "%s", wnd->waypoint_name);
  length = (uint16_t)strlen(msg);
  text_extent(wnd->base.name_font, length, msg, &pixels);
  draw_text(canvas, wnd_rect, wnd->base.name_font, color_yellow, color_hollow,
    length, msg, point_create(window_x - 25 - (pixels.dx >> 1), 24, &pt), 0, 0, 0);

  /////////////////////////////////////////////////////////////////////////////
  // Draw the HMode in the bottom left
  // the HMode is a 3 character string that is drawn in the bottom left
  // of the HSI.  The HMode is the current mode of the HSI and is
  // one of the following:
  //  OFF
  //  HDG - current heading
  //  CRS - Current AP Course
  //  INT - Intercept track
  const char* tmp_msg = nullptr;
  uint16_t nav_hdg;
  uint32_t hmode = (wnd->autopilot_mode) & HORZ_MODE_MASK;
  switch (hmode)
    {
    case 0:
      tmp_msg = "HNAV OFF";
      break;
    case NAV_MODE:
      tmp_msg = "CRS";
      nav_hdg = wnd->course;
      break;
    case HDG_MODE:
      tmp_msg = "HDG";
      nav_hdg = wnd->heading;
      break;
    case APR_MODE:
      tmp_msg = "INT";
      nav_hdg = wnd->course;
      break;
    case REV_LEFT_MODE:
      tmp_msg = "REV LEFT";
      nav_hdg = wnd->heading;
      break;
    case REV_RIGHT_MODE:
      tmp_msg = "REV RGT";
      nav_hdg = wnd->heading;
      break;
    }

  // draw the currently selected nav
    if (hmode != 0)
      {
      sprintf(msg, "%03.3d", nav_hdg);
      length = strlen(msg);
      text_extent(wnd->base.name_font, length, msg, &pixels);
      draw_text(
          canvas, wnd_rect, wnd->base.name_font, color_magenta, color_hollow,
          length, msg,
          point_create(wnd_rect->left +2, wnd_rect->bottom - 30, &pt),
          0, 0, 0);
      }

  length = (uint16_t)strlen(tmp_msg);
  text_extent(wnd->base.name_font, length, tmp_msg, &pixels);
  draw_text(canvas, wnd_rect, wnd->base.name_font, color_magenta, color_hollow,
    length, tmp_msg, point_create(31 - (pixels.dx >> 1), wnd_rect->bottom - 15, &pt), 0, 0, 0);

  /////////////////////////////////////////////////////////////////////////////
  // Draw the VMode in the bottom right
  // the VMode is a 3 character string that is drawn in the bottom right
  // of the HSI.  The VMode is the current mode of the VSI and is
  // one of the following:
  //  OFF
  //  ALT - Altitude Hold
  //  SEEK - Altitude Seek

  switch ((wnd->autopilot_mode) & VERT_MODE_MASK)
    {
    case 0:
      tmp_msg = "VNAV OFF";
      break;
    case VERT_SEEK_MODE:
    case ALT_MODE:
      tmp_msg = "ALT";
      break;
    case IAS_MODE:
      tmp_msg = "RATE";
      break;
    }

  // draw the rate
  sprintf(msg, "R:%04.3d", wnd->ap_vs);
  length = strlen(msg);
  text_extent(wnd->base.name_font, length, msg, &pixels);
  draw_text(canvas, wnd_rect, wnd->base.name_font, color_magenta, color_hollow, 
    length, msg, point_create(wnd_rect->right - pixels.dx, wnd_rect->bottom - 45, &pt), 0, 0, 0);

  // draw the assigned altitude
  sprintf(msg, "A:%05.3d", wnd->ap_altitude);
  length = strlen(msg);
  text_extent(wnd->base.name_font, length, msg, &pixels);
  draw_text(canvas, wnd_rect, wnd->base.name_font, color_magenta, color_hollow,
      length, msg, point_create(wnd_rect->right - pixels.dx, wnd_rect->bottom - 30, &pt), 0, 0, 0);

  length = (uint16_t)strlen(tmp_msg);
  text_extent(wnd->base.name_font, length, tmp_msg, &pixels);
  draw_text(canvas, wnd_rect, wnd->base.name_font, color_magenta, color_hollow,
    length, tmp_msg, point_create(window_x - 31 - (pixels.dx >> 1), wnd_rect->bottom - 15, &pt), 0, 0, 0);

  }

static result_t change_ap_mode(hsi_widget_t* widget)
  {
  canmsg_t msg;
  create_can_msg_uint16(&msg, id_autopilot_mode, widget->autopilot_mode);
  return can_send(&msg, INDEFINITE_WAIT);
  }

static result_t change_hs_mode(hsi_widget_t* widget, uint16_t mode)
  {
  widget->autopilot_mode &= ~HORZ_MODE_MASK;
  widget->autopilot_mode |= mode;
  return change_ap_mode(widget);
  }

static result_t change_vs_mode(hsi_widget_t* widget, uint16_t mode)
  {
  widget->autopilot_mode &= ~VERT_MODE_MASK;
  widget->autopilot_mode |= mode;
  return change_ap_mode(widget);
  }

void on_toggle_hs_mode(menu_widget_t* menu, widget_t* hsi)
  {
  hsi_widget_t* wnd = (hsi_widget_t*)hsi;
  switch ((wnd->autopilot_mode) & HORZ_MODE_MASK)
    {
    case 0:
    case REV_LEFT_MODE:
    case REV_RIGHT_MODE:
    case APR_MODE:
      change_hs_mode(wnd, HDG_MODE);
      break;
    case HDG_MODE:
      change_hs_mode(wnd, NAV_MODE);
      break;
    case NAV_MODE:
      change_hs_mode(wnd, APR_MODE);
      break;
    }
  invalidate(hsi);
  }

void on_toggle_vs_mode(menu_widget_t* menu, widget_t* hsi)
  {
  hsi_widget_t* wnd = (hsi_widget_t*)hsi;

  switch ((wnd->autopilot_mode) & VERT_MODE_MASK)
    {
    case 0:
    case VERT_SEEK_MODE:
      change_vs_mode(wnd, ALT_MODE);
      break;
    case ALT_MODE:
      change_vs_mode(wnd, VERT_SEEK_MODE);
      break;
    }
  invalidate(hsi);
  }

result_t hsi_wndproc(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  bool changed = false;
  hsi_widget_t* wnd = (hsi_widget_t*)wnddata;

  int16_t i16;
  uint16_t u16;
  float flt;

  switch (get_can_id(msg))
    {
    case id_magnetic_heading:
      get_param_uint16(msg, &u16);

      while (u16 < 0)
        u16 += 360;
      while (u16 > 359)
        u16 -= 360;

      changed = wnd->direction != u16;
      wnd->direction = u16;
      break;
    case id_heading:
      get_param_uint16(msg, &u16);
      changed = wnd->heading_bug != u16;
      wnd->heading_bug = u16;
      break;
    case id_true_heading:
      get_param_uint16(msg, &u16);
      changed = wnd->heading != u16;
      wnd->heading = u16;
      break;
    case id_deviation:
      get_param_int16(msg, &i16);
      changed = wnd->deviation != i16;
      wnd->deviation = i16;
      // the deviation is +/- * 10
      break;
    case id_selected_course:
      get_param_uint16(msg, &u16);
      changed = wnd->course != u16;
      wnd->course = u16;
      break;
    case id_track:
      get_param_uint16(msg, &u16);
      changed = wnd->track != u16;
      wnd->track = u16;
      break;
    case id_wind_speed:
      get_param_float(msg, &flt);
      i16 = (int16_t)meters_per_second_to_knots(flt);

      if (i16 < 0)
        i16 = 0;

      changed = wnd->wind_speed != i16;
      wnd->wind_speed = i16;
      break;
    case id_wind_direction:
      get_param_uint16(msg, &u16);

      while (u16 > 359)
        u16 -= 360;

      changed = wnd->wind_direction != u16;
      wnd->wind_direction = u16;
      break;
    case id_distance_to_next:
      get_param_float(msg, &flt);
      i16 = (int16_t)meters_to_nm(flt);
      changed = wnd->distance_to_waypoint != i16;
      wnd->distance_to_waypoint = i16;
      break;
    case id_magnetic_variation:
      get_param_float(msg, &flt);
      i16 = (int16_t)radians_to_degrees(flt);
      changed = wnd->magnetic_variation != i16;
      wnd->magnetic_variation = i16;

      break;
    case id_estimated_time_to_next:
      get_param_int16(msg, &i16);
      changed = wnd->time_to_waypoint != i16;
      wnd->time_to_waypoint = i16;
      break;
    case id_autopilot_mode:
      get_param_uint16(msg, &u16);

      changed = wnd->autopilot_mode != u16;
      wnd->autopilot_mode = u16;
      break;
    case id_ap_altitude:
      get_param_float(msg, &flt);
      u16 = (uint16_t)meters_to_feet(flt);

      changed = wnd->ap_altitude != u16;
      wnd->ap_altitude = u16;
      break;
    case id_ap_vertical_rate :
      get_param_float(msg, &flt);
      u16 = (uint16_t)meters_per_second_to_feet_per_minute(flt);

      changed = wnd->ap_vs != u16;
      wnd->ap_vs = u16;
      break;
    case id_waypoint_code:
      u16 = get_can_len(msg);
      
      if (u16 > 1 && u16 < 9 &&
          msg->data[0] == CANFLY_CHARS)
        {
        if (strncmp((const char *)&msg->data[1], wnd->waypoint_name, u16 - 1) != 0)
      {
          strncpy(wnd->waypoint_name, (const char *)&msg->data[1], u16 - 1);
          wnd->waypoint_name[u16 - 1] = 0;
          changed = true;
      }
        }
      else
        {
        if (strcmp(wnd->waypoint_name, "----")!= 0)
      {
          strcpy(wnd->waypoint_name, "----");
          changed = true;
          }
      }
      break;
    }

  if (changed)
    invalidate(hwnd);

  return widget_wndproc(hwnd, msg, wnddata);
  }

result_t on_create_hsi_widget(handle_t hwnd, widget_t* widget)
  {
  result_t result = s_ok;

  // read last settings

  return result;
  }

result_t create_hsi_widget(handle_t parent, uint32_t flags, hsi_widget_t* wnd, handle_t* out)
  {
  result_t result;
  handle_t hndl;
  if (failed(result = create_widget(parent, flags, hsi_wndproc, &wnd->base, &hndl)))
    return result;

  wnd->base.on_paint = on_paint;

  if (out != 0)
    *out = hndl;

  return s_ok;
  }