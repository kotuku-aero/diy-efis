#ifndef __geography_h__
#define __geography_h__

#include "../atomdb/spatial.h"

typedef enum _e_ground_cover_render_type {
  gct_lava_flow,
  gct_desert,
  gct_glacier,
  gct_ice_cap,
  gct_inland_water_body,
  gct_sand,
  gct_trees,
  gct_sea,
  gct_urban,
  gct_inland_water_river,
  gct_island_water_body,
  } e_ground_cover_render_type;

typedef enum _e_river_type {
  rpt_tiny_le_350K,
  rtp_small_le_500K,
  rtp_medium_le_750K,
  rpt_large_le_1M,
  rpt_canal,
  } e_river_type;

typedef enum _e_scenery_label_type {
  slt_landscape1,
  slt_landscape2,
  slt_landscape3,
  slt_landscape4,
  slt_landscape5,
  slt_landscape6,
  slt_waterscape1,
  slt_waterscape2,
  slt_waterscape3,
  slt_waterscape4,
  slt_waterscape5,
  slt_waterscape6,
  slt_river1,
  slt_river2,
  slt_river3,
  slt_mountain_summit,
  slt_country,
  } e_scenery_label_type;

typedef enum _e_railway_flags {
  rwf_default = 0,
  rwf_disused = 1,
  rwf_double = 2,
  rwf_tunnel = 4,
  } e_railway_flags;

typedef enum _e_road_flags {
  rdf_motorway,
  rdf_trunk,
  rdf_primary,
  rdf_secondary,
  rdf_tunnel,
  rdf_slip_road,
  rdf_local,
  } e_road_flags;

/*
typedef struct _ground_cover_t {
  pointset_geometry_t base;
  e_ground_cover_render_type type;
  } ground_cover_t;


typedef struct _name_line_t {
  pointset_geometry_t base;

  e_scenery_label_type type;

  spatial_string_t name;
  uint16_t min_scale;
  uint16_t max_scale;
  } name_line_t;

typedef struct _power_line_t {
  pointset_geometry_t base;

  } power_line_t;

typedef struct _railway_t {
  pointset_geometry_t base;

  e_railway_flags flags;
  } railway_t;

typedef struct _river_t {
  pointset_geometry_t base;
  e_river_type type;
  } river_t;

typedef struct _road_t {
  pointset_geometry_t base;
  e_road_flags flags;
  bool median;
  } road_t;

typedef struct _sand_t {
  pointset_geometry_t base;
  } sand_t;

typedef struct _trees_t {
  pointset_geometry_t base;

  } trees_t;
  */
#endif
