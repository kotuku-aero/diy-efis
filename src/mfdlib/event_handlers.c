#include "event_handlers.h"

static handle_t doorbell;

const char* map_position_name = "posn";         // last known location
const char* map_display_mode_name = "mode";     // north up to track up
const char* map_zoom_name = "zoom";             // zoom distance

const char* terrain_key = "terrain";
const char* contours_key = "contours";
const char* cities_key = "cities";
const char* water_key = "water";
const char* navdata_key = "navdata";
const char* transport_key = "transport";
const char* obstacles_key = "obstacles";

const char* visible_name = "show";
const char* terrain_warning_name = "warning";
const char* alarm_elevation_name = "elev-alarm";
const char* warning_elevation_name = "elev-warn";
const char* show_hillshade_name = "hillshade";
const char* show_dem_name = "dem";
const char* hypsometric_tint_name = "tint";
const char* info_panel_zoom_name = "info-zoom";
const char* detail_panel_zoom_name = "detail-zoom";

typedef struct _map_extra_data_t {
  memid_t map_settings;

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

//
// static void setting_worker(void* arg)
//   {
//   map_extra_data_t* extra = (map_extra_data_t*)arg;
//   memid_t child;
//   while (true)
//     {
//     // update the settings to the registry every 2 seconds
//     semaphore_wait(doorbell, 2000);
//
//     if (extra->position_changed)
//       {
//       cfg_set_lla(extra->map_settings, map_position_name, &extra->map_position);
//       enter_critical();
//       extra->position_changed = false;
//       exit_critical();
//       }
//
//     if (extra->display_mode_changed)
//       {
//       cfg_set_int32(extra->map_settings, map_display_mode_name, extra->display_mode);
//       enter_critical();
//       extra->display_mode_changed = false;
//       exit_critical();
//       }
//
//     if (extra->range_changed)
//       {
//       cfg_set_int32(extra->map_settings, map_zoom_name, extra->range);
//       enter_critical();
//       extra->range_changed = false;
//       exit_critical();
//       }
//
//     if (extra->view_water_changed)
//       {
//       if (failed(cfg_open_key(extra->map_settings, water_key, &child)))
//         continue;
//
//       cfg_set_bool(child, visible_name, extra->view_water);
//       enter_critical();
//       extra->view_water_changed = false;
//       exit_critical();
//       }
//
//     if (extra->view_contours_changed)
//       {
//       if (failed(cfg_open_key(extra->map_settings, contours_key, &child)))
//         continue;
//
//       cfg_set_bool(child, visible_name, extra->view_contours);
//       enter_critical();
//       extra->view_contours_changed = false;
//       exit_critical();
//       }
//
//     if (extra->view_cities_changed)
//       {
//       if (failed(cfg_open_key(extra->map_settings, cities_key, &child)))
//         continue;
//
//       cfg_set_bool(child, visible_name, extra->view_cities);
//
//       enter_critical();
//       extra->view_cities_changed = false;
//       exit_critical();
//       }
//
//     if (extra->view_transport_changed)
//       {
//       if (failed(cfg_open_key(extra->map_settings, transport_key, &child)))
//         continue;
//
//       cfg_set_bool(child, visible_name, extra->view_transport);
//       enter_critical();
//       extra->view_transport_changed = false;
//       exit_critical();
//       }
//
//     if (extra->view_obstacles_changed)
//       {
//       if (failed(cfg_open_key(extra->map_settings, obstacles_key, &child)))
//         continue;
//
//       cfg_set_bool(child, visible_name, extra->view_obstacles);
//       enter_critical();
//       extra->view_obstacles_changed = false;
//       exit_critical();
//       }
//     }
//   }
//
// void set_gps_position(map_widget_t* widget, const lla_t* position)
//   {
//   map_extra_data_t* extra = (map_extra_data_t*)widget->base.extra;
//
//   if (memcmp(position, &extra->map_position, sizeof(lla_t)) != 0)
//     {
//     memcpy(&extra->map_position, position, sizeof(lla_t));
//
//     extra->position_changed = true;
//     }
//   }
//
// void set_map_range(map_widget_t* widget, int32_t range)
//   {
//   map_extra_data_t* extra = (map_extra_data_t*)widget->base.extra;
//   if (range != extra->range)
//     {
//     extra->range = range;
//     extra->range_changed = true;
//     }
//
//   widget->params.scale = range;
//   }
//
// void get_map_range(map_widget_t* widget, int32_t* range)
//   {
//   map_extra_data_t* extra = (map_extra_data_t*)widget->base.extra;
//   *range = extra->range;
//   }
//
// void set_map_mode(map_widget_t* widget, map_display_mode mode)
//   {
//   map_extra_data_t* extra = (map_extra_data_t*)widget->base.extra;
//
//   if (mode != extra->display_mode)
//     {
//     extra->display_mode = mode;
//     extra->display_mode_changed = true;
//     semaphore_signal(doorbell);
//     }
//
//   widget->params.mode = mode;
//   }
//
// void get_map_mode(map_widget_t* widget, map_display_mode* mode)
//   {
//   map_extra_data_t* extra = (map_extra_data_t*)widget->base.extra;
//   *mode = (map_display_mode)extra->display_mode;
//   }
//
// void get_map_contours_visible(map_widget_t* wnd, bool* visible)
//   {
//   map_extra_data_t* extra = (map_extra_data_t*)wnd->base.extra;
//   *visible = extra->view_contours;
//   }
//
// void set_map_contours_visible(map_widget_t* widget, bool visible)
//   {
//   map_extra_data_t* extra = (map_extra_data_t*)widget->base.extra;
//   if (visible != extra->view_contours)
//     {
//     extra->view_contours = visible;
//     extra->view_contours_changed = true;
//     semaphore_signal(doorbell);
//     }
//
//   widget->show_contours = visible;
//   }
//
// void get_map_cities_visible(map_widget_t* wnd, bool* visible)
//   {
//   map_extra_data_t* extra = (map_extra_data_t*)wnd->base.extra;
//   *visible = extra->view_cities;
//   }
//
// void set_map_cities_visible(map_widget_t* widget, bool visible)
//   {
//   map_extra_data_t* extra = (map_extra_data_t*)widget->base.extra;
//   if (visible != extra->view_cities)
//     {
//     extra->view_cities = visible;
//     extra->view_cities_changed = true;
//     semaphore_signal(doorbell);
//     }
//
//   widget->show_cities = visible;
//   }
//
// void get_map_water_visible(map_widget_t* widget, bool* visible)
//   {
//   map_extra_data_t* extra = (map_extra_data_t*)widget->base.extra;
//   *visible = extra->view_water;
//   }
//
// void set_map_water_visible(map_widget_t* widget, bool visible)
//   {
//   map_extra_data_t* extra = (map_extra_data_t*)widget->base.extra;
//   if (visible != extra->view_water)
//     {
//     extra->view_water = visible;
//     extra->view_water_changed = true;
//     semaphore_signal(doorbell);
//     }
//
//   widget->show_water = visible;
//   }
//
// void get_map_transport_visible(map_widget_t* widget, bool* visible)
//   {
//   map_extra_data_t* extra = (map_extra_data_t*)widget->base.extra;
//   *visible = extra->view_transport;
//   }
//
// void set_map_transport_visible(map_widget_t* widget, bool visible)
//   {
//   map_extra_data_t* extra = (map_extra_data_t*)widget->base.extra;
//   if (visible != extra->view_transport)
//     {
//     extra->view_transport = visible;
//     extra->view_transport_changed = true;
//     semaphore_signal(doorbell);
//     }
//
//   widget->show_transport = visible;
//   }
//
// void get_map_obstacles_visible(map_widget_t* widget, bool* visible)
//   {
//   map_extra_data_t* extra = (map_extra_data_t*)widget->base.extra;
//   *visible = extra->view_obstacles;
//   }
//
// void set_map_obstacles_visible(map_widget_t* widget, bool visible)
//   {
//   map_extra_data_t* extra = (map_extra_data_t*)widget->base.extra;
//   if (visible != extra->view_obstacles)
//     {
//     extra->view_obstacles = visible;
//     extra->view_obstacles_changed = true;
//     semaphore_signal(doorbell);
//     }
//
//   widget->show_obstacles = visible;
//   }
//
// result_t on_create_navigator_widget(handle_t hwnd, widget_t* widget)
//   {
//   result_t result;
//   map_widget_t* wnd = (map_widget_t*)widget;
//
//   map_extra_data_t* extra;
//
//   if (failed(result = neutron_calloc(1, sizeof(map_extra_data_t), (void**)&extra)))
//     return result;
//
//   widget->extra = extra;
//
//   // open the registry key
//   memid_t memid = 0;
//   char* path;
//   neutron_strdup(wnd->config_path, 0, &path);
//   char* start = path;
//
//   if (*start == '/')
//     start++;
//   char* end = start;
//
//   // split string
//   while (*end != 0)
//     {
//     if (*end == '/')
//       *end = 0;
//
//     end++;
//     }
//
//   while (start <= end)
//     {
//     if (failed(result = cfg_open_key(memid, start, &extra->map_settings)) &&
//       failed(result = cfg_create_key(memid, start, &extra->map_settings)))
//       {
//       neutron_free(path);
//       return result;        // can't open/create key
//       }
//
//     memid = extra->map_settings;
//
//     while (*start++ != 0); // skip to end of string
//     }
//
//   neutron_free(path);
//
//   // to make sense of the map, load the last known position.
//   // when the GPS starts running it will update it
//   if (failed(result = cfg_get_lla(extra->map_settings, map_position_name, &extra->map_position)))
//     {
//     // set a default.
//     extra->map_position.alt = 0;
//     extra->map_position.lat = 0;
//     extra->map_position.lng = 0;
//
// #ifdef _DEBUG
//     extra->map_position.alt = feet_to_meters(2500);      // about 2500 ft;
//
//     // rangioira
//     // -43.290001,172.542007
//     //wnd->params.ned_center.y = float_to_fixed(-43.290001);
//     //wnd->params.ned_center.x = float_to_fixed(172.542007);
//
//     // stratford aerodrome
//     // -39.318319, 174.308126
//     //wnd->params.ned_center.y = float_to_fixed(-39.318319f);
//     //wnd->params.ned_center.x = float_to_fixed(174.308126f);
//
//     // npl VOR
//     // -39.007241, 174.183977
//     extra->map_position.lat = 174.183977f;
//     extra->map_position.lng = -39.007241f;
//
//     // Ardmore
//     // -37.064376, 174.978367
//     //wnd->params.ned_center.y = float_to_fixed(-37.064376);
//     //wnd->params.ned_center.x = float_to_fixed(174.978367);
//
//     // oshkosh wi
//     // 43.998287, -88.567753
//     //wnd->params.ned_center.y = float_to_fixed(43.998287);
//     //wnd->params.ned_center.x = float_to_fixed(-88.567753);
//
//     // sfo
//     // 37.466539, -122.242943
//     //wnd->params.ned_center.y = float_to_fixed(37.466539);
//     //wnd->params.ned_center.x = float_to_fixed(-122.242943);
//
//     // OAK
//     // 37.619909, -122.207544
//     //wnd->params.ned_center.y = float_to_fixed(37.619909f);
//     //wnd->params.ned_center.x = float_to_fixed(-122.207544f);
// #endif
//     extra->position_changed = true;
//     }
//
//   // convert the registry format to spatial
//   wnd->params.ned_center.latlng.lng = float_to_fixed(extra->map_position.lng);
//   wnd->params.ned_center.latlng.lat = float_to_fixed(extra->map_position.lat);
//   wnd->params.ned_center.elevation = (int16_t)extra->map_position.alt;
//
//   if (failed(result = cfg_get_int32(extra->map_settings, map_zoom_name, &extra->range)))
//     {
//     extra->range = (int32_t)(10 * METERS_PER_NM);             // width of the scaled screen in meters
//     extra->range_changed = true;
//     }
//
//   wnd->params.scale = extra->range;                          // update the starting params
//
//   if (failed(result = cfg_get_int32(extra->map_settings, map_display_mode_name, &extra->display_mode)))
//     {
//     extra->display_mode = mdm_north;
//     extra->display_mode_changed = true;
//     }
//
//   wnd->params.mode = (map_display_mode)extra->display_mode;

  //wnd->map_center.x = rect_width(&widget->rect) >> 1;
  //switch (wnd->params.mode)
  //  {
  //  case mdm_north :
  //    wnd->map_center.y = rect_height(&widget->rect) >> 1;     // north up mode centers map
  //    break;
  //  case mdm_track :
  //  case mdm_course :
  //    wnd->map_center.y = rect_height(&widget->rect) - 120;  // the HSI is 240 x 240 so center above bottom
  //    break;
  //  }

  memid_t child;
  /*********************************************************************/
  //  Terrain

  //if (failed(result = cfg_open_key(extra->map_settings, terrain_key, &child)) &&
  //  failed(result = cfg_create_key(extra->map_settings, terrain_key, &child)))
  //  goto config_error;

  //  // hack
  //cfg_set_bool(child, visible_name, true);
  //cfg_set_bool(child, terrain_warning_name, true);
  //cfg_set_bool(child, show_hillshade_name, true);
  //cfg_set_bool(child, show_dem_name, true);
  //cfg_set_bool(child, hypsometric_tint_name, true);

  //if (failed(result = cfg_get_bool(child, visible_name, &wnd->show_terrain)) &&
  //  failed(result = cfg_set_bool(child, visible_name, false)))
  //  goto config_error;
  //
  //if (failed(result = cfg_get_bool(child, terrain_warning_name, &wnd->terrain_params.show_terrain_warning)) &&
  //  failed(result = cfg_set_bool(child, terrain_warning_name, false)))
  //  goto config_error;

  //if (failed(result = cfg_get_uint16(child, alarm_elevation_name, &wnd->terrain_params.alarm_elevation)))
  //  {
  //  wnd->terrain_params.alarm_elevation = (uint16_t)feet_to_meters(100);
  //  if (failed(result = cfg_set_uint16(child, alarm_elevation_name, wnd->terrain_params.alarm_elevation)))
  //    goto config_error;
  //  }

  //if (failed(result = cfg_get_uint16(child, warning_elevation_name, &wnd->terrain_params.warning_elevation)))
  //  {
  //  wnd->terrain_params.warning_elevation = (uint16_t)feet_to_meters(1000);
  //  if (failed(result = cfg_set_uint16(child, warning_elevation_name, wnd->terrain_params.warning_elevation)))
  //    goto config_error;
  //  }

  //if (failed(result = cfg_get_bool(child, show_hillshade_name, &wnd->terrain_params.show_hillshade)) &&
  //  failed(result = cfg_set_bool(child, show_hillshade_name, false)))
  //  goto config_error;

  //if (failed(result = cfg_get_bool(child, show_dem_name, &wnd->terrain_params.show_terrain)) &&
  //    failed(result = cfg_set_bool(child, show_dem_name, wnd->terrain_params.show_terrain)))
  //    goto config_error;

  //if (failed(result = cfg_get_bool(child, hypsometric_tint_name, &wnd->terrain_params.hypsometric_tint)) &&
  //    failed(result = cfg_set_bool(child, hypsometric_tint_name, wnd->terrain_params.hypsometric_tint)))
  //    goto config_error;

  /*********************************************************************/
  //  Contours
//   if (failed(result = cfg_open_key(extra->map_settings, contours_key, &child)) &&
//     failed(result = cfg_create_key(extra->map_settings, contours_key, &child)))
//     goto config_error;
//
//   if (failed(result = cfg_get_bool(child, visible_name, &wnd->show_contours)))
//     {
//     if (failed(result = cfg_set_bool(child, visible_name, wnd->show_contours)))
//       goto config_error;
//     }
//
//   // TODo: load the color map
//
//   extra->view_contours = wnd->show_contours;
//
//   /*********************************************************************/
//   //  Cities
//   if (failed(result = cfg_open_key(extra->map_settings, cities_key, &child)) &&
//     failed(result = cfg_create_key(extra->map_settings, cities_key, &child)))
//     goto config_error;
//
//   if (failed(result = cfg_get_bool(child, visible_name, &wnd->show_cities)) &&
//     failed(result = cfg_set_bool(child, visible_name, wnd->show_cities)))
//     goto config_error;
//
//   extra->view_cities = wnd->show_cities;
//
//   /*********************************************************************/
//   //  Surface Water
//   if (failed(result = cfg_open_key(extra->map_settings, water_key, &child)) &&
//     failed(result = cfg_create_key(extra->map_settings, water_key, &child)))
//     goto config_error;
//
//   if (failed(result = cfg_get_bool(child, visible_name, &wnd->show_water)) &&
//     failed(result = cfg_set_bool(child, visible_name, wnd->show_water)))
//     goto config_error;
//
//   extra->view_water = wnd->show_water;
//
//   /*********************************************************************/
//   //  Transport
//   if (failed(result = cfg_open_key(extra->map_settings, transport_key, &child)) &&
//     failed(result = cfg_create_key(extra->map_settings, transport_key, &child)))
//     goto config_error;
//
//   if (failed(result = cfg_get_bool(child, visible_name, &wnd->show_transport)) &&
//     failed(result = cfg_set_bool(child, visible_name, wnd->show_transport)))
//     goto config_error;
//
//   extra->view_transport = wnd->show_transport;
//
//   /*********************************************************************/
//   //  Transport
//   if (failed(result = cfg_open_key(extra->map_settings, obstacles_key, &child)) &&
//     failed(result = cfg_create_key(extra->map_settings, obstacles_key, &child)))
//     goto config_error;
//
//   if (failed(result = cfg_get_bool(child, visible_name, &wnd->show_obstacles)) &&
//     failed(result = cfg_set_bool(child, visible_name, wnd->show_obstacles)))
//     goto config_error;
//
//   extra->view_obstacles = wnd->show_obstacles;
//
//   /*********************************************************************/
//   // Navdata
//   if (failed(result = cfg_open_key(extra->map_settings, navdata_key, &child)) &&
//     failed(result = cfg_create_key(extra->map_settings, navdata_key, &child)))
//     goto config_error;
//
//   wnd->navdata_params.info_panel_zoom = 40000;
//   wnd->navdata_params.detail_info_panel_zoom = 20000;
//
//   if (failed(result = cfg_get_int32(child, info_panel_zoom_name, &wnd->navdata_params.info_panel_zoom)) &&
//     failed(result = cfg_set_int32(child, info_panel_zoom_name, wnd->navdata_params.info_panel_zoom)))
//     goto config_error;
//
//
//   if (failed(result = cfg_get_int32(child, detail_panel_zoom_name, &wnd->navdata_params.detail_info_panel_zoom)) &&
//     failed(result = cfg_set_int32(child, detail_panel_zoom_name, wnd->navdata_params.detail_info_panel_zoom)))
//     goto config_error;
//
//
//   // this can fail with path not found on first run
//   if (failed(result = load_layers(wnd, wnd->db_path)) &&
//     result != e_path_not_found)
//     goto config_error;
//
//   text_extent(wnd->params.font, 1, "M", &wnd->font_cell_size);
//   wnd->font_cell_size.dx += 2;
//
//   on_map_widget_created(wnd);
//
//   // create the update worker
//   return task_create("NAV", DEFAULT_STACK_SIZE, setting_worker, extra, BELOW_NORMAL, 0);
//
// config_error:
//   trace_error("Cannot create the map widger");
  // return result;
  // }
