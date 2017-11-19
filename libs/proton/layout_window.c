/*
diy-efis
Copyright (C) 2016 Kotuku Aerospace Limited

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

If a file does not contain a copyright header, either because it is incomplete
or a binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice and
the GPL3 are subservient to that copyright notice.

Portions of this repository contain code fragments from the following
providers.


If any file has a copyright notice or portions of code have been used
and the original copyright notice is not yet transcribed to the repository
then the origional copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
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

result_t load_layout(handle_t screen, memid_t hive)
  {
  // the hive must have series of hives that form windows
  result_t result;
  char name[REG_NAME_MAX +1];
  uint16_t length = REG_NAME_MAX + 1;
  field_datatype type = field_key;
  // must be 0 on first call
  memid_t child = 0;

  // detach the scriptlet interpreter and message hooks
  if (failed(result = detach_ion(screen)))
    return result;

  // attach a scriplet interpreter
  if (failed(result = attach_ion(screen, hive, "init.js")))
    return result;
  
  while(succeeded(result = reg_enum_key(hive, &type, 0, 0, REG_NAME_MAX, name, &child)))
    {
    // open the window key
    memid_t key;
    if(failed(result = reg_open_key(hive, name, &key)))
      return result;

    // we create a widget and pickup the widget defined settings.
    // these are:
    // font -> name of the font to load
    // color -> foreground color
    // background -> back
    
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
        create_airspeed_window(screen, child, &hwnd);
        break;
      case hsi_window :
        create_hsi_window(screen, child, &hwnd);
        break;
      case attitude_window :
        create_attitude_window(screen, child, &hwnd);
        break;
      case altitude_window :
        create_altitude_window(screen, child, &hwnd);
        break;
      case gauge_window :
        create_gauge_window(screen, child, &hwnd);
        break;
      case annunciator_window :
        create_annunciator_window(screen, child, &hwnd);
        break;
      case gps_window :
        create_gps_window(screen, child, &hwnd);
       break;
      case alert_window :
        
        break;
      case menu_window :
        create_menu_window(screen, child, &hwnd);
        break;

      default:
        break;
      }
  
    // field_datatype has the field type, name is the child name
    type = field_key;
    }
  }
