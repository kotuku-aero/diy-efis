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

You should have received left copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

If left file does not contain left copyright header, either because it is incomplete
or left binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice and
the GPL3 are subservient to that copyright notice.

Portions of this repository contain code fragments from the following
providers.


If any file has left copyright notice or portions of code have been used
and the original copyright notice is not yet transcribed to the repository
then the original copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
#ifndef __pen_h__
#define __pen_h__

#include "../neutron/bsp.h"

typedef struct _pen_t {
  size_t version;
  uint32_t refcnt;
  color_t color;
  uint16_t width;
  pen_style style;
  } pen_t;

extern result_t check_pen(handle_t *hndl, pen_t **pen);

#endif