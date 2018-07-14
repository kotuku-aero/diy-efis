#include "../../libs/neutron/neutron.h"
#include "../../libs/atom/eeprom.h"
#include "../../libs/atom/input_capture_devices.h"
#include "../../libs/atom16/pps_maps.h"
#include "../../libs/atom/i2c.h"

#include <ctype.h>

// DSPIC33EP512GP502 Configuration Bit Settings

#include <xc.h>

// FICD
#pragma config ICS = PGD1               // ICD Communication Channel Select bits (Communicate on PGEC1 and PGED1)
#pragma config JTAGEN = OFF             // JTAG Enable bit (JTAG is disabled)

// FPOR
#pragma config ALTI2C1 = OFF            // Alternate I2C1 pins (I2C1 mapped to SDA1/SCL1 pins)
#pragma config ALTI2C2 = ON             // Alternate I2C2 pins (I2C2 mapped to SDA2/SCL2 pins)
#pragma config WDTWIN = WIN25           // Watchdog Window Select bits (WDT Window is 25% of WDT period)

// FWDT
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler bits (1:32,768)
#pragma config WDTPRE = PR128           // Watchdog Timer Prescaler bit (1:128)
#pragma config PLLKEN = ON              // PLL Lock Enable bit (Clock switch to PLL source will wait until the PLL lock signal is valid.)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (Watchdog Timer in Non-Window mode)
#ifdef _DEBUG
#pragma config FWDTEN = OFF              // Watchdog Timer Enable bit
#else
#pragma config FWDTEN = ON              // Watchdog Timer Enable bit (Watchdog timer always enabled)
#endif

// FOSC
#pragma config POSCMD = XT              // Primary Oscillator Mode Select bits (XT Crystal Oscillator Mode)
#pragma config OSCIOFNC = OFF           // OSC2 Pin Function bit (OSC2 is clock output)
#pragma config IOL1WAY = OFF            // Peripheral pin select configuration (Allow multiple reconfigurations)
#pragma config FCKSM = CSDCMD           // Clock Switching Mode bits (Both Clock switching and Fail-safe Clock Monitor are disabled)

// FOSCSEL
#pragma config FNOSC = PRIPLL           // Oscillator Source Selection (Primary Oscillator with PLL module (XT + PLL, HS + PLL, EC + PLL))
#pragma config IESO = ON                // Two-speed Oscillator Start-up Enable bit (Start up device with FRC, then switch to user-selected oscillator source)

// FGS
#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#pragma config GCP = OFF                // General Segment Code-Protect bit (General Segment Code protect is Disabled)

int main(void);

uint32_t fcy = 20000;

/**
 * Receive a message from the can bus and convert to a serial packet
 * @param 
 */
static bool process_can(const canmsg_t *, void *);
#define TX_BUFFER_LENGTH 32

// buffers of TX_
#define TX_MSG_QUEUE_LEN 32

static deque_p tx_msg_queue;

#define HARDWARE_REVISION 31
#define SOFTWARE_REVISION 1
  
#define CAN_TX_LENGTH 64
#define CAN_RX_LENGTH 32

#define RX_BUFFER_LENGTH 128

#define BELL 7
#define CR 13
#define LR 10

static uint8_t timestamping = 0;
static bool overflow_warning = false;

typedef enum _can_state_t
  {
  can_closed,
  can_open,
  can_loopback,
  can_listen,
  can_terminal
  } can_state_t;
  
static can_state_t can_state = can_closed;
static uint32_t terminal_ticks;
static uint8_t terminal_node_id;

#define I_PLLPRE 0
#define I_PLLPOST 0
#define I_PLLDIV ((((FCY * 2)/I_OSC)*(I_PLLPRE+2)*(2*(I_PLLPOST+1)))-2)

// characters to be sent to the usb uart
static deque_p ft201_tx_queue;
// tx semaphore set
static semaphore_p ft201_tx_empty;
// rx semaphore set
static semaphore_p ft201_rx_ready;

static char *write_nibble(char *ch, uint8_t value)
  {
  value = value & 0x0f;
  if (value > 9)
    value = value - 10 + 'A';
  else
    value = value + '0';

  *ch++ = value;
  return ch;
  }

