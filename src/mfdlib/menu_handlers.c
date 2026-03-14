#include "menu_handlers.h"

static const char* display_units_str = "units";


result_t get_direct_to_waypoint(menu_item_t * edit, variant_t* value)
  {
  return e_not_implemented;
  }

result_t get_nearest_airport_name(menu_item_t * edit, variant_t* value)
  {
  return e_not_implemented;
  }

result_t get_nearest_vor_name(menu_item_t * edit, variant_t* value)
  {
  return e_not_implemented;
  }

result_t get_nearest_ndb_name(menu_item_t * edit, variant_t* value)
  {
  return e_not_implemented;
  }

result_t get_current_flight_plan(menu_item_t * edit, uint32_t* value)
  {
  return e_not_implemented;
  }

result_t get_autopilot_mode(menu_item_checklist_t* checklist, uint16_t* value)
  {
  *value = mfd.autopilot_mode;
  return s_ok;
  }

result_t get_vertical_speed(menu_item_t* edit, variant_t* value)
  {
  create_variant_float(mfd.vertical_speed, value);

  return s_ok;
  }

result_t set_vertical_speed(menu_item_t* edit, const variant_t* value)
  {
  result_t result;
  float vertical_speed;
  // display is in local units, convert to m/s
  if (failed(result = coerce_to_float(value, &vertical_speed)))
    return result;

  mfd.vertical_speed = mfd.vertical_speed_converter->convert_inverse(vertical_speed);

  canmsg_t msg;
  create_can_msg_float(&msg, id_ap_set_vertical_rate, mfd.vertical_speed);
  can_send(&msg, INDEFINITE_WAIT, nullptr);

  return s_ok;
  }

result_t get_assigned_altitude(menu_item_t* edit, variant_t* value)
  {
  create_variant_float(mfd.altitude_converter->convert_inverse(mfd.altitude), value);

  return s_ok;
  }

result_t set_assigned_altitude(menu_item_t* edit, const variant_t* value)
  {
  result_t result;
  canmsg_t msg;

  // tell the autopilot to hold the altitude.  This is stored as meters, we set it
  // as centimeters
  float altitude;
  if (failed(result = coerce_to_float(value, &altitude)))
    return result;

  // convert to meters from local units
  mfd.altitude = mfd.altitude_converter->convert_inverse(altitude);

  create_can_msg_float(&msg, id_ap_set_altitude, mfd.altitude);
  can_send(&msg, INDEFINITE_WAIT, nullptr);

  return s_ok;
  }

result_t get_hnav_mode(menu_item_t* checklist, uint32_t* value)
  {
  *value = (mfd.autopilot_mode & HORZ_MODE_MASK) >> __builtin_ctz(HORZ_MODE_MASK);

  return s_ok;
  }

result_t get_vnav_mode(menu_item_t* checklist, uint32_t* value)
  {
  *value = (mfd.autopilot_mode & VERT_MODE_MASK) >> __builtin_ctz(VERT_MODE_MASK);

  return s_ok;
  }

result_t get_fuel_map(menu_item_checklist_t* checklist, uint16_t* value)
  {
  *value = mfd.selected_fuel_map;
  return s_ok;
  }

result_t get_selected_units(menu_item_checklist_t* checklist, uint16_t* value)
  {
  return cfg_get_uint16(mfd.key, display_units_str, value, nullptr);
  }

result_t get_fuel_total(menu_item_t* edit, variant_t* value)
  {
  // round to uint32 for the spin edit
  create_variant_uint32((uint32_t)mfd.fuel_converter->convert((float)mfd.fuel_total), value);

  return s_ok;
  }

result_t set_fuel_total(menu_item_t* edit, const variant_t* value)
  {
  result_t result;
  float fuel_total;
  if (failed(result = coerce_to_float(value, &fuel_total)))
    return result;

  // convert from local units to CC
  mfd.fuel_total = (uint32_t)mfd.fuel_converter->convert_inverse(fuel_total);

  canmsg_t msg;
  create_can_msg_uint32(&msg, id_set_fuel_available, mfd.fuel_total);

  return s_ok;
  }

result_t get_fuel_max(menu_item_t* edit, variant_t* value)
  {
  // round to uint32 for the spin edit
  create_variant_uint32((uint32_t)mfd.fuel_converter->convert((float)mfd.max_fuel_available), value);

  return s_ok;
  }

result_t get_node_id(menu_item_t* edit, variant_t* value)
  {
  result_t result;
  uint8_t node_id;
  if (failed(result = get_device_id(&node_id)))
    return result;

  create_variant_uint16(node_id, value);

  return s_ok;
  }

result_t set_node_id(menu_item_t* edit, const variant_t* value)
  {
  result_t result;
  uint8_t node_id;
  if (succeeded(result = coerce_to_uint8(value, &node_id)))
    result = set_device_id(node_id);

  return result;
  }

