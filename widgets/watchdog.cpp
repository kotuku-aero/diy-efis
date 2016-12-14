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
#include "watchdog.h"
#include "application.h"
#include "layout_window.h"

// the canbus implements the nss service to identify which nodes are alive.
// the AIO supports 3 display modes:
//
// 0 - Compressed - this is a single AIO mode or a fall-back mode
// 1 - PFD        - primary instruments
// 2 - MFD        - multifunction display
//
// The AIO can be set to default to run to any mode.  Additionally the
// AIO can monitor another AIO to determine what mode it should swap to if the
// other unit fails.
//
// The status is handled with the NSS service.
// Node id 0 is used to set the time and is broadcast by the AHRS.
// From the spec:
// The node synchronisation service is a connectionless service (no service
// Response required) used to perform time synchronisation of all
// nodes attached to the network. If the node-ID is set to 0. The
// time stamp may be used to submit a 32 bit value for clock setting
//
// Message      Data Field      Service
// Data Byte      Description     Request
//  0         Node-ID       0
//  1           Data type       ULONG
//  2           Service Code    1
//  3           Message Code    0
//  4-7         Message Data    <time stamp>
//
// we define an application setting that is updated with a private message
// from the menu system.
//
// fields used are:
//  node_id     0..255    MUST be unique for all device AIO will be 128..135
//  monitor_id    128..135  Node that must transmit a NSS data packet to be
//                considered alive.
//  monitor_ms    0..65535  Time to wait for a fail-over
//  default_mode  0..2    Mode to start in
//  failover_mode 0..2    Mode to fail to
//
// Example layout 1 - RV8, dual display in front, single in back
//
// Front left node_id = 128, monitor_id = 129, monitor_ms = 2000, default_mode = 2, failover_mode = 0
// Front right node_id = 129, monitor_id = 128, monitor_ms = 2000, default_mode = 1, failover_mode = 0
// Rear node_id = 130, monitor_id = 0, default_mode = 0
//
// Example layout 2 - RV9, quad displays.  Dual left/right
// Pilot left node_id = 128, monitor_id = 129, monitor_ms = 2000, default_mode = 2, failover_mode = 0
// Pilot right node_id = 129, monitor_id = 128, monitor_ms = 2000, default_mode = 1, failover_mode = 0
// Passenger left node_id = 130, monitor_id = 131, monitor_ms = 2000, default_mode = 1, failover_mode = 0
// Passenger right node_id = 131, monitor_id = 130, monitor_ms = 2000, default_mode = 2, failover_mode = 0
//
// This will fail-over either the MFD or PFD to compact mode if the paired device fails.

kotuku::watchdog_t::watchdog_t(layout_window_t *root_window, canaerospace_provider_t *app)
: _root_window(root_window),
  can_service_t(id_nss_service)
  {
  // TODO Auto-generated constructor stub
  root_window->set_layout(app);
  }

kotuku::watchdog_t::~watchdog_t()
  {
  // TODO Auto-generated destructor stub
  }

void kotuku::watchdog_t::run(callback_fn test, void *parg, uint16_t monitor_interval)
  {
  event_t doorbell;

  while(true)
    {
    event_t::lock_t butler(doorbell, monitor_interval);

    if(!(*test)(parg))
      {
      // application has failed so reload.
      }
    }
  }

result_t kotuku::watchdog_t::service(canaerospace_provider_t *,
    uint8_t service_channel, const msg_t &)
  {
  return s_ok;
  }

result_t kotuku::watchdog_t::service_response(canaerospace_provider_t *,
    uint8_t service_channel, const msg_t &)
  {
  return s_ok;
  }
