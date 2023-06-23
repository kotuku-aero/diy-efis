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
#ifndef __widget_h__
#define __widget_h__

#include <stdio.h>

#include "photon.h"
#include "window.h"

extern result_t create_child_widget(handle_t parent, memid_t key, wndproc cb, handle_t *hwnd);
  // look up a settings and return the enumeration value
  // returns -1 if setting not found otherwise integer index of lookup
extern result_t lookup_enum(memid_t key, const char *name, const char **values, int max_values, int *value);
extern result_t lookup_font(memid_t key, const char *name, handle_t  *font) ;
extern result_t lookup_color(memid_t key, const char *name, color_t *color);
extern result_t lookup_pen_style(memid_t key, const char *name, pen_style *style);
extern result_t lookup_pen(memid_t key, pen_t *pen);
/**
 * Display a roller
 * @param hwnd      Window to paint on
 * @param bounds    Area for roller
 * @param value     Value, lower nn digits are displayed smaller
 * @param digits    Number of digits to draw in small font, 0 for none
 * @param bg_color
 * @param fg_color
 * @return 
 */
extern result_t display_roller(handle_t hwnd, const rect_t *bounds, uint32_t value, int digits, color_t bg_color, color_t fg_color, handle_t  large_font, handle_t  small_font);

#endif