/**
 * Send given byte value as hexadecimal string
 *
 * @param value Byte value to send over UART
 */
static char *write_hex(char *ch, uint8_t value)
  {
  ch = write_nibble(ch, value >> 4);
  ch = write_nibble(ch, value);
  
  return ch;
  }

static inline void write_ft201(char ch)
  {
  push_back(ft201_tx_queue, &ch, INDEFINITE_WAIT);
  }

static void write_str(const char *str)
  {
  while(*str != 0)
    write_ft201(*str++);
  }

void panic()
  {
  // restart the system using the brown-out
  __builtin_disi(0x3FFF); /* disable interrupts */
  asm volatile("RESET");
  }

void malloc_failed_hook(uint16_t size)
  {
  panic();
  }


/**
 * Parse hex value of given string
 *
 * @param line Input string
 * @param len Count of characters to interpret
 * @param value Pointer to variable for the resulting decoded value
 * @return 0 on error, 1 on success
 */
static uint8_t parse_hex(const char *line, uint8_t len, unsigned long * value)
  {
  *value = 0;
  while (len--)
    {
    if (*line == 0)
      return 0;
    *value <<= 4;
    if ((*line >= '0') && (*line <= '9'))
      *value += *line - '0';
    else if ((*line >= 'A') && (*line <= 'F'))
      *value += *line - 'A' + 10;
    else if ((*line >= 'a') && (*line <= 'f'))
      *value += *line - 'a' + 10;
    else 
      return 0;
    line++;
    }
  return 1;
  }


static result_t get_line(handle_t stream,
                         char *buffer,
                         uint16_t buf_len,
                         uint16_t *read,
                         char eol)
  {
  result_t result;
  uint16_t len;
  for(len = 0; len < buf_len; len++)
    {
    if(failed(result = stream_getc(stream, buffer)) &&
       result != e_no_more_information)
      return result;
    
    if(result == e_no_more_information ||
       *buffer == eol ||
       *buffer == 0)
      {
      *buffer = 0;
      if(read != 0)
        *read = len;

      return s_ok;
      }
    
    buffer++;
    }
  
  *buffer = 0;
  return e_unexpected;
  }

/**
 * Interprets given line and transmit can message
 *
 * @param line Line string which contains the transmit command
 * @returns true if the message was posted, false if queue full
 */
static bool transmit_can(uint8_t cmd, handle_t stream)
  {
  uint32_t temp;
  uint8_t idlen;
  char *str;
  result_t result;
  
  canmsg_t canmsg;
  memset(&canmsg, 0, sizeof(canmsg_t));

  if(((cmd == 'r') || (cmd == 'R')))
    canmsg.reply = 1;
  
  // read till eof
  char line[32];
  if(failed(result = get_line(stream, line, sizeof(line), 0, '\r')) &&
     result != e_no_more_information)
    return false;
  
  idlen = 3;
  str = line;

  if (!parse_hex(str, idlen, &temp))
    return 0;
  canmsg.id = temp;
  
  str += idlen;

  if (!parse_hex(str, 1, &temp))
    return 0;
  
  canmsg.length = temp;
  
  str++;

  if (!canmsg.reply)
    {
    uint8_t i;
    uint8_t length = canmsg.length;
    if (length > 8)
      length = 8;
    
    for (i = 0; i < length; i++)
      {
      if (!parse_hex(str, 2, &temp))
        return false;
      str++;
      str++;
      canmsg.raw[i] = temp;
      }
    }

  can_send_raw(&canmsg);
  
  return true;
  }

// send a message
static char msg_buf[TX_BUFFER_LENGTH];
static void _send_status_msg(char msg, uint8_t len, uint8_t b0, uint8_t b1, uint8_t term)
  {
  char *ch = msg_buf;
  *ch++ = msg;
  if(len-- > 0)
    ch = write_hex(ch, b0);
  
  if(len > 0)
    ch = write_hex(ch, b1);
  
  *ch++ = CR;
  *ch = 0;
  
  push_back(tx_msg_queue, msg_buf, INDEFINITE_WAIT);
  }

