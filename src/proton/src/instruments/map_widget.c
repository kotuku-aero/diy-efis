#include <stdio.h>

#include "../../include/map_widget.h"

static handle_t doorbell;

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

typedef struct _map_entity_t map_entity_t;

static void on_paint(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* _msg, void* wnddata)
  {
  map_widget_t* wnd = (map_widget_t*)wnddata;

  // draw the map decoratins
  on_paint_widget_background(canvas, wnd_rect, _msg, wnddata);

  // see if borders
  rect_t map_rect;
  rect_copy(wnd_rect, &map_rect);

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

static result_t map_wndproc(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  map_widget_t* wnd = (map_widget_t*)wnddata;

  bool changed = false;
  switch (get_can_id(msg))
    {
    case id_position_lat:
      get_param_float(msg, &wnd->gps_position.lat);
      break;
    case id_position_lng:
      get_param_float(msg, &wnd->gps_position.lng);
      break;
    case id_gps_height:
      get_param_float(msg, &wnd->gps_position.alt);
      break;
    case id_true_track:
      // we don't use magnetic
      break;
    case id_gps_groundspeed:
      break;
    case id_gps_valid:
      // save the value
      changed = succeeded(map_update_position(wnd->map, &wnd->gps_position, wnd->heading, wnd->track));
      break;
    case id_magnetic_heading:
      {
      int16_t direction;
      get_param_int16(msg, &direction);

      while (direction < 0)
        direction += 360;
      while (direction > 359)
        direction -= 360;

      changed = wnd->track != direction;
      wnd->track = direction;
      }
      break;
    case id_selected_course:
      {
      int16_t value;
      get_param_int16(msg, &value);
      changed = wnd->course != value;
      wnd->course = value;
      }
      break;
    case id_track:
      {
      int16_t value;
      get_param_int16(msg, &value);
      changed = wnd->track != value;
      wnd->track = value;
      }
      break;
    case id_heading:
      {
      int16_t value;
      get_param_int16(msg, &value);
      changed = wnd->heading != value;
      wnd->heading = value;
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
    case id_magnetic_variation:
      {
      int16_t value;
      get_param_int16(msg, &value);
      map_set_mag_var(wnd->map, value);
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

  map_update_position(widget->map, position, widget->heading, widget->track);
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
  if(succeeded(map_get_layer_parameters(widget->map, MAP_LAYER_CONTOURS, sizeof(vp), &vp)))
    *visible = vp.base.show_layer;
  else
    *visible = false;
  }

void set_map_contours_visible(map_widget_t *widget, bool visible)
  {
  map_extra_data_t *extra = (map_extra_data_t *)widget->base.extra;
  contours_params_t vp;
  if (succeeded(map_get_layer_parameters(widget->map, MAP_LAYER_CONTOURS,
                                        sizeof(vp), &vp)))
    {
    vp.base.show_layer = visible;
    map_set_layer_parameters(widget->map, MAP_LAYER_CONTOURS, &vp);
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
                                        sizeof(vp), &vp)))
    *visible = vp.base.show_layer;
  else
    *visible = false;
  }

void set_map_cities_visible(map_widget_t *widget, bool visible)
  {
  map_extra_data_t *extra = (map_extra_data_t *)widget->base.extra;
  cities_params_t vp;
  if (succeeded(map_get_layer_parameters(widget->map, MAP_LAYER_CITIES,
                                        sizeof(vp), &vp)))
    {
    vp.base.show_layer = visible;
    map_set_layer_parameters(widget->map, MAP_LAYER_CITIES, &vp);
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
                                        sizeof(vp), &vp)))
    *visible = vp.base.show_layer;
  else
    *visible = false;
  }

void set_map_water_visible(map_widget_t *widget, bool visible)
  {
  map_extra_data_t *extra = (map_extra_data_t *)widget->base.extra;
  surface_water_params_t vp;
  if (succeeded(map_get_layer_parameters(widget->map, MAP_LAYER_SURFACE_WATER,
                                        sizeof(vp), &vp)))
    {
    vp.base.show_layer = visible;
    map_set_layer_parameters(widget->map, MAP_LAYER_SURFACE_WATER, &vp);
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
                                        sizeof(vp), &vp)))
    *visible = vp.base.show_layer;
  else
    *visible = false;
  }

void set_map_transport_visible(map_widget_t *widget, bool visible)
  {
  map_extra_data_t *extra = (map_extra_data_t *)widget->base.extra;
  transport_params_t vp;
  if (succeeded(map_get_layer_parameters(widget->map, MAP_LAYER_TRANSPORT,
                                        sizeof(vp), &vp)))
    {
    vp.base.show_layer = visible;
    map_set_layer_parameters(widget->map, MAP_LAYER_TRANSPORT, &vp);
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
                                        sizeof(vp), &vp)))
    *visible = vp.base.show_layer;
  else
    *visible = false;
  }

void set_map_obstacles_visible(map_widget_t *widget, bool visible)
  {
  map_extra_data_t *extra = (map_extra_data_t *)widget->base.extra;
  obstacles_params_t vp;
  if (succeeded(map_get_layer_parameters(widget->map, MAP_LAYER_OBSTACLES,
                                        sizeof(vp), &vp)))
    {
    vp.base.show_layer = visible;
    map_set_layer_parameters(widget->map, MAP_LAYER_OBSTACLES, &vp);
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
                                        sizeof(vp), &vp)))
    *visible = vp.base.show_layer;
  else
    *visible = false;
  }

