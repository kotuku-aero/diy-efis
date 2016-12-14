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
#include "eeprom.h"

#define __pack_upper_byte

#include <p33Exxxx.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "../can-aerospace/can_aerospace.h"

#include "i2c_mmx_driver.h"
 
#define EEPROM_SIZE (128L * 1024L)    // eeprom is 128k bytes
#define PARAMS_BASE (96L * 1024L)     // top 32k bytes reserved for params
static semaphore_t eeprom_worker_semp;
 
static i2c_state eeprom_get_state(i2c_task_t *task, i2c_state last_state)
  {
  if(task->bytes_xfered < task->command_length)
    return ((task->bytes_xfered == 3 || task->bytes_xfered == 0) &&
            last_state != i2c_write_command) ? i2c_write_command : i2c_write_byte;
  
  if(task->bytes_xfered >= (task->xfer_length + task->command_length))
    {
    if(task->command_length == 4 && task->read_length != 0)
      *task->read_length = task->xfer_length - task->command_length;
    
    if(task->task_callback != 0)
      signal_from_isr(task->task_callback);
    
   
    return i2c_idle;
    }
  
  return task->command_length == 3 ? i2c_write_byte : i2c_read_byte;
  }

static i2c_state eeprom_error(i2c_task_t *task, i2c_state last_state)
  {
  // if the write was busy then we poll for the start of the command
  if(task->bytes_xfered == 1)
    {
    task->bytes_xfered = 0;       // restart cmd
    return i2c_write_command;
    }
  
  // ignore the command.  Should be handled better...
  return i2c_idle;
  }

static int16_t next_param_to_define = 0;

typedef enum _ee_init_state
  {
  read_num_definitions,
  ee_write_empty_eeprom,
  ee_read_definition,
  ee_init_tasks,
  ee_idle,
  }ee_init_state;

static ee_init_state state;

static void ee_worker(void *);
static i2c_task_t task;
static const uint32_t params_base = PARAMS_BASE;
static const uint32_t definition_base = PARAMS_BASE + 128;
static eeprom_params_t *ee_params;

int8_t eeprom_init(eeprom_params_t *params)
  {
  int8_t id;
  int8_t i2c_id;
  
  ee_params = params;
  memset(ee_params->definitions, 0, sizeof(ee_definition_t) * ee_params->number_parameters);
  
  resume(id = create_task("EEPROM", params->worker_stack, params->stack_size, ee_worker, 0, NORMAL_PRIORITY));
  if(id == -1)
    return id;
  
  // we read the settings from the external eeprom.
  // this is done using the i2c_mmx driver 
  i2c_id = i2c_init(params->i2c_buffer, params->i2c_queue_len, params->i2c_worker_stack, params->i2c_worker_stack_length);         // set up driver
  
  if(params->i2c_worker_id != 0)
    *params->i2c_worker_id = i2c_id;
  
  if(i2c_id == -1)
    return -1;
  
  // now we read the ee_params->definitions for the parameters (if any)
  task.bytes_xfered = 0;
  task.command[0] = 0xA8;                 // command byte, send address
  task.command[1] = (uint8_t)(params_base >> 8);
  task.command[2] = (uint8_t)(params_base);
  task.command[3] = 0xA9;             // read data
  task.command_length = 4;
  task.p_data = &next_param_to_define;
  task.xfer_length = 2;
  task.read_length = &last_read_length;
  task.get_state_handler = eeprom_get_state;
  task.write_byte_handler = i2c_write_byte_handler;
  task.read_byte_handler = i2c_read_byte_handler;
  task.error_handler = eeprom_error;
  task.task_callback = &eeprom_worker_semp;
  
  enqueue_i2c_task(&task);         // queue the task
  
  return id;
  }

// Parameters are stored on a page.
// the limiting factor is the page size of 128 bytes

// the ee_params->definitions are 

