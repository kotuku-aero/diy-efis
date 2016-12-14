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
#ifndef __application_h__
#define __application_h__

#include "canfly.h"
#include "assert.h"
#include "thread.h"
#include "errors.h"
#include "hal.h"
#include "can_aerospace.h"
#include "layout_window.h"

#include <vector>

namespace kotuku {

class application_t : public canaerospace_provider_t {
public:
  application_t(hal_t *hal);

  // the main program must define this
  static application_t *instance;

  // initialize the application.
  result_t initialize(const char *config);

  virtual ~application_t();

  // can driver functions
  result_t publish(const can_msg_t &msg);

  uint8_t node_id() const { return _node_id; }

  result_t set_can_provider(int rate, canaerospace_provider_t *provider);
  result_t get_can_provider(canaerospace_provider_t **provider);
  /**
  * Return the main window of the application
  * @return Window that is used as the drawing surface
  */
  layout_window_t *root_window() { return _root_window; }

  hal_t *hal() { return _hal; }
private:
  uint8_t _node_id;
  hal_t *_hal;
  canaerospace_provider_t *_provider;
  layout_window_t *_root_window;
  };

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
  return min(r.height(), r.width());
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
