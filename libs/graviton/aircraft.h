#ifndef __aircraft_h__
#define __aircraft_h__

#include "../neutron/type_vector.h"
#include "../neutron/vector.h"

//  need some types common to airfields/aircraft
#include "../atomdb/db.h"
#include "../graviton/schema.h"

typedef enum _e_fuel_measurement_type {
  fm_volume,
  fm_weight,
  } e_fuel_measurement_type;

typedef enum _e_liquid_volume_type {
  lv_litres,
  lv_US_gallons,
  lv_imperial_gallons,
  } e_liquid_volume_type;

typedef enum _e_mass_type {
  mt_kilograms,
  mt_pounds
  } e_mass_type;

typedef enum _e_aircraft_type {
  at_aeroplane,
  at_helicopter,
  at_glider,
  at_microlight,
  at_airliner,
  at_fast_jet,
  at_balloon,
  at_auto_gyro,
  at_quadcopter,
  at_obstacle,
  } e_aircraft_type;

typedef enum _e_weight_and_balance_type {
  wb_simple,
  wb_standard,
  wb_helicopter,
  } weight_and_balance_type;

typedef enum _e_airspeed_type {
  ast_indicated,
  ast_true
  } e_airspeed_type;

typedef struct _aircraft_t aircraft_t;

typedef struct _lever_arm_t {
  aircraft_t *aircraft;
  char name[REG_NAME_MAX];    //  pilot, passenger etc.
  uint16_t arm;               //  distance from datum
  uint16_t default_value;     //  always kg
  } lever_arm_t;

type_vector_t(lever_arm);

typedef struct _balance_envelope_t {
  uint16_t arm;
  uint16_t value;
  } balance_envelope_vertex_t;

type_vector_t(balance_envelope_vertex);

typedef struct _cruise_profile_t {
  uint16_t altitude;
  float airspeed;               //  m/s
  uint16_t fuel_burn;           //  l/h
  } cruise_profile_t;

type_vector_t(cruise_profile);

typedef enum _e_aircraft_equipment_type {
  comm_nav_A,   //  GBAS Ldg System
  comm_nav_B,   //  LPV (APV with SBAS)
  comm_nav_C,   //  LORAN C
  comm_nav_D,   //  DME
  comm_nav_E1,   //  FMC WPR ACARS
  comm_nav_E2,   //  D-FIS ACARS
  comm_nav_E3,   //  PDC ACARS
  comm_nav_F,   //  ADF
  comm_nav_G,   //  GNSS
  comm_nav_H,   //  HF RTF
  comm_nav_I,   //  Inertial Navigation
  comm_nav_J1,   //  CPDLC ATN VDL Mode 2
  comm_nav_J2,   //  CPDLC FANS 1/A HFDL
  comm_nav_J3,   //  CPDLC FANS 1/A VDL Mode A
  comm_nav_J4,   //  CPDLC FANS 1/A VDL Mode 2
  comm_nav_J5,   //  CPDLC FANS 1/A SATCOM (INMARSAT)
  comm_nav_J6,   //  CPDLC FANS 1/A SATCOM (MTRSAT)
  comm_nav_J7,   //  CPDLC FANS 1/A SATCOM (Iridium)
  comm_nav_K,   //  MLS
  comm_nav_L,   //  ILS
  comm_nav_M1,   //  ATC RTF SATCOM (INMARSAT)
  comm_nav_M2,   //  ATC RTF (MTSAT)
  comm_nav_M3,   //  ATC RTF (Iridium)
  comm_nav_O,   //  VOR
  comm_nav_R,   //  PBN Approved
  comm_nav_T,   //  TACAN
  comm_nav_U,   //  UHF RTF
  comm_nav_V,   //  VHF RTF
  comm_nav_W,   //  RVSM Approved
  comm_nav_X,   //  MNPS Approved
  comm_nav_Y,   //  VHF with with 8.33 kHz Spacing
  comm_nav_Z,   //  Additional Equipment (not in this list)

  xpndr_A,    // Mode A
  xpndr_C,    // Mode A/C
  xpndr_E,    // Mode S (ident, alt, ADS-B)
  xpndr_H,    // Mode S (ident, alt, enhanced surveillance capability)
  xpndr_I,    // Mode S (ident)
  xpndr_L,    // Mode S (ident, alt, ADS-B, enhanced surveillance capability)
  xpndr_P,    // Mode S (alt)
  xpndr_S,    // Mode S (ident, alt)
  xpndr_X,    // Mode S
  xpndr_B1,    // ADS-B with dedicated 1090 MHz ADS-B \"out\" capability
  xpndr_B2,    // ADS-B with dedicated 1090 MHz ADS-B \"out\" and \"in\" capability
  xpndr_U1,    // ADS-B \"out\" capability using UAT
  xpndr_U2,    // ADS-B \"out\" and \"in\" capability using UAT
  xpndr_V1,    // ADS-B \"out\" capability using VDL Mode 4
  xpndr_V2,    // ADS-B \"out\" and \"in\" capability using VDL Mode 4
  xpndr_D1,    // ADS-C with FANS 1/A capabilities
  xpndr_G1,    // ADS-C with ATN capabilities

  } e_aircraft_equipment_type;
