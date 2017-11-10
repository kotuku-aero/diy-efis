#include "neutron.h"
#include "bsp.h"
#include <string.h>

// global node id
uint8_t node_id;
static uint8_t hardware_revision;
static uint8_t software_revision;

canmsg_t *create_can_msg_float(canmsg_t *msg, uint16_t message_id, uint8_t service_code, float value)
  {
  memset(msg, 0, sizeof(canmsg_t));
  msg->length = 8;
  msg->id = message_id;
  msg->canas.data_type = CANAS_DATATYPE_FLOAT;
  msg->canas.service_code = service_code;

  unsigned long *ulvalue = (unsigned long *)&value;
  msg->canas.data[0] = (unsigned char)(*ulvalue >> 24);
  msg->canas.data[1] = (unsigned char)(*ulvalue >> 16);
  msg->canas.data[2] = (unsigned char)(*ulvalue >> 0);
  msg->canas.data[3] = (unsigned char)*ulvalue;
  
  return msg;
  }
  
canmsg_t *create_can_msg_int16(canmsg_t *msg, uint16_t message_id, uint8_t service_code, int16_t data)
  {
  memset(msg, 0, sizeof(canmsg_t));
  msg->length = 6;
  msg->id = message_id;
  msg->canas.data_type = CANAS_DATATYPE_SHORT;
  msg->canas.service_code = service_code;
  msg->canas.data[0] = (uint8_t)(data >> 8);
  msg->canas.data[1] = (uint8_t)data;
  
  return msg;
  }
  
canmsg_t *create_can_msg_int16_2(canmsg_t *msg, uint16_t message_id, uint8_t service_code, int16_t data1, int16_t data2)
  {
  memset(msg, 0, sizeof(canmsg_t));
  msg->length = 6;
  msg->id = message_id;
  msg->canas.data_type = CANAS_DATATYPE_SHORT2;
  msg->canas.service_code = service_code;
  msg->canas.data[0] = (uint8_t)(data1 >> 8);
  msg->canas.data[1] = (uint8_t)data1;
  msg->canas.data[2] = (uint8_t)(data2 >> 8);
  msg->canas.data[3] = (uint8_t)data2;
  
  return msg;
  }

canmsg_t *create_can_msg_int32(canmsg_t *msg, uint16_t message_id, uint8_t service_code, int32_t data)
  {
  memset(msg, 0, sizeof(canmsg_t));
  msg->length = 8;
  msg->id = message_id;
  msg->canas.data_type = CANAS_DATATYPE_INT32;
  msg->canas.service_code = service_code;
  msg->canas.data[0] = (uint8_t)(data >> 24);
  msg->canas.data[1] = (uint8_t)(data >> 16);
  msg->canas.data[2] = (uint8_t)(data >> 8);
  msg->canas.data[3] = (uint8_t)data;

  return msg;
  }
  
canmsg_t *create_can_msg_uint16(canmsg_t *msg, uint16_t message_id, uint8_t service_code, uint16_t data)
  {
  memset(msg, 0, sizeof(canmsg_t));
  msg->length = 6;
  msg->id = message_id;
  msg->canas.data_type = CANAS_DATATYPE_USHORT;
  msg->canas.service_code = service_code;
  msg->canas.data[0] = (uint8_t)(data >> 8);
  msg->canas.data[1] = (uint8_t)data;
  
  return msg;
  }
  
canmsg_t *create_can_msg_uint16_2(canmsg_t *msg, uint16_t message_id, uint8_t service_code, uint16_t data1, uint16_t data2)
  {
  memset(msg, 0, sizeof(canmsg_t));
  msg->length = 6;
  msg->id = message_id;
  msg->canas.data_type = CANAS_DATATYPE_USHORT2;
  msg->canas.service_code = service_code;
  msg->canas.data[0] = (uint8_t)(data1 >> 8);
  msg->canas.data[1] = (uint8_t)data1;
  msg->canas.data[2] = (uint8_t)(data2 >> 8);
  msg->canas.data[3] = (uint8_t)data2;
  
  return msg;
  }

