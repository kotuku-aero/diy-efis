#include "../include/converters.h"

static const char integer_format_str[] = "%d";
static const char float_1dp_format_str[] = "%0.1f";
static const char float_2dp_format_str[] = "";

static float float_convert_no_change(float value)
  {
  return value;
  }

///////////////////////////////////////////////////////////////////////////////////
//
// convert a microsecond value to milliseconds.
//
static float float_usec_to_ms(float value)
  {
  return value / 1000;
  }

static float float_ms_to_usec(float value)
  {
  return value * 1000;
  }

const selected_unit_t convert_us_to_ms =
  {
  .name = "Microsecond to MS",
  .suffix = "ms",
  .key_name = "us-ms",
  .format = integer_format_str,
  .increment_factor = 0.1f,
  .convert = float_usec_to_ms,
  .convert_inverse = float_ms_to_usec,
  };

///////////////////////////////////////////////////////////////////////////////////
//
static float float_kelvin_to_c(float value)
  {
  return value - 273.15f;
  }

static float float_kelvin_to_c_inverse(float value)
  {
  return value + 273.15f;
  }

const selected_unit_t convert_kelvin_to_c =
  {
  .name = "Centigrade",
  .suffix = "C",
  .key_name = "k-c",
  .format = integer_format_str,
  .increment_factor = 1.0f,
  .convert = float_kelvin_to_c,
  .convert_inverse = float_kelvin_to_c_inverse,
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_kelvin_to_f(float value)
  {
  return (1.8f * (value - 273.15f)) + 32.0f;
  }

static float float_kelvin_to_f_inverse(float value)
  {
  return (value - 32.0f) / 1.8f + 273.15f;
  }

const selected_unit_t convert_kelvin_to_f =
  {
  .name = "Farenheight",
  .suffix = "F",
  .key_name = "k-f",
  .format = integer_format_str,
  .increment_factor = 1.0f,
  .convert = float_kelvin_to_f,
  .convert_inverse = float_kelvin_to_f_inverse,
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_ms_to_kmh(float value)
  {
  return value * 3.6f;
  }

static float float_ms_to_kmh_inverse(float value)
  {
  return value / 3.6f;
  }

const selected_unit_t convert_ms_to_kmh =
  {
  .name = "KPH",
  .suffix = "kph",
  .key_name = "ms-kph",
  .format = integer_format_str,
  .increment_factor = 1.0f,
  .convert = float_ms_to_kmh,
  .convert_inverse = float_ms_to_kmh_inverse,
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_ms_to_kts(float value)
  {
  return value * 1.944f;
  }

static float float_ms_to_kts_inverse(float value)
  {
  return value / 1.944f;
  }

const selected_unit_t convert_ms_to_kts =
  {
  .name = "Knots",
  .suffix = "kts",
  .key_name = "ms-kts",
  .format = integer_format_str,
  .increment_factor = 1.0f,
  .convert = float_ms_to_kts,
  .convert_inverse = float_ms_to_kts_inverse,
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_ms_to_mph(float value)
  {
  return value * 2.23694f;
  }

static float float_ms_to_mph_inverse(float value)
  {
  return value / 2.23694f;
  }

const selected_unit_t convert_ms_to_mph =
  {
  .name = "MPH",
  .suffix = "mph",
  .key_name = "ms-mph",
  .format = integer_format_str,
  .increment_factor = 1.0f,
  .convert = float_ms_to_mph,
  .convert_inverse = float_ms_to_mph_inverse,
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_cc_to_litres(float value)
  {
  return value / 1000.0f;
  }

static float float_cc_to_litres_inverse(float value)
  {
  return value * 1000.0f;
  }

const selected_unit_t convert_cc_to_litres =
  {
  .name = "litres",
  .suffix = "litres",
  .key_name = "l-l",
  .format = integer_format_str,
  .increment_factor = 1.0f,
  .convert = float_cc_to_litres,
  .convert_inverse = float_cc_to_litres_inverse,
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_cc_to_gal(float value)
  {
  return value / 3785.41f;
  }

static float float_cc_to_gal_inverse(float value)
  {
  return value * 3785.41f;
  }

const selected_unit_t convert_cc_to_gal =
  {
  .name = "US. Gal.",
  .suffix = "gal",
  .key_name = "l-g",
  .format = float_1dp_format_str,
  .increment_factor = 0.1f,
  .convert = float_cc_to_gal,
  .convert_inverse = float_cc_to_gal_inverse,
  .is_float = true,
  };

///////////////////////////////////////////////////////////////////////////////
//

const selected_unit_t convert_flow_to_litres =
  {
  .name = "L/Hr.",
  .suffix = "l/h",
  .key_name = "lh-lh",
  .format = integer_format_str,
  .increment_factor = 1.0f,
  .convert = float_cc_to_litres,
  .convert_inverse = float_cc_to_litres_inverse,
  };

///////////////////////////////////////////////////////////////////////////////
//
// TODO: This is a placeholder, we need to implement this
static float float_flow_to_gal(float value)
  {
  return value / 3785.41f;
  }

static float float_flow_to_gal_inverse(float value)
  {
  return value * 3785.41f;
  }

const selected_unit_t convert_flow_to_gal =
  {
  .name = "Gal/Hr.",
  .suffix = "g/h",
  .key_name = "lh-gh",
  .format = float_1dp_format_str,
  .is_float = true,
  .increment_factor = 1.0f,
  .convert = float_flow_to_gal,
  .convert_inverse = float_flow_to_gal_inverse,
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_kg_to_lbs(float value)
  {
  return value * 2.205f;
  }

static float float_kg_to_lbs_inverse(float value)
  {
  return value / 2.205f;
  }

const selected_unit_t convert_kg_to_lbs =
  {
  .name = "Pounds",
  .suffix = "lbs",
  .key_name = "kg-lbs",
  .format = integer_format_str,
  .increment_factor = 1.0f,
  .convert = float_kg_to_lbs,
  .convert_inverse = float_kg_to_lbs_inverse,
  };

///////////////////////////////////////////////////////////////////////////////
//

const selected_unit_t convert_kg_to_kg =
  {
  .name = "Kg",
  .suffix = "kg",
  .key_name = "kg-kg",
  .format = integer_format_str,
  .increment_factor = 1.0f,
  .convert = float_convert_no_change,
  .convert_inverse = float_convert_no_change,
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_meters_to_nm(float value)
  {
  return value / 1852;
  }

static float float_meters_to_nm_inverse(float value)
  {
  return value * 1852;
  }

const selected_unit_t convert_meters_to_nm =
  {
  .name = "NM",
  .suffix = "nm",
  .key_name = "m-nm",
  .format = float_1dp_format_str,
  .is_float = true,
  .increment_factor = 0.1f,
  .convert = float_meters_to_nm,
  .convert_inverse = float_meters_to_nm_inverse,
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_meters_to_sm(float value)
  {
  return value / 1609.34f;
  }

static float float_meters_to_sm_inverse(float value)
  {
  return value * 1609.34f;
  }

const selected_unit_t convert_meters_to_sm =
  {
  .name = "SM",
  .suffix = "sm",
  .key_name = "m-sm",
  .format = float_1dp_format_str,
  .is_float = true,
  .increment_factor = 0.1f,
  .convert = float_meters_to_sm,
  .convert_inverse = float_meters_to_sm_inverse,
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_meters_to_km(float value)
  {
  return value / 1000;
  }

static float float_meters_to_km_inverse(float value)
  {
  return value * 1000;
  }

const selected_unit_t convert_meters_to_km =
  {
  .name = "Km",
  .suffix = "km",
  .key_name = "m-km",
  .format = float_1dp_format_str,
  .is_float = true,
  .increment_factor = 0.1f,
  .convert = float_meters_to_km,
  .convert_inverse = float_meters_to_km_inverse,
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_meters_to_ft(float value)
  {
  return value * 3.28084f;
  }

static float float_meters_to_ft_inverse(float value)
  {
  return value / 3.28084f;
  }

const selected_unit_t convert_meters_to_ft =
  {
  .name = "Ft",
  .suffix = "ft",
  .key_name = "m-ft",
  .format = integer_format_str,
  .increment_factor = 1.0f,
  .convert = float_meters_to_ft,
  .convert_inverse = float_meters_to_ft_inverse,
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_meters_to_flight_level(float value)
  {
  return value / 0.00328084f;
  }

static float float_meters_to_flight_level_inverse(float value)
  {
  return value * 0.00328084f;
  }

const selected_unit_t convert_meters_to_flight_level =
  {
  .name = "FL",
  .suffix = "FL",
  .key_name = "m-fl",
  .format = integer_format_str,
  .increment_factor = 1.0f,
  .convert = float_meters_to_flight_level,
  .convert_inverse = float_meters_to_flight_level_inverse,
  };

///////////////////////////////////////////////////////////////////////////////
//
const selected_unit_t convert_meters_to_m =
  {
  .name = "M.",
  .suffix = "m",
  .key_name = "m-m",
  .format = integer_format_str,
  .increment_factor = 1.0f,
  .convert = float_convert_no_change,
  .convert_inverse = float_convert_no_change,
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_hpa_to_inhg(float value)
  {
  return value * 0.0295299f;
  }

static float float_hpa_to_inhg_inverse(float value)
  {
  return value / 0.0295299f;
  }

const selected_unit_t convert_hpa_to_inhg =
  {
  .name = "In/Hg",
  .suffix = "inhg",
  .key_name = "hpa-inhg",
  .format = float_2dp_format_str,
  .is_float = true,
  .increment_factor = 1.0f,
  .convert = float_hpa_to_inhg,
  .convert_inverse = float_hpa_to_inhg_inverse,
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_hpa_to_inhg100(float value)
  {
  return value * 2.95299f;
  }

static float float_hpa_to_inhg100_inverse(float value)
  {
  return value / 2.95299f;
  }

const selected_unit_t convert_hpa_to_inhg100 =
  {
  .name = "In/Hg * 100",
  .suffix = "inhg",
  .key_name = "hpa-inhg100",
  .format = float_2dp_format_str,
  .is_float = true,
  .increment_factor = 1.0f,
  .convert = float_hpa_to_inhg100,
  .convert_inverse = float_hpa_to_inhg100_inverse,
  };

///////////////////////////////////////////////////////////////////////////////
//
const selected_unit_t convert_hpa_to_hpa =
  {
  .name = "Hpa",
  .suffix = "hpa",
  .key_name = "hpa-hpa",
  .format = integer_format_str,
  .increment_factor = 1.0f,
  .convert = float_convert_no_change,
  .convert_inverse = float_convert_no_change,
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_hpa_to_psi(float value)
  {
  return value * 0.0145038f;
  }

static float float_hpa_to_psi_inverse(float value)
  {
  return value / 0.0145038f;
  }

const selected_unit_t convert_hpa_to_psi =
  {
  .name = "Psi",
  .suffix = "psi",
  .key_name = "hpa-psi",
  .format = integer_format_str,
  .increment_factor = 1.0f,
  .convert = float_hpa_to_psi,
  .convert_inverse = float_hpa_to_psi_inverse,
  };

///////////////////////////////////////////////////////////////////////////////
//
static float float_ms_to_fpm(float value)
  {
  return (float)(value * 196.85);
  }

static float float_ms_to_fpm_inverse(float value)
  {
  return value * 0.00508f;
  }

const selected_unit_t convert_ms_to_fpm =
  {
  .name = "Fpm",
  .suffix = "fpm",
  .key_name = "ms-fpm",
  .format = integer_format_str,
  .increment_factor = 1.0f,
  .convert = float_ms_to_fpm,
  .convert_inverse = float_ms_to_fpm_inverse,
  };


///////////////////////////////////////////////////////////////////////////////////
//
// convert a floating point voltage value in millivolts to volts.
//
static float float_mv_to_volts(float value)
  {
  int v = (int)((value * 10) + 0.5f);
  return v / 10.0f;
  }

static float float_v_to_mv(float value)
  {
  return value;
  }

const selected_unit_t convert_mv_to_volts =
  {
  .name = "Millivolts to Volts 0.1format",
  .suffix = "v",
  .key_name = "mv-v",
  .format = "%01.1f",
  .increment_factor = 0.1f,
  .is_float = true,
  .convert = float_mv_to_volts,
  .convert_inverse = float_v_to_mv,
  };