static void enqueue_parameter(void *p_data)
  {
  // we write to the lower 64k of eeprom.  These routines don't use
  // upper 
  uint32_t block_offset = 128 * (next_param_to_define + 1);
  uint32_t memory_allocated = EEPROM_SIZE - block_offset;
  
  // our page number
  ee_params->definitions[next_param_to_define].data_offset = memory_allocated;
  
  // write the parameter block first.  This is at low memory
  memory_allocated = definition_base + (sizeof(ee_definition_t) * next_param_to_define);
  
  ee_definition_t *defn = &(ee_params->definitions[next_param_to_define]);
  
  task.bytes_xfered = 0;
  task.command[0] = 0xA8;     // command byte, send address. write high bank
  task.command[1] = (uint8_t)(memory_allocated >> 8);
  task.command[2] = (uint8_t)(memory_allocated);
  task.command_length = 3;
  task.p_data = defn;
  task.xfer_length = sizeof(ee_definition_t);
  task.read_length = 0;
  task.get_state_handler = eeprom_get_state;
  task.write_byte_handler = i2c_write_byte_handler;
  task.read_byte_handler = i2c_read_byte_handler;
  task.error_handler = eeprom_error;
  task.task_callback = 0;     // no callback
  enqueue_i2c_task(&task);         // queue the task
  
  // now the definition (no callback)
  task.command[1] = (uint8_t)(defn->data_offset >> 8);
  task.command[2] = (uint8_t)defn->data_offset;
  task.xfer_length = defn->parameter_length;
  task.p_data = p_data;
  task.task_callback = 0;
  enqueue_i2c_task(&task);         // queue the task

  next_param_to_define++;

  // now the ee_params->number_parameters with callback
  task.command[1] = (uint8_t)(params_base >> 8);
  task.command[2] = (uint8_t)(params_base);
  task.xfer_length = sizeof(uint16_t);
  task.p_data = &next_param_to_define;
  task.task_callback = &eeprom_worker_semp;
  enqueue_i2c_task(&task);         // queue the task
  }

static void ee_worker(void *parg)
  {
  void *p_data;
  while(true)
    {
    wait(&eeprom_worker_semp, INDEFINITE_WAIT);
    
    if(state == ee_idle &&
       ee_params->updated != 0 &&
       ee_params->ee_update_queue != 0)
      break;        

    switch(state)
      {
      case read_num_definitions :
        if(!ee_params->init_mode &&           // don't do this if teh code is in init mode
           next_param_to_define > 0 &&        // and there are at least 1 params defined
           ee_params->number_parameters == next_param_to_define)  // and the counts match
          { 
          state = ee_init_tasks;

          // read the first one
          task.bytes_xfered = 0;
          task.command[0] = 0xA8;                 // command byte, send address
          task.command[1] = (uint8_t)(definition_base >> 8);
          task.command[2] = (uint8_t) definition_base;
          task.command[3] = 0xA9;             // read data
          task.command_length = 4;
          task.p_data = ee_params->definitions;
          task.xfer_length = sizeof(ee_definition_t) * ee_params->number_parameters;
          task.read_length = &last_read_length;
          task.get_state_handler = eeprom_get_state;
          task.write_byte_handler = i2c_write_byte_handler;
          task.read_byte_handler = i2c_read_byte_handler;
          task.error_handler = eeprom_error;
          task.task_callback = &eeprom_worker_semp;

          enqueue_i2c_task(&task);         // queue the task
          break;
          }
        else
          {
          // new eeprom. or ee_params->definitions have changed
          state = ee_write_empty_eeprom;
          next_param_to_define = 0;

          task.bytes_xfered = 0;
          task.command[0] = 0xA8;                 // command byte, send address
          task.command[1] = (uint8_t)(params_base >> 8);
          task.command[2] = (uint8_t) params_base;
          task.command_length = 3;
          task.p_data = &next_param_to_define;
          task.xfer_length = sizeof(uint16_t);
          task.read_length = 0;
          task.get_state_handler = eeprom_get_state;
          task.write_byte_handler = i2c_write_byte_handler;
          task.read_byte_handler = i2c_read_byte_handler;
          task.error_handler = eeprom_error;
          task.task_callback = &eeprom_worker_semp;

          enqueue_i2c_task(&task);         // queue the task
          state = ee_write_empty_eeprom;
          }
        break;
      case ee_write_empty_eeprom :
        if(next_param_to_define >= ee_params->number_parameters)
          state = eeprom_init_done(&eeprom_worker_semp) ? ee_idle : ee_init_tasks;
        else
          {
          define_parameter(next_param_to_define,
                            &ee_params->definitions[next_param_to_define].name,
                            &ee_params->definitions[next_param_to_define].parameter_length,
                            &p_data);
          enqueue_parameter(p_data);
          }
        break;
      case ee_init_tasks :
        state = eeprom_init_done(&eeprom_worker_semp) ? ee_idle : ee_init_tasks;
        break;
      case ee_idle :
        break;
      }
    }
  
  // if we get here then we have a worker process with a callback that
  // will get the 
  while(true)
    {
    uint16_t memid_updated;
    
    pop_front(ee_params->ee_update_queue, &memid_updated, INDEFINITE_WAIT);
    
    // call a worker process to handle the updated parameter
    // when it returns false we are done.
    while(!ee_params->updated(&eeprom_worker_semp, memid_updated))
      wait(&eeprom_worker_semp, INDEFINITE_WAIT);
    }
  }

