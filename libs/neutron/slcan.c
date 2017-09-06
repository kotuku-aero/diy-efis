#include "slcan.h"

// Filter mask settings
static const char *canbus_flag_cmd = "F\r";
static const char *canusb_setup_cmd = "S4\r";
static const char *canusb_version_cmd = "V\r";
static const char *canusb_close_cmd = "C\r";
static const char *canusb_open_cmd = "O\r";
static const char *canusb_code_cmd = "M00000000\r";
static const char *canusb_mask_cmd = "mFFFFFFFF\r";
static const char *canusb_send_cmd = "t%03.3x%01.1x";
//static const char *canusb_send_cmd_ex = "T%08.8x%01.1x";
static const char *canusb_flush_cmd = "\r";
static const char *canusb_timestamp_off = "Z0\r";

#define RX_QUEUE_LEN  128
#define MAX_MSG_LEN 64
#define NUM_RESPONSE_MSG  16

typedef enum _STATE
  {
  FIRST,
  CANMSG,
  RESPONSE,
  EMPTY,
  } STATE;

typedef struct _slcan_driver_t {
  size_t version;
  // these are the can messages decoded
  handle_t rx_can_queue;
  // message responses are queued here.
  handle_t response_queue;
  // this holds pointers to response messages.  After a message is read it
  // must be pushed back onto this queue
  handle_t response_buffers;
  // this is the serial decoder task
  handle_t rx_task;
  // max delay for send or rx
  uint32_t timeout;
  // serial driver
  handle_t comm_device;
  // buffer to prepare messages in
  char send_buffer[MAX_MSG_LEN];
  // buffer to prepare reply messages in
  char reply_buffer[MAX_MSG_LEN];
  // current read state
  STATE state;
  uint16_t offs;
  char rawmsg[MAX_MSG_LEN +1];
  // read buffer
  char read_buffer[MAX_MSG_LEN + 1];
  // message created here
  char str[MAX_MSG_LEN + 1];
} slcan_driver_t;

static slcan_driver_t *driver;

static void slcan_worker(void *);
// send a message and optionally get a reply
static result_t slcan_send_buffer(slcan_driver_t *driver,
    const char *buffer, uint32_t max_wait, uint16_t reply_len, char *reply);

static result_t verify_slcan(handle_t hndl)
  {
  if(hndl == 0 ||
      ((slcan_driver_t *)hndl)->version != sizeof(slcan_driver_t))
    return e_bad_handle;

  return s_ok;
  }

result_t slcan_create(memid_t key, handle_t msg_rx_queue, handle_t *hndl)
  {
  result_t result;
  int i;

  driver = (slcan_driver_t *)malloc(sizeof(slcan_driver_t));
  memset(driver, 0, sizeof(slcan_driver_t));

  driver->version = sizeof(slcan_driver_t);

  *hndl = driver;

  // store where the can messages are sent to
  driver->rx_can_queue = msg_rx_queue;

  // create the response buffers
  if(failed(result = deque_create(sizeof(char *), NUM_RESPONSE_MSG, &driver->response_queue)) ||
     failed(result = deque_create(sizeof(char *), NUM_RESPONSE_MSG, &driver->response_buffers)))
    return result;

  // queue up a series of response buffers
  for(i = 0; i < NUM_RESPONSE_MSG; i++)
    {
    void *buf = malloc(MAX_MSG_LEN+1);
    if(buf != 0)
    	push_back(driver->response_buffers, &buf, 0);
    }

  if(failed(result = reg_get_uint32(key, "timeout", &driver->timeout)))
    driver->timeout = 500;


  if(failed(result = comm_create_device(key, &driver->comm_device)))
    {
    slcan_close(driver);
    return result;
    }

  // create the worker
  if(failed(result = task_create("SLCAN", DEFAULT_STACK_SIZE, slcan_worker,
      driver, NORMAL_PRIORITY, &driver->rx_task)) ||
      failed(result = task_resume(driver->rx_task)))
    return result;

  slcan_send_buffer(driver, canusb_flush_cmd, driver->timeout, 0, 0);
  slcan_send_buffer(driver, canusb_flush_cmd, driver->timeout, 0, 0);
  slcan_send_buffer(driver, canusb_flush_cmd, driver->timeout, 0, 0);

  slcan_send_buffer(driver, canbus_flag_cmd, driver->timeout, 0, 0);

  if(failed(result = slcan_send_buffer(driver, canusb_close_cmd, driver->timeout, 0, 0)))
    {
    trace_error("Cannot send canbus close command - 0x%08.8x\n", result);
    //return result;
    }

  if(failed(result = slcan_send_buffer(driver, canusb_version_cmd, driver->timeout, 0, 0)))
    {
    trace_error("Cannot send canbus version command - 0x%08.8x\n", result);
    //return result;
    }

  // send a BR command to 250kbit/sec
  if(failed(result = slcan_send_buffer(driver, canusb_setup_cmd, driver->timeout, 0, 0)))
    {
    trace_error("Cannot send canbus setup command - 0x%08.8x\n", result);
    //return result;
    }

  if(failed(result = slcan_send_buffer(driver, canusb_code_cmd, driver->timeout, 0, 0)))
    {
    trace_error("Cannot send canbus acceptance command - 0x%08.8x\n", result);
    //return result;
    }

  if(failed(result = slcan_send_buffer(driver, canusb_mask_cmd, driver->timeout, 0, 0)))
    {
    trace_error("Cannot send canbus mask command - 0x%08.8x\n", result);
    //return result;
    }

  if(failed(result = slcan_send_buffer(driver, canusb_timestamp_off, driver->timeout, 0, 0)))
    {
    trace_error("Cannot send canbus timestamp off command - 0x%08.8x\n", result);
    //return result;
    }

  if(failed(result = slcan_send_buffer(driver, canusb_open_cmd, driver->timeout, 0, 0)))
    trace_error("Cannot send canbus open command - 0x%08.8x\n", result);

  return s_ok;
  }

