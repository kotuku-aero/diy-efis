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
#ifndef __eeprom_h__
#define __eeprom_h__


#include "microkernel.h"
#include "../can-aerospace/parameter_definitions.h"
#include <stdbool.h>

#include <p33Exxxx.h>
#include <stdint.h>
#include <stdint.h>

// this file defines the map assigned to trim values.
// there are 64 channel definitions, and these are assigned in the
// channel definitions passed to the different handler routines
#include "can_driver.h"
#include "i2c_mmx_driver.h"


/* The eeprom is a small block of persistent memory.  It is used to store
 * data needed between invocations of the system.  The format of the store is
 * as follows
 * 
 * uint8_t Number of parameters defined
 * uint8_t reserved
 * uint16_t memory_assigned;
 * struct parameter_defn_t {
 *  uint16_t can_id;
 *  uint8_t parameter_type;
 *  uint8_t reserved;
 *  char name[16];
 *  uint16_t parameter_length;
 *  uint16_t data_offset; 
 * };
 * 
 * The memory for parameters is assigned from the top of the eeprom backwards
 * and when all memory is assigned an error is returned
 */

typedef struct _ee_definition_t {
  mis_message_t name;             // 20 bytes
  uint16_t parameter_length;      // 24 bytes
  uint32_t data_offset;           // 26 bytes
  uint8_t reserved[6];
 } ee_definition_t;

 /* Callback when a memid is updated.
  * return true when the worker task has completed processing.
  */
typedef bool (*parameter_updated_callback)(semaphore_t *worker_task, uint16_t param);
 
typedef struct _eeprom_params_t {
  uint16_t *worker_stack;       // stack for eeprom worker
  uint16_t stack_size;          // length of eeprom worker stack
  i2c_task_t *i2c_buffer;       // i2c buffer
  uint16_t i2c_queue_len;       // length of buffer
  uint16_t *i2c_worker_stack;   // i2c worker stack
  uint16_t i2c_worker_stack_length; // length of i2c worker stack
  int8_t *i2c_worker_id;        // optional address of worker id
  ee_definition_t *definitions; // definitions of known parameters
  uint16_t number_parameters;   // number of parameters stored in the eeprom
  deque_t *ee_update_queue;     // queue of uint16_t that holds what parameters
                                // are updated when a dus message is received
                                // should be number_parameters long.  If notifications
                                // are not needed can be 0
  parameter_updated_callback updated; // callback from worker when a param changed
  bool init_mode;               // true if the code should assume an init state
  } eeprom_params_t;
 
extern int8_t eeprom_init(eeprom_params_t *params);

/**
 * Perform any post-processing init tasks.  Called on the init worker thread
 * @return true when no more init tasks to be done
 */
extern bool eeprom_init_done(semaphore_t *worker);

/**
 * Query how many parameters are defined.
 * @return Number of parameters defined in the eeprom system
 */
extern uint16_t number_defined_parameters();

typedef struct _parameter_definition_t {
  can_parameter_definition_t *parameter;
  mis_message_t nis;
  } parameter_definition_t;

/**
 * Define a parameter to be stored in the system.  Called back from the
 * eeprom init routine.  When done return false

 * @param memid     parameter number being defined
 * @param name      Name header to be filled in
 * @param length    Length of parameter to be stored
 * @param p_data    Pointer to data to initialize definition with
 */
extern void define_parameter(uint16_t memid, mis_message_t *name, 
                                   uint16_t *length, void **p_data);

// routine to write a parameter definition into EEPROM memory.  
// the param number is which 16 byte block to read/write
/**
 * Read a block of data from the eeprom
 * @param memid          ID of the setting that is stored.
 * @param length          length of the buffer
 * @param buffer          buffer to read into
 * @param bytes_read      pointer to the length of the data read
 * @param task_callback   Event to signal when read completed.
 * @return true if the parameter exists and can be read
 */
extern bool read_parameter_data(uint16_t memid, uint8_t length, void *buffer, uint16_t *bytes_read, semaphore_t *task_callback);

/**
 * Write a parameter to the eeprom
 * @param param           ID of the setting that is stored
 * @param length          length of setting to store
 * @param buffer          buffer to store
 * @param task_callback   Event to signal when the write is complete
 * @return true if the parameter data can be stored
 */
extern bool write_parameter_data(uint16_t memid, uint8_t length, const void *buffer, semaphore_t *task_callback);

/**
 * Read a parameter definition from the EEPROM memory
 * @param memid          Parameter to read
 * @param defn            Memory to read setting into
 * @param task_callback   Event to signal when read completed.
 * @return true if the system has a parameter definition matching the requested id
 */
extern bool read_parameter_definition(uint16_t memid, can_parameter_definition_t *defn, semaphore_t *task_callback);

/**
 * write a parameter definition to the EEPROM memory
 * @param memid          Parameter offset
 * @param defn
 * @param task_callback   Event to signal when read completed.
 * @return true if the system can write the parameter definition.
 */
extern bool write_parameter_definition(uint16_t memid, const can_parameter_definition_t *defn, semaphore_t *task_callback);

/**
 * Read a word setting from the eeprom
 * @param memid        ID of the setting
 * @param task_callback Event to signal when done
 * @param result        Where to put the word read
 * @return true if the setting can be read
 */
extern bool read_setting_short(uint16_t memid, short *result, semaphore_t *task_callback);

/**
 * Write a word setting
 * @param memid          ID of the setting
 * @param value           value to write
 * @param task_callback   Event to signal when done
 * @return true if the setting can be written
 */
extern bool write_setting_short(uint16_t memid, const short *value, semaphore_t *task_callback);

/**
 * Read a float from the eeprom
 * @param memid          ID of the setting
 * @param result          value to read
 * @param task_callback   Event to signal when done
 * @return true if the setting can be read
 */
extern bool read_setting_float(uint16_t memid, float *result, semaphore_t *task_callback);

/**
 * Write a float to the eeprom
 * @param memid          ID of the setting
 * @param value           value to write
 * @param task_callback   Event to signal when done
 * @return true if the setting can be written
 */
extern bool write_setting_float(uint16_t memid, const float *value, semaphore_t *task_callback);

// Write data into parameter area of the eeprom
extern void process_dds(const can_msg_t *);
// read data from the parameter area of the eeprom
extern void process_dus(const can_msg_t *);
// return the names of the parameters
extern void process_mis(const can_msg_t *);
// update the can identifiers
extern void process_css(const can_msg_t *);
// update the node identifier
extern void process_nis(const can_msg_t *);

#endif
