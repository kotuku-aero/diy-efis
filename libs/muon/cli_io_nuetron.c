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
  handle_t queue;
  // this is the thread that is servicing the channel
  handle_t worker;
  // increment as each message sent
  uint8_t message_code;
  } service_channel_t;

static service_channel_t *channels[MAX_CHANNELS];

static service_channel_t *open_channel(uint16_t can_id)
  {
  uint16_t i;
  for (i = 0; i < MAX_CHANNELS; i++)
    {
    if (channels[i]->channel_number == can_id)
      break;
    }

  if (i == MAX_CHANNELS)
    return 0;

  return channels[i];
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
  nak_msg.id = can_id + 1;
  nak_msg.length = 4; // close message length is 4
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
  
  while(size > 0)
    {
    // get the char
    if(failed(result = pop_front(channel->queue, buffer, INDEFINITE_WAIT)))
      return result;
    
    buffer = ((uint8_t *)buffer)+1;
    size--;
    
    if(*read != 0)
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
  msg.id = channel->can_id + 1;
  msg.canas.node_id = node_id;
  msg.canas.service_code = id_ccs_service;
  
  const char *str = (const char *)buffer;
  while(size > 0)
    {
    if(size >= 4)
      {
      msg.length = 8;
      msg.canas.data_type = CANAS_DATATYPE_CHAR4;
      msg.canas.data[0] = *str++;
      msg.canas.data[1] = *str++;
      msg.canas.data[2] = *str++;
      msg.canas.data[3] = *str++;
      size -= 4;
      }
    else if(size == 3)
      {
      msg.length = 7;
      msg.canas.data_type = CANAS_DATATYPE_CHAR3;
      msg.canas.data[0] = *str++;
      msg.canas.data[1] = *str++;
      msg.canas.data[2] = *str++;
      size -= 3;
      }
    else if(size == 2)
      {
      msg.length =6;
      msg.canas.data_type = CANAS_DATATYPE_CHAR2;
      msg.canas.data[0] = *str++;
      msg.canas.data[1] = *str++;
      size -= 2;
      }
    else
      {
      msg.length = 5;
      msg.canas.data_type = CANAS_DATATYPE_UCHAR;
      msg.canas.data[0] = *str++;
      size--;
      }
    }
  
  // this will set the message code correctly
  can_send(&msg);
 
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
  
  kfree(channel);
  
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
  kfree(channel);
  
  close_task(us);
  }

#define RX_BUFFER_SIZE 128

extern cli_node_t *app_cli_root;

void create_channel(const canmsg_t *msg)
  {
  uint16_t i;
  canmsg_t reply_msg;
  memset(&reply_msg, 0, sizeof (canmsg_t));
  reply_msg.id = msg->id + 1; // send on the reply channel
  reply_msg.canas.data_type = CANAS_DATATYPE_NODATA;
  reply_msg.canas.service_code = id_ccs_service;
  reply_msg.length = 4;

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
  service_channel_t *channel = (service_channel_t *) kmalloc(sizeof (service_channel_t));
  memset(channel, 0, sizeof (service_channel_t));
  
  channel->stream.version = sizeof(service_channel_t);
  channel->stream.stream_eof = css_stream_eof;
  channel->stream.stream_read = css_stream_read;
  channel->stream.stream_write = css_stream_write;
  channel->stream.stream_close = css_stream_close;

  channel->can_id = msg->id;

  channel->parser.cfg.root = app_cli_root;
  channel->parser.cfg.ch_complete = '\t';
  channel->parser.cfg.ch_erase = '\b';
  channel->parser.cfg.ch_del = 127;
  channel->parser.cfg.ch_help = '?';
  channel->parser.cfg.flags = 0;
  strcpy(channel->parser.cfg.prompt, node_name);

  channel->parser.cfg.console_in = &channel->stream;
  channel->parser.cfg.console_out = &channel->stream;
  channel->parser.cfg.console_err = &channel->stream;
  
  deque_create(sizeof(char), RX_BUFFER_SIZE, &channel->queue);

  if (s_ok != cli_init(&channel->parser.cfg, &channel->parser))
    {
    // reply to the message
    reply_msg.canas.message_code = BAD_INIT; // flag the message code won't work
    can_send_reply(&reply_msg);

    kfree(channel);
    return;
    }

  // the channel is open.  create a task.
  if (failed(task_create("CLI", DEFAULT_STACK_SIZE, parser_worker, 0, NORMAL_PRIORITY, &channel->worker)))
    {
    // reply to the message
    reply_msg.canas.message_code = NO_WORKER;
    can_send_reply(&reply_msg);

    kfree(channel);
    return;
    }

  channels[i] = channel;

  // flag the worker is ok
  reply_msg.canas.message_code = PARSER_OK;
  can_send_reply(&reply_msg);
  }

void process_ccs(const canmsg_t *msg)
  {
  // decide how to create 
  // find the channel
  service_channel_t *channel = open_channel(msg->id);
  if(channel == 0)
    {
    // open a channel if possible
    create_channel(msg);
    return;
    }

  // push the characters onto the worker queue
  switch(msg->canas.data_type)
    {
    case CANAS_DATATYPE_UCHAR :
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
  
  // send ak
  canmsg_t reply_msg;
  reply_msg.id = msg->id + 1;
  reply_msg.length = 4;
  reply_msg.canas.data_type = CANAS_DATATYPE_NODATA;
  reply_msg.canas.node_id = node_id;
  reply_msg.canas.service_code = id_ccs_service;
  reply_msg.canas.message_code = PARSER_OK;
  can_send_reply(&reply_msg);
  }

result_t muon_initialize_cli()
  {
  return register_service(id_ccs_service, process_ccs);
  }