result_t slcan_close(handle_t hndl)
  {
  result_t result;
  if(failed(result = verify_slcan(hndl)))
    return result;

  slcan_driver_t *driver = (slcan_driver_t *)hndl;

  free(driver);
  return s_ok;
  }

result_t slcan_send(handle_t hndl, const canmsg_t *msg)
  {
  result_t result;
  if(failed(result = verify_slcan(hndl)))
    return result;

  slcan_driver_t *driver = (slcan_driver_t *)hndl;

  uint16_t len = msg->length;

  // TODO: make sure this is an extended ID
  sprintf(driver->send_buffer, canusb_send_cmd, msg->id, len);
  char byte_str[16];

  uint16_t n;
  for(n = 0; n < len; n++)
    {
    sprintf(byte_str, "%2.2x", msg->raw[n]);
    strcat(driver->send_buffer, byte_str);
    }

  strcat(driver->send_buffer, "\r");

  //trace_error("CANBUS Write %s\n", driver->send_buffer);

  if(failed(result = slcan_send_buffer(driver, driver->send_buffer, 0, 0, 0)))
    return result;

  // we now block until we see the fifo empty
  bool fifo_full;
  int max_tries = 3;
  do
    {
    if(failed(result = slcan_send_buffer(driver, canbus_flag_cmd, 500, MAX_MSG_LEN, driver->reply_buffer)))
      return result;

    max_tries--;

    if(strlen(driver->reply_buffer) != 3 ||
        driver->reply_buffer[0] != 'F')
      continue;

    uint32_t flags = strtoul(driver->reply_buffer +1, 0, 16);
    fifo_full = (flags & 0x01) != 0;

    } while(fifo_full && max_tries > 0);

  return s_ok;

  }

