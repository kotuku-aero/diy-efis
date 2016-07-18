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
#include "layout_window.h"
#include "airspeed_window.h"
#include "hsi_window.h"
#include "annunciator_window.h"
#include "altitude_window.h"
#include "attitude_window.h"
#include "gauge_window.h"
#include "gps_window.h"
#include "fuel_totalizer_window.h"
#include "alert_window.h"
#include "nav_window.h"
#include "menu_window.h"

kotuku::layout_window_t::layout_window_t(screen_t *screen)
: widget_t(0, screen)
  {
  }

// called to update the screen.
bool kotuku::layout_window_t::repaint(bool force)
  {
  return paint(force);
  }

  enum window_type
    {
    airspeed_window,            // vertical ticker-tape airspeed window
    hsi_window,                 // rectangular gyro position and heading
    attitude_window,            // rectangular attitude window
    altitude_window, // vertical ticker tape altitude and vertical speed indicator window
    gauge_window,               // general purpose round or bar gauge
    annunciator_window,         // time, oat etc.
    gps_window,                 // gps route map window
    map_window,                 // moving map display
    fuel_totalizer_window,      // horizontal fuel totalizer
    alert_window,               // vertical alert window
    comm_tranciever_window,     // comm tcvr
    nav_receiver_window,        // nav receiver
    transponder_window,         // transponder
    nav_window,                 // navigation display
    e_menu_window,                // popup menu window
    last_layout_enum
    };

static const char *layout_names[] = {
    "airspeed_window",            // vertical ticker-tape airspeed window
    "hsi_window",                 // rectangular gyro position and heading
    "attitude_window",            // rectangular attitude window
    "altitude_window", // vertical ticker tape altitude and vertical speed indicator window
    "gauge_window",               // general purpose round or bar gauge
    "annunciator_window",         // time, oat etc.
    "gps_window",                 // gps route map window
    "map_window",                 // moving map display
    "fuel_totalizer_window",      // horizontal fuel totalizer
    "alert_window",               // vertical alert window
    "comm_tranciever_window",     // comm tcvr
    "nav_receiver_window",        // nav receiver
    "transponder_window",         // transponder
    "nav_window",                 // navigation display
    "menu_window",                // menu window
};

void kotuku::layout_window_t::set_layout(canaerospace_provider_t *can_provider)
  {
  // we are created before the application so we have to accept the can_provider
  // after we are constructed
  set_can_provider(can_provider);

  std::vector<std::string> section_names;

  if(failed(the_hal()->get_section_names(0, section_names)))
    {
    trace_error("Cannot enumerate the section names");
    return;
    }

  for(std::vector<std::string>::const_iterator it = section_names.begin();
      it != section_names.end(); it++)
    {

    widget_t *widget = 0;

    std::string section = *it;

    std::string widget_type;
    if(strncmp(section.c_str(), "widget-", 7)== 0)
      {
      if(failed(the_hal()->get_config_value(section.c_str(), "type", widget_type)))
        {
        trace_warning("Widget section %s does not have a type=value, ignoring\n", section.c_str());
        continue;
        }

      int ordinal = lookup_enum(widget_type.c_str(), layout_names, last_layout_enum);
      if(ordinal == -1)
        {
        trace_warning("Section %s has a type of %s which is not recognised\n", section.c_str(), widget_type.c_str());
        continue;
        }

      switch((window_type)ordinal)
        {
        case airspeed_window :
          widget = new airspeed_window_t(*this, section.c_str());
          break;
        case hsi_window :
          widget = new hsi_window_t(*this, section.c_str());
          break;
        case attitude_window :
          widget = new attitude_window_t(*this, section.c_str());
          break;
        case altitude_window :
          widget = new altitude_window_t(*this, section.c_str());
          break;
        case gauge_window :
          widget = new gauge_window_t(*this, section.c_str());
          break;
        case annunciator_window :
          widget = new annunciator_window_t(*this, section.c_str());
          break;
        case gps_window :
          widget = new gps_window_t(*this, section.c_str());
         break;
        case map_window :
          break;
        case alert_window :
          widget = new alert_window_t(*this, section.c_str());
          break;
        case nav_window :
          widget = new nav_window_t(*this, section.c_str());
          break;
        }
      }
    else
      continue;

    if(widget != 0)
      widget->invalidate();
    }
  }

bool kotuku::layout_window_t::ev_msg(const msg_t &data)
  {
  return true;
  }

void kotuku::layout_window_t::update_window()
  {
  }
