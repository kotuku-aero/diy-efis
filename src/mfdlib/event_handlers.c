#include "event_handlers.h"

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
  ss_complete,
} settings_state_e;

typedef struct _map_extra_data_t
  {
  overlapped_t *base;
  settings_state_e state;

  memid_t map_settings;
  memid_t water_key;
  memid_t contours_key;
  memid_t cities_key;
  memid_t obstacles_key;
  memid_t terrain_key;
  memid_t transport_key;
  memid_t airspace_key;

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

    data->state = ss_complete;
    data->view_obstacles_changed = false;
    break;
  case ss_complete:
    data->state = ss_init; // wait till next id_timer message
    break;
    }
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
    semaphore_signal(doorbell);
    }

  map_set_mode(widget->map, mode);
  }

void get_map_mode(map_widget_t *widget, map_display_mode *mode)
  {
  map_extra_data_t *extra = (map_extra_data_t *)widget->base.extra;
  *mode = (map_display_mode)extra->display_mode;
  }

void get_map_contours_visible(map_widget_t *wnd, bool *visible)
  {
  map_extra_data_t *extra = (map_extra_data_t *)wnd->base.extra;
  *visible = extra->view_contours;
  }

void set_map_contours_visible(map_widget_t *widget, bool visible)
  {
  map_extra_data_t *extra = (map_extra_data_t *)widget->base.extra;
  if (visible != extra->view_contours)
    {
    extra->view_contours = visible;
    extra->view_contours_changed = true;
    }

  uint32_t layers;
  map_get_layer_visible(widget->map, &layers);
  if (visible)
    layers |= MAP_LAYER_CONTOURS;
  else
    layers &= ~MAP_LAYER_CONTOURS;
  map_set_layer_visible(widget->map, &layers);
  }

void get_map_cities_visible(map_widget_t *wnd, bool *visible)
  {
  map_extra_data_t *extra = (map_extra_data_t *)wnd->base.extra;
  *visible = extra->view_cities;
  }

void set_map_cities_visible(map_widget_t *widget, bool visible)
  {
  map_extra_data_t *extra = (map_extra_data_t *)widget->base.extra;
  if (visible != extra->view_cities)
    {
    extra->view_cities = visible;
    extra->view_cities_changed = true;
    }

  uint32_t layers;
  map_get_layer_visible(widget->map, &layers);
  if (visible)
    layers |= MAP_LAYER_CITIES;
  else
    layers &= ~MAP_LAYER_CITIES;
  map_set_layer_visible(widget->map, &layers);
  }

void get_map_water_visible(map_widget_t *widget, bool *visible)
  {
  map_extra_data_t *extra = (map_extra_data_t *)widget->base.extra;
  *visible = extra->view_water;
  }

void set_map_water_visible(map_widget_t *widget, bool visible)
  {
  map_extra_data_t *extra = (map_extra_data_t *)widget->base.extra;
  if (visible != extra->view_water)
    {
    extra->view_water = visible;
    extra->view_water_changed = true;
    }

  uint32_t layers;
  map_get_layer_visible(widget->map, &layers);
  if (visible)
    layers |= MAP_LAYER_SURFACE_WATER;
  else
    layers &= ~MAP_LAYER_SURFACE_WATER;
  map_set_layer_visible(widget->map, &layers);
  }

void get_map_transport_visible(map_widget_t *widget, bool *visible)
  {
  map_extra_data_t *extra = (map_extra_data_t *)widget->base.extra;
  *visible = extra->view_transport;
  }

void set_map_transport_visible(map_widget_t *widget, bool visible)
  {
  map_extra_data_t *extra = (map_extra_data_t *)widget->base.extra;
  if (visible != extra->view_transport)
    {
    extra->view_transport = visible;
    extra->view_transport_changed = true;
    }

  uint32_t layers;
  map_get_layer_visible(widget->map, &layers);
  if (visible)
    layers |= MAP_LAYER_TRANSPORT;
  else
    layers &= ~MAP_LAYER_TRANSPORT;
  map_set_layer_visible(widget->map, &layers);
  }

void get_map_obstacles_visible(map_widget_t *widget, bool *visible)
  {
  map_extra_data_t *extra = (map_extra_data_t *)widget->base.extra;
  *visible = extra->view_obstacles;
  }

void set_map_obstacles_visible(map_widget_t *widget, bool visible)
  {
  map_extra_data_t *extra = (map_extra_data_t *)widget->base.extra;
  if (visible != extra->view_obstacles)
    {
    extra->view_obstacles = visible;
    extra->view_obstacles_changed = true;
    }

  uint32_t layers;
  map_get_layer_visible(widget->map, &layers);
  if (visible)
    layers |= MAP_LAYER_OBSTACLES;
  else
    layers &= ~MAP_LAYER_OBSTACLES;
  map_set_layer_visible(widget->map, &layers);
  }

static result_t ensure_key(memid_t key, const char *name, memid_t *child)
  {
  result_t result;
  if (failed(result = cfg_open_key(key, name, child, nullptr)))
    return cfg_create_key(key, name, child, nullptr);

  return result;
  }

