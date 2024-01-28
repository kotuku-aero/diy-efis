#include "neutron.h"
#include <string.h>
// global node id
uint8_t node_id;

result_t create_can_msg_nodata(canmsg_t *msg, uint16_t message_id)
  {
  memset(msg, 0, sizeof(canmsg_t));
  set_can_len(msg, 1);
  set_can_id(msg, message_id);
  msg->data[0] = CANFLY_NODATA;

  return s_ok;
  }

result_t create_can_msg_error(canmsg_t *msg, uint16_t message_id, uint32_t data)
  {
  memset(msg, 0, sizeof(canmsg_t));
  set_can_len(msg, 5);
  set_can_id(msg, message_id);
  msg->data[0] = CANFLY_ERROR;
  msg->data[1] = (uint8_t)(data >> 24);
  msg->data[2] = (uint8_t)(data >> 16);
  msg->data[3] = (uint8_t)(data >> 8);
  msg->data[4] = (uint8_t)data;

  return s_ok;
  }

result_t create_can_msg_float(canmsg_t *msg, uint16_t message_id, float value)
  {
  memset(msg, 0, sizeof(canmsg_t));
  set_can_len(msg, 5);
  set_can_id(msg, message_id);

  uint32_t ulvalue = *((uint32_t *)&value);
  msg->data[0] = CANFLY_FLOAT;
  msg->data[1] = (uint8_t)(ulvalue >> 24);
  msg->data[2] = (uint8_t)(ulvalue >> 16);
  msg->data[3] = (uint8_t)(ulvalue >> 8);
  msg->data[4] = (uint8_t)ulvalue;
  
  return s_ok;
  }
 
result_t create_can_msg_int16(canmsg_t *msg, uint16_t message_id, int16_t data)
  {
  memset(msg, 0, sizeof(canmsg_t));
  set_can_len(msg, 3);
  set_can_id(msg, message_id);
  msg->data[0] = CANFLY_INT16;
  msg->data[1] = (uint8_t)(data >> 8);
  msg->data[2] = (uint8_t)data;
  
  return s_ok;
  }

result_t create_can_msg_int32(canmsg_t *msg, uint16_t message_id, int32_t data)
  {
  memset(msg, 0, sizeof(canmsg_t));
  set_can_len(msg, 5);
  set_can_id(msg, message_id);
  msg->data[0] = CANFLY_INT32;
  msg->data[1] = (uint8_t)(data >> 24);
  msg->data[2] = (uint8_t)(data >> 16);
  msg->data[3] = (uint8_t)(data >> 8);
  msg->data[4] = (uint8_t)data;

  return s_ok;
  }
  
result_t create_can_msg_uint16(canmsg_t *msg, uint16_t message_id, uint16_t data)
  {
  memset(msg, 0, sizeof(canmsg_t));
  set_can_len(msg, 3);
  set_can_id(msg,  message_id);
  msg->data[0] = CANFLY_UINT16;
  msg->data[1] = (uint8_t)(data >> 8);
  msg->data[2] = (uint8_t)data;
  
  return s_ok;
  }

result_t create_can_msg_uint32(canmsg_t *msg, uint16_t message_id, uint32_t data)
  {
  memset(msg, 0, sizeof(canmsg_t));
  set_can_len(msg, 5);
  set_can_id(msg, message_id);
  msg->data[0] = CANFLY_UINT32;
  msg->data[1] = (uint8_t)(data >> 24);
  msg->data[2] = (uint8_t)(data >> 16);
  msg->data[3] = (uint8_t)(data >> 8);
  msg->data[4] = (uint8_t)data;

  return s_ok;
  }

result_t create_can_msg_bool(canmsg_t *msg, uint16_t message_id, bool value)
  {
  memset(msg, 0, sizeof(canmsg_t));
  set_can_len(msg, 1);
  set_can_id(msg, message_id);
  msg->data[0] = value ? CANFLY_BOOL_TRUE : CANFLY_BOOL_FALSE;

  return s_ok;
  }

result_t create_can_msg_int8(canmsg_t *msg, uint16_t message_id, int8_t value)
  {
  memset(msg, 0, sizeof(canmsg_t));
  set_can_len(msg, 2);
  set_can_id(msg, message_id);
  msg->data[0] = CANFLY_INT8;
  msg->data[1] = (uint8_t)value;

  return s_ok;
  }

result_t create_can_msg_uint8(canmsg_t *msg, uint16_t message_id, uint8_t value)
  {
  memset(msg, 0, sizeof(canmsg_t));
  set_can_len(msg, 2);
  set_can_id(msg, message_id);

  msg->data[0] = CANFLY_UINT8;
  msg->data[1] = value;

  return s_ok;
  }  

result_t create_can_msg_utc(canmsg_t *msg, uint16_t message_id, const tm_t *value)
  {
  set_can_len(msg, 8);
  set_can_id(msg, message_id);
  
  msg->data[0] = CANFLY_UTC;
  msg->data[1] = (uint8_t)(value->year >> 8);
  msg->data[2] = (uint8_t)value->year;
  msg->data[3] = (uint8_t)value->month;
  msg->data[4] = (uint8_t)value->day;
  msg->data[5] = (uint8_t)value->hour;
  msg->data[6] = (uint8_t)value->minute;
  msg->data[7] = (uint8_t)value->second;

  return s_ok;
  }

