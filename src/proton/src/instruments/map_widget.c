#include <stdio.h>

#include "../../include/map_widget.h"

const char *map_position_name = "posn";     // last known location
const char *map_display_mode_name = "mode"; // north up to track up
const char *map_zoom_name = "zoom";         // zoom distance

const char *terrain_key = "terrain";
const char *contours_key = "contours";
const char *cities_key = "cities";
const char *water_key = "water";
const char *navdata_key = "navdata";
const char *transport_key = "transport";
const char *obstacles_key = "obstacles";
const char *airspace_key = "airspace";

const char *visible_name = "show";
const char *terrain_warning_name = "warning";
const char *alarm_elevation_name = "elev-alarm";
const char *warning_elevation_name = "elev-warn";
const char *show_hillshade_name = "hillshade";
const char *show_dem_name = "dem";
const char *hypsometric_tint_name = "tint";
const char *info_panel_zoom_name = "info-zoom";
const char *detail_panel_zoom_name = "detail-zoom";
const char *vrp_zoom_name = "vrp-zoom";
const char *navaid_zoom_name = "navaid-zoom";
const char *obstacle_zoom_name = "obstacle-zoom";
const char *hazard_zoom_name = "hazard-zoom";

typedef struct _map_entity_t map_entity_t;

#define INFO_BOX_TOP_WIDTH 50
#define INFO_BOX_TOP_HEIGHT 30
#define INFO_BOX_BOTTOM_WIDTH  60
#define INFO_BOX_BOTTOM_HEIGHT 45