result_t on_create_navigator_widget(handle_t hwnd, widget_t *widget)
  {
  result_t result;
  map_widget_t *wnd = (map_widget_t *)widget;

  map_extra_data_t *extra;

  if (failed(result =
                 neutron_calloc(1, sizeof(map_extra_data_t), (void **)&extra)))
    return result;

  widget->extra = extra;

  // open the registry key
  memid_t memid = 0;
  char *path;
  neutron_strdup(wnd->config_path, 0, &path);
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
    if (failed(result =
                   ensure_key(memid, start, &extra->map_settings)))
      {
      neutron_free(path);
      return result; // can't open/create key
      }

    memid = extra->map_settings;

    while (*start++ != 0)
      ; // skip to end of string
    }

  neutron_free(path);

  // now create the remaining keys
  if (failed(result = ensure_key(extra->map_settings, contours_key, &extra->contours_key)) ||
      failed(result = ensure_key(extra->map_settings, water_key, &extra->water_key)) ||
      failed(result = ensure_key(extra->map_settings, cities_key, &extra->cities_key)) ||
      failed(result = ensure_key(extra->map_settings, obstacles_key, &extra->obstacles_key)) ||
      failed(result = ensure_key(extra->map_settings, terrain_key, &extra->terrain_key)) ||
      failed(result = ensure_key(extra->map_settings, transport_key, &extra->transport_key)) ||
      failed(result = ensure_key(extra->map_settings, airspace_key, &extra->airspace_key)))
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
    extra->map_position.alt = feet_to_meters(2500); // about 2500 ft;

    // rangioira
    // -43.290001,172.542007
    // wnd->params.ned_center.y = float_to_fixed(-43.290001);
    // wnd->params.ned_center.x = float_to_fixed(172.542007);

    // stratford aerodrome
    // -39.318319, 174.308126
    // wnd->params.ned_center.y = float_to_fixed(-39.318319f);
    // wnd->params.ned_center.x = float_to_fixed(174.308126f);

    // npl VOR
    // -39.007241, 174.183977
    extra->map_position.lat = 174.183977f;
    extra->map_position.lng = -39.007241f;

    // Ardmore
    // -37.064376, 174.978367
    // wnd->params.ned_center.y = float_to_fixed(-37.064376);
    // wnd->params.ned_center.x = float_to_fixed(174.978367);

    // oshkosh wi
    // 43.998287, -88.567753
    // wnd->params.ned_center.y = float_to_fixed(43.998287);
    // wnd->params.ned_center.x = float_to_fixed(-88.567753);

    // sfo
    // 37.466539, -122.242943
    // wnd->params.ned_center.y = float_to_fixed(37.466539);
    // wnd->params.ned_center.x = float_to_fixed(-122.242943);

    // OAK
    // 37.619909, -122.207544
    // wnd->params.ned_center.y = float_to_fixed(37.619909f);
    // wnd->params.ned_center.x = float_to_fixed(-122.207544f);
