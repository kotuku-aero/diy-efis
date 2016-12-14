#include "win32_hal.h"
#include "../../widgets/can_driver.h"
#include "../../widgets/can_aerospace.h"
#include "comm_device.h"

////////////////////////////////////////////////////////////////////////////////
//
// CAN Driver worker thread
//

inline int tohex(char ch)
  {
  return ch >= '0' && ch <= '9' ? ch - '0' :(tolower(ch) - 'a')+10;
  }

class can_msg_impl_t : public kotuku::msg_t
  {
  public:
    can_msg_impl_t(const std::string &msg);
  };

can_msg_impl_t::can_msg_impl_t(const std::string &msg)
  {
  switch(msg[0])
    {
    case 'T' :
      flags |= EXTENDED_ADDRESS;
      flags &= ~REPLY_MSG;
      break;
    case 'R' :
      flags |= EXTENDED_ADDRESS;
      flags |= REPLY_MSG;
      break;
    case 'r' :
      flags &= ~EXTENDED_ADDRESS;
      flags |= REPLY_MSG;
      break;
    case 't' :
    default :
      flags &= ~EXTENDED_ADDRESS;
      flags &= ~REPLY_MSG;
      break;
    }

  id = 0;
  size_t num_digits = (flags & EXTENDED_ADDRESS) ? 8 : 3;
  size_t index = 1;
  char val;
  int num;
  while(num_digits--)
    {
    if(index >= msg.size())
      {
      id = 0;
      return;
      }
    // todo: range checks?
    val = msg[index++];
    num = tohex(val);
    id = (id  << 4) |(num & 0x0f);
    }

  if(index >= msg.size())
    {
    id = 0;
    return;
    }

  //trace_error("Can ID is %d\n", id);
  flags &= ~DLC_MASK;
  flags |= tohex(msg[index++]) & DLC_MASK;

  //trace_error("Can length is %d\n", len);
  if(flags & REPLY_MSG)
    return;

  for(size_t i = 0; i < (flags & DLC_MASK); i++)
    {
    if(index >= msg.size())
      {
      id = 0;
      return;
      }
    else
      {
      this->msg.raw[i] = tohex(msg[index]) << 4 | tohex(msg[index + 1]);
      index += 2;
      }
    }
  }

class can_worker_t : public kotuku::thread_t {
public:
  can_worker_t(kotuku::serial_can_device_t *);
  ~can_worker_t();

  // Initialize the device.
  result_t init(const std::string &name, uint32_t acceptance, uint32_t mask, int baud_rate);
  // Reset and initialize the device
  result_t reset();

  result_t send_msg(const char *str, size_t len, uint32_t timeout = 200, std::string *reply = 0);
private:
  uint32_t run();
  static uint32_t do_run(void *);

  kotuku::event_t _doorbell;
  kotuku::event_t _response_worker;
  kotuku::serial_can_device_t *_owner;

  kotuku::event_t _write_completion;
  kotuku::event_t _read_completion;

  kotuku::critical_section_t _cs;
  std::deque<std::string> _responses;

  char *ensure_buffer(size_t len);

  enum STATE
    {
    FIRST,
    CANMSG,
    RESPONSE,
    EMPTY,
    };
  STATE _state;
  std::string _rawmsg;
  std::string parse_char(char ch, STATE &is_can_message);

  kotuku::comm_device_t *_comm_port;

  char *_rcv_buffer;
  size_t _rcv_buffer_len;
  };

// Filter mask settings
static const char *canbus_flag_cmd = "F\r";
static const char *canusb_setup_cmd = "S4\r";
static const char *canusb_version_cmd = "V\r";
static const char *canusb_close_cmd = "C\r";
static const char *canusb_open_cmd = "O\r";
static const char *canusb_code_cmd = "M00000000\r";
static const char *canusb_mask_cmd = "mFFFFFFFF\r";
static const char *canusb_send_cmd = "t%03.3x%01.1x";
static const char *canusb_send_cmd_ex = "T%08.8x%01.1x";
static const char *canusb_flush_cmd = "\r";
static const char *canusb_timestamp_off = "Z0\r";

can_worker_t::can_worker_t(kotuku::serial_can_device_t *owner)
  : thread_t(1024, this, do_run),
  _state(FIRST),
  _rcv_buffer(0),
  _rcv_buffer_len(0),
  _owner(owner)
  {
  }