result_t get_param_float(const canmsg_t *msg, float *v)
  {
  if (msg == 0 || v == 0)
    return e_bad_parameter;

  variant_t var;

  result_t result;
  if (failed(result = msg_to_variant(msg, &var)))
    return result;

  return coerce_to_float(&var, v);
  }

result_t get_param_bool(const canmsg_t *msg, bool *v)
  {
  if (msg == 0 || v == 0)
    return e_bad_parameter;

  variant_t var;

  result_t result;
  if (failed(result = msg_to_variant(msg, &var)))
    return result;

  return coerce_to_bool(&var, v);
  }

result_t get_param_int8(const canmsg_t *msg, int8_t *v)
  {
  if (msg == 0 || v == 0)
    return e_bad_parameter;

  variant_t var;

  result_t result;
  if (failed(result = msg_to_variant(msg, &var)))
    return result;

  return coerce_to_int8(&var, v);
  }

result_t get_param_uint8(const canmsg_t *msg, uint8_t *v)
  {
  if (msg == 0 || v == 0)
    return e_bad_parameter;

  variant_t var;

  result_t result;
  if (failed(result = msg_to_variant(msg, &var)))
    return result;

  return coerce_to_uint8(&var, v);
  }

result_t get_param_int16(const canmsg_t *msg, int16_t *v)
  {
  if (msg == 0 || v == 0)
    return e_bad_parameter;

  variant_t var;

  result_t result;
  if (failed(result = msg_to_variant(msg, &var)))
    return result;

  return coerce_to_int16(&var, v);
  }

result_t get_param_uint16(const canmsg_t *msg, uint16_t *v)
  {
  if (msg == 0 || v == 0)
    return e_bad_parameter;

  variant_t var;

  result_t result;
  if (failed(result = msg_to_variant(msg, &var)))
    return result;

  return coerce_to_uint16(&var, v);
  }

result_t get_param_int32(const canmsg_t *msg, int32_t *v)
  {
  if (msg == 0 || v == 0)
    return e_bad_parameter;

  variant_t var;

  result_t result;
  if (failed(result = msg_to_variant(msg, &var)))
    return result;

  return coerce_to_int32(&var, v);
  }

result_t get_param_uint32(const canmsg_t *msg, uint32_t *v)
  {
  if (msg == 0 || v == 0)
    return e_bad_parameter;

  variant_t var;

  result_t result;
  if (failed(result = msg_to_variant(msg, &var)))
    return result;

  return coerce_to_uint32(&var, v);
  }

result_t get_param_utc(const canmsg_t *msg, tm_t *value)
  {
  if (msg == 0 || value == 0 || get_can_len(msg) != 8)
    return e_bad_parameter;

  value->year = msg->data[1] * 256;
  value->year += msg->data[2];
  value->month = msg->data[3];
  value->day = msg->data[4];
  value->hour = msg->data[5];
  value->minute = msg->data[6];
  value->second = msg->data[7];
  value->milliseconds = 0;
  
  return s_ok;
  }

static handle_t can_tx_queue;
static handle_t can_rx_queue;

/**
 * Worker task to queue messages
 * @param parg
 */
    
void can_tx_task(void *parg)
  {
  canmsg_t tx_msg;
  while(true)
    {
    // block till a message is ready
    pop_front(can_tx_queue, &tx_msg, INDEFINITE_WAIT);
    bsp_send_can(&tx_msg);
    }
  }

result_t can_send(const canmsg_t *msg, uint32_t timeout)
  {
  if(msg == 0 ||
     get_can_len(msg) == 0)
    return e_bad_parameter;

  return push_back(can_tx_queue, msg, timeout);
  }

/**
 * @struct msg_hook_t
 * @param next    next hook handler, or 0
 * @param prev    previous hook handler, or 0
 * @param callback  Function to be called when the service is called
 */
typedef struct _msg_hook_t
  {
  base_t base;
  struct _msg_hook_t *next;
  struct _msg_hook_t *prev;
  msg_hook_fn callback;
  void *parg;
  } msg_hook_t;

static const typeid_t msg_hook_type = { "msg_hook_t" };

static msg_hook_t *listener = 0;

