#ifndef __map_widget_h__
#define __map_widget_h__

#include "widget.h"
#include "../graviton/aircraft.h"
#include "../graviton/viewport.h"
#include "../graviton/schema.h"
#include "../graviton/layer.h"

typedef struct _map_widget_t map_widget_t;

extern void on_gps_position(map_widget_t *widget, const lla_t *position);
extern void on_map_zoom(map_widget_t *widget, int32_t zoom);
extern void on_map_rotate(map_widget_t *widget, int32_t rotate);
extern void on_map_mode(map_widget_t *widget, map_display_mode mode);

typedef viewport_t *viewportp_t;
type_vector_t(viewportp);

typedef struct _map_widget_t {
  widget_t base;
  // actual display window
  handle_t hwnd;

  extent_t font_cell_size;
  aircraft_t* aircraft;

  const char *db_path;
  const char *config_path;
  // this sets how far the aircraft image can crawl over a
  // canvas before a refresh of the background is requested
  uint32_t max_image_crawl;

  // spatial_point_t position;
  map_params_t params;

  // temporary from GPS, only published if the
  // gps_valid event is received as well
  lla_t gps_position;
  // this is the last calculated gps position.
  lla_t prev_gps_position;

  // layers
  // created bitmap that holds the background that
  // will be rendered by render_background
  handle_t background_bitmap;
  bool terminate_background_worker;
  bool render_background_busy;
  bool render_background_complete;

  // signal this to update the background
  handle_t worker_start;
  handle_t background_worker;

  // these are in render order
  // coastline polylines (always rendered)
  viewportps_t coastlines_layers;
  coastline_params_t coastlines_params;

  // these are in render order
  // coastline polygons (always rendered)
  viewportps_t landareas_layers;
  landareas_params_t landareas_params;

  bool show_terrain;
  // these are the static layers
  // gtopo30 terrain
  viewport_t *terrain_layer;
  // specific to the terrain renderer
  terrain_params_t terrain_params;

  bool show_contours;
  // terrain contours
  viewportps_t contours_layers;
  contours_params_t contours_params;

  bool show_water;
  // rivers and lakes
  viewportps_t water_layers;
  surface_water_params_t water_params;

  bool show_cities;
  // cities
  viewportps_t cities_layers;
  cities_params_t cities_params;

  bool show_transport;
  // cities
  viewportps_t transport_layers;
  cities_params_t transport_params;

  bool show_obstacles;
  // obstacles
  viewportps_t obstacles_layers;
  obstacles_params_t obstacles_params;

  // navdata
  viewportps_t navdata_layers;
  airspace_params_t navdata_params;

  // dynamic layers
  // adsb-in traffic
  viewport_t* traffic_layer;
  point_t map_center;         // where the aircraft and hsi arc is centered

  handle_t atom_db;           // spatial database

  int32_t direction;                  // current direction
  int32_t course;                     // course
  int32_t track;

  // individual bitmaps that are displayed
  handle_t map_aircraft;
  } map_widget_t;

extern result_t create_map_widget(handle_t parent, uint16_t id, aircraft_t *aircraft, map_widget_t* wnd, handle_t* hndl);

extern result_t load_layers(map_widget_t* wnd, const char *path);


#endif