static void parse_char(slcan_driver_t *driver, char ch, STATE *msg_state, uint16_t len, char *retval)
  {
  *msg_state = EMPTY;
  ch &= 0x7f;
  // Process char
  switch(driver->state)
    {
    case EMPTY :
      return;
    case FIRST:
      if (ch == '\r')
        {
        *msg_state = RESPONSE;
        strncpy(retval, "\r", len);
        }
      // If there is an \a, this is taken to be a NACK
      else if (ch == '\a')
        {
        *msg_state = RESPONSE;
        strncpy(retval, "\a", len);
        }
      // Check for a CAN message received
      else if ((ch == 't') || (ch == 'T') ||
        (ch == 'r') || (ch == 'R'))
        {
        // Clear the string and add the byte..
        driver->offs = 0;
        driver->rawmsg[driver->offs++] = ch;
        driver->rawmsg[driver->offs] = 0;
        // Get the rest of the message
        driver->state = CANMSG;
        }
      // Check for a response to see if it a response to a messages that may have been sent
      else if ((ch == 'F') || (ch == 'V') || (ch == 'N') ||
        (ch == 'z') || (ch == 'Z'))
        {
        // Clear the string and add the byte..
        driver->offs = 0;
        driver->rawmsg[driver->offs++] = ch;
        driver->rawmsg[driver->offs] = 0;
        // Get the rest of the message
        driver->state = RESPONSE;
        }
      else
        {
        // Unrecognised char. Idle.
        driver->state = FIRST;
        }
      break;
    case CANMSG:
      // Append the received char
      if (((ch >= '0') && (ch <= '9')) || ((ch >= 'A') && (ch <= 'F')))
        {
        // The message body
        driver->rawmsg[driver->offs++] = ch;
        driver->rawmsg[driver->offs] = 0;
        }
      else
        {
        driver->state = FIRST;
        // Not a hex value
        if (ch == '\r')
          {
          // We have reached the end of the message
          *msg_state = CANMSG;
          strncpy(retval, driver->rawmsg, len);
          }
        else if (ch == '\a')
          {
          //Console.WriteLine("Unexpected NACK Rxved during reception of CAN Message" + ch.ToString());
          }
        else
          {
          // We have received an invalid char Exit
          //Console.WriteLine("Invalid char Rxved during reception of CAN Message" + ch.ToString());
          }
        }
      break;
    case RESPONSE:
      // Append the received char
      if ((isalnum(ch)) || (ch == '*'))
        {
        // The message body
        driver->rawmsg[driver->offs++] = ch;
        driver->rawmsg[driver->offs] = 0;
        }
      else
        {
        driver->state = FIRST;
        // Not a hex value
        if (ch == '\r')
          {
          // We have reached the end of the message
          *msg_state = RESPONSE;
          strncpy(retval, driver->rawmsg, len);
          }
        else if (ch == '\a')
          {
          //Console.WriteLine("Unexpected NACK Rxved during reception of Response" + ch.ToString());
          }
        else
          {
          // We have received an invalid char Exit
         // Console.WriteLine("Invalid char Rxved during reception of Response" + ch.ToString());
          }
        driver->offs = 0;
        driver->rawmsg[0] = 0;
        }
      break;
    }

  if(driver->offs >= MAX_MSG_LEN)
    {
    driver->state = FIRST;
    driver->offs = 0;
    driver->rawmsg[0] = 0;
    }
  }


static inline int tohex(char ch)
  {
  return ch >= '0' && ch <= '9' ? ch - '0' :(tolower(ch) - 'a')+10;
  }

/**
 * Worker process
 * @param parg handle to the can message queue
 */