void set_map_airspace_visible(map_widget_t *widget, bool visible)
  {
  map_extra_data_t *extra = (map_extra_data_t *)widget->base.extra;
  airspace_params_t vp;
  if (succeeded(map_get_layer_parameters(widget->map, MAP_LAYER_AIRSPACES,
                                        sizeof(vp), &vp)))
    {
    vp.base.show_layer = visible;
    map_set_layer_parameters(widget->map, MAP_LAYER_AIRSPACES, &vp);
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

result_t create_map_widget(handle_t parent, uint32_t flags, map_widget_t* wnd, handle_t* out)
  {
  result_t result;
  if (failed(result = create_widget(parent, flags, map_wndproc, &wnd->base, &wnd->hwnd)))
    return result;

  extent_t background_ex = { rect_width(&wnd->base.rect), rect_height(&wnd->base.rect) };

  // TODO: should be smarter than this.
  if (failed(result = map_create_canvas(wnd->hwnd, 0, &background_ex, wnd->db_path, &wnd->day_theme, &wnd->map)))
    return result;

  wnd->base.on_paint = on_paint;
  // TODO: this should be from a setting
  wnd->map_mode = mdm_track;

  // create our extra storage
  map_extra_data_t *extra = calloc(1, sizeof(map_extra_data_t));

  wnd->base.extra = extra;

  // open the registry key
  memid_t memid = 0;
  char *path = strdup(wnd->config_path);
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

#ifdef _DEBUG

    int test_case = 0;

    switch (test_case)
      {
      default:
      case 0 :
        // npl threshold 05
        // -39.010476, 174.170100
        extra->map_position.lng = 174.170100f;
        extra->map_position.lat = -39.010476f;
        extra->map_position.alt = feet_to_meters(150);
        wnd->track = 50;
        wnd->heading = 50;
        break;
      case 1 :
        // rangioira
        // -43.290001,172.542007
        extra->map_position.lat = -43.290001;
        extra->map_position.lng = 172.542007;
        extra->map_position.alt = feet_to_meters(180);
        wnd->track = 220;
        wnd->heading = 220;
        break;
      case 2 :
        // stratford aerodrome
        // -39.318319, 174.308126
        extra->map_position.lat = -39.318319f;
        extra->map_position.lng = 174.308126f;
        extra->map_position.alt = feet_to_meters(951);
        wnd->track = 90;
        wnd->heading = 90;
        break;
      case 3 :
        // Ardmore
        // -37.064376, 174.978367
        extra->map_position.lat = -37.064376;
        extra->map_position.lng = 174.978367;
        extra->map_position.alt = feet_to_meters(111);
        wnd->track = 21;
        wnd->heading = 21;
        break;
      case 4 :
        // oshkosh wi
        // 43.998287, -88.567753
        extra->map_position.lat = 43.998287;
        extra->map_position.lng = -88.567753;
        extra->map_position.alt = feet_to_meters(809);
        wnd->track = 27;
        wnd->heading = 27;
        break;
      case 5 :
        // sfo
        // 37.466539, -122.242943
        extra->map_position.lat = 37.466539;
        extra->map_position.lng = -122.242943;
        extra->map_position.alt = feet_to_meters(13);
        wnd->track = 28;
        wnd->heading = 28;
        break;
      case 6 :
        // OAK
        // 37.619909, -122.207544
        extra->map_position.lat = 37.619909f;
        extra->map_position.lng = -122.207544f;
        extra->map_position.alt = feet_to_meters(8.5);
        wnd->track = 10;
        wnd->heading = 10;
        break;
      }

#endif
    extra->position_changed = true;
    }

  if (failed(result = map_update_position(wnd->map, &extra->map_position,
                                          wnd->heading, wnd->track)))
    goto config_error;

  if (failed(result = cfg_get_int32(extra->map_settings, map_zoom_name,
                                    &extra->range, nullptr)))
    {
    extra->range =
        (int32_t)(10 * METERS_PER_NM); // width of the scaled screen in meters
    extra->range_changed = true;
    }

  if (failed(result = map_set_range(wnd->map, extra->range)))
    goto config_error;

  if (failed(result = cfg_get_int32(extra->map_settings, map_display_mode_name,
                                    &extra->display_mode, nullptr)))
    {
    extra->display_mode = mdm_track;
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

  if (failed(result = cfg_get_int32(extra->airspaces_key, info_panel_zoom_name, &asp.info_panel_zoom, nullptr)) &&
      failed(result = cfg_set_int32(extra->airspaces_key, info_panel_zoom_name, asp.info_panel_zoom, nullptr)))
    goto config_error;

  if (failed(result = cfg_get_int32(extra->airspaces_key, detail_panel_zoom_name, &asp.detail_info_panel_zoom, nullptr)) &&
      failed(result = cfg_set_int32(extra->airspaces_key, detail_panel_zoom_name, asp.detail_info_panel_zoom, nullptr)))
    goto config_error;

  if (failed(result = sys_map_set_layer_parameters(wnd->map, MAP_LAYER_AIRSPACES, &asp.base)))
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


