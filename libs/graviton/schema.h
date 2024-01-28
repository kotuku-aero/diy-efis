#ifndef __schema_h__
#define __schema_h__

#include "../atomdb/db.h"

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief These are stored in the content_type of a database
 * header to describe the layer that database is rendered on
*/
typedef enum {
  ct_terrain,           // DEM encoded landmass
  ct_contours,          // contours polygons
  ct_landarea,          // land area polygons. Fills in coastlines and should not be
                        // displayed if countours exist
  ct_cities,            // city polygons
  ct_surface_water,     // surface water polygons and linestrings
  ct_transport,         // transport features
  ct_obstacles,         // airspace obstacles
  ct_airspace,          // airspaces
  ct_coastlines,        // coastline linestrings
  } db_content_type;


// Holder for a null type
typedef struct PACKED _db_spatial_null_t {
  spatial_entity_hdr_t base;
  } db_spatial_null_t;

/**
 * @brief Collection of points
*/
typedef struct PACKED _db_spatial_points_t {
  spatial_entity_hdr_t base;
  uint32_t num_points;            // number of points
  // spatial_string_t strings[0]
  // spatial_point_t points[0];             // variable length points
  } db_spatial_points_t;

typedef struct PACKED _db_spatial_polylines_t {
  spatial_entity_hdr_t base;
  uint32_t num_parts;             // number of polylines
  uint32_t num_points;            // number of points
  // spatial_string_t strings[num_strings];
  // uint32_t parts[num_parts];
  // spatial_point_t points[num_points];
  } db_spatial_polylines_t;

typedef enum _e_intensity {
  intensity_none = 0,
  intensity_low = 1,
  intensity_medium = 2,
  intensity_high = 3
  } e_intensity;

typedef enum _e_runway_surface {
  rs_concrete,
  rs_asphalt,
  rs_gravel,
  rs_water,
  rs_grass,
  rs_ice,
  rs_metal,
  rs_sand,
  rs_unknown,
  rs_snow,
  rs_earth,
  rs_mixed,
  } e_runway_surface;

typedef enum _e_runway_accuracy {
  ra_unknown,
  ra_official,
  ra_aerial_survey,
  ra_reported,
  } e_runway_accuracy;

typedef enum _e_primary_traffic_type {
  at_airfield,
  at_heliport,
  at_seaplane_base,
  at_microlight_site,
  at_gliding_site,
  at_disused,
  at_ballooning_site,
  } e_primary_traffic_type;

typedef enum _e_airfield_facility_type {
  af_civilian,
  af_military,
  af_private_airfield,
  af_hospital,
  af_offshore_platform,
  } e_airfield_facility_type;

typedef enum _e_airfield_availability {
  aa_unknown,
  aa_private,
  aa_public,
  } e_airfield_availability;

/**
 * @brief Display type of a vertical position
 * always stored as meters
*/
typedef enum _e_vertical_position_type {
  vpt_height,
  vpt_altitude,
  vpt_flight_level,
  } e_vertical_position_type;

typedef enum _e_controlled_airspace_class {
  act_unknown,
  act_A,
  act_B,
  act_C,
  act_D,
  act_E,
  act_F,
  act_G,
  } e_controlled_airspace_class;

typedef enum _e_entity_type {
  // airspace types
  ast_danger_area                 = 0x00000010,
  ast_restricted_area             = 0x00000020,
  ast_general_aviation            = 0x00000030,
  ast_ctr                         = 0x00000040,
  ast_cta                         = 0x00000050,
  ast_millitary_zone              = 0x00000060,
  ast_mandatory_broadcast_zone    = 0x00000070,
  ast_aerodrome                   = 0x00000080,
  ast_heliport                    = 0x00000090,
  ast_volcanic_hazard             = 0x000000A0,
  ast_visual_reporting_point      = 0x000000B0,
  ast_common_frequency_zone       = 0x000000C0,
  ast_low_flying_zone             = 0x000000D0,
  ast_transit_lane                = 0x000000E0,
  ast_fiscom                      = 0x000000F0,
  ast_runway                      = 0x00000100,
  ast_fir                         = 0x00000200,
  } e_entity_type;

typedef enum _e_reporting_point_type {
  rpt_visual = 0x0001,
  rpt_disused_airfield = 0x0002,
  rpt_mandatory = 0x0004,
  rpt_helicopter = 0x0008,
  rpt_airways = 0x0010,
  rpt_glider = 0x0020,
  rpt_aerodrome_designated = 0x0040
  } e_reporting_point_type;

typedef enum _e_circuit_direction_type {
  cdt_left_hand,
  cdt_right_hand
  } e_circuit_direction_type;

typedef enum _e_circuit_join_type {
  cjt_downwind = 0x0001,
  cjt_base = 0x0002,
  cjt_long_finals = 0x0004,
  cjt_standard_overhead = 0x0008,
  cjt_standard_downwind45 = 0x0010,
  cjt_standard_downwind90 = 0x0020,
  } e_circuit_join_type;