canmsg_t *create_can_msg_uint32(canmsg_t *msg, uint16_t message_id, uint8_t service_code, uint32_t data)
  {
  memset(msg, 0, sizeof(canmsg_t));
  msg->length = 8;
  msg->id = message_id;
  msg->canas.data_type = CANAS_DATATYPE_UINT32;
  msg->canas.service_code = service_code;
  msg->canas.data[0] = (uint8_t)(data >> 24);
  msg->canas.data[1] = (uint8_t)(data >> 16);
  msg->canas.data[2] = (uint8_t)(data >> 8);
  msg->canas.data[3] = (uint8_t)data;

  return msg;
  }

canmsg_t *create_can_msg_int8(canmsg_t *msg, uint16_t message_id, uint8_t service_code, int8_t value)
  {
  memset(msg, 0, sizeof(canmsg_t));
  msg->length = 5;
  msg->id = message_id;
  msg->canas.data_type = CANAS_DATATYPE_UCHAR;
  msg->canas.service_code = service_code;
  msg->canas.data[0] = (uint8_t) value;

  return msg;
  }

canmsg_t *create_can_msg_int8_2(canmsg_t *msg, uint16_t message_id, uint8_t service_code, int8_t d0, int8_t d1)
  {
  memset(msg, 0, sizeof(canmsg_t));
  msg->length = 6;
  msg->id = message_id;
  msg->canas.data_type = CANAS_DATATYPE_UCHAR2;
  msg->canas.service_code = service_code;
  msg->canas.data[0] = d0;
  msg->canas.data[1] = d1;

  return msg;
  }

canmsg_t *create_can_msg_int8_3(canmsg_t *msg, uint16_t message_id, uint8_t service_code, int8_t d0, int8_t d1, int8_t d2)
  {
  memset(msg, 0, sizeof(canmsg_t));
  msg->length = 7;
  msg->id = message_id;
  msg->canas.data_type = CANAS_DATATYPE_UCHAR3;
  msg->canas.service_code = service_code;
  msg->canas.data[0] = d0;
  msg->canas.data[1] = d1;
  msg->canas.data[2] = d2;

  return msg;
  }

canmsg_t *create_can_msg_int8_4(canmsg_t *msg, uint16_t message_id, uint8_t service_code, int8_t d0, int8_t d1, int8_t d2, int8_t d3)
  {
  memset(msg, 0, sizeof(canmsg_t));
  msg->length = 8;
  msg->id = message_id;
  msg->canas.data_type = CANAS_DATATYPE_UCHAR4;
  msg->canas.service_code = service_code;
  msg->canas.data[0] = d0;
  msg->canas.data[1] = d1;
  msg->canas.data[2] = d2;
  msg->canas.data[3] = d3;

  return msg;
  }

canmsg_t *create_can_msg_uint8(canmsg_t *msg, uint16_t message_id, uint8_t service_code, uint8_t value)
  {
  memset(msg, 0, sizeof(canmsg_t));
  msg->length = 5;
  msg->id = message_id;
  msg->canas.data_type = CANAS_DATATYPE_UCHAR;
  msg->canas.service_code = service_code;
  msg->canas.data[0] = value;
  
  return msg;
  }  
  
canmsg_t *create_can_msg_uint8_2(canmsg_t *msg, uint16_t message_id, uint8_t service_code, uint8_t d0, uint8_t d1)
  {
  memset(msg, 0, sizeof(canmsg_t));
  msg->length = 6;
  msg->id = message_id;
  msg->canas.data_type = CANAS_DATATYPE_UCHAR2;
  msg->canas.service_code = service_code;
  msg->canas.data[0] = d0;
  msg->canas.data[1] = d1;
  
  return msg;
  }  
 
