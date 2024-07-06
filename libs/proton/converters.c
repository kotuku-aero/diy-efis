#include "converters.h"

///////////////////////////////////////////////////////////////////////////////////
//
// convert a microsecond value to milliseconds.
//
static float float_usec_to_ms(float value)
  {
  return value / 1000;
  }

static int16_t int16_usec_to_ms(int16_t value)
  {
  return (int16_t)(((float)value)/1000);
  }

static uint16_t uint16_usec_to_ms(uint16_t value)
  {
  return (int16_t)(((float)value) / 1000);
  }

const selected_unit_t convert_us_to_ms =
  {
  .name = "Microsecond to MS",
  .suffix = "ms",
  .key_name = "us-ms",
  .convert_float = float_usec_to_ms,
  .convert_int16 = int16_usec_to_ms,
  .convert_uint16 = uint16_usec_to_ms
  };

///////////////////////////////////////////////////////////////////////////////////
//
static float float_kelvin_to_c(float value)
  {
  return value - 273.15f;
  }

static int16_t int16_kelvin_to_c(int16_t value)
  {
  return value - 273;
  }

static uint16_t uint16_kelvin_to_c(uint16_t value)
  {
  return value - 273;
  }

const selected_unit_t convert_kelvin_to_c =
  {
  .name = "Centigrade",
  .suffix = "C",
  .key_name = "k-c",
  .convert_float = float_kelvin_to_c,
  .convert_int16 = int16_kelvin_to_c,
  .convert_uint16 = uint16_kelvin_to_c
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_kelvin_to_f(float value)
  {
  return (1.8f * (value - 273.15f)) + 32.0f;
  }

static int16_t int16_kelvin_to_f(int16_t value)
  {
  return (int16_t)float_kelvin_to_f(value);
  }

static uint16_t uint16_kelvin_to_f(uint16_t value)
  {
  return (uint16_t)float_kelvin_to_f(value);
  }

const selected_unit_t convert_kelvin_to_f =
  {
  .name = "Farenheight",
  .suffix = "F",
  .key_name = "k-f",
  .convert_float = float_kelvin_to_f,
  .convert_int16 = int16_kelvin_to_f,
  .convert_uint16 = uint16_kelvin_to_f
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_ms_to_kmh(float value)
  {
  return value * 3.6f;
  }

static int16_t int16_ms_to_kmh(int16_t value)
  {
  return (int16_t)float_ms_to_kmh(value);
  }

static uint16_t uint16_ms_to_kmh(uint16_t value)
  {
  return (uint16_t)float_ms_to_kmh(value);
  }

const selected_unit_t convert_ms_to_kmh =
  {
  .name = "KMH",
  .suffix = "kmh",
  .key_name = "ms-kmh",
  .convert_float = float_ms_to_kmh,
  .convert_int16 = int16_ms_to_kmh,
  .convert_uint16 = uint16_ms_to_kmh
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_ms_to_kts(float value)
  {
  return value * 1.944f;
  }

static int16_t int16_ms_to_kts(int16_t value)
  {
  return (int16_t)float_ms_to_kts(value);
  }

static uint16_t uint16_ms_to_kts(uint16_t value)
  {
  return (uint16_t)float_ms_to_kts(value);
  }

const selected_unit_t convert_ms_to_kts =
  {
  .name = "Knots",
  .suffix = "kts",
  .key_name = "ms-kts",
  .convert_float = float_ms_to_kts,
  .convert_int16 = int16_ms_to_kts,
  .convert_uint16 = uint16_ms_to_kts
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_ms_to_mph(float value)
  {
  return value * 2.23694f;
  }

static int16_t int16_ms_to_mph(int16_t value)
  {
  return (int16_t)float_ms_to_mph(value);
  }

static uint16_t uint16_ms_to_mph(uint16_t value)
  {
  return (uint16_t)float_ms_to_mph(value);
  }

const selected_unit_t convert_ms_to_mph =
  {
  .name = "MPH",
  .suffix = "mph",
  .key_name = "ms-mph",
  .convert_float = float_ms_to_mph,
  .convert_int16 = int16_ms_to_mph,
  .convert_uint16 = uint16_ms_to_mph
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_litres_to_litres(float value)
  {
  return value;
  }

static int16_t int16_litres_to_litres(int16_t value)
  {
  return value;
  }

static uint16_t uint16_litres_to_litres(uint16_t value)
  {
  return value;
  }

const selected_unit_t convert_litres_to_litres =
  {
  .name = "litres",
  .suffix = "litres",
  .key_name = "l-l",
  .convert_float = float_litres_to_litres,
  .convert_int16 = int16_litres_to_litres,
  .convert_uint16 = uint16_litres_to_litres
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_litres_to_gal(float value)
  {
  return value / 3.785f;
  }

static int16_t int16_litres_to_gal(int16_t value)
  {
  return (int16_t)float_litres_to_gal(value);
  }

static uint16_t uint16_litres_to_gal(uint16_t value)
  {
  return (uint16_t)float_litres_to_gal(value);
  }

const selected_unit_t convert_litres_to_gal =
  {
  .name = "US. Gal.",
  .suffix = "gal",
  .key_name = "l-g",
  .convert_float = float_litres_to_gal,
  .convert_int16 = int16_litres_to_gal,
  .convert_uint16 = uint16_litres_to_gal
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_flow_to_litres(float value)
  {
  return value;
  }

static int16_t int16_flow_to_litres(int16_t value)
  {
  return value;
  }

static uint16_t uint16_flow_to_litres(uint16_t value)
  {
  return value;
  }

const selected_unit_t convert_flow_to_litres =
  {
  .name = "L/Hr.",
  .suffix = "l/h",
  .key_name = "lh-lh",
  .convert_float = float_flow_to_litres,
  .convert_int16 = int16_flow_to_litres,
  .convert_uint16 = uint16_flow_to_litres
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_flow_to_gal(float value)
  {
  return value;
  }

static int16_t int16_flow_to_gal(int16_t value)
  {
  return value;
  }

static uint16_t uint16_flow_to_gal(uint16_t value)
  {
  return value;
  }

const selected_unit_t convert_flow_to_gal =
  {
  .name = "Gal/Hr.",
  .suffix = "g/h",
  .key_name = "lh-gh",
  .convert_float = float_flow_to_gal,
  .convert_int16 = int16_flow_to_gal,
  .convert_uint16 = uint16_flow_to_gal
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_kg_to_lbs(float value)
  {
  return value * 2.205f;
  }

static int16_t int16_kg_to_lbs(int16_t value)
  {
  return (int16_t)float_kg_to_lbs(value);
  }

static uint16_t uint16_kg_to_lbs(uint16_t value)
  {
  return (uint16_t)float_kg_to_lbs(value);
  }

const selected_unit_t convert_kg_to_lbs =
  {
  .name = "Pounds",
  .suffix = "lbs",
  .key_name = "kg-lbs",
  .convert_float = float_kg_to_lbs,
  .convert_int16 = int16_kg_to_lbs,
  .convert_uint16 = uint16_kg_to_lbs
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_kg_to_kg(float value)
  {
  return value;
  }

static int16_t int16_kg_to_kg(int16_t value)
  {
  return value;
  }

static uint16_t uint16_kg_to_kg(uint16_t value)
  {
  return value;
  }

const selected_unit_t convert_kg_to_kg =
  {
  .name = "Kg",
  .suffix = "kg",
  .key_name = "kg-kg",
  .convert_float = float_kg_to_kg,
  .convert_int16 = int16_kg_to_kg,
  .convert_uint16 = uint16_kg_to_kg
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_meters_to_nm(float value)
  {
  return value / 1852;
  }

static int16_t int16_meters_to_nm(int16_t value)
  {
  return value / 1852;
  }

static uint16_t uint16_meters_to_nm(uint16_t value)
  {
  return value / 1852;
  }

const selected_unit_t convert_meters_to_nm =
  {
  .name = "NM",
  .suffix = "nm",
  .key_name = "m-nm",
  .convert_float = float_meters_to_nm,
  .convert_int16 = int16_meters_to_nm,
  .convert_uint16 = uint16_meters_to_nm
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_meters_to_sm(float value)
  {
  return value / 1609.34f;
  }

static int16_t int16_meters_to_sm(int16_t value)
  {
  return value / 1609;
  }

static uint16_t uint16_meters_to_sm(uint16_t value)
  {
  return value / 1609;
  }

const selected_unit_t convert_meters_to_sm =
  {
  .name = "SM",
  .suffix = "sm",
  .key_name = "m-sm",
  .convert_float = float_meters_to_sm,
  .convert_int16 = int16_meters_to_sm,
  .convert_uint16 = uint16_meters_to_sm
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_meters_to_km(float value)
  {
  return value / 1000;
  }

static int16_t int16_meters_to_km(int16_t value)
  {
  return value / 1000;
  }

static uint16_t uint16_meters_to_km(uint16_t value)
  {
  return value / 1000;
  }

const selected_unit_t convert_meters_to_km =
  {
  .name = "Km",
  .suffix = "km",
  .key_name = "m-km",
  .convert_float = float_meters_to_km,
  .convert_int16 = int16_meters_to_km,
  .convert_uint16 = uint16_meters_to_km
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_meters_to_ft(float value)
  {
  return value / 3.28084f;
  }

static int16_t int16_meters_to_ft(int16_t value)
  {
  return (int16_t)float_meters_to_ft(value);
  }

static uint16_t uint16_meters_to_ft(uint16_t value)
  {
  return (uint16_t)float_meters_to_ft(value);
  }

const selected_unit_t convert_meters_to_ft =
  {
  .name = "Ft",
  .suffix = "ft",
  .key_name = "m-ft",
  .convert_float = float_meters_to_ft,
  .convert_int16 = int16_meters_to_ft,
  .convert_uint16 = uint16_meters_to_ft
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_meters_to_flight_level(float value)
  {
  return value / 0.00328084f;
  }

static int16_t int16_meters_to_flight_level(int16_t value)
  {
  return (int16_t)float_meters_to_ft(value);
  }

static uint16_t uint16_meters_to_flight_level(uint16_t value)
  {
  return (uint16_t)float_meters_to_flight_level(value);
  }

const selected_unit_t convert_meters_to_flight_level =
  {
  .name = "FL",
  .suffix = "FL",
  .key_name = "m-fl",
  .convert_float = float_meters_to_flight_level,
  .convert_int16 = int16_meters_to_flight_level,
  .convert_uint16 = uint16_meters_to_flight_level
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_meters_to_m(float value)
  {
  return value;
  }

static int16_t int16_meters_to_m(int16_t value)
  {
  return value;
  }

static uint16_t uint16_meters_to_m(uint16_t value)
  {
  return value;
  }

const selected_unit_t convert_meters_to_m =
  {
  .name = "M.",
  .suffix = "m",
  .key_name = "m-m",
  .convert_float = float_meters_to_m,
  .convert_int16 = int16_meters_to_m,
  .convert_uint16 = uint16_meters_to_m
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_hpa_to_inhg(float value)
  {
  return value * 0.0295299f;
  }

static int16_t int16_hpa_to_inhg(int16_t value)
  {
  return (int16_t)float_hpa_to_inhg(value);
  }

static uint16_t uint16_hpa_to_inhg(uint16_t value)
  {
  return (uint16_t)float_hpa_to_inhg(value);
  }

const selected_unit_t convert_hpa_to_inhg =
  {
  .name = "In/Hg",
  .suffix = "inhg",
  .key_name = "hpa-inhg",
  .convert_float = float_hpa_to_inhg,
  .convert_int16 = int16_hpa_to_inhg,
  .convert_uint16 = uint16_hpa_to_inhg
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_hpa_to_inhg100(float value)
  {
  return value * 2.95299f;
  }

static int16_t int16_hpa_to_inhg100(int16_t value)
  {
  return (int16_t)float_hpa_to_inhg(value);
  }

static uint16_t uint16_hpa_to_inhg100(uint16_t value)
  {
  return (uint16_t)float_hpa_to_inhg(value);
  }

const selected_unit_t convert_hpa_to_inhg100 =
  {
  .name = "In/Hg * 100",
  .suffix = "inhg",
  .key_name = "hpa-inhg100",
  .convert_float = float_hpa_to_inhg100,
  .convert_int16 = int16_hpa_to_inhg100,
  .convert_uint16 = uint16_hpa_to_inhg100
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_hpa_to_hpa(float value)
  {
  return value;
  }

static int16_t int16_hpa_to_hpa(int16_t value)
  {
  return value;
  }

static uint16_t uint16_hpa_to_hpa(uint16_t value)
  {
  return value;
  }

const selected_unit_t convert_hpa_to_hpa =
  {
  .name = "Hpa",
  .suffix = "hpa",
  .key_name = "hpa-hpa",
  .convert_float = float_hpa_to_hpa,
  .convert_int16 = int16_hpa_to_hpa,
  .convert_uint16 = uint16_hpa_to_hpa
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_hpa_to_psi(float value)
  {
  return value * 0.0145038f;
  }

static int16_t int16_hpa_to_psi(int16_t value)
  {
  return (int16_t)float_hpa_to_psi(value);
  }

static uint16_t uint16_hpa_to_psi(uint16_t value)
  {
  return (uint16_t)float_hpa_to_psi(value);
  }

const selected_unit_t convert_hpa_to_psi =
  {
  .name = "Psi",
  .suffix = "psi",
  .key_name = "hpa-psi",
  .convert_float = float_hpa_to_psi,
  .convert_int16 = int16_hpa_to_psi,
  .convert_uint16 = uint16_hpa_to_psi
  };

static const selected_unit_t *conversions[] =
  {
  &convert_kelvin_to_c,
  &convert_kelvin_to_f,
  &convert_ms_to_kmh,
  &convert_ms_to_kts,
  &convert_ms_to_mph,
  &convert_litres_to_litres,
  &convert_litres_to_gal,
  &convert_flow_to_litres,
  &convert_flow_to_gal,
  &convert_kg_to_lbs,
  &convert_kg_to_kg,
  &convert_meters_to_nm,
  &convert_meters_to_km,
  &convert_meters_to_ft,
  &convert_meters_to_flight_level,
  &convert_meters_to_m,
  &convert_hpa_to_inhg,
  &convert_hpa_to_inhg100,
  &convert_hpa_to_hpa,
  &convert_hpa_to_psi,
  &convert_us_to_ms,
  0,
  };

const selected_unit_t *to_display_temperature = &convert_kelvin_to_f;
const selected_unit_t *to_display_airspeed = &convert_ms_to_kts;
const selected_unit_t *to_display_volume = &convert_litres_to_litres;
const selected_unit_t *to_display_flow = &convert_flow_to_litres;
const selected_unit_t *to_display_kg = &convert_kg_to_kg;
const selected_unit_t *to_display_meters = &convert_meters_to_nm;
const selected_unit_t *to_display_alt = &convert_meters_to_ft;
const selected_unit_t *to_display_map = &convert_hpa_to_inhg;
const selected_unit_t *to_display_qnh = &convert_hpa_to_hpa;
const selected_unit_t *to_display_pressure = &convert_hpa_to_psi;

typedef struct _global_conveter_t {
  const char *name;
  const selected_unit_t **converter;
  } global_converter_t;

static const global_converter_t global_converters[] =
  {
    { "temp", &to_display_temperature },
    { "airspeed", &to_display_airspeed },
    { "volume", &to_display_volume },
    { "flow", &to_display_flow },
    { "mass", &to_display_kg },
    { "distance", &to_display_meters },
    { "altitude", &to_display_alt },
    { "map", &to_display_map },
    { "qnh", &to_display_qnh },
    { "pressure", &to_display_pressure },
    { 0, 0 }
  };

result_t lookup_conversion_by_value(memid_t parent, const char *value_name, const selected_unit_t **converter)
  {
  result_t result;
  char name[REG_NAME_MAX];
  if (failed(result = reg_get_string(parent, value_name, name, 0)))
    return result;

  return lookup_conversion(name, converter);
  }

result_t lookup_conversion(const char *name, const selected_unit_t **converter)
  {
  // check for system settings first
  for (const global_converter_t *gc = global_converters; gc->name != 0; gc++)
    {
    if (strcmp(name, gc->name) == 0)
      {
      *converter = *gc->converter;
      return s_ok;
      }
    }

  for (const selected_unit_t **conv = conversions; *conv != 0; conv++)
    {
    const selected_unit_t *builtin = *conv;

    if (strcmp(name, builtin->key_name) == 0)
      {
      *converter = builtin;
      return s_ok;
      }
    }

  return e_not_found;
  }