static void send_status_msg(char msg, uint8_t len, uint8_t b0, uint8_t b1)
  {
  _send_status_msg(msg, len, b0, b1, CR);
  }

static void send_ack_msg()
  {
  _send_status_msg(CR, 0, 0, 0, 0);
  }

static void send_nak_msg()
  {
  _send_status_msg(BELL, 0, 0, 0, 0);
  }

// Setup with user defined timing settings for CNF1/CNF2/CNF3

static void set_userdefined_timing(uint8_t cmd, handle_t stream)
  {
  send_ack_msg();
  }

// Setup with standard CAN bitrates

static void setup_standard_timing(uint8_t cmd, handle_t stream)
  {
  char ch;
  
  // since this is a CanFLY device we always run at 125k
  if(succeeded(stream_getc(stream, &ch)) && ch == '4')
    send_ack_msg();
  else
    send_nak_msg();
  }

static void get_hardware_version(uint8_t cmd, handle_t stream)
  {
  send_status_msg('V', 2, 1, 0);
  }

// Get firmware version

static void get_firmware_version(uint8_t cmd, handle_t stream)
  {
  send_status_msg('v', 2, 1, 0);
  }

// Get serial number

static void get_serial_number(uint8_t cmd, handle_t stream)
  {
  send_status_msg('N', 2, 0xff, 0xff);
  }

// Open CAN channel

static void set_state(uint8_t cmd, handle_t stream)
  {
  // O = open
  // l = loop-back mode
  // L = listen only mode
  // C = close
  switch (cmd)
    {
    case 'O':
      can_state = can_open;
      break;
    case 'l':
      can_state = can_loopback;
      break;
    case 'L':
      can_state = can_listen;
      break;
    case 'C':
      can_state = can_closed;
      break;
    }
  
  send_ack_msg();
  }

// Read status flags

static void read_status_flags(uint8_t cmd, handle_t stream)
  {
  uint8_t status = 0;

  if (overflow_warning)
    status |= 0x08;
  /*
    if (flags & 0x01) status |= 0x04; // error warning
    if (flags & 0xC0) status |= 0x08; // data overrun
    if (flags & 0x18) status |= 0x20; // passive error
    if (flags & 0x20) status |= 0x80; // bus error
   */
  send_status_msg('F', 1, 0, 0);

  overflow_warning = false;
  }

static void process_transmit_command(uint8_t cmd, handle_t stream)
  {
  if (can_state != can_open &&
      can_state != can_loopback)
    send_nak_msg();

  // r -> transmit rtr 11 bits
  // t -> transmit 11 bit id
  // R -> transmit RTR 29 bits
  // T -> transmit 29 bit id
  if (transmit_can(cmd, stream))
    send_status_msg('z', 0, 0, 0);
  else
    send_nak_msg();
  }

static void set_timestamping(uint8_t cmd, handle_t stream)
  {
  unsigned long stamping;
  // read till eof
  char line[32];
  if(failed(get_line(stream, line, sizeof(line), 0, '\r')))
    {
    send_nak_msg();
    return;
    }

  if (parse_hex(line, 1, &stamping))
     {
    timestamping = (stamping != 0);
    send_ack_msg();
    }
  else
    send_nak_msg();
  }

static void set_acceptance_mask(uint8_t cmd, handle_t stream)
  {
  send_ack_msg();
  }


static uint8_t message_code = 0;

