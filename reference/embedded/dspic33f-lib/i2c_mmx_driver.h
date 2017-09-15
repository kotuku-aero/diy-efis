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
#ifndef I2C_MMX_DRIVER_H
#define	I2C_MMX_DRIVER_H


#include <stdint.h>
#include "microkernel.h"

/**
   * This driver allows for multiplexing the I2C controller
   * amount multiple tasks.  The I2C driver knows nothing
   * about the type of device and all work is done with
   * callbacks.
   */

  typedef enum _i2c_state
  {
    i2c_idle,           // i2c command completed
    i2c_write_command,  // sending a command.
    i2c_write_byte,     // write a byte to the i2c
    i2c_read_byte       // read a byte from the i2c
  } i2c_state;
  
  struct _i2c_task_t;

  typedef i2c_state (*get_task_state)(struct _i2c_task_t * task, i2c_state last_state);
  typedef uint8_t (*i2c_byte_to_write)(struct _i2c_task_t * task);
  typedef void (*i2c_byte_read)(struct _i2c_task_t * task, uint8_t); 
  /** return the new state to transition to on an error.  If idle is returned the
   the command is cancelled.  otherwise can return i2c_write_command to restart
   the task, caution needed as this could result in an infinite loop */
  typedef i2c_state (*i2c_error)(struct _i2c_task_t *task, i2c_state last_state);
  
  // default routines

  typedef struct _i2c_task_t
    {
    // callback routines
    get_task_state get_state_handler;
    i2c_byte_to_write write_byte_handler;
    i2c_byte_read read_byte_handler;
    i2c_error error_handler;
    uint8_t command_length;       // total number of command bytes
    uint8_t repeat_start;         // point in command string a repeat-start is done
                                  // only used if >0
    void *p_data;
    uint16_t xfer_length;
    uint16_t bytes_xfered;        // will be xfer_length + 2
    uint16_t *read_length;
    uint8_t command[16];           // up to 8 bytes sent as a command header
    semaphore_t *task_callback;   // will be signaled when complete
    } i2c_task_t;
    
extern uint16_t last_read_length;

/**
 * return a state change based on a simple write/read protocol
 * @param task
 * @return 
 */
extern i2c_state i2c_get_state_handler(i2c_task_t *task, i2c_state last_state);
/**
 * Get a byte to write, will be a command byte or data byte depending on
 * protocol
 * @param task
 * @return 
 */
extern uint8_t i2c_write_byte_handler(i2c_task_t *task);
/**
 * store a byte into memory
 * @param task
 * @param data
 */
extern void i2c_read_byte_handler(i2c_task_t *task, uint8_t data);
/**
 * Default error handler.  returns idle on error.
 * @param task
 * @param last_state
 * @return 
 */
extern i2c_state i2c_error_handler(i2c_task_t *task, i2c_state last_state);
/**
 * Set up a task with defaults that use built-in routines
 * @param task
 */
extern void init_defaults(i2c_task_t *task);

/*  Adds a task to the i2c queue.  The state machine callbacks will
 * be called when the controller is free.
 */
extern bool enqueue_i2c_task(const i2c_task_t *task);

/**
 * Initialize the i2c worker
 * @param buffer                  Buffer for tasks
 * @param len                     Number of tasks
 * @param i2c_worker_stack        Stack for worker
 * @param i2c_worker_stack_length Stack length
 * @return                        process id of the worker (-1 if no workers)
 */
extern int8_t i2c_init(i2c_task_t *buffer,
                     uint16_t len,
                     uint16_t *i2c_worker_stack,
                     uint16_t i2c_worker_stack_length);

/**
 * Return true if the i2c state machine is busy
 * @return 
 */
extern bool i2c_busy();

#endif	/* I2C_MMX_DRIVER_H */