/**
 * Look up a parameter that is assigned to the can_id
 * @param can_id    Identifier for a type 0 parameter
 * @return 
 */
static ee_definition_t *get_definition(uint16_t memid)
  {
  int i;
  for(i = 0; i < ee_params->number_parameters; i++)
    {
    ee_definition_t *defn = ee_params->definitions + i;
    if(defn->name.memid == memid)
      return defn;
    }
  
  return 0;
  };

bool read_parameter_data(uint16_t memid, uint8_t length, void *buffer, uint16_t *bytes_read, semaphore_t *task_callback)
  {
  ee_definition_t *defn = get_definition(memid);
  
  if(defn == 0)
    {
    if(task_callback != 0)
      signal(task_callback);
    
    return false;
    }
  
  task.bytes_xfered = 0;
  task.command[0] = 0xA8;                 // command byte, send address
  task.command[1] = (uint8_t) (defn->data_offset >> 8);
  task.command[2] = (uint8_t) defn->data_offset;
  task.command[3] = 0xA9;             // read data
  task.command_length = 4;
  task.p_data = buffer;
  task.xfer_length = defn->parameter_length;
  task.read_length = bytes_read;
  task.get_state_handler = eeprom_get_state;
  task.write_byte_handler = i2c_write_byte_handler;
  task.read_byte_handler = i2c_read_byte_handler;
  task.error_handler = eeprom_error;
  task.task_callback = task_callback;
  
  enqueue_i2c_task(&task);         // queue the task

  return true;
  }

bool write_parameter_data(uint16_t can_id, uint8_t length, const void *buffer, semaphore_t *task_callback)
  {
  ee_definition_t *defn = get_definition(can_id);
  
  if(defn == 0)
    return false;
  
  task.bytes_xfered = 0;
  task.command[0] = 0xA8;                 // command byte, send address
  task.command[1] = (uint8_t)(defn->data_offset >> 8);
  task.command[2] = (uint8_t) defn->data_offset;
  task.command_length = 3;
  task.p_data = (void *) buffer;
  task.xfer_length = (length < defn->parameter_length) ? length : defn->parameter_length;
  task.read_length = 0;
  task.get_state_handler = eeprom_get_state;
  task.write_byte_handler = i2c_write_byte_handler;
  task.read_byte_handler = i2c_read_byte_handler;
  task.error_handler = eeprom_error;
  task.task_callback = task_callback;
  
  enqueue_i2c_task(&task);         // queue the task

  return true;
  }