canmsg_t *create_can_msg_uint8_3(canmsg_t *msg, uint16_t message_id, uint8_t service_code, uint8_t d0, uint8_t d1, uint8_t d2)
  {
  memset(msg, 0, sizeof(canmsg_t));
  msg->length = 7;
  msg->id = message_id;
  msg->canas.data_type = CANAS_DATATYPE_UCHAR3;
  msg->canas.service_code = service_code;
  msg->canas.data[0] = d0;
  msg->canas.data[1] = d1;
  msg->canas.data[2] = d2;
  
  return msg;
  }
 
canmsg_t *create_can_msg_uint8_4(canmsg_t *msg, uint16_t message_id, uint8_t service_code, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
  {
  memset(msg, 0, sizeof(canmsg_t));
  msg->length = 8;
  msg->id = message_id;
  msg->canas.data_type = CANAS_DATATYPE_UCHAR4;
  msg->canas.service_code = service_code;
  msg->canas.data[0] = d0;
  msg->canas.data[1] = d1;
  msg->canas.data[2] = d2;
  msg->canas.data[3] = d3;
  
  return msg;
  }

result_t get_param_float(const canmsg_t *msg, float *v)
  {
  if (msg == 0 || v == 0)
    return e_bad_parameter;

  uint32_t value = (((uint32_t) msg->canas.data[0]) << 24) |
                        (((uint32_t) msg->canas.data[1]) << 16) |
                        (((uint32_t) msg->canas.data[2]) << 8) |
                        ((uint32_t) msg->canas.data[3]);
  *v = *(float *)(&value);
  return s_ok;
  }

result_t get_param_int8(const canmsg_t *msg, uint16_t index, int8_t *v)
  {
  if (index > 3 || msg == 0 || v == 0)
    return e_bad_parameter;

  *v = (int8_t) msg->canas.data[index];
  return s_ok;
  }

result_t get_param_uint8(const canmsg_t *msg, uint16_t index, uint8_t *v)
  {
  if (index > 3 || msg == 0 || v == 0)
    return e_bad_parameter;

  *v = msg->canas.data[index];
  return s_ok;
  }

result_t get_param_int16(const canmsg_t *msg, uint16_t index, int16_t *v)
  {
  if (index > 1 || msg == 0 || v == 0)
    return e_bad_parameter;

  *v = (int16_t)((((uint16_t) msg->canas.data[index << 1]) << 8) |
    ((uint16_t) msg->canas.data[(index << 1)+1]));

  return s_ok;
  }

result_t get_param_uint16(const canmsg_t *msg, uint16_t index, uint16_t *v)
  {
  if (index > 1 || msg == 0 || v == 0)
    return e_bad_parameter;

  *v = ((((uint16_t)msg->canas.data[index << 1]) << 8) |
    ((uint16_t)msg->canas.data[(index << 1) + 1]));

  return s_ok;
  }

result_t get_param_int32(const canmsg_t *msg, int32_t *v)
  {
  if (msg == 0 || v == 0)
    return e_bad_parameter;

  *v = (int32_t)(
    (((uint32_t) msg->canas.data[0]) << 24) |
    (((uint32_t) msg->canas.data[1]) << 16) | 
    (((uint32_t) msg->canas.data[2]) << 8) | 
     ((uint32_t) msg->canas.data[3]));

  return s_ok;
  }

result_t get_param_uint32(const canmsg_t *msg, uint32_t *v)
  {
  if (msg == 0 || v == 0)
    return e_bad_parameter;

  *v =
    (((uint32_t) msg->canas.data[0]) << 24) |
    (((uint32_t) msg->canas.data[1]) << 16) | 
    (((uint32_t) msg->canas.data[2]) << 8) | 
     ((uint32_t) msg->canas.data[3]);

  return s_ok;
  }

const matrix_t identity_matrix = {
    {
    { 1.0, 0.0, 0.0 },
    { 0.0, 1.0, 0.0 },
    { 0.0, 0.0, 1.0 }
    }
  };

void matrix_multiply(matrix_t mat, const matrix_t a, const matrix_t b)
  {
  int x;
  int y;
  int w;
  float op[3];

  for(x = 0; x < 3; x++)
    {
    for(y = 0; y < 3; y++)
      {
      for(w = 0; w < 3; w++)
        op[w] = a.v[x][w] * b.v[w][y];

      mat.v[x][y] = 0;
      mat.v[x][y] = op[0] + op[1] + op[2];
      }
    }
  }

static handle_t can_tx_queue;
static handle_t can_rx_queue;
static handle_t can_rpc_queue;
static uint8_t message_code;

/**
 * Worker task to queue messages
 * @param parg
 */
    
static canmsg_t tx_msg;
void can_tx_task(void *parg)
  {
  while(true)
    {
    // block till a message is ready
    pop_front(can_tx_queue, &tx_msg, INDEFINITE_WAIT);
    bsp_send_can(&tx_msg);
    }
  }

result_t can_send_raw(canmsg_t *msg)
  {
  if(msg == 0 ||
     msg->length == 0)
    return e_bad_parameter;
  
  // see if this is a service channel request where the
  // node-id == this node, if that is the case then loop-back the message
  if(((msg->id >= node_service_channel_0 && msg->id <= (node_service_channel_35+1)) ||
      (msg->id >= node_service_channel_100 && msg->id <= (node_service_channel_115+1))) &&
     msg->canas.node_id == node_id)
    return push_back(can_rx_queue, msg, INDEFINITE_WAIT);
  
  return push_back(can_tx_queue, msg, INDEFINITE_WAIT);
  }

result_t can_send_reply(canmsg_t *msg)
  {
  msg->canas.node_id = node_id;
  return can_send_raw(msg);
  }

result_t can_send(canmsg_t *msg)
  {
  msg->canas.message_code = ++message_code;
  return can_send_reply(msg);
  }

static msg_hook_t *listener = 0;

result_t subscribe(msg_hook_t *handler)
  {
  if(handler == 0)
    return e_bad_parameter;
  
  enter_critical();
  
  if(listener != 0)
    {
    listener->prev = handler;
    handler->next = listener;
    }
    
  listener = handler;
  
  exit_critical();
  
  return s_ok;
  }

result_t unsubscribe(msg_hook_t *handler)
  {
  enter_critical();
  
  if(handler->prev != 0)
    handler->prev->next = handler->next;
  
  if(handler->next != 0)
    handler->next->prev = handler->prev;
  
  if(handler == listener)
    listener = handler->next;
  
  handler->next = 0;
  handler->prev = 0;
  
  exit_critical();
  
  return s_ok;
  }

static uint8_t ids_reply[4];

static bool publish_ids(const canmsg_t *service_msg, void *parg)
  {
  
  // we only accept a publish ids if the addressed node id is 0 or the node if
  if(service_msg->canas.node_id == 0 ||
     service_msg->canas.node_id == node_id)
    {
    canmsg_t msg;

    ids_reply[0] = hardware_revision;
    ids_reply[1] = software_revision;

    can_send(create_can_msg_uint8_4(&msg, service_msg->id + 1, 0,
                                    ids_reply[0], ids_reply[1],
                                    ids_reply[2], ids_reply[3]));
    
    return true;
    }
  
  return false;
  }

static msg_hook_t ids_hook = { 0, 0, publish_ids, 0 };

static canmsg_t rx_msg;

static msg_hook_t *services[num_services];

result_t register_service(uint8_t service, msg_hook_t *handler)
  {
  if(service == 0 ||
     service >= num_services)
    return e_bad_parameter;
  
  enter_critical();
  
  if(services[service] != 0)
    {
    handler->prev = services[service];
    handler->prev->next = handler;
    }
  
  services[service] = handler;
  
  exit_critical();
  
  return s_ok;
  }

void publish_local(const canmsg_t *msg)
  {
  push_back(can_rx_queue, msg, INDEFINITE_WAIT);
  }

void can_rx_task(void *parg)
  {
  while(true)
    {
    pop_front(can_rx_queue, &rx_msg, INDEFINITE_WAIT);
    
    // call all of the message handlers....
    msg_hook_t *handler;
    for(handler = listener; handler != 0; handler = handler->next)
      (handler->callback)(&rx_msg, handler->parg);

    // handle the builtin services next.  The service channel +1 is the reply channel
    switch(rx_msg.id & 0xfffe)
      {
      case node_service_channel_0:
      case node_service_channel_1:
      case node_service_channel_2:
      case node_service_channel_3:
      case node_service_channel_4:
      case node_service_channel_5:
      case node_service_channel_6:
      case node_service_channel_7:
      case node_service_channel_8:
      case node_service_channel_9:
      case node_service_channel_10:
      case node_service_channel_11:
      case node_service_channel_12:
      case node_service_channel_13:
      case node_service_channel_14:
      case node_service_channel_15:
      case node_service_channel_16:
      case node_service_channel_17:
      case node_service_channel_18:
      case node_service_channel_19:
      case node_service_channel_20:
      case node_service_channel_21:
      case node_service_channel_22:
      case node_service_channel_23:
      case node_service_channel_24:
      case node_service_channel_25:
      case node_service_channel_26:
      case node_service_channel_27:
      case node_service_channel_28:
      case node_service_channel_29:
      case node_service_channel_30:
      case node_service_channel_31:
      case node_service_channel_32:
      case node_service_channel_33:
      case node_service_channel_34:
      case node_service_channel_35:
      case node_service_channel_100 :
      case node_service_channel_101 :
      case node_service_channel_102 :
      case node_service_channel_103 :
      case node_service_channel_104 :
      case node_service_channel_105 :
      case node_service_channel_106 :
      case node_service_channel_107 :
      case node_service_channel_108 :
      case node_service_channel_109 :
      case node_service_channel_110 :
      case node_service_channel_111 :
      case node_service_channel_112 :
      case node_service_channel_113 :
      case node_service_channel_114 :
      case node_service_channel_115 :
        if(rx_msg.canas.service_code < num_services &&
           services[rx_msg.canas.service_code] != 0)
          {
          msg_hook_t *service = services[rx_msg.canas.service_code];
          
          while(service != 0)
            {
            if((*service->callback)(&rx_msg, service->parg))
              break;      // message was handled
            
            service = service->prev;
            }
          }
        break;
      }
    }
  }

extern result_t neutron_init(const neutron_parameters_t *params, bool init_mode);

result_t can_aerospace_init(const neutron_parameters_t *params, bool init_mode)
  {
  handle_t task_id;
  result_t result;

  hardware_revision = params->hardware_revision;
  software_revision = params->software_revision;
  node_id = params->node_id;

  if (failed(result = deque_create(sizeof(canmsg_t),
    params->tx_length == 0 ? 64 : params->tx_length,
    &can_tx_queue)))
    {
    trace_error("Cannot create can_txt_queue");
    return result;
    }

  if (failed(result = deque_create(sizeof(canmsg_t),
    params->rx_length == 0 ? 128 : params->rx_length,
    &can_rx_queue)))
    {
    trace_error("Cannot create can_rx_queue");
    return result;
    }

  if (failed(result = task_create("CAN_TX",
    params->tx_stack_length,
    can_tx_task, 0,
    NORMAL_PRIORITY, &task_id)))
    {
    trace_error("Cannot create the can_tx task");
    return result;
    }

  if (failed(result = task_create("CAN_RX",
    params->rx_stack_length,
    can_rx_task, 0,
    NORMAL_PRIORITY + 1, &task_id)))
    {
    trace_error("Cannot create the can_rx task");
    return result;
    }
  
  if(failed(result = neutron_init(params, init_mode)))
    return result;

  // start the can driver running.
  return bsp_can_init(can_rx_queue);
  }

void send_param_float(float value, uint16_t id)
  {
  canmsg_t msg;
  can_send(create_can_msg_float(&msg, id, 0, value));
  }

void send_param_int16(int16_t value, uint16_t id)
  {
  canmsg_t msg;
  can_send(create_can_msg_int16(&msg, id, 0, value));
  }