static void on_paint(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* _msg, void* wnddata)
  {
  map_widget_t* wnd = (map_widget_t*)wnddata;

  // draw the map decoratins
  on_paint_widget_background(canvas, wnd_rect, _msg, wnddata);

  // see if borders
  rect_t map_rect;
  rect_copy(wnd_rect, &map_rect);

  const gdi_dim_t window_x = rect_width(wnd_rect);
  const gdi_dim_t window_y = rect_height(wnd_rect);


  uint32_t style = wnd->base.style;
  
  if (style & BORDER_LEFT)
    map_rect.left++;

  if (style & BORDER_RIGHT)
    map_rect.right--;

  if (style & BORDER_TOP)
    map_rect.top++;

  if (style & BORDER_BOTTOM)
    map_rect.bottom--;

  map_render_canvas(wnd->map, &map_rect, canvas);

  // get the current position on the screen for the aircraft center
  point_t pt;
  sys_map_get_position(wnd->map, nullptr, &pt, nullptr, nullptr);

  // this draws a sample aircraft at the current map center
  point_t aircraft[] = {
      {pt.x + 0, pt.y - 15},
      {pt.x + 2, pt.y - 14},
      {pt.x + 3, pt.y - 10},
      {pt.x + 3, pt.y - 8},
      {pt.x + 16, pt.y - 8},
      {pt.x + 17, pt.y - 7},
      {pt.x + 18, pt.y - 5},
      {pt.x + 19, pt.y - -1},
      {pt.x + 2, pt.y - -1},
      {pt.x + 2, pt.y - -8},
      {pt.x + 6, pt.y - -9},
      {pt.x + 7, pt.y - -10},
      {pt.x + 7, pt.y - -13},
      {pt.x + 1, pt.y - -13},
      {pt.x + 0, pt.y - -15},
      {pt.x + -1, pt.y - -13},
      {pt.x + -7, pt.y - -13},
      {pt.x + -7, pt.y - -10},
      {pt.x + -6, pt.y - -9},
      {pt.x + -2, pt.y - -8},
      {pt.x + -2, pt.y - -1},
      {pt.x + -19, pt.y - -1},
      {pt.x + -18, pt.y - 5},
      {pt.x + -17, pt.y - 7},
      {pt.x + -16, pt.y - 8},
      {pt.x + -3, pt.y - 8},
      {pt.x + -3, pt.y - 10},
      {pt.x + -2, pt.y - 14},
      {pt.x + -0, pt.y - 15},
  };

  polygon(canvas, wnd_rect, color_black, color_white, numelements(aircraft),
          aircraft);

  extent_t ex;
  rect_extents(wnd_rect, &ex);

  // draw a range circle that is 1/2 the width of the actual display
  int32_t range_dist = ex.dx >> 2;

  rect_t range_rect = {
      pt.x - range_dist, pt.y - range_dist,
      pt.x + range_dist, pt.y + range_dist};

  ellipse(canvas, wnd_rect, color_white, color_hollow, &range_rect);

  uint32_t range_in_meters;
  sys_map_get_range(wnd->map, &range_in_meters);

  // TODO: map in km, miles
  //
  // range at right of arc (maybe at right) This may change.
  // 7400 is 1nm in meters * 4
  char range[8];
  sprintf(range, "%d nm", range_in_meters / 7400);

  text_extent(wnd->font, 0, range, &ex);

  // TODO: when vertical fonts supported use other dims
  point_t txt_point = { range_rect.right - ex.dx, pt.y };

  draw_text(canvas, wnd_rect, wnd->font, color_white, color_hollow, 0,
            range, &txt_point, wnd_rect, eto_vertical, 0);

  /////////////////////////////////////////////////////////////////////////////
  // Draw the wind information in the upper left
  rectangle(canvas, wnd_rect, color_black, color_black,
            rect_create(0, 0, INFO_BOX_TOP_WIDTH, INFO_BOX_TOP_HEIGHT, &map_rect));

  char msg[32];
  sprintf(msg, "%03.3d", wnd->wind_direction);
  uint16_t length = (uint16_t)strlen(msg);

  extent_t pixels;
  text_extent( wnd->font, length, msg, &pixels);
  draw_text(canvas, wnd_rect,  wnd->font, color_yellow, color_hollow,
            length, msg, point_create(25 - (pixels.dx >> 1), 2, &pt), 0, 0, 0);

  sprintf(msg, "%d", wnd->wind_speed);
  length = (uint16_t)strlen(msg);
  text_extent( wnd->font, length, msg, &pixels);
  draw_text(canvas, wnd_rect,  wnd->font, color_yellow, color_hollow,
            length, msg, point_create(25 - (pixels.dx >> 1), 13, &pt), 0, 0, 0);

  /////////////////////////////////////////////////////////////////////////////
  // Draw the estimated time to waypoint. drawn in top right as distance/time
  rectangle(canvas, wnd_rect, color_black, color_black,
            rect_create(window_x - INFO_BOX_TOP_WIDTH, 0, window_x, INFO_BOX_TOP_HEIGHT,
                        &map_rect));

  sprintf(msg, "%d", wnd->distance_to_waypoint);
  length = (uint16_t)strlen(msg);
  text_extent( wnd->font, length, msg, &pixels);
  draw_text(canvas, wnd_rect,  wnd->font, color_yellow, color_hollow,
            length, msg, point_create(window_x - 25 - (pixels.dx >> 1), 2, &pt),
            0, 0, 0);

  sprintf(msg, "%02.2d:%02.2d", wnd->time_to_waypoint / 60,
          wnd->time_to_waypoint % 60);
  length = (uint16_t)strlen(msg);
  text_extent( wnd->font, length, msg, &pixels);
  draw_text(canvas, wnd_rect,  wnd->font, color_yellow, color_hollow,
            length, msg,
            point_create(window_x - 25 - (pixels.dx >> 1), 13, &pt), 0, 0, 0);

  sprintf(msg, "%s", wnd->waypoint_name);
  length = (uint16_t)strlen(msg);
  text_extent( wnd->font, length, msg, &pixels);
  draw_text(canvas, wnd_rect,  wnd->font, color_yellow, color_hollow,
            length, msg,
            point_create(window_x - 25 - (pixels.dx >> 1), 24, &pt), 0, 0, 0);

  /////////////////////////////////////////////////////////////////////////////
  // Draw the HMode in the bottom left
  // the HMode is a 3 character string that is drawn in the bottom left
  // of the HSI.  The HMode is the current mode of the HSI and is
  // one of the following:
  //  OFF
  //  HDG - current heading
  //  CRS - Current AP Course
  //  INT - Intercept track
  rectangle(canvas, wnd_rect, color_black, color_black,
            rect_create(0, window_y - INFO_BOX_BOTTOM_HEIGHT,
                        INFO_BOX_BOTTOM_WIDTH,window_y, &map_rect));

  const char *tmp_msg = nullptr;
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
    nav_hdg = wnd->magnetic_heading;
    break;
  case APR_MODE:
    tmp_msg = "INT";
    nav_hdg = wnd->course;
    break;
  case REV_LEFT_MODE:
    tmp_msg = "REV LEFT";
    nav_hdg = wnd->magnetic_heading;
    break;
  case REV_RIGHT_MODE:
    tmp_msg = "REV RGT";
    nav_hdg = wnd->magnetic_heading;
    break;
    }

  // draw the currently selected nav
  if (hmode != 0)
    {
    sprintf(msg, "%03.3d", nav_hdg);
    length = strlen(msg);
    text_extent( wnd->font, length, msg, &pixels);
    draw_text(canvas, wnd_rect,  wnd->font, color_magenta,
              color_hollow, length, msg,
              point_create(wnd_rect->left + 2, wnd_rect->bottom - 30, &pt), 0,
              0, 0);
    }

  length = (uint16_t)strlen(tmp_msg);
  text_extent( wnd->font, length, tmp_msg, &pixels);
  draw_text(canvas, wnd_rect,  wnd->font, color_magenta, color_hollow,
            length, tmp_msg,
            point_create(31 - (pixels.dx >> 1), wnd_rect->bottom - 15, &pt), 0,
            0, 0);

  /////////////////////////////////////////////////////////////////////////////
  // Draw the VMode in the bottom right
  // the VMode is a 3 character string that is drawn in the bottom right
  // of the HSI.  The VMode is the current mode of the VSI and is
  // one of the following:
  //  OFF
  //  ALT - Altitude Hold
  //  SEEK - Altitude Seek
  rectangle(canvas, wnd_rect, color_black, color_black,
            rect_create(window_x - INFO_BOX_BOTTOM_WIDTH,
                        window_y - INFO_BOX_BOTTOM_HEIGHT,
                        window_x, window_y, &map_rect));


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
  text_extent( wnd->font, length, msg, &pixels);
  draw_text(
      canvas, wnd_rect,  wnd->font, color_magenta, color_hollow,
      length, msg,
      point_create(wnd_rect->right - pixels.dx, wnd_rect->bottom - 45, &pt), 0,
      0, 0);

  // draw the assigned altitude
  sprintf(msg, "A:%05.3d", wnd->ap_altitude);
  length = strlen(msg);
  text_extent( wnd->font, length, msg, &pixels);
  draw_text(
      canvas, wnd_rect,  wnd->font, color_magenta, color_hollow,
      length, msg,
      point_create(wnd_rect->right - pixels.dx, wnd_rect->bottom - 30, &pt), 0,
      0, 0);

  length = (uint16_t)strlen(tmp_msg);
  text_extent( wnd->font, length, tmp_msg, &pixels);
  draw_text(canvas, wnd_rect,  wnd->font, color_magenta, color_hollow,
            length, tmp_msg,
            point_create(window_x - 31 - (pixels.dx >> 1),
                         wnd_rect->bottom - 15, &pt),
            0, 0, 0);
  }