typedef enum _e_spot_entity_type {
  set_elevation,                  // an elevation
  set_windsock,                   // windsock at airfield
  set_yellow_cross,               // cross on ground
  set_beacon,                     // navigation beacon
  set_fuel_pump,                  // fuel pump
  set_helicopter_landing_area,    // helicopter landing area
  } e_spot_entity_type;

typedef enum _e_obstruction_type {
  ot_unknown,
  ot_building,
  ot_bridge,
  ot_cathedral,
  ot_chimney,
  ot_cooling_tower,
  ot_crane,
  ot_gas_flare,
  ot_mining,
  ot_refinery,
  ot_industrial,
  ot_high_tension_plylon,
  ot_antenna,
  ot_turbine,
  ot_waste,
  ot_place_of_worship,
  ot_lighthouse,
  ot_cableway,
  ot_power_line,
  ot_phone_line,
  ot_offshore_platform,
  } e_obstruction_type;

typedef enum _e_obstruction_lit {
  ol_unknown = 0,
  ol_no = 1,
  ol_yes,
  // these are the morse code dash-dots
  ol_A = 'A',
  ol_B = 'B',
  ol_C = 'C',
  ol_D = 'D',
  ol_E = 'E',
  ol_F = 'F',
  ol_G = 'G',
  ol_H = 'H',
  ol_I = 'I',
  ol_J = 'J',
  ol_K = 'K',
  ol_L = 'L',
  ol_M = 'M',
  ol_N = 'N',
  ol_O = 'O',
  ol_P = 'P',
  ol_Q = 'Q',
  ol_R = 'R',
  ol_S = 'S',
  ol_T = 'T',
  ol_U = 'U',
  ol_V = 'V',
  ol_W = 'W',
  ol_X = 'X',
  ol_Y = 'Y',
  ol_Z = 'Z'
  } e_obstruction_lit;

typedef enum _e_fuel_type {
  ft_none_provided = 0,
  ft_av_gas_100LL = 0x0001,
  ft_jet_a = 0x0002,
  ft_mogas = 0x0004,
  ft_av_gas_UL91 = 0x0008,
  ft_mo_diesel = 0x0010,
  ft_electricity = 0x0020,
  } e_fuel_type;

typedef enum _e_navaid_type {
  nt_ndb,
  nt_vor,
  nt_dme,
  nt_ndb_dme,
  nt_vor_dme,
  } e_navaid_type;

typedef struct PACKED _navigation_object_t {
  spatial_entity_hdr_t base;
  char identifier[6];
  spatial_string_t name;                 // friendly name
  ymd_t effective_date;
  spatial_point_t position;
  } navigation_object_t;

typedef struct _runway_t runway_t;
typedef struct _aerodrome_t aerodrome_t;

typedef struct PACKED _runway_t {
  navigation_object_t base;       // location of the runway

  e_runway_surface surface_type;
  e_runway_accuracy accuracy;

  // aerodrome_t
  uint32_t aerodrome;
  bool right_hand_circuits;

  spatial_string_t designator_end1;
  latlng_t designator_end1_position;
  uint32_t designator_end1_azumith;

  spatial_string_t designator_end2;
  latlng_t designator_end2_position;
  uint32_t designator_end2_azumith;

  spatial_string_t notes;       // runway notes
  uint32_t length;              // length in meters
  uint32_t width;               // width
  } runway_t;

typedef struct PACKED _radio_station_t {
  navigation_object_t base;

  uint16_t id;
  spatial_string_t designation;
  char callsign[4];
  uint32_t frequency;     // frequencies are stored as freq (mhz) * 1000  so 119.1 is 119100
  uint32_t radius;        // range in meters
  spatial_string_t qualifier;
  } navaid_t;

typedef struct PACKED _aerodrome_t {
  navigation_object_t base;

  // runway_t
  uint32_t longest_runway;

  // these are stored in the string table.
  spatial_string_t telephone;
  spatial_string_t email;
  spatial_string_t landing_fee;
  spatial_string_t circuits;
  spatial_string_t caution;
  spatial_string_t remarks;
  spatial_string_t opening_hours;
  spatial_string_t local_identifier;
  spatial_string_t url;
  spatial_string_t operator_name;
  spatial_string_t fuel_comments;

  e_primary_traffic_type traffic_type;
  e_airfield_facility_type faclity_type;
  e_airfield_availability availability;
  e_fuel_type fuel_type;

  ymd_t last_updated;
  } aerodrome_t;

typedef struct PACKED _airspace_t {
  navigation_object_t base;

  e_controlled_airspace_class classification;
  spatial_string_t airspace_active;
  spatial_string_t applicability;
  fixed_t area;           // used in zoom calculations is m2
  int32_t upper_limit;
  int32_t lower_limit;
  bool transponder_mandatory;
  spatial_string_t notes;
  spatial_string_t csabbrev;
  spatial_string_t frequency;

  } airspace_t;

#if defined(__cplusplus)
}
#endif

#endif
