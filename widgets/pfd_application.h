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
#ifndef __pfd_application_h__
#define __pfd_application_h__

#include "../gdi-lib/stddef.h"

#include <map>
#include <vector>
#include <math.h>

#include "../gdi-lib/pen.h"
#include "../gdi-lib/font.h"
#include "../gdi-lib/canvas.h"
#include "../gdi-lib/application.h"
#include "../gdi-lib/spatial.h"
#include "../gdi-lib/can_aerospace.h"
#include "../gdi-lib/std_identification.h"

#include "fonts.h"
#include "pens.h"
#include "colors.h"

class gps_device_t;
class layout_window_t;
class can_message_handler_t;
class msg_t;

namespace kotuku {

class pfd_application_t : public application_t, public canaerospace_provider_t
  {
public:
  pfd_application_t(layout_window_t *root_window);
  ~pfd_application_t();

  virtual uint8_t node_id() const;
  layout_window_t *root_window();
private:
  uint8_t _node_id;
  layout_window_t *_root_window;

  };

inline layout_window_t *pfd_application_t::root_window()
  {
  return _root_window;
  }

inline double round(double d)
  {
  int64_t n = static_cast<int64_t> (d);
  return (d - double(n)) > 0.5 ? double(n + 1) : double(n);
  }

// display a roller gauge.  will format as needed
// digits can only be 1, 2
extern void display_roller(canvas_t &dest, const rect_t &bounds, double value,
                           int digits, color_t bg_color = color_black,
                           color_t fg_color = color_white);

#define countof(x)( sizeof(x) / sizeof(x[0]))

inline long smallest_dimension(const rect_t &r)
  {
  return std::min(r.height(), r.width());
  }

inline point_t center_of(const rect_t &r)
  {
  gdi_dim_t x = r.width() >> 1;
  gdi_dim_t y = r.height() >> 1;
  return point_t(x, y);
  }
  };

// aliases for the serial adapter
#define id_key0 (id_user_defined_start)
#define id_key1 (id_user_defined_start + 1)
#define id_key2 (id_user_defined_start + 2)
#define id_key3 (id_user_defined_start + 3)
#define id_key4 (id_user_defined_start + 4)
#define id_decka (id_user_defined_start + 5)
#define id_deckb (id_user_defined_start + 6)
#define id_menu_up (id_user_defined_start + 7)
#define id_menu_dn (id_user_defined_start + 8)
#define id_menu_left (id_user_defined_start + 9)
#define id_menu_right (id_user_defined_start + 10)
#define id_menu_ok  (id_user_defined_start + 11)
#define id_menu_cancel (id_user_defined_start + 12)
#define id_menu_edit (id_user_defined_start + 13)
#define id_menu_back (id_user_defined_start + 14)
#define id_menu_select (id_user_defined_start + 15)
#define id_timer (id_user_defined_start + 16)
#define id_buttonpress (id_user_defined_start + 17)

#endif