can_worker_t::~can_worker_t()
  {
  delete[] _rcv_buffer;
  _rcv_buffer = 0;
  }

char *can_worker_t::ensure_buffer(size_t len)
  {
  if(_rcv_buffer == 0 || len > _rcv_buffer_len)
    {
    delete[] _rcv_buffer;
    _rcv_buffer_len = max(size_t(32), size_t(((len -1)| 0x1f)+1));
    _rcv_buffer = new char[_rcv_buffer_len];
    }

  return _rcv_buffer;
  }

result_t can_worker_t::init(const std::string &name,
  uint32_t acceptance_code,
  uint32_t acceptance_mask,
  int baud_rate)
  {
  // open the comm port
  _comm_port = new kotuku::comm_device_t("CAN0", name.c_str()); 

  // set rate to 240000 baud
  kotuku::comms_state_ioctl_t ioctl;
  memset(&ioctl, 0, sizeof(kotuku::comms_state_ioctl_t));
  ioctl.baud_rate = (kotuku::baud_rate_t) baud_rate;
  ioctl.parity = kotuku::no_parity;
  ioctl.eof_char = '\r';
  ioctl.byte_size = 8;
  ioctl.ioctl_type = kotuku::comms_state_ioctl;
  ioctl.version = sizeof(kotuku::comms_state_ioctl_t);
  
  result_t result;
  kotuku::device_t::overlapped_t req;
  req.action.ioctl.write_ptr = &ioctl;
  req.action.ioctl.write_size = sizeof(kotuku::comms_state_ioctl_t);
  req.action.ioctl.read_ptr = 0;
  req.action.ioctl.read_size = 0;
  req.action.ioctl.ioctl = kotuku::device_t::set_device_ctl;
  req.operation = kotuku::device_t::overlapped_t::ioctl_opn;
  req.completion = 0;
  req.result = &result;

  _comm_port->device_request(&req);
  if(failed(result))
    return result;


  resume();

  send_msg(canusb_flush_cmd, strlen(canusb_flush_cmd));
  send_msg(canusb_flush_cmd, strlen(canusb_flush_cmd));
  send_msg(canusb_flush_cmd, strlen(canusb_flush_cmd));

  if(failed(result = send_msg(canbus_flag_cmd, strlen(canbus_flag_cmd))))
    {
    trace_error("Cannot send canbus flag command - 0x%08.8x\n", result);
    return result;
    }

  if(failed(result = send_msg(canusb_version_cmd, strlen(canusb_version_cmd))))
    {
    trace_error("Cannot send canbus version command - 0x%08.8x\n", result);
    return result;
    }

  if(failed(result = send_msg(canusb_close_cmd, strlen(canusb_close_cmd))))
    {
    trace_error("Cannot send canbus version command - 0x%08.8x\n", result);
    return result;
    }

  // send a BR command to 250kbit/sec
  if(failed(result = send_msg(canusb_setup_cmd, strlen(canusb_setup_cmd))))
    {
    trace_error("Cannot send canbus setup command - 0x%08.8x\n", result);
    return result;
    }

  if(failed(result = send_msg(canusb_code_cmd, strlen(canusb_code_cmd))))
    {
    trace_error("Cannot send canbus acceptance command - 0x%08.8x\n", result);
    return result;
    }

  if(failed(result = send_msg(canusb_mask_cmd, strlen(canusb_mask_cmd))))
    {
    trace_error("Cannot send canbus mask command - 0x%08.8x\n", result);
    return result;
    }

  if(failed(result = send_msg(canusb_timestamp_off, strlen(canusb_timestamp_off))))
    return result;

  if(failed(result = send_msg(canusb_open_cmd, strlen(canusb_open_cmd))))
    trace_error("Cannot send canbus open command - 0x%08.8x\n", result);

  return result;
  }

result_t can_worker_t::reset()
  {
    
  return s_ok;
  }

uint32_t can_worker_t::do_run(void *obj)
  {
  return reinterpret_cast<can_worker_t *>(obj)->run();
  }