#define LAST_NAVIGATION_EQUIPMENT_TYPE aet_Z


vector_t(e_aircraft_equipment_type);

/**
 * @brief Used to lookup the e_aircraft_equipment_type with its display name
*/
extern const enum_t *equipment_lookups;

/**
 * @brief In-memory representation of an aircraft
*/
typedef struct _aircraft_t {
  base_t base;

  uint32_t hex_code;
  e_aircraft_type type;
  uint32_t minimum_runway_length;
  char *name;
  char registration[REG_NAME_MAX];
  char *description;
  char *color_markings;
  char *other_comms_equipment;
  char *other_nav_equipment;
  char *pbn_equipment;

  e_fuel_type fuel_type;
  e_fuel_type alternate_fuel_type;
  e_fuel_measurement_type fuel_measurement;
  e_liquid_volume_type fuel_volume;
  e_mass_type mass_type;
  e_airspeed_type performance_speed_type;

  // all airspeeds are m/s
  float vs0;
  float vs1;
  float vfe;                  // flap extension speed
  float vno;
  float vne;
  float va;
  float vx;
  float glide_airspeed;       //  glide airspeed, usually ((vy-vx)/2)+vx
  float vy;
  
  bool auto_flap_retract;     // set true if the controller will retract flaps if vfe_max exceeded
  float vfe_max;              // vfe limit m/s
  
  float vtrim;                // speed at which trim is divided by 2

  float climb_airspeed;       //  climb airspeed
  float climb_amsl_rate;      //  sea level climb m/s
  float climb_ceiling_rate;   //  service ceiling climb m/s
  float descent_airspeed;     //  descent airspeed
  float descent_rate;         //  m/s

  uint16_t critical_aoa;
  uint16_t approach_aoa;
  uint16_t climb_aoa;
  uint16_t cruise_aoa;
  uint16_t yaw_max;

  uint16_t climb_amsl_burn_rate;  //  l/h at sea level
  uint16_t climb_ceiling_burn_rate; //  l/h at top of climb
  uint16_t descent_burn_rate; //  l/h
  uint16_t service_ceiling;
  uint16_t default_level;
  uint16_t taxi_fuel;         //  always litres
  uint16_t landing_fuel;
  uint16_t holding_minutes;
  uint16_t hourly_cost;       //  is cost * 100
  uint16_t empty_weight;      //  always KG
  uint16_t maximum_weight;
  uint16_t empty_arm;         //  always in mm
  uint16_t max_fwd;           //  maximum fwd arm
  uint16_t max_rear;          //  maximum rearward arm
  uint16_t glide_ratio;       //  rel 100, 40 = 40:100

  bool hourly_cost_includes_fuel;
  weight_and_balance_type weight_and_balance_type;
  lever_arms_t lever_arms;    //  all weight/balance pts
  balance_envelope_vertexs_t normal_envelope;
  balance_envelope_vertexs_t utility_envelope;
  balance_envelope_vertexs_t aerobatic_envelope;
  e_aircraft_equipment_types_t equipment; // all equipment (can be comms/xpdr)
  cruise_profiles_t cruise_profiles;      //  sorted in order of altitude
  } aircraft_t;

/**
 * @brief load a saved aircraft definition
 * @param regn      Registration to load
 * @param aircraft  Aircraft details to be created
 * @return s_ok if the definition loads ok
*/
extern result_t load_aircraft(const char *regn, aircraft_t **aircraft);
/**
 * @brief Save an aircraft definition
 * @param regn      Name to save as
 * @param aircraft  Data to be saved
 * @return s_ok if saved ok
*/
extern result_t save_aircraft_as(const char *regn, const aircraft_t *aircraft);
/**
 * @brief Save a changed aircraft details
 * @param aircraft aircraft to save
 * @return s_ok if saved ok
*/
extern result_t save_aircraft(const aircraft_t *aircraft);
/**
 * @brief Delete an aircraft from the database
 * @param regn registration to delete
 * @return 
*/
extern result_t delete_aircraft(const char *regn);
/**
 * @brief Release all memory allocated to an aircraft
 * @param aircraft aircraft to free
 * @return s_ok if all data cleaned up
*/
extern result_t free_aircraft(aircraft_t *aircraft);
/**
 * @brief Enumerate the registrations available
 * @param child last child found, set to 0 on first call
 * @param regn  Registration number retrieved
 * @param len   Length of buffer, should be REG_STRING_MAX
 * @return s_ok if another registration, e_not_found when no more available
*/
extern result_t enum_aircraft(memid_t *child, char *regn, uint16_t *len);

#endif