static void slcan_worker(void *parg)
  {
  do
    {
    // read a buffer of characters from the message
    uint16_t chars_actual = 0;
    if(failed(comm_read(driver->comm_device, (byte_t *)driver->read_buffer,
        MAX_MSG_LEN, &chars_actual, driver->timeout)))
      continue;

    // get the pointer...
    char *rx_msg = driver->read_buffer;
    /*
         How it works..
         * RECEIVED
         * ========
         * The FTDI DLL unblocks the parserTask (BackgroundWorker) when data arrives.
         * When unblocked the parserTask dequeues the string data from the FTDI driver. The string is parsed byte by byte to reassemble fragmented messages.
         * Messages are split into 'streams'. These streams are:
         *  a) RECEIVED (asynchronous) are converted into a CANMsg datatype and placed on a queue. An msgRxvd_Semaphore is given as notification.
         *  b) RESPONSE messages are written to a string. A RspRxcd_Semaphore is given as notification.
         *
         * TRANSMITTED
         * ===========
         * Sending will block the caller until the response is received.
         *
         * Here is quick reference of message strings:
         * SENT COMMANDS and their RESPONSES
         *  Sn\r, O\r, C\r, Miiiiiiii\r, miiiiiiii\r, Zn\r -> \r or \b  (Baud, Open, Close, Acceptance Code, Acceptance Mask, Timestamps)
         *  tiiin[dd...dd]\r, Tiiiiiiiin[dd..ddvvv]\r        -> Z\r or \b (Transmit standard, Transmit extended)
         *  riiin\r,          Riiiiiiiin\r                -> Z\r or \b (Transmit standard RTR, Transmit extended RTR)
         *  F\r -> Fxx\r   (STATUS{b0:RX Q Full, b1:TX Q Full, b2:ErrWrn, b3:DatOvRn, b4:na, b5:ErrPassiv, b6:ArbLost, b7:BussErr})
         *  V\r -> Vxx\r   (Version: 4 x nibbles in BCD)
         *  N\r -> Nxxxx\r (Serial Number)
         * RECEIVED (ansynchronous)
         * tiiin[dd...dd]\r, Tiiiiiiiin[dd..dd]\r  (Received standard, Received extended)
         *
         * // Other messages seen on the CANBUS -> But NOT documented are:
         * When reading back the serial number..
         * N\r -> N****\r   (Serial number could not be read? Dont know why. Changed parser to accept '*' chars)
         */
    // When traffic gets heavy, expect data to arrive in random chunks with no regard for the start
    // or end of the CanUsb message.
    // Don’t expect the \r to be the last char we receive.. the string often ends half way
    // through a message.. It could be the next .Read() starts with a /r at the beginning of the string.
    // If a fragment of a CanUsb message is at the end of the string, we can’t throw it away.. it
    // needs to prefixed to the next incoming message otherwise we are going to throw away data.
    // As well as this, when we connect to a CanUsb device that is in full flow, dont expect it
    // to start at the beginning of a CanUsb message..
    // - This explains some of the complexity of the code below...
    // There is the added headache that an ACK to some messages is only indicated by a \r
    // inserted between the received messages - which are themselves terminated by a \r
    //  "t12300\rt12300\r\rt12300"   - spot the ack to RTR transmit..
    // .. it could arrive like this.. "t12300\rt12300\r" then "\rt12300"
    // To avoid ugly code, I have chosen to parse incoming data byte by byte using a statemachine.
    // The number of times the timestamp has
    // Block until we receive a signal from the FTDI driver that data has arrived..

    if(chars_actual > 0)
      {
      rx_msg[chars_actual] = 0;
      //trace_debug("CAN Read %s\n", rx_msg);
      uint16_t i;
      // Split into two streams.. Incoming message - and ACKs / NAKs
      for(i = 0; i < chars_actual; i++)
        {
        char ch = rx_msg[i];
        // Extract the incoming received CAN messages ('t' or 'T')
        STATE messageType;
        parse_char(driver, ch, &messageType, MAX_MSG_LEN, driver->str);
        if(messageType == CANMSG)
          {
          // We have received an async CAN message
          //trace_error("Message from canbus %s\n", str);
          canmsg_t msg;
          memset(&msg, 0, sizeof(canmsg_t));
          switch(driver->str[0])
            {
            case 'T' :
            case 'R' :
              continue;
            case 'r' :
              msg.reply = 1;
              break;
            case 't' :
            default :
              break;
            }

          uint16_t msg_len = strlen(driver->str);

          uint16_t num_digits = 3;
          uint16_t index = 1;
          char val;
          int num;
          while(num_digits--)
            {
            if(index >= msg_len)
              {
              msg.id = 0;
              break;
              }
            // todo: range checks?
            val = driver->str[index++];
            num = tohex(val);
            msg.id = (msg.id  << 4) |(num & 0x0f);
            }

          if(index >= msg_len)
            continue;

          //trace_error("Can ID is %d\n", msg.id);
          msg.length = tohex(driver->str[index++]);

          //trace_error("Can length is %d\n", len);
          if(msg.reply)
            continue;

          uint16_t dp;
          for(dp = 0; dp < msg.length; dp++)
            {
            if(index >= msg_len)
              {
              msg.id = 0;
              break;
              }
            else
              {
              msg.raw[dp] = tohex(driver->str[index]) << 4 | tohex(driver->str[index + 1]);
              index += 2;
              }
            }

          // send the message
          if(msg.id != 0)
            {
//#define _DEBUG_SLCAN
#ifdef _DEBUG_SLCAN
        	  int i;
            trace_debug("SLCAN t%03.3x%01.1x", msg.id, msg.length);
            for(i = 0; i < msg.length; i++)
              trace_debug("%03.2x", msg.raw[i]);
            trace_debug("\n");
#endif

            push_back(driver->rx_can_queue, &msg, 0);
            }
          }
        else if(messageType == RESPONSE)
          {
/*
          char *response;
          if(succeeded(pop_front(driver->response_buffers, (void **)&response, 0)))
            {
            //trace_error("Response from canbus %s\n", str);
            // We have and ACK NACK or RSP. Add it to the queue
            // AckNackRspQueue.Enqueue(str);
            // TODO: handle this?
            if(driver->str[0] == '\a')
              strcpy(response, "(NAK)\r");
            else if(driver->str[0] == '\r')
              strcpy(response, "(ACK)\r");

            // the queues are the same size so if we have a response buffer
            // then it must fit
            push_back(driver->response_queue, &response, 0);
            }
          else
            {
            trace_error("No response buffers so discarded\n");
            } */
          }
        }
      }

    // we have processed the message ok
    }while(true);
  }

result_t slcan_send_buffer(slcan_driver_t *driver,
                           const char *buffer,
                           uint32_t max_wait,
                           uint16_t reply_len,
                           char *reply)
  {
  result_t result;

  if(failed(result = comm_write(driver->comm_device, (const byte_t *) buffer, strlen(buffer), max_wait)))
    return result;
/*
  char *response_msg;
    // see if we have a response
  if(failed(result = pop_front(driver->response_queue, &response_msg, max_wait)))
    return result;

  if(reply != 0 && reply_len != 0)
    strncpy(reply, response_msg, reply_len);

  if(failed(push_back(driver->response_buffers, &response_msg, 0)))
    {
    trace_error("No space when returning a response buffer!!!");
    }*/
  return s_ok;
  }
