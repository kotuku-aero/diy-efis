#ifndef __converters_h__
#define __converters_h__

#include "proton.h"

typedef float (*convert_value_float_fn)(float value);

typedef struct _selected_unit_t {
  const char* name;       // name to present in menu select i.e Celcius, Farenheight
  const char* suffix;     // Suffix to use i.e. C F
  const char* key_name;   // name to use in registry to select this converter
  const char* format;     // format to use when displaying the value
  bool is_float;          // true if the value is a float, false if it is an int
  float increment_factor; // factor to use when incrementing / decrementing the value

  convert_value_float_fn convert;   // method to do conversion
  convert_value_float_fn convert_inverse;   // method to do reverse conversion
  } selected_unit_t;


// quick display converters
extern const selected_unit_t convert_meters_to_flight_level;
extern const selected_unit_t convert_meters_to_feet;

extern const selected_unit_t convert_kelvin_to_c;
extern const selected_unit_t convert_kelvin_to_f;
extern const selected_unit_t convert_ms_to_kmh;
extern const selected_unit_t convert_ms_to_kts;
extern const selected_unit_t convert_ms_to_mph;
extern const selected_unit_t convert_cc_to_litres;
extern const selected_unit_t convert_cc_to_gal;
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
extern const selected_unit_t convert_ms_to_fpm;
extern const selected_unit_t convert_mv_to_volts;

#endif