/******************************************************************************/
// Data Download service.  Used to write parameters
//
// when the dds is downloading a block, this holds the number of expected messages
static uint32_t dds_chksum;
// this holds the parameter we are updating.
static uint16_t dds_next_address = 0;
static can_msg_t dds_reply;
static uint8_t dds_command[7];
static uint8_t dds_length;
static uint8_t dds_ptr;

static i2c_state dds_get_task_state(i2c_task_t * task, i2c_state last_state)
  {
  if(dds_ptr >= dds_length)
    {
    // send the reply
    publish_reply(create_can_msg_chksum(&dds_reply, dds_reply.id, dds_reply.msg.canas.service_code, dds_chksum));
    
    // if we have a notification queue then we push the parameter we have
    // updated onto the back of the queue so the worker can process it.
    if(ee_params->ee_update_queue != 0)
      push_back_from_isr(ee_params->ee_update_queue, &dds_next_address);
    
    return i2c_idle;
    }
  
  return i2c_write_byte;
  }

static uint8_t dds_byte_to_write(i2c_task_t * task)
  {
  return dds_command[dds_ptr++];
  }

static i2c_task_t dds_task = { dds_get_task_state, dds_byte_to_write, 0 };

void process_dds(const can_msg_t *msg)
  {
  uint32_t data = get_param_long(msg);
  ee_definition_t *dds_definition;

  dds_reply.msg.canas.message_code = msg->msg.canas.message_code;
  dds_reply.id = msg->id + 1;
  dds_reply.msg.canas.service_code = msg->msg.canas.service_code;

  if(msg->msg.canas.data_type == CANAS_DATATYPE_MEMID)
    {
    if(msg->msg.canas.message_code > 4 ||
       data > ee_params->number_parameters)
      {
      // send error
      publish_reply(create_can_msg_long(&dds_reply, msg->id + 1, msg->msg.canas.service_code, -1));
      return;
      }

    // and the data we are writing
    dds_definition = get_definition(data);
    // we are starting a new download request
    dds_next_address = dds_definition->data_offset;   // offset to write to
    
    dds_chksum = 0;

    // send an xon
    publish_reply(create_can_msg_long(&dds_reply, msg->id + 1, msg->msg.canas.service_code, 1));
    return;
    }

  // else we expect an uchar4
  if(msg->msg.canas.data_type != CANAS_DATATYPE_UCHAR4 ||
     msg->msg.canas.data_type != CANAS_DATATYPE_UCHAR3 ||
     msg->msg.canas.data_type != CANAS_DATATYPE_UCHAR2 ||
     msg->msg.canas.data_type != CANAS_DATATYPE_UCHAR)
    {
    // send error
    publish_reply(create_can_msg_long(&dds_reply, msg->id + 1, msg->msg.canas.service_code, -1));
    return;
    }
  
  
  // the memid needs to be byte aligned
  const uint8_t *data_ptr = msg->msg.canas.data;
  
  dds_length = 0;
  dds_command[dds_length++] = 0xA4;
  dds_command[dds_length++] = ((uint8_t)dds_next_address >> 8);
  dds_command[dds_length++] = (uint8_t)dds_next_address;
  
  dds_command[dds_length++] = *data_ptr++;

  if(msg->msg.canas.data_type != CANAS_DATATYPE_UCHAR4 ||
     msg->msg.canas.data_type != CANAS_DATATYPE_UCHAR3 ||
     msg->msg.canas.data_type != CANAS_DATATYPE_UCHAR2)
    dds_command[dds_length++] = *data_ptr++;

  if(msg->msg.canas.data_type != CANAS_DATATYPE_UCHAR4 ||
     msg->msg.canas.data_type != CANAS_DATATYPE_UCHAR3)
    dds_command[dds_length++] = *data_ptr++;

  if(msg->msg.canas.data_type != CANAS_DATATYPE_UCHAR4)
    dds_command[dds_length++] = *data_ptr++;

  dds_next_address += dds_length - 3;
  // check to see that we have enough space
  if(dds_next_address > (dds_definition->data_offset + dds_definition->parameter_length))
    {
    // send error
    publish_reply(create_can_msg_long(&dds_reply, msg->id + 1, msg->msg.canas.service_code, -1));
    return;
    }

  enqueue_i2c_task(&dds_task);
  }


