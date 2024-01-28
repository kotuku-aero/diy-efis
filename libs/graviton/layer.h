#ifndef __layer_h__
#define __layer_h__

#include "../photon/photon.h"
// definitions for the on-disk formats
#include "../atomdb/db.h"
#include "viewport.h"

#ifdef __cplusplus
extern "C" {
#endif

// all of these are named assumed a NED project (mdm_north)
typedef struct {
  point_t top_left;
  point_t bottom_left;
  point_t top_right;
  point_t bottom_right;
  } pixel_rhombus_t;

typedef struct {
  int32_t elevation;
  color_t color;
  color_t contour_color;
  } hypsometric_theme_t;

typedef struct {
  color_t alarm_color;
  color_t warning_color;
  color_t water_color;
  color_t land_color;
  color_t coastline_color;
  // TODO: these should be pens
  color_t residential_street;
  color_t residential_area;
  color_t big_road;
  color_t major_road;
  color_t highway;
  color_t class_a_airspace;
  color_t class_b_airspace;
  color_t class_c_airspace;
  color_t class_d_airspace;
  color_t class_e_airspace;
  color_t class_f_airspace;
  color_t class_g_airspace;
  color_t class_m_airspace;
  color_t cfz_airspace;
  color_t mbz_airspace;
  color_t danger_area;
  color_t restricted_area;

  color_t runway_grass_pen;
  color_t runway_grass_color;
  color_t runway_grass_ext_color;

  color_t runway_pen;
  color_t runway_color;
  color_t runway_ext_color;

  hypsometric_theme_t theme[20];    // up to 10,000 ft
  } map_theme_t;

typedef enum {
  mdm_north,          // north up
  mdm_track,          // track up
  mdm_course          // course up
  } map_display_mode;


typedef struct _map_params_t {
  pixel_rhombus_t screen_pos;   // screen position
  point_t body_center;          // body center relative to the map
  geo_rhombus_t geo_pos;        // geometric position of the calculated viewport
  spatial_point_t ned_center;   // rotation center of the map in geocoordinates
  uint32_t rotation;            // rotation from north in degrees

  // this is the scaled distance for the width of the screen / 4
  int32_t scale;                // scale of the selected viewport
  
  bool is_night;                // is map render at night
  // viewport is rendered.
  map_display_mode mode;        // how to display the moving map
  map_theme_t day_theme;
  map_theme_t night_theme;

  const font_t *font;
  } map_params_t;

/**
 * @brief Convert the map scale which is in meters to a fixed point nautical mile distance
 * @param scale_m Scale in meters
 * @return converted scale
*/
static inline fixed_t scale_nm(int32_t scale_m) { return div_fixed(int_to_fixed(scale_m), float_to_fixed(meters_per_nm)); }

#ifdef __cplusplus
}
#endif

#endif
