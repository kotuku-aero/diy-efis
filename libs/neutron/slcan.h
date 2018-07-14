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
#ifndef __slcan_h__
#define __slcan_h__

#include "neutron.h"

#ifdef __cplusplus
extern "C" {
#endif
////////////////////////////////////////////////////////////////////////////
//
//  Generic Neutron based serial can driver
//
//  Assumes a serial driver that can read or write ascii strings that
// comply with the Lawicel SLCAN protocol
//
// to use this driver the following code should exist in the application
//
/*
#include <slcan.h>

#ifndef __cplusplus
extern "C" {
#endif

static comm_device_p driver;

result_t bsp_can_init(handle_t rx_queue, handle_t worker)
  {
  result_t result;
  memid_t key;
  if(failed(result = reg_open_key(0, "slcan", &key, worker)))
    return result;

  return slcan_create(key, rx_queue, worker, &driver);
  }

result_t bsp_send_can(const canmsg_t *msg)
  {
  return slcan_send(driver, msg);
  }

#ifndef __cplusplus
}
#endif
 */

////////////////////////////////////////////////////////////////////////////////
//
// Serial library linkage routines
/**
 * Load the slcan library routines
 * @param key       Key that holds the slcan configs
 * @param rx_queue  Queue for received can messages
 * @param worker    semaphore to wait on
 * @param device    Generated serial can driver
 * @return s_ok if initialize ok
 *
 * The registry key contains the following (defaults shown for raspberry-pi):
 * baud-rate=uint32:38400
 * device=/dev/ttyAMA0
 */
extern result_t slcan_create(memid_t key, handle_t rx_queue, comm_device_p *device);
/**
 * Call to send an slcan message.
 * @param msg Message to send
 * @return s_ok if sent ok
 */
extern result_t slcan_send(comm_device_p device, const canmsg_t *msg);
/**
 * Close and release all resources
 * @param device  Device to close
 * @return s_ok if closed.
 */
extern result_t slcan_close(comm_device_p device);

#ifdef __cplusplus
}
#endif

#endif /* SLCAN_H_ */
