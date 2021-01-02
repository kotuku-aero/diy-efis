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
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../neutron/neutron.h"
#include "cli.h"
#include "../neutron/stream.h"

#define CTRL_A (1)
#define CTRL_E (5)
#define CTRL_N (14)
#define CTRL_P (16)

/*
 * This code will create a channel when called initially.
 */

// allow for up to 4 concurrent channels
#define MAX_CHANNELS  4

// this should be defined externally in the main program
extern cli_node_t cli_root;
// name of the node.  Is a friendly name rather than an id
extern const char *node_name;
// normally defined in can_aerospace.c
extern uint8_t node_id;

#define PARSER_KEEP_ALIVE 1
#define PARSER_OK         0
#define CHANNEL_CLOSE     -1
#define NO_CHANNELS       -2
#define BAD_INIT          -3
#define NO_WORKER         -4

typedef struct _service_channel_t
  {
  stream_handle_t stream;
  // this MUST be first so we can cast a parser to a channel !!!
  // parser handling the commands
  cli_t parser;
  // this is the channel number we are responding to
  uint16_t channel_number;
  // this is our index, used when we close
  uint16_t can_id;
  // queue of characters from remote device
  deque_p queue;
  // this is the thread that is servicing the channel
  task_p worker;
  // increment as each message sent
  uint8_t message_code;
  } service_channel_t;

static service_channel_t *channels[MAX_CHANNELS];

static service_channel_t *open_channel(uint16_t can_id)
  {
  uint16_t i;
  for (i = 0; i < MAX_CHANNELS; i++)
    {
    if (channels[i] != 0 &&
        channels[i]->can_id == can_id)
      return channels[i];
    }

  return 0;
  }

void close_channel(uint16_t can_id)
  {
  uint16_t i;
  for (i = 0; i < MAX_CHANNELS; i++)
    {
    if (channels[i]->channel_number == can_id)
      break;
    }

  if (i == MAX_CHANNELS)
    return;
  //
  canmsg_t nak_msg;
  memset(&nak_msg, 0, sizeof (canmsg_t));

  // set reply channel
  set_can_id(&nak_msg, can_id + 1);
  set_can_len(&nak_msg, 4); // close message length is 4
  nak_msg.canas.data_type = CANAS_DATATYPE_NODATA;
  nak_msg.canas.service_code = id_ccs_service;
  nak_msg.canas.message_code = CHANNEL_CLOSE; // close message
  can_send_reply(&nak_msg);

  // TODO: free other memory.
  // now we free the channel
  channels[i]->parser.done = 1;
  
  deque_close(channels[i]->queue);
  channels[i]->queue = 0;
  }

static result_t get_channel(stream_handle_t *stream, uint16_t *channel)
  {
  if(stream == 0 || 
     channel == 0 ||
     stream->version != sizeof(service_channel_t))
    return e_bad_parameter;
  
  service_channel_t *sc = (service_channel_t *)stream;
  *channel = sc->can_id;
  
  return s_ok;
  }

static result_t css_stream_eof(stream_handle_t *hndl)
  {
  result_t result;
  uint16_t can_id;
  if(failed(result = get_channel(hndl, &can_id)))
    return result;
  
  service_channel_t *channel = (service_channel_t *)hndl;
  
  return channel->queue == 0 ? s_ok : s_false;
  }

static result_t css_stream_read(stream_handle_t *hndl, void *buffer, uint16_t size, uint16_t *read)
  {
  if(hndl == 0 || buffer == 0 || size == 0)
    return e_bad_parameter;
  
  result_t result;
  uint16_t can_id;
  if(failed(result = get_channel(hndl, &can_id)))
    return result;
  
  service_channel_t *channel = (service_channel_t *)hndl;
  
  if(read != 0)
    *read = 0;
  
  char *str = (char *)buffer;
  
  while(size--)
    {
    // get the char
    if(failed(result = pop_front(channel->queue, buffer, INDEFINITE_WAIT)))
      return result;
    
    if(*str == '\r')
      *str = '\n';
    
    str++;
    
    if(read != 0)
      *read = *read+1;
    }
  
  return s_ok;
  }

