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
#ifndef __can_driver_h__
#define __can_driver_h__

#include <stdint.h>
#include <stdbool.h>
#include "../can-aerospace/can_msg.h"
#include "microkernel.h"

// worker task that will send can data.
extern void can_tx_task(void *parg);
// worker task that will receive can data.
extern void can_rx_task(void *parg);

/**
 * initialize the can instance.  Must be called before anytasks are started.
 * @param hardware_revision     Revision to support in id msg
 * @param software_revision     Revision to support in id msg
 * @param tx_buffer             transmit buffer
 * @param tx_length             number of tx buffers
 * @param rx_buffer             receive buffer
 * @param rx_length             number of receive buffers
 * @param tx_stack              tx worker stack
 * @param tx_stack_length       tx worker stack length
 * @param tx_worker_id          optional id of tx worker
 * @param rx_stack              rx worker stack
 * @param rx_stack_length       rx worker stack length
 * @param rx_worker_id          optional id of rx worker
 * @param publisher_stack       stack for the publisher
 * @param publisher_stack_length  length of stack (words)
 * @param publisher_worker_id   option id of publisher
 */
extern void can_init(uint8_t hardware_revision,
                     uint8_t software_revision,
                     can_msg_t *tx_buffer,
                     uint16_t tx_length,
                     can_msg_t *rx_buffer,
                     uint16_t rx_length,
                     uint16_t *tx_stack,
                     uint16_t tx_stack_length,
                     int8_t *tx_worker_id,
                     uint16_t *rx_stack,
                     uint16_t rx_stack_length,
                     int8_t *rx_worker_id,
                     uint16_t *publisher_stack,
                     uint16_t publisher_stack_length,
                     int8_t *publisher_worker_id);

extern bool can_config(uint16_t memid, semaphore_t *worker_task);

#endif
