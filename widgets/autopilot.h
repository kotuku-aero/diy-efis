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
#ifndef __autopilot_h__
#define __autopilot_h__

#include "../gdi-lib/thread.h"
#include "../gdi-lib/can_aerospace.h"

namespace kotuku {
/*
 * Class that interfaces and manages the autopilot
 * The autopilot is in the ACU module (assuming that is the implementation)
 * and this class interfaces the menu system with the ACU and displays
 * important information on the display.
 */
class autopilot_t : public thread_t,
public canaerospace_provider_t::can_parameter_handler_t
  {
public:
  autopilot_t(canaerospace_provider_t *can_provider);
  ~autopilot_t();


  bool enabled() const;
  void enabled(bool);

  uint16_t mode() const;

  void alt_mode(bool);
  void vs_mode(bool);
  void nav_mode(bool);
  void hdg_mode(bool);

  short altitude() const;
  void altitude(short);

  short vs() const;
  void vs(short);

  short current_vs() const;
  short current_qnh() const;

  short current_altitude() const;
private:
  static uint32_t do_run(void *);
  uint32_t run();

  virtual void receive_parameter(canaerospace_provider_t *, const msg_t &);
  virtual bool is_equal(const can_parameter_handler_t &) const;

  event_t _doorbell;

  canaerospace_provider_t *_can_provider;
  uint16_t _mode;
  short _altitude;
  short _vs;

  short _current_vs;
  short _qnh;
  short _current_alt;
  };
  };

#endif
