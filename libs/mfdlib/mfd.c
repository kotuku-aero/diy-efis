#include "mfd.h"

#include "../graviton/aircraft.h"
#include "../proton/converters.h"
#include "../proton/menu_window.h"

typedef struct _mfd_wnd_t {
  widget_t base;

  // performance details of the aircraft
  aircraft_t* aircraft;
  memid_t key;
  } mfd_wnd_t;

static mfd_wnd_t mfd;

static uint16_t left_fuel_map;
static uint16_t right_fuel_map;
static uint16_t selected_fuel_map;


static const char *display_units_str = "units";

static void update_selected_fuel_map()
  {
  // TODO: check status of mags as well
  // see code in ECU widget
  if(left_fuel_map == right_fuel_map)
    selected_fuel_map = left_fuel_map;
  }

result_t get_fuel_map(menu_item_checklist_t* checklist, uint16_t* value)
  {
  *value = selected_fuel_map;
  return s_ok;
  }

result_t get_selected_units(menu_item_checklist_t* checklist, uint16_t* value)
  {
  return reg_get_uint16(mfd.key, display_units_str, value);
  }

static void set_si_units()
  {
  // set converters
  to_display_temperature = &convert_kelvin_to_c;
  to_display_airspeed = &convert_ms_to_kts;
  to_display_volume = &convert_litres_to_litres;
  to_display_flow = &convert_flow_to_litres;
  to_display_kg = &convert_kg_to_kg;
  to_display_meters = &convert_meters_to_km;
  to_display_alt = &convert_meters_to_ft;
  to_display_map = &convert_hpa_to_inhg;
  to_display_qnh = &convert_hpa_to_hpa;
  to_display_pressure = &convert_hpa_to_psi;

  reg_set_uint16(mfd.key, display_units_str, 0);
  }

static void set_us_units()
  {
  // set converters
  to_display_temperature = &convert_kelvin_to_f;
  to_display_airspeed = &convert_ms_to_mph;
  to_display_volume = &convert_litres_to_gal;
  to_display_flow = &convert_flow_to_gal;
  to_display_kg = &convert_kg_to_lbs;
  to_display_meters = &convert_meters_to_sm;
  to_display_alt = &convert_meters_to_ft;
  to_display_map = &convert_hpa_to_inhg;
  to_display_qnh = &convert_hpa_to_inhg100;
  to_display_pressure = &convert_hpa_to_psi;

  reg_set_uint16(mfd.key, display_units_str, 1);
  }

static result_t mfd_wndproc(handle_t wnd, const canmsg_t* msg, void* wnddata)
  {
  // handle the well known 
  uint16_t id = get_can_id(msg);
  uint16_t valu16;

  switch (id)
    {
    case id_display_units :
      get_param_uint16(msg, &valu16);
      if(valu16 == 0)
        set_si_units();
      else if(valu16 == 1)
        set_us_units();
      break;

    case id_left_fuel_map :
      get_param_uint16(msg, &left_fuel_map);
      update_selected_fuel_map();
      break;

    case id_right_fuel_map :
      get_param_uint16(msg, &right_fuel_map);
      update_selected_fuel_map();
      break;
    }

  return defwndproc(wnd, msg, wnddata);
  }

result_t create_layout(handle_t parent, memid_t key, create_widgets_fn layout_fn, handle_t* hndl)
  {
  result_t result;

  rect_t rect;
  if (failed(result = window_rect(parent, &rect)))
    return result;

  handle_t hwnd;

  if (failed(result = window_create(parent, &rect, mfd_wndproc, &mfd, 100, &hwnd)))
    return result;

  char regn[REG_STRING_MAX];        // aircraft
  uint16_t len = REG_STRING_MAX;
  aircraft_t* aircraft;

  if (succeeded(result = reg_get_string(key, "regn", regn, &len)) &&
    failed(result = load_aircraft(regn, &aircraft)))
    return result;
  else if (failed(load_aircraft(0, &aircraft)))
    return result;

  mfd.aircraft = aircraft;
  mfd.key = key;

  uint16_t selected_units = 0;

  reg_get_uint16(key, display_units_str, &selected_units);

  if(selected_units == 0)
    set_si_units();
  else
    set_us_units();

  *hndl = hwnd;

  if (failed(result = (*layout_fn)(hwnd, aircraft)))
    return result;

  invalidate_background_rect(hwnd, &rect);
  invalidate_foreground_rect(hwnd, &rect);

  return s_ok;
  }