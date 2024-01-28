#include "proton.h"
#include "converters.h"

static const char *proton_str = "proton";

// if parg is given then is a handle to a stream which will be closed
result_t run_proton(proton_params_t *args)
  {
  result_t result;
  // run the message queue
  memid_t key;
  if (failed(result = reg_open_key(0, proton_str, &key)) &&
      failed(result = reg_create_key(0, proton_str, &key)))
    {
    trace_error("cannot find the proton key\n");
    return result;
    }

  // set the defaults
  lookup_conversion_by_value(key, "temp-units", &to_display_temperature);
  lookup_conversion_by_value(key, "speed-units", &to_display_airspeed);
  lookup_conversion_by_value(key, "volume-units", &to_display_volume);
  lookup_conversion_by_value(key, "flow-units", &to_display_flow);
  lookup_conversion_by_value(key, "weight-units", &to_display_kg);
  lookup_conversion_by_value(key, "distance-units", &to_display_meters);
  lookup_conversion_by_value(key, "altitude-units", &to_display_alt);
  lookup_conversion_by_value(key, "qnh-units", &to_display_qnh);
  lookup_conversion_by_value(key, "map-units", &to_display_map);
  lookup_conversion_by_value(key, "pressure-units", &to_display_pressure);
  
  handle_t screen;
  if (failed(get_screen(&screen)))
    {
    trace_error("cannot open the screen\n");
    return e_unexpected;
    }

  handle_t hwnd = 0;

  if(failed(result = (args->create_layout)(screen, key, args->create_widgets, &hwnd)))
    {
    trace_error("Cannot create the layout\n");
    return result;
    }
  

  // despatch messages
  canmsg_t msg;
  // returns s_false to stop the queue
  while (true)
    if (succeeded(get_message(&hwnd, &msg)))
      {
      if(hwnd == 0)
        send_message(screen, &msg);
      else
        send_message(hwnd, &msg);
      }

  return s_ok;
  }
