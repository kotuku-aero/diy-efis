#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../libs/neutron/neutron.h"
#include "../../libs/muon/cli.h"
#include "../../libs/neutron/stream.h"

#define CTRL_A (1)
#define CTRL_E (5)
#define CTRL_N (14)
#define CTRL_P (16)

/*
 * This code will create a channel when called initially.
 */

// name of the node.  Is a friendly name rather than an id
static const char *node_name;
// normally defined in canfly.c
extern uint8_t node_id;

typedef struct _service_channel_t
  {
  stream_t stream;
  // this MUST be first so we can cast a parser to a channel !!!
  // parser handling the commands
  cli_t parser;
  // this is the channel number we are responding to
  uint16_t channel_number;
  // this is the channel that is used to send messages to
  handle_t  pipe;
  // queue of characters from remote device
  handle_t queue;
  // this is the thread that is servicing the channel
  handle_t worker;
  } service_channel_t;

static const typeid_t service_type =
  {
  .name = "service",
  .base = &stream_type
  };

static result_t css_stream_eof(handle_t hndl)
  {
  result_t result;
  service_channel_t *channel;
  if (failed(result = is_typeof(hndl, &service_type, (void **)&channel)))
    return result;
  
  return channel->queue == 0 ? s_ok : s_false;
  }

static result_t css_stream_read(handle_t hndl, void *buffer, uint16_t size, uint16_t *read)
  {
  if(hndl == 0 || buffer == 0 || size == 0)
    return e_bad_parameter;
  
  result_t result;
  service_channel_t *channel;
  if (failed(result = is_typeof(hndl, &service_type, (void **)&channel)))
    return result;

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

static result_t css_stream_write(handle_t hndl, const void *buffer, uint16_t size)
  {
  if(hndl == 0 || buffer == 0 || size == 0)
    return e_bad_parameter;
  
  result_t result;
  service_channel_t *channel;
  if (failed(result = is_typeof(hndl, &service_type, (void **)&channel)))
    return result;

  return send_bearer_message(channel->pipe, size, buffer);
  }

static void parser_worker(void *parg)
  {
  service_channel_t *channel = (service_channel_t *)parg;

  cli_run(&channel->parser);

  // this is guarded as the remote channel may be closed
  // and we don't want to close the channel twice.
  request_close_pipe(channel->pipe);

  if (channel->queue != 0)
    close_handle(channel->queue);

  neutron_free(channel);

  }

static result_t css_stream_receive(handle_t hndl, const char *str, uint16_t len)
  {
  result_t result;
  service_channel_t *channel;
  if (failed(result = is_typeof(hndl, &service_type, (void **)&channel)))
    return result;

  while(len-- > 0 && *str != 0)
    push_back(channel->queue, str++, INDEFINITE_WAIT);

  return s_ok;
  }

#define RX_BUFFER_SIZE 128

static cli_node_t *cli_root;

result_t open_console(handle_t pipe, handle_t *parg)
  {
  result_t result;
  get_pipe_mutex(pipe, INDEFINITE_WAIT);

  service_channel_t *channel;
  
  if (failed(result = neutron_malloc(sizeof(service_channel_t), (void **)&channel)))
    {
    release_pipe_mutex(pipe);
    return result;
    }

  *parg = (handle_t) channel;

  channel->stream.base.type = &service_type;
  channel->stream.stream_eof = css_stream_eof;
  channel->stream.stream_read = css_stream_read;
  channel->stream.stream_write = css_stream_write;

  channel->pipe = pipe;

  channel->parser.cfg.root = cli_root;
  channel->parser.cfg.ch_complete = '\t';
  channel->parser.cfg.ch_erase = '\b';
  channel->parser.cfg.ch_del = 127;
  channel->parser.cfg.ch_help = '?';
  channel->parser.cfg.flags = 0;
  channel->parser.cfg.prompt = node_name;

  channel->parser.cfg.console_in = channel;
  channel->parser.cfg.console_out = channel;
  channel->parser.cfg.console_err = channel;

  channel->parser.cfg.receive_char = css_stream_receive;

  deque_create(sizeof(char), RX_BUFFER_SIZE, &channel->queue);

  if (s_ok != cli_init(&channel->parser.cfg, &channel->parser))
    {
    release_pipe_mutex(pipe);
    neutron_free(channel);
    return e_invalid_operation;
    }

  // the channel is open.  create a task.
  if (failed(task_create("CLI", DEFAULT_STACK_SIZE * 4, parser_worker, channel, NORMAL_PRIORITY - 1, &channel->worker)))
    {
    release_pipe_mutex(pipe);
    neutron_free(channel);
    return e_invalid_operation;
    }

  release_pipe_mutex(pipe);

  return s_ok;
  }

result_t close_console(handle_t  pipe, handle_t parg)
  {
  get_pipe_mutex(pipe, INDEFINITE_WAIT);

  // close a console, once opened.
  service_channel_t *channel;

  get_pipe_arg(pipe, (void **)&channel);


  if (channel != 0)
    {
    cli_abort(&channel->parser);
    close_handle(channel->worker);
    close_handle(channel->queue);

    neutron_free(channel);
    }

  release_pipe_mutex(pipe);

  return s_ok;
  }

static result_t recv_console(handle_t  pipe, handle_t parg, uint16_t len, const uint8_t *data)
  {
  service_channel_t *channel;

  get_pipe_arg(pipe, (void **)&channel);

  if (channel != 0)
    {
    result_t result;
    for (uint16_t i = 0; i < len; i++)
      if (failed(result = push_back(channel->queue, &data[i], 500)))
        return result;
    }

  return s_ok;
  }

result_t muon_initialize_cli(cli_node_t *_cli_root, const char *name)
  {
  cli_root = _cli_root;
  node_name = name;

  // we don't need the service handle as the callback is all that is needed
  return create_service(service_console, 0, open_console, recv_console, close_console, 0);
  }