result_t subscribe(msg_hook_fn cb, void *parg, handle_t *hndl)
  {
  if(cb == 0)
    return e_bad_parameter;

  msg_hook_t *handler;
  result_t result;
  if (failed(result = neutron_malloc(sizeof(msg_hook_t), (void **)&handler)))
    return result;

  memset(handler, 0, sizeof(msg_hook_t));

  handler->base.type = &msg_hook_type;
  handler->callback = cb;
  handler->parg = parg;

  if (hndl != 0)
    *hndl = (handle_t)handler;
  
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

result_t unsubscribe(handle_t hndl)
  {
  result_t result;
  msg_hook_t *handler;

  if (failed(result = is_typeof(hndl, &msg_hook_type, (void **)&handler)))
    return result;

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

  memset(&handler, 0, sizeof(msg_hook_t));
  neutron_free(handler);
  
  return s_ok;
  }

result_t publish_local(const canmsg_t *msg, uint32_t timeout)
  {
  return push_back(can_rx_queue, msg, timeout);
  }

static set_node_id_fn set_id = 0;

void can_rx_task(void *parg)
  {
  canmsg_t rx_msg;
  while (true)
    {
    pop_front(can_rx_queue, &rx_msg, INDEFINITE_WAIT);

    if(get_can_id(&rx_msg) == id_set_node_id)
      {
      // check to see if the serial number of the node matches the
      // set-id message
      // structure of the message is at neutron.h line 148
      if(rx_msg.data[0] != CANFLY_BINARY ||
         get_can_len(&rx_msg) != 8 ||
         rx_msg.data[5] != node_id ||
         set_id == 0)       // can't set the id
        continue;     // ignore this
      
      uint32_t set_serial = (((uint32_t)rx_msg.data[1]) << 24) |
        (((uint32_t)rx_msg.data[2]) << 16) |
        (((uint32_t)rx_msg.data[3]) << 8) |
        ((uint32_t)rx_msg.data[4]);
      
        // get our serial number
      uint32_t serial_number;
      bsp_cpu_id(&serial_number, 0, 0);
      
      // if not this serial then ignore
      if(serial_number != set_serial)
        continue;

      // update the node id
      node_id = rx_msg.data[6];
      
      (*set_id)(rx_msg.data[6], rx_msg.data[7]);
      }
    // handle the services next.  The service channel +1 is the reply channel
    else if(failed(check_pipe_msg(&rx_msg)))
      {
      // call all of the message handlers....
      msg_hook_t *handler;
      for (handler = listener; handler != 0; handler = handler->next)
        (handler->callback)(&rx_msg, handler->parg);
      }
    }
  }

result_t canfly_init(const neutron_parameters_t *params, bool create_publish_task)
  {
  handle_t task_handle;
  result_t result;

  if (failed(result = pipe_init(params)))
    return result;

  node_id = params->node_id;
  set_id = params->set_id;

  if (failed(result = deque_create(sizeof(canmsg_t),params->tx_length, &can_tx_queue)))
    {
    trace_error("Cannot create can_txt_queue");
    return result;
    }

  if (failed(result = deque_create(sizeof(canmsg_t), params->rx_length, &can_rx_queue)))
    {
    trace_error("Cannot create can_rx_queue");
    return result;
    }

  if (failed(result = task_create("CAN_TX",
    params->tx_stack_length,
    can_tx_task, 0,
    NORMAL_PRIORITY, &task_handle)))
    {
    trace_error("Cannot create the can_tx task");
    return result;
    }

  if (failed(result = task_create("CAN_RX",
    params->rx_stack_length,
    can_rx_task, 0,
    NORMAL_PRIORITY + 1, &task_handle)))
    {
    trace_error("Cannot create the can_rx task");
    return result;
    }
  
  if(failed(result = neutron_init(params, create_publish_task)))
    return result;

  // start the can driver running.
  return bsp_can_init(can_rx_queue, params);
  }

result_t create_can_msg_status(canmsg_t *msg,
                               uint8_t node_id,
                               uint8_t node_type,
                               e_board_status status)
  {
  // get our serial number
  uint32_t serial_number;
  bsp_cpu_id(&serial_number, 0, 0);
  
  memset(msg, 0, sizeof(canmsg_t));
  set_can_len(msg, 8);
  set_can_id(msg, id_status_node_0 + node_id);
  msg->data[0] = CANFLY_BINARY;
  msg->data[1] = node_id;
  msg->data[2] = (uint8_t)status;
  msg->data[3] = node_type;
  msg->data[4] = (uint8_t)(serial_number >> 24);
  msg->data[5] = (uint8_t)(serial_number >> 16);
  msg->data[6] = (uint8_t)(serial_number >> 8);
  msg->data[7] = (uint8_t)(serial_number);

  return s_ok;
  }

result_t get_param_status(const canmsg_t *msg,
                          uint8_t *node_id,
                          uint8_t *node_type,
                          uint32_t *serial_number,
                          e_board_status *status)
  {
  if(get_can_id(msg) < id_status_node_0 ||
     get_can_id(msg) > id_status_node_15 ||
     get_can_len(msg) != 8 ||
     get_can_type(msg) != CANFLY_BINARY)
    return e_bad_parameter;

  if(node_id != 0)
    *node_id = msg->data[1];
  
  if(status != 0)
    *status = (e_board_status) msg->data[2];

  if(node_type != 0)
    *node_type = msg->data[3];
  
  if(serial_number != 0)
    *serial_number = 
    (((uint32_t)msg->data[4]) << 24) |
    (((uint32_t)msg->data[5]) << 16) | 
    (((uint32_t)msg->data[6]) << 8) | 
    ((uint32_t)msg->data[7]);
  
  
  return s_ok;
  }
