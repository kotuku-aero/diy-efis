#include "mfd_layout.h"

#include "../proton/map_widget.h"

static handle_t doorbell;

const char* map_position_name = "posn";         // last known location
const char* map_display_mode_name = "mode";     // north up to track up
const char* map_zoom_name = "zoom";             // zoom distance
const char* map_rotation_name = "rotn";         // map rotation

const char* terrain_key = "terrain";
const char* contours_key = "contours";
const char* cities_key = "cities";
const char* water_key = "water";

const char* visible_name = "show";
const char* terrain_warning_name = "warning";
const char* alarm_elevation_name = "elev-alarm";
const char* warning_elevation_name = "elev-warn";
const char* show_hillshade_name = "hillshade";
const char* show_dem_name = "dem";
const char* hypsometric_tint_name = "tint";

typedef struct _map_extra_data_t {
  memid_t map_settings;

  bool position_changed;
  lla_t map_position;

  bool display_mode_changed;
  int32_t display_mode;

  bool zoom_changed;
  int32_t zoom;

  bool rotation_changed;
  uint32_t rotation;
  } map_extra_data_t;


static void setting_worker(void* arg)
  {
  map_extra_data_t* extra = (map_extra_data_t*)arg;
  while (true)
    {
    // update the settings to the registry every 2 seconds
    semaphore_wait(doorbell, 2000);

    if (extra->position_changed)
      {
      reg_set_lla(extra->map_settings, map_position_name, &extra->map_position);
      enter_critical();
      extra->position_changed = false;
      exit_critical();
      }

    if (extra->display_mode_changed)
      {
      reg_set_int32(extra->map_settings, map_display_mode_name, extra->display_mode);
      enter_critical();
      extra->display_mode_changed = false;
      exit_critical();
      }

    if (extra->zoom_changed)
      {
      reg_set_int32(extra->map_settings, map_rotation_name, extra->rotation);
      enter_critical();
      extra->zoom_changed = false;
      exit_critical();
      }

    if (extra->rotation_changed)
      {
      reg_set_uint32(extra->map_settings, map_zoom_name, extra->zoom);
      enter_critical();
      extra->rotation_changed = false;
      exit_critical();
      }
    }
  }

void on_gps_position(map_widget_t* widget, const lla_t* position)
  {
  map_extra_data_t* extra = (map_extra_data_t*)widget->base.extra;

  if (memcmp(position, &extra->map_position, sizeof(lla_t)) != 0)
    {
    memcpy(&extra->map_position, position, sizeof(lla_t));

    extra->position_changed = true;
    }
  }

void on_map_zoom(map_widget_t* widget, int32_t zoom)
  {
  map_extra_data_t* extra = (map_extra_data_t*)widget->base.extra;
  if (zoom != extra->zoom)
    {
    extra->zoom = zoom;
    extra->zoom_changed = true;
    }

  }

void on_map_rotate(map_widget_t* widget, int32_t rotate)
  {
  map_extra_data_t* extra = (map_extra_data_t*)widget->base.extra;

  if (rotate != extra->rotation)
    {
    extra->rotation = rotate;
    extra->rotation_changed = true;
    }
  }

void on_map_mode(map_widget_t* widget, map_display_mode mode)
  {
  map_extra_data_t* extra = (map_extra_data_t*)widget->base.extra;

  if (mode != extra->display_mode)
    {
    extra->display_mode = mode;
    extra->display_mode_changed = true;
    }
  }