static result_t css_stream_write(stream_handle_t *hndl, const void *buffer, uint16_t size)
  {
  if(hndl == 0 || buffer == 0 || size == 0)
    return e_bad_parameter;
  
  result_t result;
  uint16_t can_id;
  if(failed(result = get_channel(hndl, &can_id)))
    return result;
  
  service_channel_t *channel = (service_channel_t *)hndl;
  canmsg_t msg;
  set_can_id(&msg, channel->can_id + 1);
  msg.canas.node_id = node_id;
  msg.canas.service_code = id_ccs_service;
  
  const char *str = (const char *)buffer;
  while(size > 0)
    {
    if(size >= 4)
      {
      set_can_len(&msg, 8);
      msg.canas.data_type = CANAS_DATATYPE_CHAR4;
      msg.canas.data[0] = *str++;
      msg.canas.data[1] = *str++;
      msg.canas.data[2] = *str++;
      msg.canas.data[3] = *str++;
      size -= 4;
      }
    else if(size == 3)
      {
      set_can_len(&msg, 7);
      msg.canas.data_type = CANAS_DATATYPE_CHAR3;
      msg.canas.data[0] = *str++;
      msg.canas.data[1] = *str++;
      msg.canas.data[2] = *str++;
      size -= 3;
      }
    else if(size == 2)
      {
      set_can_len(&msg, 6);
      msg.canas.data_type = CANAS_DATATYPE_CHAR2;
      msg.canas.data[0] = *str++;
      msg.canas.data[1] = *str++;
      size -= 2;
      }
    else
      {
      set_can_len(&msg, 5);
      msg.canas.data_type = CANAS_DATATYPE_CHAR;
      msg.canas.data[0] = *str++;
      size--;
      }
  
    // this will set the message code correctly
    can_send(&msg);
    }
 
  return s_ok;
  }

static result_t css_stream_close(stream_handle_t *hndl)
  {
  result_t result;
  uint16_t can_id;
  if(failed(result = get_channel(hndl, &can_id)))
    return result;
  
  service_channel_t *channel = (service_channel_t *)hndl;
  close_channel(channel->can_id);
  
  // make sure any in-mem ptrs gone.
  memset(channel, 0, sizeof(service_channel_t));
  
  neutron_free(channel);
  
  return s_ok;
  }


static void parser_worker(void *parg)
  {
  service_channel_t *channel = (service_channel_t *) parg;

  cli_run(&channel->parser);
  
  // clean ourselves up
  handle_t us = channel->worker;
  
  if(channel->queue != 0)
    deque_close(channel->queue);
    
  channels[channel->can_id] = 0;
  neutron_free(channel);
  
  close_task(us);
  }

#define RX_BUFFER_SIZE 128