static void open_terminal(uint8_t cmd, handle_t stream)
  {
  // 
  // open a ccs session with a remote device
  //
  // Xnnn     Open a device with the id nnn
  // xsssss[nnn]  Open a well known decvice
  //
  // Example xahrs(1) will open and ahrs with std node id of ahrs_node_id
  //         xahrs2  will open an ahrs with std node if of ahrs_node_id+1
  //
  // ids:
  //   ahrs, edu, mfd, scan, efi
  //
  // negotiates a session
  
  // read till eof
  char line[32];
  result_t result;
  if(failed(result = get_line(stream, line, sizeof(line), 0, '\r')) &&
     result != e_no_more_information)
    {
    send_nak_msg();
    return;
    }
  
  if(isdigit(line[0]))
    terminal_node_id = atoi(line);
  else if(strncmp(line, "ahrs", 4)== 0)
    {
    terminal_node_id = ahrs_node_id;
    if(line[4] != 0)
      terminal_node_id += atoi(line + 3);
    }
  else if(strncmp(line, "edu", 3) == 0)
    {
    terminal_node_id = edu_node_id;
    if(line[3] != 0)
      terminal_node_id += atoi(line + 3);
    }
  else if(strncmp(line, "mfd", 3)== 0)
    {
    terminal_node_id = mfd_node_id;
    if(line[3] != 0)
      terminal_node_id += atoi(line + 3);
    }
  else if(strncmp(line, "scan", 4)== 0)
    terminal_node_id = scan_id;
  else if(strncmp(line, "efi", 3)== 0)
    terminal_node_id = efi_node_id;
  else
    {
    send_nak_msg();
    return;
    }
  
  // create an open message
  terminal_ticks = ticks();
  can_state = can_terminal;
  
  message_code = 0;
  
  canmsg_t msg;
  msg.flags = 0;
  msg.id = node_service_channel_100;
  msg.canas.data_type = CANAS_DATATYPE_NODATA;
  msg.canas.node_id = terminal_node_id;         // node we are calling
  msg.canas.service_code = id_ccs_service;
  msg.canas.message_code = ++message_code;
  msg.length = 4;
  
  can_send_raw(&msg); 
  }

static void send_terminal(handle_t stream)
  {
  canmsg_t msg;
  msg.flags = 0;
  msg.id = node_service_channel_100;
  msg.canas.data_type = CANAS_DATATYPE_NODATA;
  msg.canas.node_id = terminal_node_id;         // node we are calling
  msg.canas.service_code = id_ccs_service;
  msg.canas.message_code = ++message_code;
  msg.length = 4;
  
  char c;
  // read till eof
  while(succeeded(stream_getc(stream, &c)))
    {
    switch(msg.length)
      {
      case 4 :
        msg.canas.data_type = CANAS_DATATYPE_CHAR;
        msg.canas.data[0] = (uint8_t) c;
        msg.length++;
        break;
      case 5 :
        msg.canas.data_type = CANAS_DATATYPE_CHAR2;
        msg.canas.data[1] = (uint8_t) c;
        msg.length++;
        break;
      case 6 :
        msg.canas.data_type = CANAS_DATATYPE_CHAR3;
        msg.canas.data[2] = (uint8_t) c;
        msg.length++;
        break;
      case 7 :
        msg.canas.data_type = CANAS_DATATYPE_CHAR4;
        msg.canas.data[3] = (uint8_t) c;
        msg.length++;
        
        can_send(&msg);
        msg.length = 4;       // reset buffer.
        break;
      }
    }
  
  if(msg.length > 4)
    can_send_raw(&msg);
  }

static uint32_t terminal_timeout = 3000;      // 3 seconds

static bool process_terminal(const canmsg_t *msg, void *parg)
  {
  // we only respond to reply messages from the remote end.
  if(msg->canas.service_code != id_ccs_service ||
     msg->id != node_service_channel_100 +1 ||
     msg->canas.node_id != terminal_node_id)
    return false;
  
  if(msg->canas.message_code == -1)
    {
    can_state = can_closed;
    return true;
    }
  
  // receive a message
  switch(msg->canas.data_type)
    {
    case CANAS_DATATYPE_CHAR :
      write_ft201(msg->canas.data[0]);
      break;
    case CANAS_DATATYPE_CHAR2 :
      write_ft201(msg->canas.data[0]);
      write_ft201(msg->canas.data[1]);
      break;
    case CANAS_DATATYPE_CHAR3 :
      write_ft201(msg->canas.data[0]);
      write_ft201(msg->canas.data[1]);
      write_ft201(msg->canas.data[2]);
      break;
    case CANAS_DATATYPE_CHAR4 :
      write_ft201(msg->canas.data[0]);
      write_ft201(msg->canas.data[1]);
      write_ft201(msg->canas.data[2]);
      write_ft201(msg->canas.data[3]);
      break;
    }
  
  // flag we have heard from the remote end so is open
  terminal_ticks = ticks();
  return true;
  }