// when the dus is downloading a block, this holds the number of expected messages
static uint32_t dus_chksum;
// this holds the parameter we are updating.
static uint16_t dus_next_address = 0;
static uint16_t dus_num_replies = 0;
static ee_definition_t *dus_definition = 0;
static can_msg_t dus_reply;
static uint8_t dus_command[8];
static uint8_t dus_ptr;

static i2c_state dus_get_task_state(i2c_task_t * task, i2c_state last_state)
  {
  if(dus_ptr >= 8)
    {
    dus_reply.msg.canas.message_code++;
    dus_reply.msg.canas.data[0] = dus_command[4];
    dus_reply.msg.canas.data[1] = dus_command[5];
    dus_reply.msg.canas.data[2] = dus_command[6];
    dus_reply.msg.canas.data[3] = dus_command[7];
    publish_reply(&dus_reply);
    
    dus_ptr = 4;
    if(dus_next_address >= (dus_definition->data_offset + dus_definition->parameter_length) ||
       dus_num_replies == 0)
      return i2c_idle;            // transfer complete      
    }
  
  return dus_ptr < 4 ? i2c_write_byte : i2c_read_byte;
    
  }

static uint8_t dus_byte_to_write(i2c_task_t * task)
  {
  return dus_command[dus_ptr++];
  }

static void dus_byte_to_read(i2c_task_t * task, uint8_t value)
  {
  dus_command[dus_ptr++] = value;
  }

static i2c_task_t dus_task = { dus_get_task_state, dus_byte_to_write, dus_byte_to_read };


void process_dus(const can_msg_t *msg)
  {
  long memid = get_param_long(msg);
  dus_num_replies = msg->msg.canas.message_code;

  // if we get a CHKSUM message then it is part of a reply
  // message to a DUS which we ignore.
  if(msg->msg.canas.data_type == CANAS_DATATYPE_CHKSUM)
    return;


  dus_reply.msg.canas.message_code = msg->msg.canas.message_code;
  if(memid < 0 ||
     memid > 65535 ||
     memid > (ee_params->number_parameters-1))       // don't read past end of buffer
    {
    // send error
    publish_reply(create_can_msg_long(&dus_reply, msg->id + 1, msg->msg.canas.service_code, -1));
    return;
    }

  dus_reply.msg.canas.data_type = CANAS_DATATYPE_UCHAR4;
  dus_reply.flags = 8;
  dus_reply.id = msg->id + 1;
  dus_reply.msg.canas.service_code = msg->msg.canas.service_code;
  dus_reply.msg.canas.message_code = 0;

  dus_definition = get_definition(memid);
  dus_next_address = dus_definition->data_offset;

  dus_ptr = 0;      // start the command.
 
  // schedule the read task
  dus_command[0] = 0xA4;                 // command byte, send address
  dus_command[1] =((uint8_t)dus_next_address >> 8);
  dus_command[2] = (uint8_t)dus_next_address;
  dus_command[3] = 0xA5;             // read data

  enqueue_i2c_task(&dus_task);
  }

/*
 * The MIS will return the names of the defines parameters in the system
 *
 * memid 0 returns the number of defined parameters
 */
