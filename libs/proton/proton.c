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
#include "fonts.h"

#include "../neutron/bsp.h"
#include "../photon/ion_proxy.h"

extern result_t layout_wndproc(handle_t hwnd, const canmsg_t *msg);

/**
 * Load a registry hive that describes a series of windows
 * @param canvas    Parent window to create child windows within
 * @param hive      list of keys that describe each window
 * @return s_ok if loaded ok
 */
extern result_t load_layout(handle_t canvas, memid_t hive);

handle_t main_window = 0;

void run_proton(void *parg)
  {
  result_t result;
  memid_t key;

  if(succeeded(reg_open_key(0, "proton", &key)))
    {
    // create the root window
    uint16_t orientation;

    if(failed(reg_get_uint16(key, "orientation", &orientation)))
      orientation = 0;

    char layout_name[REG_STRING_MAX];

    memid_t layout_key;

    if (failed(result = reg_get_string(key, "layout", layout_name, 0)) ||
      failed(result = reg_open_key(0, layout_name, &layout_key)))
      return;

    if(succeeded(result = open_screen(orientation, layout_wndproc, 0, &main_window)))
      {
      char startup_script[REG_STRING_MAX];
      const char *init_script = 0;
      if (succeeded(reg_get_string(key, "init", startup_script, 0)))
        init_script = startup_script;

      // attach the ion interpreter to the screen
      if (succeeded(attach_ion(main_window, key, init_script)) &&
         init_script != 0)
        {
        // the interpreter has loaded ok.  if the init_script is > 0 then
        // we can attach any events to run on window events.
        memid_t events;
        if (succeeded(reg_open_key(key, "events", &events)))
          {
          // enumerate the keys
          field_datatype dt = field_key;
          char name[REG_NAME_MAX];
          char event_fn[REG_STRING_MAX];

          memid_t child = 0;

          while (succeeded(reg_enum_key(key, &dt, 0, 0, REG_NAME_MAX, name, &child)))
            {
            uint16_t can_id = (uint16_t) strtoul(name, 0, 10);

            if(can_id > 0)
              {

              // now enumerate the strings in it
              memid_t handler = 0;
              dt = field_string;

              while (succeeded(reg_enum_key(child, &dt, 0, 0, REG_NAME_MAX, name, &handler)))
                {
                if (succeeded(reg_get_string(child, name, event_fn, 0)))
                  {
                  add_handler(main_window, can_id, event_fn);
                  }

                dt = field_string;
                }

              dt = field_key;
              }
            }
          }
        }

      // finally load the layout
      load_layout(main_window, layout_key);
      }
    }
  
  // run the message queue
  // despatch messages
  canmsg_t msg;
  handle_t hwnd;
  // returns s_false to stop the queue
  while (true)
    if(succeeded(get_message(main_window, &hwnd, &msg)))
      dispatch_message(hwnd, &msg);
  }