static char can_buf[TX_BUFFER_LENGTH];


static bool process_can(const canmsg_t *msg, void *parg)
  {
  int i;
  
  if(msg == 0)
    return false;         // nothing there
  
  // if this is a terminal then we do nothing as the message
  // is processed by the terminal using ccs messages
  if(can_state == can_terminal)
    return false;
  
  // see if we are open
  if(can_state != can_closed)
    {
    char *ch = can_buf;

    // runt packets are ignored
    if(msg->length == 0)
      return true;

    *ch++ = 't';
    ch = write_nibble(ch, msg->id >> 8); // upper 3 bits
    ch = write_hex(ch, msg->id); // lower 8 bits
    ch = write_nibble(ch, msg->length); // length

    for (i = 0; i < msg->length; i++)
      ch = write_hex(ch, msg->raw[i]);

    *ch++ =  CR;
    *ch = 0;
    push_back(tx_msg_queue, can_buf, INDEFINITE_WAIT);
    }
  
  return true;
  }

static bool portb_tx_empty = false;
static bool portb_rx_ready = false;

void __attribute__ ((__interrupt__, auto_psv)) _CNInterrupt(void)
  {
  // Insert ISR code here
  IFS1bits.CNIF = 0; // Clear CN interrupt
  // check for a change on port 14
  if((PORTBbits.RB14 == 0) && !portb_tx_empty)
    signal_from_isr(ft201_tx_empty);

  portb_tx_empty = PORTBbits.RB14 == 0;
  
  if((PORTBbits.RB15 == 0) && !portb_rx_ready)
    signal_from_isr(ft201_rx_ready);

  portb_rx_ready = PORTBbits.RB15 == 0;
  }

#define BUFFER_LEN 128;
// this allows for the read bit as the low bit
#define FT201_ADDRESS (0x22 << 1)

static void ft201_tx_worker(void *parg)
  {
  i2c_task_t task;
  uint8_t ch;
  
  // set up our uart write task
  init_defaults(&task);
  semaphore_create(&task.task_callback);
  task.p_data = &ch;
  task.xfer_length = 1;
  task.command_length = 1;
  task.command[0] = FT201_ADDRESS;
  
  while(true)
    {
    if(failed(pop_front(ft201_tx_queue, &ch, INDEFINITE_WAIT)))
      continue;
    
    // see if the uart is ready
    while(PORTBbits.RB14 == 1)
      semaphore_wait(ft201_tx_empty, INDEFINITE_WAIT);
    
    // we set up the task
    enqueue_i2c_task(I2C_CHANNEL_2, &task);
    semaphore_wait(task.task_callback, INDEFINITE_WAIT);
    }
  }