// the settings worker is started with a timer message
// and then processes each parameter in succession when
// the callback is made

typedef enum
{
  ss_init,
  ss_position,
  ss_display_mode,
  ss_range,
  ss_water,
  ss_contours,
  ss_cities,
  ss_transport,
  ss_obstacles,
  ss_airspaces,
  ss_complete,
} settings_state_e;

typedef struct _map_extra_data_t
  {
  overlapped_t base;
  settings_state_e state;

  memid_t map_settings;
  memid_t water_key;
  memid_t contours_key;
  memid_t cities_key;
  memid_t obstacles_key;
  memid_t terrain_key;
  memid_t transport_key;
  memid_t airspaces_key;

  bool position_changed;
  lla_t map_position;

  bool display_mode_changed;
  int32_t display_mode;

  bool range_changed;
  int32_t range;

  bool view_water_changed;
  bool view_water;

  bool view_contours_changed;
  bool view_contours;

  bool view_cities_changed;
  bool view_cities;

  bool view_transport_changed;
  bool view_transport;

  bool view_obstacles_changed;
  bool view_obstacles;

  bool view_airspaces_changed;
  bool view_airspaces;
  } map_extra_data_t;

static void state_callback(overlapped_t *context, result_t result)
  {
  map_extra_data_t *data = (map_extra_data_t *)context;

  switch (data->state)
    {
  case ss_init:
    if (data->position_changed)
      cfg_set_lla(data->map_settings, map_position_name, &data->map_position,
                  &data->base);

    data->state = ss_position;
    data->position_changed = false;
    break;
  case ss_position:
    if (data->display_mode_changed)
      cfg_set_int32(data->map_settings, map_display_mode_name,
                    data->display_mode, &data->base);

    data->state = ss_range;
    data->display_mode_changed = false;
    break;
  case ss_range:
    if (data->range_changed)
      cfg_set_int32(data->map_settings, map_zoom_name, data->range,
                    &data->base);

    data->state = ss_water;
    data->range_changed = false;
    break;
  case ss_water:
    if (data->view_water_changed)
      cfg_set_bool(data->water_key, visible_name, data->view_water,
                   &data->base);

    data->state = ss_contours;
    data->view_water_changed = false;
    break;
  case ss_contours:
    if (data->view_contours_changed)
      cfg_set_bool(data->contours_key, visible_name, data->view_contours,
                   &data->base);

    data->state = ss_cities;
    data->view_contours_changed = false;
    break;
  case ss_cities:
    if (data->view_cities_changed)
      cfg_set_bool(data->cities_key, visible_name, data->view_cities,
                   &data->base);

    data->state = ss_transport;
    data->view_transport_changed = false;
    break;
  case ss_obstacles:
    if (data->view_obstacles_changed)
      cfg_set_bool(data->obstacles_key, visible_name, data->view_obstacles,
                   &data->base);

    data->state = ss_airspaces;
    data->view_obstacles_changed = false;
    break;
  case ss_airspaces:
    if (data->view_airspaces_changed)
      cfg_set_bool(data->airspaces_key, visible_name, data->view_airspaces,
                   &data->base);

    data->state = ss_complete;
    data->view_airspaces_changed = false;
    break;
  case ss_complete:
    data->state = ss_init; // wait till next id_timer message
    break;
    }
  }

static result_t change_ap_mode(map_widget_t *widget)
  {
  canmsg_t msg;
  create_can_msg_uint16(&msg, id_autopilot_mode, widget->autopilot_mode);
  return can_send(&msg, INDEFINITE_WAIT, nullptr);
  }

static result_t change_hs_mode(map_widget_t *widget, uint16_t mode)
  {
  widget->autopilot_mode &= ~HORZ_MODE_MASK;
  widget->autopilot_mode |= mode;
  return change_ap_mode(widget);
  }

static result_t change_vs_mode(map_widget_t *widget, uint16_t mode)
  {
  widget->autopilot_mode &= ~VERT_MODE_MASK;
  widget->autopilot_mode |= mode;
  return change_ap_mode(widget);
  }

static void on_toggle_hs_mode(map_widget_t *wnd)
  {
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

  invalidate(wnd->hwnd);
  }

static void on_toggle_vs_mode(map_widget_t *wnd)
  {
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

  invalidate(wnd->hwnd);
  }

