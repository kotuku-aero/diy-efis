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
#ifndef __airspeed_window_h__
#define __airspeed_window_h__

#include "../gdi-lib/window.h"
#include "widget.h"

namespace kotuku {
class airspeed_window_t : public widget_t
  {
public:
  airspeed_window_t(widget_t &parent, const char *ini_section);

private:
  int _vs0;
  int _vs1;
  int _vfe;
  int _vno;
  int _vne;
  int _va;
  int _vx;
  int _vy;

  virtual void update_window();
  virtual bool ev_msg(const msg_t &);

  // this is updated on each window message
  long _airspeed;
  canvas_t _background_canvas;
  };
  }

#endif