static void ft201_rx_worker(void *parg)
  {
  stream_p rx_stream;
  uint32_t len;
  uint16_t read_length = 0;
  i2c_task_t task;
  char ch;
  
  // set up our uart write task
  init_defaults(&task);
  semaphore_create(&task.task_callback);
  task.p_data = &ch;
  task.xfer_length = 1;
  task.command_length = 1;
  task.read_length = &read_length;
  task.command[0] = FT201_ADDRESS | 0x01;     // set up a read
  
  // create the string stream to buffer our console
  strstream_create(0, 0, false, &rx_stream);
  
  while(true)
    {
    // state machine is idle so we need the rx_ready
    if(PORTBbits.RB15 == 1)
      semaphore_wait(ft201_rx_ready, INDEFINITE_WAIT);
    
    // see if an erroneous signal occurred.
    if(PORTBbits.RB15 == 1)
      continue;
    
    // reset read length
    read_length = 0;
    // we set up the task to read the character from the USB port
    enqueue_i2c_task(I2C_CHANNEL_2, &task);
    semaphore_wait(task.task_callback, INDEFINITE_WAIT);

    if(can_state == can_terminal)
      {
      // store the char in a stream.
      stream_putc(rx_stream, ch);
      // if there are more characters then we continue to read the stream
      if(PORTBbits.RB15 == 0)
        continue;
      // see if there is any information
      stream_length(rx_stream, &len);
      if(len > 0)
        {
        stream_setpos(rx_stream, 0);
        // the stream has a buffer to be sent...            
        send_terminal(rx_stream);
        stream_truncate(rx_stream, 0);
        }
      continue;
      }
    
    if(ch == '\r' || ch == '\n')
      {
      // rewind our stream
      stream_setpos(rx_stream, 0);
      
      // we have a serial line
      if(succeeded(stream_getc(rx_stream, &ch)))
        {
        switch(ch)
          {
          case 'S': 
            setup_standard_timing(ch, rx_stream);
            break;
          case 's': 
            set_userdefined_timing(ch, rx_stream);
            break;
          case 'V': 
            get_hardware_version(ch, rx_stream);
            break;
          case 'v': 
            get_firmware_version(ch, rx_stream);
            break;
          case 'N': 
            get_serial_number(ch, rx_stream);
            break;
          case 'O': 
            set_state(ch, rx_stream);
            break;
          case 'l': 
            set_state(ch, rx_stream);
            break;
          case 'L': 
            set_state(ch, rx_stream);
            break;
          case 'C': 
            set_state(ch, rx_stream);
            break;
          case 'r': // Transmit standard RTR (11 bit) frame
          case 't': // Transmit standard (11 bit) frame
            process_transmit_command(ch, rx_stream);
            break;
          case 'F': 
            read_status_flags(ch, rx_stream);
            break;
          case 'Z': // Set time stamping
            set_timestamping(ch, rx_stream);
            break;
          case 'm':
          case 'M':
            set_acceptance_mask(ch, rx_stream);
            break;
          case 'x' :
          case 'X' :
            open_terminal(ch, rx_stream);
            break;
          }
        }
      
      // clear the stream, ready for the next char
      stream_truncate(rx_stream, 0);
      }
    else
      // otherwise ready for next one
      stream_putc(rx_stream, ch);
    }
    
  }

static neutron_parameters_t init_params = {
  .hardware_revision = HARDWARE_REVISION,
  .software_revision = SOFTWARE_REVISION,
  .node_type = unit_usb,
  .node_id = scan_id+1
  };

static char tx_buffer[TX_BUFFER_LENGTH];

static void tx_queue_task(void *parg)
  {
  while(true)
    {
    pop_front(tx_msg_queue, tx_buffer, INDEFINITE_WAIT);
    
    write_str(tx_buffer);
    }
  }

static const char *rx_queue_length_str = "rxq_len";
static const char *tx_queue_length_str  = "txq_len";

static msg_hook_t hook = { 0, 0, process_can };
static msg_hook_t service_hook = { 0, 0, process_terminal, 0 };

#define EEPROM_SIZE (128L * 1024L)    // eeprom is 128k bytes

