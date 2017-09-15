#include "widget.h"

extern result_t create_airspeed_window(handle_t parent, memid_t section, handle_t *hwnd);
extern result_t create_altitude_window(handle_t parent, memid_t section, handle_t *hwnd);
extern result_t create_annunciator_window(handle_t parent, memid_t section, handle_t *hwnd);
extern result_t create_attitude_window(handle_t parent, memid_t section, handle_t *hwnd);
extern result_t create_gauge_window(handle_t parent, memid_t section, handle_t *hwnd);
extern result_t create_gps_window(handle_t parent, memid_t section, handle_t *hwnd);
extern result_t create_hsi_window(handle_t parent, memid_t section, handle_t *hwnd);
extern result_t create_menu_window(handle_t parent, memid_t section, handle_t *hwnd);

  enum window_type
    {
    airspeed_window,            // vertical ticker-tape airspeed window
    hsi_window,                 // rectangular gyro position and heading
    attitude_window,            // rectangular attitude window
    altitude_window, // vertical ticker tape altitude and vertical speed indicator window
    gauge_window,               // general purpose round or bar gauge
    annunciator_window,         // time, oat etc.
    gps_window,                 // gps route map window
    alert_window,               // vertical alert window
    menu_window,                // popup menu window
    last_layout_enum
    };

static const char *layout_names[] = {
    "airspeed",            // vertical ticker-tape airspeed window
    "hsi",                 // rectangular gyro position and heading
    "attitude",            // rectangular attitude window
    "altitude", // vertical ticker tape altitude and vertical speed indicator window
    "gauge",               // general purpose round or bar gauge
    "annunciator",         // time, oat etc.
    "gps",                 // gps route map window
    "alert",               // vertical alert window
    "menu_window",         // menu window
};

result_t load_layout(handle_t parent, memid_t hive)
  {
  // the hive must have series of hives that form windows
  result_t result;
  char name[REG_NAME_MAX +1];
  uint16_t length = REG_NAME_MAX + 1;
  field_datatype type = field_key;
  // must be 0 on first call
  memid_t child = 0;
  
  while(succeeded(result = reg_enum_key(hive, &type, 0, 0, REG_NAME_MAX, name, &child)))
    {
    // open the window key
    memid_t key;
    if(failed(result = reg_open_key(hive, name, &key)))
      return result;
    
    char widget_type[REG_STRING_MAX +1];
    length = REG_STRING_MAX +1;
    
    int ordinal;
    if(failed(lookup_enum(key, "type", layout_names, last_layout_enum, &ordinal)))
      {
      trace_warning("Section %s has a type of %s which is not recognised\n", name, widget_type);
      continue;
      }

    trace_info("Create widget %s\n", name);
    
    handle_t hwnd;

    switch(ordinal)
      {
      case airspeed_window :
        create_airspeed_window(parent, child, &hwnd);
        break;
      case hsi_window :
        create_hsi_window(parent, child, &hwnd);
        break;
      case attitude_window :
        create_attitude_window(parent, child, &hwnd);
        break;
      case altitude_window :
        create_altitude_window(parent, child, &hwnd);
        break;
      case gauge_window :
        create_gauge_window(parent, child, &hwnd);
        break;
      case annunciator_window :
        create_annunciator_window(parent, child, &hwnd);
        break;
      case gps_window :
        create_gps_window(parent, child, &hwnd);
       break;
      case alert_window :
        
        break;
      case menu_window :
        create_menu_window(parent, child, &hwnd);
        break;

      default:
        break;
      }
  
    // field_datatype has the field type, name is the child name
    type = field_key;
    }
  }
