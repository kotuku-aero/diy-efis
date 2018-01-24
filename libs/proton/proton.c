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
#include "../neutron/bsp.h"
#include "proton.h"

extern result_t layout_wndproc(handle_t hwnd, const canmsg_t *msg);

/**
 * Load a registry hive that describes a series of windows
 * @param canvas    Parent window to create child windows within
 * @param hive      list of keys that describe each window
 * @return s_ok if loaded ok
 */
extern result_t load_layout(handle_t canvas, memid_t hive);

handle_t main_window = 0;

extern const uint8_t neo[];
extern const uint16_t neo_length;

// if parg is given then is a handle to a stream which will be closed
void run_proton(proton_args_t *args)
  {
  result_t result;
  memid_t key;


  if (succeeded(reg_open_key(0, "proton", &key)))
    {
    // create the root window
    uint16_t orientation;

    if (failed(reg_get_uint16(key, "orientation", &orientation)))
      orientation = 0;

    // load and cache the neo font

    char layout_name[REG_STRING_MAX];

    memid_t layout_key;

    if (failed(result = reg_get_string(key, "layout", layout_name, 0)) ||
      failed(result = reg_open_key(key, layout_name, &layout_key)))
      return;

    if (succeeded(result = open_screen(orientation, layout_wndproc, 0, &main_window)))
      {
      if (args->stream != 0)
        {
        // determine if the destination window is suitable for
        // expanding the PNG.  We must have a colorref that
        // is 32 bits.  If not we create a canvas that is
        // guaranteed to be 32 bits
        extent_t ex;
        uint16_t bpp;
        if (succeeded(get_canvas_extents(main_window, &ex, &bpp)) &&
          bpp < 32)
          {
          handle_t canvas = 0;
          if (succeeded(create_rect_canvas(&ex, &canvas)) &&
            succeeded(load_png(canvas, args->stream, 0)))
            {
            rect_t wnd_rect = { 0, 0, ex.dx, ex.dy };
            point_t pt = { 0, 0 };

            // copy the rendered bitmap over
            bit_blt(main_window, &wnd_rect, &wnd_rect, canvas, &wnd_rect, &pt);
            }
          canvas_close(canvas);
          }
        else
          load_png(main_window, args->stream, 0);
        stream_close(args->stream);
        }

      // load the neon font.
      register_font(neo, neo_length);

      char startup_script[REG_STRING_MAX];
      const char *init_script = 0;
      if (succeeded(reg_get_string(key, "init", startup_script, 0)))
        init_script = startup_script;

      // attach the ion interpreter to the screen
      attach_ion(main_window, key, init_script, args->ci, args->co, args->cerr);

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