static void main_task(void *parg)
  {
  result_t result;
  bool init_eeprom = PORTBbits.RB1 == 0;
  uint16_t tx_queue_length = 256;
  uint16_t rx_queue_length = 256;
  
  result = eeprom_init(init_eeprom, I2C_CHANNEL_2, EEPROM_SIZE);

  if(result == e_not_initialized)
    {
    init_eeprom = true;
    reg_set_uint16(0, rx_queue_length_str, rx_queue_length);
    reg_set_uint16(0, tx_queue_length_str, tx_queue_length);
    }

  // start the canbus stuff working
  can_aerospace_init(&init_params, true, false);
  
  // set up the queues
  deque_create(sizeof(uint8_t), tx_queue_length, &ft201_tx_queue);
  semaphore_create(&ft201_tx_empty);
  semaphore_create(&ft201_rx_ready);
   
  // write_str("Test message\r\n");
 
  // create the tasks
  task_create("RX", DEFAULT_STACK_SIZE, ft201_rx_worker, 0, NORMAL_PRIORITY, 0);
  task_create("TX", DEFAULT_STACK_SIZE, ft201_tx_worker, 0, NORMAL_PRIORITY, 0);
  
  // enable change notifications
  CNENBbits.CNIEB14 = 1;
  CNENBbits.CNIEB15 = 1;
  IEC1bits.CNIE = 1; // Enable CN interrupts
  IFS1bits.CNIF = 0; // Reset CN interrupt  
 
  // create the queue worker
  deque_create(TX_BUFFER_LENGTH, TX_MSG_QUEUE_LEN, &tx_msg_queue);
  
  task_create("TXTQUE", DEFAULT_STACK_SIZE, tx_queue_task, 0, NORMAL_PRIORITY, 0);

  // general can msg receiver
  subscribe(&hook);
  
  // subcribe to the service
  register_service(id_ccs_service, &service_hook);

  semaphore_p terminal_semp;
  semaphore_create(&terminal_semp);

  // we wait for a terminal to open, and if the service channel
  // does not respond we close the channel
  while(true)
    {
    // wait 250 msec
    semaphore_wait(terminal_semp, 250);
    
    if(can_state == can_terminal)
      {
        canmsg_t msg;
        msg.flags = 0;
        msg.id = node_service_channel_100;
        msg.canas.data_type = CANAS_DATATYPE_NODATA;
        msg.canas.node_id = terminal_node_id;         // node we are calling
        msg.canas.service_code = id_ccs_service;
        msg.length = 4;
        
      if(ticks() > (terminal_ticks +  terminal_timeout))
        {
        can_state = can_closed;     // close the terminal
        msg.canas.message_code = -1;        // send a close...
        
        write_str("\r\nRemote terminal timed out.  Connection forcibly closed.\r\n");
        terminal_node_id = 0;
        }
      else
        {
        msg.canas.message_code = 0;         // send a ping....
        }
        
      can_send_raw(&msg);
      }
    }
  }

int main(void)
  {
  // allow input to settle while clock switch in progress
  TRISBbits.TRISB1 = 1;     // input port
  TRISBbits.TRISB14 = 1;
  TRISBbits.TRISB15 = 1;
  CNPUBbits.CNPUB1 = 1;     // with a pull-up
  CNPUBbits.CNPUB14 = 1;
  CNPUBbits.CNPUB15 = 1;
  
  // set up the PLL for 20Mips
  // FOSC = 10mHz
  // FPLLI = 5Mhz
  // FSYS = 280Mhz
  // FOSC = 40Mhz
  PLLFBD = I_PLLDIV;                      // PLL divider
  CLKDIVbits.PLLPRE = I_PLLPRE;           // Prescaler divide by 2
  CLKDIVbits.PLLPOST = I_PLLPOST;         // Postscale divide by 2
  
  
    // Initiate Clock Switch to Primary Oscillator with PLL (NOSC=0b011)
  if(OSCCONbits.COSC != 0b011)
    {
    __builtin_write_OSCCONH(0x03);
    __builtin_write_OSCCONL(OSCCON | 0x01);
    // Wait for Clock switch to occur
    while (OSCCONbits.COSC!= 0b011);
    }
  // Wait for PLL to lock
  while (OSCCONbits.LOCK!= 1);
 
  // unlock the PPS functions
  __builtin_write_OSCCONL(OSCCON & 0xbf);
  
  // seem to have a startup problem with the I2C when running with the FT201
  // regulator so we spin for a while...
  uint16_t delay = 0;
  while(++delay != 0);
  
    // map pins
  map_rpo(RP36R, rpo_c1tx);
  map_rpi(rpi_rpi45, rpi_c1rx);
 
  // pick up a small malloc amount to get the heap pointer
  uint32_t mem = (uint32_t) malloc(4);
  mem = ((mem -1) | 0x0f)+1;
  // this is for dsPIC33EP256GP which is the largest we support
  // we loose 20k as the code space is mapped
  size_t length = 0x8000 - mem;
  
  neutron_run((void *)mem, length, "MAIN", DEFAULT_STACK_SIZE << 1, main_task, 0, NORMAL_PRIORITY, 0);


  return 0;
  }