result_t can_worker_t::send_msg(const char *msg, size_t len, uint32_t timeout, std::string *reply)
  {
  //trace_error("Write to canbus %s", msg);
  kotuku::device_t::overlapped_t write_request;
  size_t write_length;
  result_t result = e_timeout_error;
  write_request.operation = kotuku::device_t::overlapped_t::write_opn;
  write_request.action.write.write_ptr = msg;
  write_request.action.write.write_size = len;
  write_request.action.write.xfer_length = &write_length;
  write_request.completion = &_write_completion;
  write_request.result = &result;
  write_request.os_data = 0;

  _comm_port->device_request(&write_request);

  kotuku::event_t::lock_t lock(_write_completion, timeout);

  delete write_request.os_data;
  write_request.os_data = 0;

  if(failed(result))
    return result;

  // see if we have a response
  kotuku::event_t::lock_t wait_for_response(_response_worker, timeout);

  kotuku::critical_section_t::lock_t cs_lock(_cs);

  while(!_responses.empty())
    {
    std::string response = _responses.front();
    _responses.pop_front();

    if(reply != 0)
      *reply = response;
    }

  return s_ok;
  }

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
uint32_t can_worker_t::run()
  {
  while(!should_terminate())
    {
    // When traffic gets heavy, expect data to arrive in random chunks with no regard for the start
    // or end of the CanUsb message.
    // Don’t expect the \r to be the last char we receive.. the string often ends half way
    // through a message.. It could be the next .Read() starts with a /r at the beginning of the string.
    // If a fragment of a CanUsb message is at the end of the string, we can’t throw it away.. it
    // needs to prefixed to the next incoming message otherwise we are going to throw away data.
    // Aswell as this, when we connect to a CanUsb device that is in full flow, dont expect it
    // to start at the beginning of a CanUsb message.. 
    // - This explains some of the complexity of the code below...
    // There is the added headache that an ACK to some messages is only indicated by a \r
    // inserted between the received messages - which are themselves terminated by a \r
    //  "t12300\rt12300\r\rt12300"   - spot the ack to RTR transmit..
    // .. it could arrive like this.. "t12300\rt12300\r" then "\rt12300"
    // To avoid ugly code, I have chosen to parse incoming data byte by byte using a statemachine.
    // The number of times the timestamp has  
    // Block until we receive a signal from the FTDI driver that data has arrived..
    kotuku::event_t::lock_t butler(_doorbell, 100);

    if(should_terminate())
      break;

    size_t chars_expected = 80;

    // Check how much data there is
    while(!should_terminate())
      {
      // Get the data..
      size_t chars_actual = 0;
      char *buffer = ensure_buffer(chars_expected);


      kotuku::device_t::overlapped_t read_request;

      result_t result;
      read_request.operation = kotuku::device_t::overlapped_t::read_opn;
      read_request.action.read.read_ptr = buffer;
      read_request.action.read.read_size = chars_expected;
      read_request.action.read.xfer_length = &chars_actual;
      read_request.completion = &_read_completion;
      read_request.result = &result;

      _comm_port->device_request(&read_request);

      kotuku::event_t::lock_t lock(_read_completion);

      delete read_request.os_data;
      read_request.os_data = 0;

      if(succeeded(result))
        {
        // Convert to stream for easy parsing..
        if(chars_actual > 0)
          {
          // Split into two streams.. Incoming message - and ACKs / NAKs
          std::string str;
          for(size_t i = 0; i < chars_actual; i++)
            {
            char ch = buffer[i];
            // Extract the incoming received CAN messages ('t' or 'T')
            STATE messageType;
            str = parse_char(ch, messageType);
            if(messageType == CANMSG)
              {
              // We have received an async CAN message
              //trace_error("Message from canbus %s\n", str.c_str());
              can_msg_impl_t msg(str);
              _owner->process_msg(msg);
              }
            else if(messageType == RESPONSE)
              {
              // We have and ACK NACK or RSP. Add it to the queue
              // AckNackRspQueue.Enqueue(str);
              // TODO: handle this?
              if(str[0] == '\a')
                str = "(NAK)\r";
              else if(str[0] == '\r')
                str = "(ACK)\r";

              //trace_error("Response from canbus %s\n", str.c_str());
              kotuku::critical_section_t::lock_t lock(_cs);
              _responses.push_back(str);
              _response_worker.set();
              }
            }
          }
        }
      }
    }

  return 0;
  }

