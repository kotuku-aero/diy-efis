#ifndef __converters_h__
#define __converters_h__

#include "proton.h"

typedef float (*convert_value_float_fn)(float value);
typedef int16_t(*convert_value_int16_fn)(int16_t value);
typedef uint16_t(*convert_value_uint16_fn)(uint16_t value);
typedef int32_t(*convert_value_int32_fn)(int32_t value);
typedef uint32_t(*convert_value_uint32_fn)(uint32_t value);

typedef struct _selected_unit_t {
  const char* name;       // name to present in menu select i.e Celcius, Farenheight
  const char* suffix;     // Suffix to use i.e. C F
  const char* key_name;   // name to use in registry to select this converter
  convert_value_float_fn convert_float;   // method to do conversion
  convert_value_int16_fn convert_int16;
  convert_value_uint16_fn convert_uint16;
  convert_value_int32_fn convert_int32;
  convert_value_uint32_fn convert_uint32;
  } selected_unit_t;

/**
 * @brief convert a temperature in kelvin to the selected display format celcius or farenheight
*/
extern const selected_unit_t* to_display_temperature;
/**
 * @brief convert a speed in meters/sec to the selected display format kts, km.h, mph
*/
extern const selected_unit_t* to_display_airspeed;
/**
 * @brief convert volume to the selected display format litres, gallons
*/
extern const selected_unit_t* to_display_volume;
/**
 * @brief convert the flow in l/h to the selected display format l/h or gal/h
*/
extern const selected_unit_t* to_display_flow;
/**
 * @brief Convert the weight in kg to the selected display format kg, or lbs
*/
extern const selected_unit_t* to_display_kg;
/**
 * @brief Convert the distance in meters to the selected display format km, nm, or miles
*/
extern const selected_unit_t* to_display_meters;
/**
 * @brief Convert the altitude in meters to ft/meters
*/
extern const selected_unit_t* to_display_alt;
/**
 * @brief convert barometric pressure from manifold
*/
extern const selected_unit_t* to_display_map;
/**
 * @brief convert barometric pressure for QNH
*/
extern const selected_unit_t* to_display_qnh;
/**
 * @brief display of pressure (fuel, oil).
*/
extern const selected_unit_t* to_display_pressure;
/**
 * @brief Lookup a user-defined converter
 * @param parent
 * @param name
 * @param converter
 * @return
*/
extern result_t lookup_conversion_by_value(memid_t parent, const char* name, const selected_unit_t** converter);
extern result_t lookup_conversion(const char* name, const selected_unit_t** converter);

// quick display converters
extern const selected_unit_t convert_meters_to_flight_level;
extern const selected_unit_t convert_meters_to_feet;

extern const selected_unit_t convert_kelvin_to_c;
extern const selected_unit_t convert_kelvin_to_f;
extern const selected_unit_t convert_ms_to_kmh;
extern const selected_unit_t convert_ms_to_kts;
extern const selected_unit_t convert_ms_to_mph;
extern const selected_unit_t convert_litres_to_litres;
extern const selected_unit_t convert_litres_to_gal;
extern const selected_unit_t convert_flow_to_litres;
extern const selected_unit_t convert_flow_to_gal;
extern const selected_unit_t convert_kg_to_lbs;
extern const selected_unit_t convert_kg_to_kg;
extern const selected_unit_t convert_meters_to_nm;
extern const selected_unit_t convert_meters_to_sm;
extern const selected_unit_t convert_meters_to_km;
extern const selected_unit_t convert_meters_to_ft;
extern const selected_unit_t convert_meters_to_flight_level;
extern const selected_unit_t convert_meters_to_m;
extern const selected_unit_t convert_hpa_to_inhg;
extern const selected_unit_t convert_hpa_to_inhg100;
extern const selected_unit_t convert_hpa_to_hpa;
extern const selected_unit_t convert_hpa_to_psi;
extern const selected_unit_t convert_us_to_ms;

#endif