#endif
    extra->position_changed = true;
    }

  if (failed(result = map_update_position(wnd->map, &extra->map_position,  wnd->heading, wnd->track)))
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
    extra->display_mode = mdm_north;
    extra->display_mode_changed = true;
    }

  if (failed(result = map_set_mode(wnd->map, extra->display_mode)))
    goto config_error;


  wnd->map_center.x = rect_width(&widget->rect) >> 1;
  switch (extra->display_mode)
    {
  case mdm_north:
    wnd->map_center.y =
        rect_height(&widget->rect) >> 1; // north up mode centers map
    break;
  case mdm_track:
  case mdm_course:
    wnd->map_center.y = rect_height(&widget->rect) -
                        120; // the HSI is 240 x 240 so center above bottom
    break;
    }

  /*********************************************************************/
  //  Terrain

  terrain_params_t tp;
  if (failed(result = sys_map_get_layer_parameters(wnd->map, MAP_LAYER_TERRAIN, sizeof(tp), &tp)))
    goto config_error;

  // turn on/off the 
  if (failed(result = cfg_get_bool(extra->terrain_key, visible_name, &tp.base.show_layer, nullptr)) &&
      failed(result = cfg_set_bool(extra->terrain_key, visible_name, tp.base.show_layer, nullptr)))
    goto config_error;

  if (failed(result = cfg_get_bool(extra->terrain_key, terrain_warning_name, &tp.show_terrain_warning, nullptr)) &&
      failed(result = cfg_set_bool(extra->terrain_key, terrain_warning_name, tp.show_terrain_warning, nullptr)))
    goto config_error;

  if (failed(result = cfg_get_uint16(extra->terrain_key, alarm_elevation_name, &tp.alarm_elevation, nullptr)))
    {
    tp.alarm_elevation = (uint16_t)feet_to_meters(100);
    if (failed(result = cfg_set_uint16(extra->terrain_key, alarm_elevation_name, tp.alarm_elevation, nullptr)))
      goto config_error;
    }

  if (failed(result = cfg_get_uint16(extra->terrain_key, warning_elevation_name, &tp.warning_elevation, nullptr)))
    {
    tp.warning_elevation = (uint16_t)feet_to_meters(1000);
    if (failed(result = cfg_set_uint16(extra->terrain_key, warning_elevation_name, tp.warning_elevation, nullptr)))
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

  if (failed(result = sys_map_set_layer_parameters(wnd->map, MAP_LAYER_TERRAIN, &tp)))
    goto config_error;

  /*********************************************************************/
  // Contours 
  contours_params_t cp;

  if (failed(sys_map_get_layer_parameters(wnd->map, MAP_LAYER_CONTOURS, sizeof(cp), &cp)))
    goto config_error;

  if (failed(result = cfg_get_bool(extra->contours_key, visible_name, &cp.base.show_layer, nullptr)) &&
      failed(result = cfg_set_bool(extra->contours_key, visible_name, cp.base.show_layer, nullptr)))
      goto config_error;

  // TODo: load the color map

  if (failed(result = sys_map_set_layer_parameters(wnd->map, MAP_LAYER_CONTOURS, &cp)))
    goto config_error;

  /*********************************************************************/
  //  Cities
  cities_params_t ctp;

  if (failed(sys_map_get_layer_parameters(wnd->map, MAP_LAYER_CITIES, sizeof(ctp), &ctp)))
    goto config_error;

  if (failed(result = cfg_get_bool(extra->cities_key, visible_name, &ctp.base.show_layer, nullptr)) &&
      failed(result = cfg_set_bool(extra->cities_key, visible_name, ctp.base.show_layer, nullptr)))
    goto config_error;

  if (failed(result = sys_map_set_layer_parameters(wnd->map, MAP_LAYER_CITIES, &ctp)))
    goto config_error;

  /*********************************************************************/
  //  Surface Water
  surface_water_params_t swp;

  if (failed(sys_map_get_layer_parameters(wnd->map, MAP_LAYER_SURFACE_WATER, sizeof(swp), &swp)))
    goto config_error;

  if (failed(result = cfg_get_bool(extra->water_key, visible_name, &swp.base.show_layer, nullptr)) &&
      failed(result = cfg_set_bool(extra->water_key, visible_name, swp.base.show_layer, nullptr)))
    goto config_error;

  if (failed(result = sys_map_set_layer_parameters(wnd->map, MAP_LAYER_SURFACE_WATER, &swp)))
    goto config_error;

  /*********************************************************************/
  //  Transport

  transport_params_t trp;

  if (failed(sys_map_get_layer_parameters(wnd->map, MAP_LAYER_TRANSPORT, sizeof(trp), &trp)))
    goto config_error;

  if (failed(result = cfg_get_bool(extra->transport_key, visible_name, &trp.base.show_layer, nullptr)) &&
      failed(result = cfg_set_bool(extra->transport_key, visible_name, trp.base.show_layer, nullptr)))
    goto config_error;

  if (failed(result = sys_map_set_layer_parameters(wnd->map, MAP_LAYER_TRANSPORT, &trp)))
    goto config_error;

  /*********************************************************************/
  //  Obstacles
  obstacles_params_t op;

  if (failed(sys_map_get_layer_parameters(wnd->map, MAP_LAYER_OBSTACLES, sizeof(op), &op)))
    goto config_error;

  if (failed(result = cfg_get_bool(extra->obstacles_key, visible_name, &op.base.show_layer, nullptr)) &&
      failed(result = cfg_set_bool(extra->obstacles_key, visible_name, op.base.show_layer, nullptr)))
    goto config_error;

  if (failed(result = sys_map_set_layer_parameters(wnd->map, MAP_LAYER_OBSTACLES, &op)))
    goto config_error;

  /*********************************************************************/
  // Navdata
  airspace_params_t asp;

  if (failed(sys_map_get_layer_parameters(wnd->map, MAP_LAYER_AIRSPACES, sizeof(asp), &asp)))
    goto config_error;

  // TODO: these belong in the params init code
  asp.info_panel_zoom = 40000;
  asp.detail_info_panel_zoom = 20000;

  if (failed(result = cfg_get_int32(extra->airspace_key, info_panel_zoom_name, &asp.info_panel_zoom, nullptr)) &&
      failed(result = cfg_set_int32(extra->airspace_key, info_panel_zoom_name, asp.info_panel_zoom, nullptr)))
    goto config_error;

  if (failed(result = cfg_get_int32(extra->airspace_key, detail_panel_zoom_name, &asp.detail_info_panel_zoom, nullptr)) &&
      failed(result = cfg_set_int32(extra->airspace_key, detail_panel_zoom_name, asp.detail_info_panel_zoom, nullptr)))
    goto config_error;

  // this can fail with path not found on first run
  if (failed(result = load_layers(wnd, wnd->db_path)) &&
      result != e_path_not_found)
    goto config_error;

  text_extent(wnd->font, 1, "M", &wnd->font_cell_size);
  wnd->font_cell_size.dx += 2;

  return on_map_widget_created(wnd);

config_error:
  trace_error("Cannot create the map widget");
  return result;
  }