result_t on_create_navigator_widget(handle_t hwnd, widget_t* widget)
  {
  result_t result;
  map_widget_t *wnd = (map_widget_t *)widget;

  map_extra_data_t *extra;

  if(failed(result = neutron_calloc(1, sizeof(map_extra_data_t), (void **) &extra)))
    return result;

  widget->extra = extra;

  // open the registry key
  memid_t memid = 0;
  char *path;
  neutron_strdup(wnd->config_path, 0, &path);
  char *start = path;

  if(*start == '/')
    start++;
  char *end = start;
    
  // split string
  while (*end != 0)
    {
    if(*end == '/')
      *end = 0;

    end++;
    }

  while (start <= end)
    {
    if (failed(result = reg_open_key(memid, start, &extra->map_settings)) &&
      failed(result = reg_create_key(memid, start, &extra->map_settings)))
      {
      neutron_free(path);
      return result;        // can't open/create key
      }

      memid = extra->map_settings;
      
      while(*start++ != 0); // skip to end of string
    }

  neutron_free(path);
  
  // to make sense of the map, load the last known position.
  // when the GPS starts running it will update it
  if (failed(result = reg_get_lla(extra->map_settings, map_position_name, &extra->map_position)))
    {
    // set a default.
    extra->map_position.alt = 0;
    extra->map_position.lat = 0;
    extra->map_position.lng = 0;

#ifdef _DEBUG
    extra->map_position.alt = feet_to_meters(2500);      // about 2500 ft;

  // rangioira
  // -43.290001,172.542007
  //wnd->params.ned_center.y = float_to_fixed(-43.290001);
  //wnd->params.ned_center.x = float_to_fixed(172.542007);

  // stratford aerodrome
  // -39.318319, 174.308126
  //wnd->params.ned_center.y = float_to_fixed(-39.318319f);
  //wnd->params.ned_center.x = float_to_fixed(174.308126f);

  // npl VOR
  // -39.007241, 174.183977
    extra->map_position.lat = 174.183977f;
    extra->map_position.lng = -39.007241f;

  // Ardmore
  // -37.064376, 174.978367
  //wnd->params.ned_center.y = float_to_fixed(-37.064376);
  //wnd->params.ned_center.x = float_to_fixed(174.978367);

  // oshkosh wi
  // 43.998287, -88.567753
  //wnd->params.ned_center.y = float_to_fixed(43.998287);
  //wnd->params.ned_center.x = float_to_fixed(-88.567753);

  // sfo
  // 37.466539, -122.242943
  //wnd->params.ned_center.y = float_to_fixed(37.466539);
  //wnd->params.ned_center.x = float_to_fixed(-122.242943);

  // OAK
  // 37.619909, -122.207544
  //wnd->params.ned_center.y = float_to_fixed(37.619909f);
  //wnd->params.ned_center.x = float_to_fixed(-122.207544f);
    #endif
    extra->position_changed = true;
    }

  // convert the registry format to spatial
  wnd->params.ned_center.latlng.lng = float_to_fixed(extra->map_position.lng);
  wnd->params.ned_center.latlng.lat = float_to_fixed(extra->map_position.lat);
  wnd->params.ned_center.elevation = (int16_t)extra->map_position.alt;

  if (failed(result = reg_get_int32(extra->map_settings, map_zoom_name, &extra->zoom)))
    {
    extra->zoom = (int32_t)(90 * meters_per_nm);             // width of the scaled screen in meters
    extra->zoom_changed = true;
    }

  wnd->params.scale = extra->zoom;                          // update the starting params

  if (failed(result = reg_get_int32(extra->map_settings, map_display_mode_name, &extra->display_mode)))
    {
    extra->display_mode = mdm_north;
    extra->display_mode_changed = true;
    }

  wnd->params.mode = (map_display_mode) extra->display_mode;

  wnd->map_center.x = rect_width(&widget->rect) >> 1;
  switch (wnd->params.mode)
    {
    case mdm_north :
  wnd->map_center.y = rect_height(&widget->rect) >> 1;     // north up mode centers map
      break;
    case mdm_track :
    case mdm_course :
      wnd->map_center.y = rect_height(&widget->rect) - 120;  // the HSI is 240 x 240 so center above bottom
      break;
    }

  if (failed(result = reg_get_uint32(extra->map_settings, map_rotation_name, &extra->rotation)))
    {
    extra->rotation_changed = true;
    }

  memid_t child;
  /*********************************************************************/
  //  Terrain

  if (failed(result = reg_open_key(extra->map_settings, terrain_key, &child)) &&
    failed(result = reg_create_key(extra->map_settings, terrain_key, &child)))
    goto config_error;

  if (failed(result = reg_get_bool(child, visible_name, &wnd->show_terrain)) &&
    failed(result = reg_set_bool(child, visible_name, false)))
    goto config_error;
  
  if (failed(result = reg_get_bool(child, terrain_warning_name, &wnd->terrain_params.show_terrain_warning)) &&
    failed(result = reg_set_bool(child, terrain_warning_name, false)))
    goto config_error;

  if (failed(result = reg_get_uint16(child, alarm_elevation_name, &wnd->terrain_params.alarm_elevation)))
    {
    wnd->terrain_params.alarm_elevation = (uint16_t)feet_to_meters(100);
    if (failed(result = reg_set_uint16(child, alarm_elevation_name, wnd->terrain_params.alarm_elevation)))
      goto config_error;
    }

  if (failed(result = reg_get_uint16(child, warning_elevation_name, &wnd->terrain_params.warning_elevation)))
    {
    wnd->terrain_params.warning_elevation = (uint16_t)feet_to_meters(1000);
    if (failed(result = reg_set_uint16(child, warning_elevation_name, wnd->terrain_params.warning_elevation)))
      goto config_error;
    }

  if (failed(result = reg_get_bool(child, show_hillshade_name, &wnd->terrain_params.show_hillshade)) &&
    failed(result = reg_set_bool(child, show_hillshade_name, false)))
    goto config_error;

  if (failed(result = reg_get_bool(child, show_dem_name, &wnd->terrain_params.show_terrain)) &&
      failed(result = reg_set_bool(child, show_dem_name, wnd->terrain_params.show_terrain)))
      goto config_error;

  if (failed(result = reg_get_bool(child, hypsometric_tint_name, &wnd->terrain_params.show_terrain)) &&
      failed(result = reg_set_bool(child, hypsometric_tint_name, wnd->terrain_params.show_terrain)))
      goto config_error;

  /*********************************************************************/
  //  Contours
  if (failed(result = reg_open_key(extra->map_settings, contours_key, &child)) &&
    failed(result = reg_create_key(extra->map_settings, contours_key, &child)))
    goto config_error;

  if (failed(result = reg_get_bool(child, visible_name, &wnd->show_contours)))
    {
    if (failed(result = reg_set_bool(child, visible_name, wnd->show_contours)))
      goto config_error;
    }

  // TODo: load the color map

  /*********************************************************************/
  //  Cities
  if (failed(result = reg_open_key(extra->map_settings, cities_key, &child)) &&
    failed(result = reg_create_key(extra->map_settings, cities_key, &child)))
    goto config_error;

  if (failed(result = reg_get_bool(child, visible_name, &wnd->show_cities)) &&
      failed(result = reg_set_bool(child, visible_name, wnd->show_cities)))
      goto config_error;

  /*********************************************************************/
  //  Cities
  if (failed(result = reg_open_key(extra->map_settings, water_key, &child)) &&
    failed(result = reg_create_key(extra->map_settings, water_key, &child)))
    goto config_error;

  if (failed(result = reg_get_bool(child, visible_name, &wnd->show_water)) &&
      failed(result = reg_set_bool(child, visible_name, wnd->show_water)))
      goto config_error;

  // this can fail with path not found on first run
  if (failed(result = load_layers(wnd, wnd->db_path)) &&
      result != e_path_not_found)
    return result;

  text_extent(wnd->params.font, 1, "M", &wnd->font_cell_size);
  wnd->font_cell_size.dx += 2;

  // create the update worker
  return task_create("NAV", DEFAULT_STACK_SIZE, setting_worker, extra, BELOW_NORMAL, 0);

config_error:
  trace_error("Cannot create the terrain key");
  return result;
  }