void process_mis(const can_msg_t *msg)
  {
  can_msg_t reply;
  int num_replies = msg->msg.canas.message_code;
  long memid = get_param_long(msg);

  // if we get a CHKSUM message then it is part of a reply
  // message to a DUS which we ignore.
  if(msg->msg.canas.data_type == CANAS_DATATYPE_CHKSUM)
    return;

  reply.msg.canas.message_code = msg->msg.canas.message_code;
  if(memid < 0 ||
     memid > 65535 ||
     memid > (ee_params->number_parameters) ||
     num_replies > 5)        // don't read past end of buffer
    {
    // send error
    publish_reply(create_can_msg_long(&reply, msg->id + 1, msg->msg.canas.service_code, -1));
    return;
    }

  if(memid == 0)
    {
    // send the number of parameters that are defined.
    create_can_msg_short(&reply, msg->id + 1, msg->msg.canas.service_code, ee_params->number_parameters);
    reply.msg.canas.message_code = 0;
    publish_reply(&reply);
    return;
    }

  reply.msg.canas.data_type = CANAS_DATATYPE_UCHAR4;
  reply.flags = 8;
  reply.id = msg->id + 1;
  reply.msg.canas.service_code = msg->msg.canas.service_code;
  reply.msg.canas.message_code = 0;
  
  uint8_t *data = (uint8_t *)ee_params->definitions + (memid-1);

  while(num_replies--)
    {
    int bytes;
    uint8_t *dest = reply.msg.canas.data;

    for(bytes = 0; bytes < 4; bytes++)
      *dest++ = *data++;
    
    publish_reply(&reply);

    reply.msg.canas.message_code++;

    // and yield so that the messages are sent
    can_tx_task(0);
    }
  }

void process_nis(const can_msg_t *msg)
  {
  can_msg_t reply;
  reply.flags = 4;
  reply.id = msg->id + 1;
  reply.msg.canas.data_type = CANAS_DATATYPE_NODATA;
  reply.msg.canas.service_code = 11;

  if(msg->msg.canas.message_code >= ahrs_node_id && 
     msg->msg.canas.message_code <= ahrs_node_id_last)
    {
    /*
    short current_id = read_setting(0);
    if(msg->msg.canas.message_code != current_id)
      {
      write_setting(0, msg->msg.canas.message_code);
      }
     * */
    reply.msg.canas.message_code = 0;
    }
  else
    reply.msg.canas.message_code = -6;

  publish_reply(&reply);
  }

void process_css(const can_msg_t *msg)
  {
  can_msg_t reply;
  uint32_t new_id = (uint32_t)get_param_long(msg);
  uint16_t param_index = (uint16_t)(new_id >> 16);

  reply.flags = 4;
  reply.id = msg->id + 1;
  reply.msg.canas.data_type = CANAS_DATATYPE_NODATA;
  reply.msg.canas.service_code = 14;


  if(param_index >= ee_params->number_parameters)
    reply.msg.canas.message_code = -6;          // Error as out of range
  else
    {
    /*
    EE_addr += param_index * sizeof(can_parameter_definition_t);

    // first word is the CAN identifier
    _erase_eedata(EE_addr, _EE_WORD);
    _wait_eedata();
    _write_eedata_word(EE_addr, new_id);
    _wait_eedata();
     * */

    reply.msg.canas.message_code = 0;            // updated ok
    }

  publish_reply(&reply);
  }

bool read_parameter_definition(uint16_t memid, can_parameter_definition_t *defn, semaphore_t *task_callback)
  {
  return read_parameter_data(memid, sizeof(can_parameter_definition_t), (uint8_t *)defn, 0, task_callback);
  }

bool write_parameter_definition(uint16_t memid, const can_parameter_definition_t *defn, semaphore_t *task_callback)
  {
  return write_parameter_data(memid, defn->length, (const uint8_t *)defn, task_callback);
  }

bool read_setting_short(uint16_t memid, short *result, semaphore_t *task_callback)
  {
  return read_parameter_data(memid, sizeof(short), (uint8_t *)result, 0, task_callback);
  }

bool write_setting_short(uint16_t memid, const short *value, semaphore_t *task_callback)
  {
  return write_parameter_data(memid, sizeof(short), (const uint8_t *) value, task_callback);
  }

bool read_setting_float(uint16_t memid, float *result, semaphore_t *task_callback)
  {
  return read_parameter_data(memid, sizeof(float), (uint8_t *)result, 0, task_callback);
  }

bool write_setting_float(uint16_t memid, const float *value, semaphore_t *task_callback)
  {
  return write_parameter_data(memid, sizeof(float), (const uint8_t *) value, task_callback);
  }
