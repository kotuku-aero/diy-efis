#include "aircraft.h"


/**
 * @brief Open a registry key as an aircraft database
 * @param name Registration of the aircraft
 * @param create_if_not_there If the aircraft does not exist, create a template
 * @param key resulting registry key
 * @return s_ok if opened/created ok
*/

static const char *aircraft_config_str = "aircraft";
static const char *aircraft_default_str = "default";
static const char *aircraft_dflt_name_str = "N0000";

static const char *aircraft_name_str = "name";
static const char *aircraft_descr_str = "description";
static const char *aircraft_hex_code_str = "hex-code";
static const char *aircraft_rego_str = "regn";
static const char *aircraft_vs0_str = "vs0";
static const char *aircraft_vs1_str = "vs1";
static const char *aircraft_vfe_str = "vfe";
static const char *aircraft_vno_str = "vno";
static const char *aircraft_vne_str = "vne";
static const char *aircraft_va_str = "va";
static const char *aircraft_vx_str = "vx";
static const char *aircraft_vy_str = "vy";
static const char *aircraft_glide_str = "glide";//  glide airspeed, usually ((vy-vx)/2)+vx
static const char *aircraft_climb_str = "climb";
static const char *aircraft_climb_amsl_str = "climb-amsl-rate";
static const char *aircraft_climb_ceiling_str = "climb-ceiling-rate";
static const char *aircraft_descent_str = "descent";
static const char *aircraft_descent_rate_str = "descent_rate";
/*
  e_aircraft_type type;
  uint32_t minimum_runway_length;
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
*/

static result_t open_or_create_aircraft(const char *name, bool create_if_not_there, memid_t *key)
  {
  result_t result;
  memid_t hive;

  // open or set up the database
  if (failed(result = reg_open_key(0, aircraft_config_str, &hive)) &&
    failed(result = reg_create_key(0, aircraft_config_str, &hive)))
    return result;

  char dflt_name[REG_NAME_MAX];
  uint16_t len;
  if (name == 0)
    {
    len = REG_NAME_MAX;
    // means default aircraft
    if (failed(result = reg_get_string(hive, aircraft_default_str, dflt_name, &len)))
      {
      if (!create_if_not_there ||
        failed(result = reg_set_string(hive, aircraft_default_str, aircraft_dflt_name_str)))
        return result; 

      name = aircraft_dflt_name_str;
      }
    else
      name = dflt_name;
    }

#define KNOTS_TO_MS(k) (k * 0.514444f)
#define FT_SEC_TO_MS(f) (f * 0.3048f)

  if (failed(result = reg_open_key(hive, name, key)))
    {
    if (!create_if_not_there)
      return result;

    if (failed(result = reg_create_key(hive, name, key)))
      return result;

    // set initial values
    if (failed(result = reg_set_string(*key, aircraft_name_str, name)))
      return result;

    if (failed(result = reg_set_string(*key, aircraft_rego_str, name)))
      return result;

    if (failed(result = reg_set_string(*key, aircraft_descr_str, "Default settings")))
      return result;

    if(failed(result = reg_set_float(*key, aircraft_vs0_str, KNOTS_TO_MS(65))))
      return result;

    if (failed(result = reg_set_float(*key, aircraft_vs1_str, KNOTS_TO_MS(45))))
      return result;

    if (failed(result = reg_set_float(*key, aircraft_vfe_str, KNOTS_TO_MS(90))))
      return result;

    if (failed(result = reg_set_float(*key, aircraft_vno_str, KNOTS_TO_MS(145))))
      return result;

    if (failed(result = reg_set_float(*key, aircraft_vne_str, KNOTS_TO_MS(200))))
      return result;

    if (failed(result = reg_set_float(*key, aircraft_va_str, KNOTS_TO_MS(120))))
      return result;

    if (failed(result = reg_set_float(*key, aircraft_vx_str, KNOTS_TO_MS(75))))
      return result;

    if (failed(result = reg_set_float(*key, aircraft_vs0_str, KNOTS_TO_MS(105))))
      return result;

    if (failed(result = reg_set_float(*key, aircraft_glide_str, KNOTS_TO_MS(90))))
      return result;

    if (failed(result = reg_set_float(*key, aircraft_climb_str, KNOTS_TO_MS(120))))
      return result;

    if (failed(result = reg_set_float(*key, aircraft_climb_amsl_str, FT_SEC_TO_MS(2500))))
      return result;

    if (failed(result = reg_set_float(*key, aircraft_climb_ceiling_str, FT_SEC_TO_MS(500))))
      return result;

    if (failed(result = reg_set_float(*key, aircraft_climb_str, KNOTS_TO_MS(145))))
      return result;

    if (failed(result = reg_set_float(*key, aircraft_climb_amsl_str, FT_SEC_TO_MS(500))))
      return result;
    }

  return s_ok;
  }

result_t load_aircraft(const char *name, aircraft_t **aircraft)
  {
  result_t result;
  memid_t key;
  if (failed(result = open_or_create_aircraft(name, name == 0 ? true : false, &key)))
    return result;

  if (failed(result = neutron_malloc(sizeof(aircraft_t), (void **)aircraft)))
    return result;

  aircraft_t *ac = *aircraft;
  memset(ac, 0, sizeof(aircraft_t));

  uint16_t len = REG_NAME_MAX;
  reg_get_string(key, aircraft_name_str, ac->name, &len);

  reg_get_float(key, aircraft_vs0_str, &ac->vs0);
  reg_get_float(key, aircraft_vs1_str, &ac->vs1);
  reg_get_float(key, aircraft_vfe_str, &ac->vfe);
  reg_get_float(key, aircraft_vno_str, &ac->vno);
  reg_get_float(key, aircraft_vne_str, &ac->vne);
  reg_get_float(key, aircraft_va_str, &ac->va);
  reg_get_float(key, aircraft_vx_str, &ac->vx);
  reg_get_float(key, aircraft_vy_str, &ac->vy);

  return s_ok;
  }

result_t save_aircraft_as(const char *name, const aircraft_t *aircraft)
  {
  return e_not_implemented;
  }

result_t save_aircraft(const aircraft_t *aircraft)
  {
  return e_not_implemented;
  }

result_t free_aircraft(aircraft_t *aircraft)
  {
  return e_not_implemented;
  }

result_t enum_aircraft(memid_t *child, char *regn, uint16_t *len)
  {
  return e_not_implemented;
  }