static result_t map_wndproc(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  map_widget_t* wnd = (map_widget_t*)wnddata;
  map_extra_data_t *extra = (map_extra_data_t *)wnd->base.extra;

  int16_t i16;
  uint16_t u16;
  float flt;


  bool changed = false;
  switch (get_can_id(msg))
    {
    case id_position_lat:
      get_param_float(msg, &extra->map_position.lat);
      break;
    case id_position_lng:
      get_param_float(msg, &extra->map_position.lng);
      break;
    case id_gps_height:
      get_param_float(msg, &extra->map_position.alt);
      break;
    case id_true_track:
      // we don't use magnetic
      break;
    case id_gps_groundspeed:
      break;
    case id_gps_valid:
      // save the value
      changed = succeeded(map_update_position(wnd->map, &extra->map_position,
                                              wnd->true_heading, wnd->track));
      break;
    case id_magnetic_heading:
      {
      get_param_int16(msg, &i16);

      while (i16 < 0)
        i16 += 360;
      while (i16 > 359)
        i16 -= 360;

      changed = wnd->magnetic_heading != i16;
      if (changed)
        {
        wnd->magnetic_heading = i16;
        wnd->true_heading = wnd->magnetic_heading - wnd->mag_var;
        map_update_position(wnd->map, &extra->map_position,
                            wnd->true_heading, wnd->track);
        }
      }
      break;
    case id_magnetic_variation:
      {
      get_param_int16(msg, &i16);
      changed = wnd->mag_var != i16;
      if (changed)
        {
        wnd->mag_var = i16;
        map_set_mag_var(wnd->map, i16);
        wnd->true_heading = wnd->magnetic_heading - wnd->mag_var;
        map_update_position(wnd->map, &extra->map_position, wnd->true_heading,
                            wnd->track);
        map_set_mag_var(wnd->map, i16);
        }
      }
      break;
    case id_selected_course:
      get_param_int16(msg, &i16);
      changed = wnd->course != i16;
      wnd->course = i16;
      break;
    case id_track:
      get_param_int16(msg, &i16);
      changed = wnd->track != i16;
      if (changed)
        {
        wnd->track = i16;
        map_update_position(wnd->map, &extra->map_position, wnd->true_heading,
                            wnd->track);
        }
      break;
    case id_touch_zoom:
      {
      // TODO: pass message as binary
      // withthe zoom center 
      int16_t value;
      get_param_int16(msg, &value);

      map_zoom(wnd->map, value);

      changed = true;
      }
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
    case id_ap_vertical_rate:
      get_param_float(msg, &flt);
      u16 = (uint16_t)meters_per_second_to_feet_per_minute(flt);

      changed = wnd->ap_vs != u16;
      wnd->ap_vs = u16;
      break;
    case id_waypoint_code:
      u16 = get_can_len(msg);

      if (u16 > 1 && u16 < 9 && msg->data[0] == CANFLY_CHARS)
        {
        if (strncmp((const char *)&msg->data[1], wnd->waypoint_name, u16 - 1) !=
            0)
          {
          strncpy(wnd->waypoint_name, (const char *)&msg->data[1], u16 - 1);
          wnd->waypoint_name[u16 - 1] = 0;
          changed = true;
          }
        }
      else
        {
        if (strcmp(wnd->waypoint_name, "----") != 0)
          {
          strcpy(wnd->waypoint_name, "----");
          changed = true;
          }
        }
      break;

    case id_touch_pan:
      {
      uint32_t value;
      get_param_uint32(msg, &value);

      extent_t ex;
      ex.dx = (gdi_dim_t)(((int32_t)value) >> 16);
      ex.dy = (gdi_dim_t)((int16_t)(value & 0xffff));

      map_pan(wnd->map, &ex);

      changed = true;
      }
      break;
    case id_timer:
      {
      map_extra_data_t *extra = (map_extra_data_t *)wnd->base.extra;
      if (extra->state == ss_init)
        state_callback((overlapped_t *)extra, s_ok);
      }
      break;
    }

  if (changed)
    invalidate(hwnd);

  // pass to default
  return widget_wndproc(hwnd, msg, wnddata);
  }

  void set_gps_position(map_widget_t *widget, const lla_t *position)
  {
  map_extra_data_t *extra = (map_extra_data_t *)widget->base.extra;

  if (memcmp(position, &extra->map_position, sizeof(lla_t)) != 0)
    {
    memcpy(&extra->map_position, position, sizeof(lla_t));

    extra->position_changed = true;
    }

  map_update_position(widget->map, position, widget->magnetic_heading, widget->track);
  }

void set_map_range(map_widget_t *widget, int32_t range)
  {
  map_extra_data_t *extra = (map_extra_data_t *)widget->base.extra;
  if (range != extra->range)
    {
    extra->range = range;
    extra->range_changed = true;
    }

  // TODO: converion to meters?
  map_set_range(widget->map, range);
  }

void get_map_range(map_widget_t *widget, int32_t *range)
  {
  map_extra_data_t *extra = (map_extra_data_t *)widget->base.extra;
  *range = extra->range;
  }

void set_map_mode(map_widget_t *widget, map_display_mode mode)
  {
  map_extra_data_t *extra = (map_extra_data_t *)widget->base.extra;

  if (mode != extra->display_mode)
    {
    extra->display_mode = mode;
    extra->display_mode_changed = true;
    }

  map_set_mode(widget->map, mode);
  }