std::string can_worker_t::parse_char(char ch, STATE &msg_state)
  {
  std::string retval;
  msg_state = EMPTY;
  ch &= 0x7f;
  // Process char
  switch(_state)
    {
    case FIRST:
      if (ch == '\r')
        {
        msg_state = RESPONSE;
        retval = "\r";
        }
      // If there is an \a, this is taken to be a NACK
      else if (ch == '\a')
        {
        msg_state = RESPONSE;
        retval = "\a";
        }
      // Check for a CAN message received
      else if ((ch == 't') || (ch == 'T') ||
        (ch == 'r') || (ch == 'R'))
        {
        // Clear the string and add the byte..
        _rawmsg.resize(0); // Clear string builder contents
        _rawmsg.append(1, ch);
        // Get the rest of the message
        _state = CANMSG;
        }
      // Check for a response to see if it a response to a messages that may have been sent
      else if ((ch == 'F') || (ch == 'V') || (ch == 'N') ||
        (ch == 'z') || (ch == 'Z'))
        {
        // Clear the string and add the byte..
        _rawmsg.resize(0); // Clear string builder contents
        _rawmsg.append(1, ch);
        // Get the rest of the message
        _state = RESPONSE;
        }
      else
        {
        // Unrecognised char. Idle.
        _state = FIRST;
        }
      break;
    case CANMSG:
      // Append the received char
      if (((ch >= '0') && (ch <= '9')) || ((ch >= 'A') && (ch <= 'F')))
        {
        // The message body
        _rawmsg.append(1, ch);
        }
      else
        {
        _state = FIRST;
        // Not a hex value
        if (ch == '\r')
          {
          // We have reached the end of the message
          msg_state = CANMSG;
          retval = _rawmsg;
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
        _rawmsg.append(1, ch);
        }
      else
        {
        _state = FIRST;
        // Not a hex value
        if (ch == '\r')
          {
          // We have reached the end of the message
          msg_state = RESPONSE;
          retval = _rawmsg;
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
        }
      break;
    }

  return retval;
  }

///////////////////////////////////////////////////////////////////////////////
//
// FTDI CANUSB driver
//
kotuku::serial_can_device_t::serial_can_device_t(const std::string &name, int baud_rate)
  : _provider(0)
  {
  _doorbell = 0;

  // create our worker class
  _worker = new can_worker_t(this);
  _worker->init(name, 0xFFFF, 0XFFFF, baud_rate);
  }

result_t kotuku::serial_can_device_t::reset()
  {
  if(_worker == 0)
    return e_generic_error;

  return _worker->reset();
  }

result_t kotuku::serial_can_device_t::send(const can_msg_t &msg)
  {
  size_t len = msg.flags & DLC_MASK;
  char buffer[64];
  // TODO: make sure this is an extended ID
  sprintf(buffer, (msg.flags & EXTENDED_ADDRESS) ? canusb_send_cmd_ex : canusb_send_cmd, msg.id, len);
  char byte_str[16];
  for(size_t n = 0; n < len; n++)
    {
    sprintf(byte_str, "%02.2x", msg.msg.raw[n]);
    strcat(buffer, byte_str);
    }

  strcat(buffer, "\r");

  //trace_error("CANBUS Write %s\n", buffer);

  result_t result;
  if(failed(result = _worker->send_msg(buffer, strlen(buffer))))
    return result;

  // we now block until we see the fifo empty
  std::string flag_response;
  bool fifo_full;
  int max_tries = 3;
  do
    {
    if(failed(result = _worker->send_msg(canbus_flag_cmd, strlen(canbus_flag_cmd), 500, &flag_response)))
      return result;

    max_tries--;

    if(flag_response.length() != 3 ||
       flag_response[0] != 'F')
      continue;

    uint32_t flags = strtoul(flag_response.c_str() +1, 0, 16);
    fifo_full = (flags & 0x01) != 0;

    } while(fifo_full && max_tries > 0);

  return s_ok;
  }

result_t kotuku::serial_can_device_t::process_msg(const can_msg_t &msg)
  {
  if(_provider == 0)
    return e_generic_error;

  return _provider->receive(msg);
  }

result_t kotuku::serial_can_device_t::set_can_provider(canaerospace_provider_t *provider)
  {
  _provider = provider;

  return s_ok;
  }
