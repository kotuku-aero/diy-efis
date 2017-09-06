/*
 * slcan.h
 *
 *  Created on: 20/02/2017
 *      Author: peter
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

static handle_t driver;

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
extern result_t slcan_create(memid_t key, handle_t rx_queue, handle_t *device);
/**
 * Call to send an slcan message.
 * @param msg Message to send
 * @return s_ok if sent ok
 */
extern result_t slcan_send(handle_t device, const canmsg_t *msg);
/**
 * Close and release all resources
 * @param device  Device to close
 * @return s_ok if closed.
 */
extern result_t slcan_close(handle_t device);

#ifdef __cplusplus
}
#endif

#endif /* SLCAN_H_ */