void get_map_mode(map_widget_t *widget, map_display_mode *mode)
  {
  map_extra_data_t *extra = (map_extra_data_t *)widget->base.extra;
  *mode = (map_display_mode)extra->display_mode;
  }

void get_map_contours_visible(map_widget_t *widget, bool *visible)
  {
  contours_params_t vp;
  if(succeeded(map_get_layer_parameters(widget->map, MAP_LAYER_CONTOURS, sizeof(vp), &vp.base)))
    *visible = vp.base.show_layer;
  else
    *visible = false;
  }

void set_map_contours_visible(map_widget_t *widget, bool visible)
  {
  map_extra_data_t *extra = (map_extra_data_t *)widget->base.extra;
  contours_params_t vp;
  if (succeeded(map_get_layer_parameters(widget->map, MAP_LAYER_CONTOURS,
                                        sizeof(vp), &vp.base)))
    {
    vp.base.show_layer = visible;
    map_set_layer_parameters(widget->map, MAP_LAYER_CONTOURS, &vp.base);
    }
  else
    vp.base.show_layer = false;

  if (vp.base.show_layer != extra->view_contours)
    {
    extra->view_contours = vp.base.show_layer;
    extra->view_contours_changed = true;
    }
  }

void get_map_cities_visible(map_widget_t *widget, bool *visible)
  {
  cities_params_t vp;
  if (succeeded(map_get_layer_parameters(widget->map, MAP_LAYER_CONTOURS,
                                        sizeof(vp), &vp.base)))
    *visible = vp.base.show_layer;
  else
    *visible = false;
  }

void set_map_cities_visible(map_widget_t *widget, bool visible)
  {
  map_extra_data_t *extra = (map_extra_data_t *)widget->base.extra;
  cities_params_t vp;
  if (succeeded(map_get_layer_parameters(widget->map, MAP_LAYER_CITIES,
                                        sizeof(vp), &vp.base)))
    {
    vp.base.show_layer = visible;
    map_set_layer_parameters(widget->map, MAP_LAYER_CITIES, &vp.base);
    }
  else
    vp.base.show_layer = false;

  if (vp.base.show_layer != extra->view_cities)
    {
    extra->view_cities = vp.base.show_layer;
    extra->view_cities_changed = true;
    }
  }

void get_map_water_visible(map_widget_t *widget, bool *visible)
  {
  surface_water_params_t vp;
  if (succeeded(map_get_layer_parameters(widget->map, MAP_LAYER_SURFACE_WATER,
                                        sizeof(vp), &vp.base)))
    *visible = vp.base.show_layer;
  else
    *visible = false;
  }

void set_map_water_visible(map_widget_t *widget, bool visible)
  {
  map_extra_data_t *extra = (map_extra_data_t *)widget->base.extra;
  surface_water_params_t vp;
  if (succeeded(map_get_layer_parameters(widget->map, MAP_LAYER_SURFACE_WATER,
                                        sizeof(vp), &vp.base)))
    {
    vp.base.show_layer = visible;
    map_set_layer_parameters(widget->map, MAP_LAYER_SURFACE_WATER, &vp.base);
    }
  else
    vp.base.show_layer = false;

  if (vp.base.show_layer != extra->view_water)
    {
    extra->view_water = vp.base.show_layer;
    extra->view_water_changed = true;
    }
  }

void get_map_transport_visible(map_widget_t *widget, bool *visible)
  {
  transport_params_t vp;
  if (succeeded(map_get_layer_parameters(widget->map, MAP_LAYER_TRANSPORT,
                                        sizeof(vp), &vp.base)))
    *visible = vp.base.show_layer;
  else
    *visible = false;
  }

void set_map_transport_visible(map_widget_t *widget, bool visible)
  {
  map_extra_data_t *extra = (map_extra_data_t *)widget->base.extra;
  transport_params_t vp;
  if (succeeded(map_get_layer_parameters(widget->map, MAP_LAYER_TRANSPORT,
                                        sizeof(vp), &vp.base)))
    {
    vp.base.show_layer = visible;
    map_set_layer_parameters(widget->map, MAP_LAYER_TRANSPORT, &vp.base);
    }
  else
    vp.base.show_layer = false;

  if (vp.base.show_layer != extra->view_transport)
    {
    extra->view_transport = vp.base.show_layer;
    extra->view_transport_changed = true;
    }
  }

void get_map_obstacles_visible(map_widget_t *widget, bool *visible)
  {
  obstacles_params_t vp;
  if (succeeded(map_get_layer_parameters(widget->map, MAP_LAYER_OBSTACLES,
                                        sizeof(vp), &vp.base)))
    *visible = vp.base.show_layer;
  else
    *visible = false;
  }

void set_map_obstacles_visible(map_widget_t *widget, bool visible)
  {
  map_extra_data_t *extra = (map_extra_data_t *)widget->base.extra;
  obstacles_params_t vp;
  if (succeeded(map_get_layer_parameters(widget->map, MAP_LAYER_OBSTACLES,
                                        sizeof(vp), &vp.base)))
    {
    vp.base.show_layer = visible;
    map_set_layer_parameters(widget->map, MAP_LAYER_OBSTACLES, &vp.base);
    }
  else
    vp.base.show_layer = false;

  if (vp.base.show_layer != extra->view_obstacles)
    {
    extra->view_obstacles = vp.base.show_layer;
    extra->view_obstacles_changed = true;
    }
  }

