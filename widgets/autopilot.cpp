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
#include "autopilot.h"
#include "../gdi-lib/can_aerospace.h"

kotuku::autopilot_t::autopilot_t(canaerospace_provider_t *can_provider)
: thread_t(4096, this, do_run),
  _mode(0),
  _altitude(0),
  _vs(0),
  _current_vs(0),
  _qnh(0),
  _current_alt(0),
  _can_provider(can_provider)
  {
  can_provider->subscribe(id_qnh, this);
  }

kotuku::autopilot_t::~autopilot_t()
  {

  }

uint32_t kotuku::autopilot_t::do_run(void *pthis)
  {
  return reinterpret_cast<autopilot_t *>(pthis)->run();
  }

bool kotuku::autopilot_t::enabled() const
  {
  // check to see if sensible
  return (_mode & APSTATUSENGAGED) != 0;

  }

void kotuku::autopilot_t::enabled(bool is_it)
  {
  _can_provider->publish(msg_t(id_autopilot_engage, (short)is_it));
  }

uint16_t kotuku::autopilot_t::mode() const
  {
  return _mode;
  }

void kotuku::autopilot_t::alt_mode(bool is_it)
  {
  // check to see if valid
  _can_provider->publish(msg_t(id_autopilot_alt_mode, (short)is_it));
  }

void kotuku::autopilot_t::vs_mode(bool is_it)
  {
  _can_provider->publish(msg_t(id_autopilot_vs_mode, (short)is_it));
  }

void kotuku::autopilot_t::nav_mode(bool is_it)
  {
  _can_provider->publish(msg_t(id_autopilot_vs_mode, (short)(is_it ? APMODENAV : APMODEHOLD)));
  }

void kotuku::autopilot_t::hdg_mode(bool is_it)
  {
  _can_provider->publish(msg_t(id_autopilot_vs_mode, (short)(is_it ? APMODENAV : APMODEHDGHOLD)));
  }

short kotuku::autopilot_t::altitude() const
  {
  return _altitude;
  }

void kotuku::autopilot_t::altitude(short alt)
  {
  _altitude = alt;
  _can_provider->publish(msg_t(id_autopilot_altitude, alt));
  }

short kotuku::autopilot_t::vs() const
  {
  return _vs;
  }

void kotuku::autopilot_t::vs(short vs)
  {
  _vs = vs;
  _can_provider->publish(msg_t(id_autopilot_vertical_speed, vs));
  }

short kotuku::autopilot_t::current_vs() const
  {
  return _current_vs;
  }

short kotuku::autopilot_t::current_qnh() const
  {
  return _qnh;
  }

short kotuku::autopilot_t::current_altitude() const
  {
  return _current_alt;
  }

void kotuku::autopilot_t::receive_parameter(canaerospace_provider_t *prov, const msg_t &msg)
  {
  switch(msg.message_id())
    {
    case id_qnh :
      _qnh = msg.value<short>();
      break;
    case id_autopilot_altitude :
      _altitude = msg.value<short>();
      break;
    case id_autopilot_vertical_speed :
      _vs = msg.value<short>();
      break;
    case id_autopilot_status :
      _mode = msg.value<short>();
      break;
    }
  }

bool kotuku::autopilot_t::is_equal(const can_parameter_handler_t &other) const
  {
  return this == &other;
  }

uint32_t kotuku::autopilot_t::run()
  {
  // wait for a doorbell event or wake up every 500msec
  while(!should_terminate())
    {
    event_t::lock_t butler(_doorbell, 500);

    if(should_terminate())
      break;

    // check to see if autopilot enabled
    }

  return 0;
  }
