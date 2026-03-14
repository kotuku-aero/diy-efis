#include "mfd.h"
#include "event_handlers.h"
#include "menu_handlers.h"

// the global MFD window data
mfd_wnd_t mfd;

static inline void update_selected_fuel_map()
  {
  // TODO: check status of mags as well
  // see code in ECU widget
  if (mfd.left_fuel_map == mfd.right_fuel_map)
    mfd.selected_fuel_map = mfd.left_fuel_map;
  }

static inline void change_hs_mode(uint16_t mode)
  {
  mfd.autopilot_mode &= ~HORZ_MODE_MASK;
  mfd.autopilot_mode |= mode;
  }

static inline void change_vs_mode(uint16_t mode)
  {
  mfd.autopilot_mode &= ~VERT_MODE_MASK;
  mfd.autopilot_mode |= mode;
  }

result_t mfd_wndproc(handle_t wnd, const canmsg_t* msg, void* wnddata)
  {
  // check to see if the message is a navigation message
  // this is id_next, id_prev
  if (succeeded(is_dialog_message(wnd, msg)))
    return s_ok;

  // handle the well known messages
  uint16_t id = get_can_id(msg);
  switch (id)
    {
    case id_autopilot_mode:
      // todo more work here to handle errors
      if (failed(get_param_uint16(msg, &mfd.autopilot_mode)))
        mfd.autopilot_mode = 0;
      break;

    case id_baro_corrected_altitude:
      get_param_float(msg, &mfd.altitude);
      break;

    case id_left_fuel_map:
      get_param_uint16(msg, &mfd.left_fuel_map);
      update_selected_fuel_map();
      break;

    case id_right_fuel_map:
      get_param_uint16(msg, &mfd.right_fuel_map);
      update_selected_fuel_map();
      break;

    case id_max_fuel_available:
      get_param_uint32(msg, &mfd.max_fuel_available);
      break;

    case id_fuel_total:
      get_param_uint32(msg, &mfd.fuel_total);
      break;

    case id_ap_vertical_rate:
      get_param_float(msg, &mfd.vertical_speed);
      break;

    case id_ap_altitude:
      get_param_float(msg, &mfd.altitude);
      break;

    // case id_set_mode :
    //   if (succeeded(get_param_uint16(msg, &ui16)))
    //     set_selected_layout(ui16);
    //   break;
    }

  return defwndproc(wnd, msg, wnddata);
  }

/*
    case id_toggle_vs_mode:
      // determine if in a prior mode to close it.
      switch (autopilot_mode & VERT_MODE_MASK)
        {
        case 0:
          // enter vs_hold mode and set the altitude to the current altitude
          change_vs_mode(ALT_MODE);

          // tell the autopilot to hold the altitude.  This is stored as meters, we set it
          // as centimeters
          create_can_msg_float(&msg, id_ap_set_altitude, altitude);
          can_send(&msg, INDEFINITE_WAIT);

          create_can_msg_float(&msg, id_ap_set_vertical_rate, 0.0f);
          can_send(&msg, INDEFINITE_WAIT);

          create_can_msg_uint16(&msg, id_autopilot_enable, autopilot_mode);
          can_send(&msg, INDEFINITE_WAIT);
          break;
        case ALT_MODE:
          // enter vs_seek mode.  We promprt using a menu to get the target VS rate in m/s but is
          // based on the converter.
          change_vs_mode(VERT_SEEK_MODE);
          break;
        case VERT_SEEK_MODE:
          change_vs_mode(IAS_MODE);
          break;
        case IAS_MODE:
          change_vs_mode(0);
          break;
        }
      // redraw the widget
      invalidate(hwnd);
      break;
    case id_toggle_hs_mode:
      // determine if in a prior mode to close it.
      switch ((autopilot_mode)&HORZ_MODE_MASK)
        {
        case 0:
          // enter hs_hold mode and set the heading to the current heading
          change_hs_mode(HDG_MODE);
          break;
        case HDG_MODE:
          // enter hs_seek mode.  We promprt using a menu to get the target heading in degrees but is
          // based on the converter.
          change_hs_mode(NAV_MODE);
          break;
        case NAV_MODE:
          change_hs_mode(0);
          break;
        default:
          change_hs_mode(0);

          break;
        case id_autopilot_mode:
      get_param_uint16(msg, &mfd.autopilot_mode);
      break;
*/

static void hub_status_changed(aircraft_t* aircraft, result_t result)
  {
  // this will send a message to the MFD using the message queue.
  }

result_t create_layout(handle_t parent, memid_t key, create_widgets_fn layout_fn, wndproc_fn wndproc, handle_t* hndl)
  {
  result_t result;

  rect_t rect;
  if (failed(result = window_rect(parent, &rect)))
    return result;

  handle_t hwnd;

  if (wndproc == 0)
    wndproc = mfd_wndproc;

  if (failed(result = window_create(parent, &rect, wndproc, &mfd, 100 | DS_ENABLED | DS_VISIBLE,  &hwnd)))
    return result;

  if (failed(result = get_aircraft(&mfd.aircraft, nullptr)))
    return result;

  // load the converters
  if (mfd.aircraft.units == display_si)
    {
    mfd.altitude_converter = &convert_meters_to_ft;
    mfd.airspeed_converter = &convert_ms_to_kts;
    mfd.distance_converter = &convert_meters_to_nm;
    mfd.fuel_converter = &convert_cc_to_litres;
    mfd.temp_converter = &convert_kelvin_to_c;
    mfd.vertical_speed_converter = &convert_ms_to_fpm;
    mfd.flow_converter = &convert_flow_to_litres;
    }
  else
    {
    mfd.altitude_converter = &convert_meters_to_ft;
    mfd.airspeed_converter = &convert_ms_to_mph;
    mfd.distance_converter = &convert_meters_to_sm;
    mfd.fuel_converter = &convert_cc_to_gal;
    mfd.temp_converter = &convert_kelvin_to_f;
    mfd.vertical_speed_converter = &convert_ms_to_fpm;
    mfd.flow_converter = &convert_flow_to_gal;
    }

  mfd.key = key;

  *hndl = hwnd;

  if (failed(result = (*layout_fn)(hwnd)))
    return result;

  invalidate(hwnd);

  return s_ok;
  }

result_t get_brightness(menu_item_t* edit, variant_t* value)
{
  uint16_t brightness;
  get_display_brightness(&brightness);

  create_variant_uint32(brightness, value);

  return s_ok;
}

result_t set_brightness(menu_item_t* edit, const variant_t* value)
{
  uint16_t brightness;
  coerce_to_uint16(value, &brightness);
  return set_display_brightness(brightness);
}