void get_map_airspacevisible(map_widget_t *widget, bool *visible)
  {
  airspace_params_t vp;
  if (succeeded(map_get_layer_parameters(widget->map, MAP_LAYER_AIRSPACES,
                                        sizeof(vp), &vp.base)))
    *visible = vp.base.show_layer;
  else
    *visible = false;
  }

void set_map_airspace_visible(map_widget_t *widget, bool visible)
  {
  map_extra_data_t *extra = (map_extra_data_t *)widget->base.extra;
  airspace_params_t vp;
  if (succeeded(map_get_layer_parameters(widget->map, MAP_LAYER_AIRSPACES,
                                        sizeof(vp), &vp.base)))
    {
    vp.base.show_layer = visible;
    map_set_layer_parameters(widget->map, MAP_LAYER_AIRSPACES, &vp.base);
    }
  else
    vp.base.show_layer = false;

  if (vp.base.show_layer != extra->view_airspaces)
    {
    extra->view_obstacles = vp.base.show_layer;
    extra->view_obstacles_changed = true;
    }
  }


static result_t ensure_key(memid_t key, const char *name, memid_t *child)
  {
  result_t result;
  if (failed(result = cfg_open_key(key, name, child, nullptr)))
    return cfg_create_key(key, name, child, nullptr);

  return result;
  }

