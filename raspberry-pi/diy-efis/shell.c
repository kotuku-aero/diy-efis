#include "../../libs/neutron/neutron.h"
#include "../../libs/electron/console.h"

/*  Neutron shell service for diy-efis
 *
 * creates a linux shell when a id_shl_service message arrives
 */

#define MAX_CHANNELS 4
#define PARSER_KEEP_ALIVE 1
#define PARSER_OK         0
#define CHANNEL_CLOSE     -1
#define NO_CHANNELS       -2
#define BAD_INIT          -3
#define NO_WORKER         -4

typedef struct _service_channel_t
  {
  // this is the channel number we are responding to
  uint16_t channel_number;
  // this is our index, used when we close
  uint16_t can_id;
  // rx_queue of characters from remote device
  deque_p rx_queue;
  // this is the thread that is servicing the channel
  task_p rx_worker;
  // task to read from the linux console and write to the pipe
  task_p tx_worker;
  // increment as each message sent
  uint8_t message_code;
  bool close_channel;
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

static void close_channel(uint16_t can_id)
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
  memset(&nak_msg, 0, sizeof(canmsg_t));

  // set reply channel
  nak_msg.id = can_id + 1;
  nak_msg.length = 4; // close message length is 4
  nak_msg.canas.data_type = CANAS_DATATYPE_NODATA;
  nak_msg.canas.service_code = id_shl_service;
  nak_msg.canas.message_code = CHANNEL_CLOSE; // close message
  can_send_reply(&nak_msg);

  channel_close(channels[i]->channel_number);
  deque_close(channels[i]->rx_queue);
  channels[i]->rx_queue = 0;
  }

static void rx_worker(void *parg)
  {
  service_channel_t *channel = (service_channel_t *)parg;
  result_t result;
  // we are a running channel, 
  while (!channel->close_channel)
    {

    }
  // clean ourselves up
  handle_t us = channel->rx_worker;

  if (channel->rx_queue != 0)
    deque_close(channel->rx_queue);

  channels[channel->can_id] = 0;
  close_task(us);
  }


static void tx_worker(void *parg)
  {
  service_channel_t *channel = (service_channel_t *)parg;

  // we are a running channel, 
  while (!channel->close_channel)
    {

    }

  // clean ourselves up
  handle_t us = channel->tx_worker;
  close_task(us);
  }


#define RX_BUFFER_SIZE 128

void create_channel(const canmsg_t *msg)
  {
  uint16_t i;
  canmsg_t reply_msg;
  memset(&reply_msg, 0, sizeof(canmsg_t));
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
  service_channel_t *channel = (service_channel_t *)neutron_malloc(sizeof(service_channel_t));
  memset(channel, 0, sizeof(service_channel_t));

  channel->can_id = msg->id;
  deque_create(sizeof(char), RX_BUFFER_SIZE, &channel->rx_queue);

  // the channel is open.  create a task.
  if (failed(task_create("CLI", DEFAULT_STACK_SIZE * 4, rx_worker, channel, NORMAL_PRIORITY - 1, &channel->rx_worker)))
    {
    // reply to the message
    reply_msg.canas.message_code = NO_WORKER;
    can_send_reply(&reply_msg);

    neutron_free(channel);
    return;
    }

  channels[i] = channel;

  // flag the rx_worker is ok
  reply_msg.canas.message_code = PARSER_OK;
  can_send_reply(&reply_msg);
  }

bool process_shl(const canmsg_t *msg, void *parg)
  {
  // decide how to create 
  // find the channel
  service_channel_t *channel = open_channel(msg->id);
  if (channel == 0)
    {
    // open a channel if possible
    create_channel(msg);
    return true;
    }

  if (msg->canas.data_type != CANAS_DATATYPE_NODATA)
    {
    // push the characters onto the rx_worker rx_queue
    switch (msg->canas.data_type)
      {
      case CANAS_DATATYPE_CHAR:
        push_back(channel->rx_queue, &msg->canas.data[0], 0);
        break;
      case CANAS_DATATYPE_CHAR2:
        push_back(channel->rx_queue, &msg->canas.data[0], 0);
        push_back(channel->rx_queue, &msg->canas.data[1], 0);
        break;
      case CANAS_DATATYPE_CHAR3:
        push_back(channel->rx_queue, &msg->canas.data[0], 0);
        push_back(channel->rx_queue, &msg->canas.data[1], 0);
        push_back(channel->rx_queue, &msg->canas.data[2], 0);
        break;
      case CANAS_DATATYPE_CHAR4:
        push_back(channel->rx_queue, &msg->canas.data[0], 0);
        push_back(channel->rx_queue, &msg->canas.data[1], 0);
        push_back(channel->rx_queue, &msg->canas.data[2], 0);
        push_back(channel->rx_queue, &msg->canas.data[3], 0);
        break;
      }
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

  return true;
  }
static msg_hook_t service_hook = { 0, 0, process_shl, 0 };

result_t shell_initialize()
  {
  return register_service(id_shl_service, &service_hook);
  }