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
then the original copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
#if !defined(__dsPIC33)
#include "../ion/ion.h"
#include "uart_device.h"

// we register the serial handler functions.
// if used we allow the device specific uart code to be handled
// by nano.  The code only supports basic serial comms
// and all messages are passed as strings to nano

typedef struct _serial_handle_t {
  uart_config_t config;           // will be passed back in the callback
  ion_context_t *ctx;                        // context owning serial port
  uint16_t msg_id;
  char callback[];                // dynamically allocated
  } serial_handle_t;
  
// these are the application specific serial slots
static serial_handle_t *channels[NUM_SERIAL_CHANNELS];

static uint8_t datatypes[4] = {
  CANAS_DATATYPE_CHAR,
  CANAS_DATATYPE_CHAR2,
  CANAS_DATATYPE_CHAR3,
  CANAS_DATATYPE_CHAR4
  };

/*
// handler for the serial callback
static void rx_callback(uart_config_t *uart, uint8_t *buffer, uint8_t length)
  {
  serial_handle_t *handle = (serial_handle_t *)uart;
  

  ion_context_t *ion = (ion_context_t *)mem_funcs.udata;
  
  canmsg_t msg;
  memset(&msg, 0, sizeof(canmsg_t));
  set_can_id(&msg, handle->msg_id);
  while(length > 0)
    {
    uint16_t len = length;
    if(len > 4)
      len = 4;

    set_can_len(&msg, 4 + len);
    msg.canas.data_type = datatypes[len-1];
    
    length -= len;
    
    uint8_t idx;
    for(idx = 0; idx < len; idx++)
      {
      msg.canas.data[idx] = *buffer++;
      len--;
      }
    
    // send the message to the script engine
    ion_queue_message(ion, handle->callback, &msg);
    }
  }

// int open_serial(int channel, int baud_rate, char eol_char, int id_msg, string callback);
//
// id_msg is the callback id of a can msg that receives the
// serial characters.
//
// the on_msg function will be called when the serial port has
// a character, or characters
static duk_ret_t lib_open_serial(duk_context *ctx) 
  {
  uint16_t channel = (uint16_t)duk_get_uint(ctx, 0);
  uint32_t baud_rate = (uint32_t)duk_get_uint(ctx, 1);
  char eol_char = (char)duk_get_int(ctx, 2);
  uint16_t id_msg = (uint16_t)duk_get_uint(ctx, 3);
  const char *callback = duk_get_string(ctx, 4);
  
  if(channel < 1 || channel > NUM_SERIAL_CHANNELS ||
     channels[channel-1] != 0)
    return DUK_RET_TYPE_ERROR;
    
  if(id_msg < id_user_defined_start || id_msg > id_user_defined_end)
    return DUK_RET_TYPE_ERROR;
  
  if(callback == 0)
    return DUK_RET_TYPE_ERROR;

  size_t len = strlen(callback) +1;
  len += sizeof(serial_handle_t);
  
  serial_handle_t *handle = (serial_handle_t *)neutron_malloc(len);
  
  memset(handle, 0, len);
  strcpy(handle->callback, callback);
  
  handle->ctx = ctx;
  handle->config.uart_number = channel;
  handle->config.rate = baud_rate;
  handle->config.rx_length = 128;
  handle->config.num_rx_buffers = 4;
  handle->config.eol_char = eol_char;
  handle->config.callback.uart_callback = rx_callback;
  
  if(failed(open_uart(&handle->config, DEFAULT_STACK_SIZE, DEFAULT_STACK_SIZE)))
    {
    neutron_free(handle);
    return DUK_RET_TYPE_ERROR;
    }
    
  channels[channel-1] = handle;
  duk_push_pointer(ctx, handle);
  
  return 1;
  }

//
// void send_serial(int uart, string msg);
// send a string of characters to the serial port
//
static duk_ret_t lib_send_serial(duk_context *ctx)
  {
  uint16_t channel = (uint16_t)duk_get_uint(ctx, 0);
  const char *msg = duk_get_string(ctx, 1);
  
  if(channel == 0 || msg == 0 || channel > NUM_SERIAL_CHANNELS ||
     channels[channel-1] == 0)
    return DUK_RET_TYPE_ERROR;

  send_str(&channels[channel-1]->config, msg);
  
  return 0;
  }

//
// void close_serial(int handle)
//
static duk_ret_t lib_close_serial(duk_context *ctx)
  {
  uint16_t channel = (uint16_t)duk_get_uint(ctx, 0);
  if(channel == 0 || channel > NUM_SERIAL_CHANNELS ||
     channels[channel-1] == 0)
    return DUK_RET_TYPE_ERROR;
  
  if(failed(close_uart(&channels[channel-1]->config)))
    return DUK_RET_TYPE_ERROR;
  
  neutron_free(channels[channel-1]);
  channels[channel-1] = 0;
  
  return 0;
  }

static void add_function(duk_context *ctx, duk_c_function fn, const char *name, duk_idx_t nargs)
  {
  duk_push_c_function(ctx, fn, nargs);
  duk_put_global_string(ctx, name);
  }

result_t register_atom_functions(duk_context *ctx, handle_t co)
  {
  add_function(ctx, lib_open_serial, "open_serial", 5);
  add_function(ctx, lib_send_serial, "send_serial", 2);
  add_function(ctx, lib_close_serial, "close_serial", 1);
  
  return s_ok;
  }
 * */

#endif