result_t create_map_widget(handle_t parent, handle_t spatial_db, uint32_t flags, map_widget_t* wnd, handle_t* out)
  {
  result_t result;
  if (failed(result = create_widget(parent, flags, map_wndproc, &wnd->base, &wnd->hwnd)))
    return result;

  extent_t background_ex = { rect_width(&wnd->base.rect), rect_height(&wnd->base.rect) };

  // TODO: should be smarter than this.
  if (failed(result = map_create_canvas(wnd->hwnd, 0, &background_ex, spatial_db, &wnd->day_theme, &wnd->map)))
    return result;

  wnd->base.on_paint = on_paint;

  // create our extra storage
  map_extra_data_t *extra = calloc(1, sizeof(map_extra_data_t));

  wnd->base.extra = extra;

  // open the registry key
  memid_t memid = 0;
  size_t len = strlen(wnd->config_path);

  char *path = malloc(len+1);

  strcpy(path, wnd->config_path);

  char *start = path;

  if (*start == '/')
    start++;
  char *end = start;

  // split string
  while (*end != 0)
    {
    if (*end == '/')
      *end = 0;

    end++;
    }

  while (start <= end)
    {
    if (failed(result = ensure_key(memid, start, &extra->map_settings)))
      {
      free(path);
      return result; // can't open/create key
      }

    memid = extra->map_settings;

    while (*start++ != 0)
      ; // skip to end of string
    }

  free(path);

  // now create the remaining keys
  if (failed(result = ensure_key(extra->map_settings, contours_key, &extra->contours_key)) ||
      failed(result = ensure_key(extra->map_settings, water_key, &extra->water_key)) ||
      failed(result = ensure_key(extra->map_settings, cities_key, &extra->cities_key)) ||
      failed(result = ensure_key(extra->map_settings, obstacles_key, &extra->obstacles_key)) ||
      failed(result = ensure_key(extra->map_settings, terrain_key, &extra->terrain_key)) ||
      failed(result = ensure_key(extra->map_settings, transport_key, &extra->transport_key)) ||
      failed(result = ensure_key(extra->map_settings, airspace_key, &extra->airspaces_key)))
    goto config_error;

  // to make sense of the map, load the last known position.
  // when the GPS starts running it will update it
  if (failed(result = cfg_get_lla(extra->map_settings, map_position_name,
                                  &extra->map_position, nullptr)))
    {
    // set a default.
    extra->map_position.alt = 0;
    extra->map_position.lat = 0;
    extra->map_position.lng = 0;

#ifdef DEBUG

    int test_case = 0;

    switch (test_case)
      {
      default:
      case 0 :
        // npl threshold 05
        // -39.010476, 174.170100
        extra->map_position.lng = 174.170100f;
        extra->map_position.lat = -39.010476f;
        extra->map_position.alt = feet_to_meters(89);
        wnd->track = 45;
        wnd->magnetic_heading = 45;
        break;
      case 1 :
        // rangioira
        // -43.290001,172.542007
        extra->map_position.lat = -43.290001;
        extra->map_position.lng = 172.542007;
        extra->map_position.alt = feet_to_meters(180);
        wnd->track = 220;
        wnd->magnetic_heading = 220;
        break;
      case 2 :
        // stratford aerodrome
        // -39.318319, 174.308126
        extra->map_position.lat = -39.318319f;
        extra->map_position.lng = 174.308126f;
        extra->map_position.alt = feet_to_meters(951);
        wnd->track = 90;
        wnd->magnetic_heading = 90;
        break;
      case 3 :
        // Ardmore
        // -37.064376, 174.978367
        extra->map_position.lat = -37.064376;
        extra->map_position.lng = 174.978367;
        extra->map_position.alt = feet_to_meters(111);
        wnd->track = 21;
        wnd->magnetic_heading = 21;
        break;
      case 4 :
        // oshkosh wi
        // 43.998287, -88.567753
        extra->map_position.lat = 43.998287;
        extra->map_position.lng = -88.567753;
        extra->map_position.alt = feet_to_meters(809);
        wnd->track = 27;
        wnd->magnetic_heading = 27;
        break;
      case 5 :
        // sfo
        // 37.466539, -122.242943
        extra->map_position.lat = 37.466539;
        extra->map_position.lng = -122.242943;
        extra->map_position.alt = feet_to_meters(13);
        wnd->track = 28;
        wnd->magnetic_heading = 28;
        break;
      case 6 :
        // OAK
        // 37.619909, -122.207544
        extra->map_position.lat = 37.619909f;
        extra->map_position.lng = -122.207544f;
        extra->map_position.alt = feet_to_meters(8.5);
        wnd->track = 10;
        wnd->magnetic_heading = 10;
        break;
      }

#endif
    extra->position_changed = true;
    }

  if (failed(result = map_update_position(wnd->map, &extra->map_position,
                                          wnd->magnetic_heading, wnd->track)))
    goto config_error;

  if (failed(result = cfg_get_int32(extra->map_settings, map_zoom_name,
                                    &extra->range, nullptr)))
    {
    extra->range =
        (int32_t)(4 * METERS_PER_NM); // width of the scaled screen in meters
    extra->range_changed = true;
    }

  if (failed(result = map_set_range(wnd->map, extra->range)))
    goto config_error;

  if (failed(result = cfg_get_int32(extra->map_settings, map_display_mode_name,
                                    &extra->display_mode, nullptr)))
    {
    extra->display_mode = mdm_heading;
    extra->display_mode_changed = true;
    }

  if (failed(result = map_set_mode(wnd->map, extra->display_mode)))
    goto config_error;

  /*********************************************************************/
  //  Terrain - NOT SUPPORTED YET, code is there display too slow.
#ifdef ALLOW_TERRAIN
  terrain_params_t tp;
  if (failed(result = sys_map_get_layer_parameters(wnd->map, MAP_LAYER_TERRAIN, sizeof(tp), &tp.base)))
    goto config_error;

  // turn on/off the
  if (failed(result = cfg_get_bool(extra->terrain_key, visible_name, &tp.base.show_layer, nullptr)) &&
      failed(result = cfg_set_bool(extra->terrain_key, visible_name, tp.base.show_layer, nullptr)))
    goto config_error;

  if (failed(result = cfg_get_bool(extra->terrain_key, terrain_warning_name,
                                   &tp.show_terrain_warning, nullptr)) &&
      failed(result = cfg_set_bool(extra->terrain_key, terrain_warning_name,
                                   tp.show_terrain_warning, nullptr)))
    goto config_error;

  if (failed(result = cfg_get_uint16(extra->terrain_key, alarm_elevation_name,
                                     &tp.alarm_elevation, nullptr)))
    {
    tp.alarm_elevation = (uint16_t)feet_to_meters(100);
    if (failed(result = cfg_set_uint16(extra->terrain_key, alarm_elevation_name,
                                       tp.alarm_elevation, nullptr)))
      goto config_error;
    }

  if (failed(result = cfg_get_uint16(extra->terrain_key, warning_elevation_name,
                                     &tp.warning_elevation, nullptr)))
    {
    tp.warning_elevation = (uint16_t)feet_to_meters(1000);
    if (failed(result =
                   cfg_set_uint16(extra->terrain_key, warning_elevation_name, tp.warning_elevation, nullptr)))
      goto config_error;
    }

  if (failed(result = cfg_get_bool(extra->terrain_key, show_hillshade_name, &tp.show_hillshade, nullptr)) &&
      failed(result = cfg_set_bool(extra->terrain_key, show_hillshade_name, tp.show_hillshade, nullptr)))
    goto config_error;

  if (failed(result = cfg_get_bool(extra->terrain_key, show_dem_name, &tp.show_terrain, nullptr)) &&
      failed(result = cfg_set_bool(extra->terrain_key, show_dem_name, tp.show_terrain, nullptr)))
    goto config_error;

  if (failed(result = cfg_get_bool(extra->terrain_key, hypsometric_tint_name, &tp.hypsometric_tint, nullptr)) &&
      failed(result = cfg_set_bool(extra->terrain_key, hypsometric_tint_name, tp.hypsometric_tint, nullptr)))
    goto config_error;

  if (failed(result = sys_map_set_layer_parameters(wnd->map, MAP_LAYER_TERRAIN, &tp.base)))
    goto config_error;

#endif
  /*********************************************************************/
  // Contours
  contours_params_t cp;

  if (failed(sys_map_get_layer_parameters(wnd->map, MAP_LAYER_CONTOURS, sizeof(cp), &cp.base)))
    goto config_error;

  if (failed(result = cfg_get_bool(extra->contours_key, visible_name, &cp.base.show_layer, nullptr)) &&
      failed(result = cfg_set_bool(extra->contours_key, visible_name, cp.base.show_layer, nullptr)))
    goto config_error;

  // TODo: load the color map

  if (failed(result = sys_map_set_layer_parameters(wnd->map, MAP_LAYER_CONTOURS, &cp.base)))
    goto config_error;

  /*********************************************************************/
  //  Cities
  cities_params_t ctp;

  if (failed(sys_map_get_layer_parameters(wnd->map, MAP_LAYER_CITIES, sizeof(ctp), &ctp.base)))
    goto config_error;

  if (failed(result = cfg_get_bool(extra->cities_key, visible_name, &ctp.base.show_layer, nullptr)) &&
      failed(result = cfg_set_bool(extra->cities_key, visible_name,
                                   ctp.base.show_layer, nullptr)))
    goto config_error;

  if (failed(result = sys_map_set_layer_parameters(wnd->map, MAP_LAYER_CITIES, &ctp.base)))
    goto config_error;

  /*********************************************************************/
  //  Surface Water
  surface_water_params_t swp;

  if (failed(sys_map_get_layer_parameters(wnd->map, MAP_LAYER_SURFACE_WATER, sizeof(swp), &swp.base)))
    goto config_error;

  if (failed(result = cfg_get_bool(extra->water_key, visible_name, &swp.base.show_layer, nullptr)) &&
      failed(result = cfg_set_bool(extra->water_key, visible_name, swp.base.show_layer, nullptr)))
    goto config_error;

  if (failed(result = sys_map_set_layer_parameters(wnd->map, MAP_LAYER_SURFACE_WATER, &swp.base)))
    goto config_error;

  /*********************************************************************/
  //  Transport

  transport_params_t trp;

  if (failed(sys_map_get_layer_parameters(wnd->map, MAP_LAYER_TRANSPORT, sizeof(trp), &trp.base)))
    goto config_error;

  if (failed(result = cfg_get_bool(extra->transport_key, visible_name, &trp.base.show_layer, nullptr)) &&
      failed(result = cfg_set_bool(extra->transport_key, visible_name, trp.base.show_layer, nullptr)))
    goto config_error;

  if (failed(result = sys_map_set_layer_parameters(wnd->map, MAP_LAYER_TRANSPORT, &trp.base)))
    goto config_error;

  /*********************************************************************/
  //  Obstacles
  obstacles_params_t op;

  if (failed(sys_map_get_layer_parameters(wnd->map, MAP_LAYER_OBSTACLES, sizeof(op), &op.base)))
    goto config_error;

  if (failed(result = cfg_get_bool(extra->obstacles_key, visible_name, &op.base.show_layer, nullptr)) &&
      failed(result = cfg_set_bool(extra->obstacles_key, visible_name, op.base.show_layer, nullptr)))
    goto config_error;

  if (failed(result = sys_map_set_layer_parameters(wnd->map, MAP_LAYER_OBSTACLES, &op.base)))
    goto config_error;

  /*********************************************************************/
  // Navdata
  airspace_params_t asp;

  if (failed(sys_map_get_layer_parameters(wnd->map, MAP_LAYER_AIRSPACES, sizeof(asp), &asp.base)))
    goto config_error;

  // TODO: these belong in the params init code
  asp.info_panel_zoom = 40000;
  asp.detail_info_panel_zoom = 20000;
  // Per-entity-type visibility thresholds in metres (range from observer).
  // Tuned for a typical VFR cockpit: VRPs/hazards only close in, navaids
  // visible from farther out for IFR planning, obstacles in-between.
  asp.vrp_zoom = 75000;             // ~40 NM
  asp.navaid_zoom = 200000;         // ~108 NM
  asp.obstacle_zoom = 100000;       // ~54 NM
  asp.hazard_zoom = 75000;          // ~40 NM

  if (failed(result = cfg_get_int32(extra->airspaces_key, info_panel_zoom_name, &asp.info_panel_zoom, nullptr)) &&
      failed(result = cfg_set_int32(extra->airspaces_key, info_panel_zoom_name, asp.info_panel_zoom, nullptr)))
    goto config_error;

  if (failed(result = cfg_get_int32(extra->airspaces_key, detail_panel_zoom_name, &asp.detail_info_panel_zoom, nullptr)) &&
      failed(result = cfg_set_int32(extra->airspaces_key, detail_panel_zoom_name, asp.detail_info_panel_zoom, nullptr)))
    goto config_error;

  if (failed(result = sys_map_set_layer_parameters(wnd->map, MAP_LAYER_AIRSPACES, &asp.base)))
    goto config_error;
    
  if (failed(result = cfg_get_int32(extra->airspaces_key, obstacle_zoom_name, &asp.obstacle_zoom, nullptr)) &&
      failed(result = cfg_set_int32(extra->airspaces_key, obstacle_zoom_name, asp.obstacle_zoom, nullptr)))
    goto config_error;

  if (failed(result = cfg_get_int32(extra->airspaces_key, hazard_zoom_name, &asp.hazard_zoom, nullptr)) &&
      failed(result = cfg_set_int32(extra->airspaces_key, hazard_zoom_name, asp.hazard_zoom, nullptr)))
    goto config_error;

  if (failed(result = map_set_layer_parameters(wnd->map, MAP_LAYER_AIRSPACES, &asp.base)))
    goto config_error;

  text_extent(wnd->font, 1, "M", &wnd->font_cell_size);
  wnd->font_cell_size.dx += 2;

  if (out != 0)
    *out = wnd->hwnd;

  return s_ok;

config_error:
  trace_error("Cannot create the map widget");
  return result;
  }