void create_channel(const canmsg_t *msg, cli_node_t *app_cli_root)
  {
  uint16_t i;
  canmsg_t reply_msg;
  memset(&reply_msg, 0, sizeof (canmsg_t));
  set_can_id(&reply_msg, get_can_id(msg) + 1); // send on the reply channel
  reply_msg.canas.data_type = CANAS_DATATYPE_NODATA;
  reply_msg.canas.service_code = id_ccs_service;
  set_can_len(&reply_msg, 4);

  for (i = 0; i < MAX_CHANNELS; i++)
    {
    if (channels[i] == 0)
      break;
    }

  if (i >= MAX_CHANNELS)
    {
    // reply to the message
    reply_msg.canas.node_id = node_id;
    reply_msg.canas.message_code = NO_CHANNELS; // flag the message code won't work
    can_send_reply(&reply_msg);
    return;
    }

  // we have a channels so create one.
  service_channel_t *channel = (service_channel_t *) neutron_malloc(sizeof (service_channel_t));
  memset(channel, 0, sizeof (service_channel_t));
  
  channel->stream.version = sizeof(service_channel_t);
  channel->stream.stream_eof = css_stream_eof;
  channel->stream.stream_read = css_stream_read;
  channel->stream.stream_write = css_stream_write;
  channel->stream.stream_close = css_stream_close;

  channel->can_id = get_can_id(msg);

  channel->parser.cfg.root = app_cli_root;
  channel->parser.cfg.ch_complete = '\t';
  channel->parser.cfg.ch_erase = '\b';
  channel->parser.cfg.ch_del = 127;
  channel->parser.cfg.ch_help = '?';
  channel->parser.cfg.flags = 0;
  channel->parser.cfg.prompt = node_name;

  channel->parser.cfg.console_in = &channel->stream;
  channel->parser.cfg.console_out = &channel->stream;
  channel->parser.cfg.console_err = &channel->stream;
  
  deque_create(sizeof(char), RX_BUFFER_SIZE, &channel->queue);

  if (s_ok != cli_init(&channel->parser.cfg, &channel->parser))
    {
    // reply to the message
    reply_msg.canas.message_code = BAD_INIT; // flag the message code won't work
    can_send_reply(&reply_msg);

    neutron_free(channel);
    return;
    }

  // the channel is open.  create a task.
  if (failed(task_create("CLI", DEFAULT_STACK_SIZE * 4, parser_worker, channel, NORMAL_PRIORITY-1, &channel->worker)))
    {
    // reply to the message
    reply_msg.canas.message_code = NO_WORKER;
    can_send_reply(&reply_msg);

    neutron_free(channel);
    return;
    }

  channels[i] = channel;

  // flag the worker is ok
  reply_msg.canas.message_code = PARSER_OK;
  can_send_reply(&reply_msg);
  }

bool process_ccs(const canmsg_t *msg, void *parg)
  {
  // decide how to create 
  // find the channel
  service_channel_t *channel = open_channel(get_can_id(msg));
  if(channel == 0)
    {
    // open a channel if possible
    create_channel(msg, (cli_node_t *)parg);
    return true;
    }

  if(msg->canas.data_type != CANAS_DATATYPE_NODATA)
    {
    // push the characters onto the worker queue
    switch(msg->canas.data_type)
      {
      case CANAS_DATATYPE_CHAR :
        push_back(channel->queue, &msg->canas.data[0], 0);
        break;
      case CANAS_DATATYPE_CHAR2 :
        push_back(channel->queue, &msg->canas.data[0], 0);
        push_back(channel->queue, &msg->canas.data[1], 0);
        break;
      case CANAS_DATATYPE_CHAR3 :
        push_back(channel->queue, &msg->canas.data[0], 0);
        push_back(channel->queue, &msg->canas.data[1], 0);
        push_back(channel->queue, &msg->canas.data[2], 0);
        break;
      case CANAS_DATATYPE_CHAR4 :
        push_back(channel->queue, &msg->canas.data[0], 0);
        push_back(channel->queue, &msg->canas.data[1], 0);
        push_back(channel->queue, &msg->canas.data[2], 0);
        push_back(channel->queue, &msg->canas.data[3], 0);
        break;
      }
    }
  
  // send ak
  canmsg_t reply_msg;
  set_can_id(&reply_msg, get_can_id(msg) + 1);
  set_can_len(&reply_msg, 4);
  reply_msg.canas.data_type = CANAS_DATATYPE_NODATA;
  reply_msg.canas.node_id = node_id;
  reply_msg.canas.service_code = id_ccs_service;
  reply_msg.canas.message_code = PARSER_OK;
  can_send_reply(&reply_msg);
  
  return true;
  }

static msg_hook_t service_hook = { 0, 0, process_ccs, 0 };

result_t muon_initialize_cli(cli_node_t *cli_root)
  {
  service_hook.parg = cli_root;
  return register_service(id_ccs_service, &service_hook);
  }