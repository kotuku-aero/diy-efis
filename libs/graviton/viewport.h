#ifndef __viewport_h__
#define __viewport_h__

#include "../neutron/vector.h"
#include "../neutron/type_vector.h"
#include "../photon/photon.h"
#include "../atomdb/db.h"

extern const typeid_t viewport_type;

typedef struct _map_params_t map_params_t;
/**
 * @brief data common to all filters
*/
typedef struct _viewport_params_t {
  const map_params_t* map;      // details about the map being rendered

  // these are calculated when the spatial entities are selected and
  // are then needed to ge-locate points for the current renderer
  fixed_t geo_scale_y;          // degrees per pixel in y-axis
  fixed_t geo_scale_x;          // degrees per pixel in x-axis

  } viewport_params_t;

typedef struct _airspace_params_t {
  viewport_params_t base;

  int32_t info_panel_zoom;             // zoom below which an info panel displayed
  int32_t detail_info_panel_zoom;      // zoom below which a detailed info panel
  } airspace_params_t;

typedef struct _cities_params_t {
  viewport_params_t base;
  } cities_params_t;

typedef struct _coastline_params_t {
  viewport_params_t base;

  } coastline_params_t;

typedef struct _landmass_params_t {
  viewport_params_t base;

  } landareas_params_t;

typedef struct _contours_params_t {
  viewport_params_t base;
  } contours_params_t;

typedef struct _surface_water_params_t {
  viewport_params_t base;
  } surface_water_params_t;

typedef struct _obstances_params_t {
  viewport_params_t base;
  } obstacles_params_t;

typedef struct _terrain_params_t {
  viewport_params_t base;

  bool show_terrain_warning;  // tint areas that are cause of alarm
  uint16_t alarm_elevation;   // height above ground for alarm
  uint16_t warning_elevation; // height aboce ground for warning
  bool show_terrain;          // show the terrain pixels.
  bool show_hillshade;        // show the hillshading
  bool hypsometric_tint;      // shade the terrain
  } terrain_params_t;

typedef struct _transport_params_t {
  viewport_params_t base;
  } transport_params_t;


typedef struct _viewport_t viewport_t;
/**
 * @brief callback to select records and render them onto a canvas
 * @param viewport      created viewport
 * @param params        client parameters
 * @return s_ok once rendered
*/
typedef result_t(*render_entities_fn)(viewport_t* viewport, viewport_params_t* params, handle_t canvas);
typedef result_t (*close_viewport_fn)(viewport_t* viewport);
/**
 * @brief This is the result of a spatial query on a database
 * It does not necessarily contain data.
*/
typedef struct _viewport_t {
  handle_t container;              // opened database

  const db_header_t* hdr;         // cached header of DB

  render_entities_fn select;
  close_viewport_fn close;
  } viewport_t;

extern result_t create_terrain_viewport(handle_t container, const db_header_t* hdr, viewport_t** viewport);
extern result_t create_coastlines_viewport(handle_t container, const db_header_t* hdr, viewport_t** viewport);
extern result_t create_land_viewport(handle_t container, const db_header_t* hdr, viewport_t** viewport);
extern result_t create_contours_viewport(handle_t container, const db_header_t* hdr, viewport_t** viewport);
extern result_t create_cities_viewport(handle_t container, const db_header_t* hdr, viewport_t** viewport);
extern result_t create_surface_water_viewport(handle_t container, const db_header_t* hdr, viewport_t** viewport);
extern result_t create_transport_viewport(handle_t container, const db_header_t* hdr, viewport_t** viewport);
extern result_t create_obstacles_viewport(handle_t container, const db_header_t* hdr, viewport_t** viewport);
extern result_t create_airspace_viewport(handle_t container, const db_header_t* hdr, viewport_t** viewport);

// runtime string loaded from the database
typedef struct _string_t {
  uint32_t length;        // length of the loaded string
  const char *c_str;      // actial string
  } string_t;

type_vector_t(string);

// runtime types
typedef struct _point_spatial_entity_hdr_t {
  spatial_entity_hdr_t base;        // base from database
  strings_t strings;            // any strings associated
  spatial_points_t pointset;    // loaded points
  } point_spatial_entity_hdr_t;

typedef struct _polyline_spatial_entity_hdr_t {
  spatial_entity_hdr_t base;        // base from database
  strings_t strings;            // any strings associated
  polylines_t linestrings;      // all loaded lines
  } polyline_spatial_entity_hdr_t;

typedef struct _polygon_spatial_entity_hdr_t {
  spatial_entity_hdr_t base;        // base from database
  strings_t strings;            // any strings associated
  polylines_t polygons;         // all loaded polygons
  } polygon_spatial_entity_hdr_t;

/**
 * @brief Select all of the entities that could be in a viewport
 * @param db        database file to use
 * @param viewport  spatial rhombus to select
 * @param loaded    where the spatial entity is loaded into
 * @param overlapped  callback when the spatial query is ready
 * @return
*/
static inline result_t select_and_render_viewport(viewport_t* viewport, viewport_params_t* params, handle_t canvas)
  {
  return (*viewport->select)(viewport, params, canvas);
  }

extern void to_screen_coordinates(viewport_params_t* params, const latlng_t* top_left, const latlng_t *pt, point_t *gdi_pt);
extern void rotate_points(const spatial_point_t* center, int16_t angle, spatial_points_t* pts);
extern void rotate_contours(const spatial_point_t* center, int16_t angle, polylines_t* contours);

#endif
