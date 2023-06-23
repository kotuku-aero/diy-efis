#ifndef __i2c_h__
#define __i2c_h__

#include "../neutron/neutron.h"

/**
 * @function result_t i2c_init(memid_t key, deque_p rx_queue, handle_t *handle)
 * initialize an i2c interface to a can bus
 * @param key       registry key with the settings
 * @param rx_queue  receive queue to push canmsg_t msgs to
 * @param handle    opaque handle to the queue
 * @return s_ok if the i2c interface is opened ok
*/
extern result_t i2c_init(memid_t key, deque_p rx_queue, handle_t *handle);
/**
 * @function result_t i2c_close(handle_t driver)
 * Close a driver and release all resources
 * @param driver    handle to the driver
 * @return s_ok if closed ok
*/
extern result_t i2c_close(handle_t driver);
/**
 * @function result_t i2c_send_can(handle_t driver, const canmsg_t *msg)
 * Send a canfly message to the bus
 * @param driver      Driver information
 * @param msg         Message to send
 * @return s_ok if message sent ok
*/
extern result_t i2c_send_can(handle_t driver, const canmsg_t *msg);

#endif
