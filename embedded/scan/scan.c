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
then the origional copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/

#include <p33Exxxx.h>

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "../dspic33f-lib/microkernel.h"
#include "../dspic33f-lib/can_driver.h"
#include "../dspic33f-lib/uart_device.h"
#include "../dspic33f-lib/eeprom.h"
#include "../dspic33f-lib/microkernel.h"
#include "../dspic-lib/publisher.h"

// DSPIC33EP512GP504 Configuration Bit Settings

#include <xc.h>

// FICD
#pragma config ICS = PGD1               // ICD Communication Channel Select bits (Communicate on PGEC1 and PGED1)
#pragma config JTAGEN = OFF             // JTAG Enable bit (JTAG is disabled)

// FPOR
#pragma config ALTI2C1 = OFF            // Alternate I2C1 pins (I2C1 mapped to SDA1/SCL1 pins)
#pragma config ALTI2C2 = OFF             // Alternate I2C2 pins (I2C2 mapped to SDA2/SCL2 pins)
#pragma config WDTWIN = WIN25           // Watchdog Window Select bits (WDT Window is 25% of WDT period)

// FWDT
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler bits (1:32,768)
#pragma config WDTPRE = PR128           // Watchdog Timer Prescaler bit (1:128)
#pragma config PLLKEN = ON              // PLL Lock Enable bit (Clock switch to PLL source will wait until the PLL lock signal is valid.)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (Watchdog Timer in Non-Window mode)
#ifdef DEBUG
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

/**
 * Receive a message from the can bus and convert to a serial packet
 * @param 
 */
static void process_can(const can_msg_t *);
/**
 * Receive a buffer from the serial interface and convert to a can message
 * @param config
 * @param 
 * @param len
 */
static void process_serial(uart_config_t *uart, uint8_t *buffer, uint8_t len);


#define HARDWARE_REVISION 30
#define SOFTWARE_REVISION 1

/*
 * This enumerates the settings in the NV ram.
 */
typedef enum _memid_values {
  memid_nodeid,
  memid_baudrate,
  memid_last
  } memid_values;
  
/**
 * Lookup the parameter read from the eeprom for the passed in memid
 * @param memid   memid to lookup
 * @return        pointer to the definition.
 */
extern can_parameter_definition_t *get_parameter(memid_values memid);
/**
 * Lookup a parameter and return it cast to a type 0
 * @param memid   memid to lookup
 * @return        pointer to the definition.
 */
extern can_parameter_type_0_t *get_parameter_type_0(memid_values memid);
/**
 * Lookup a parameter and return it cast to a type 1
 * @param memid   memid to lookup
 * @return        pointer to the definition.
 */
extern can_parameter_type_1_t *get_parameter_type_1(memid_values memid);
  
#define CAN_TX_LENGTH 64
#define CAN_RX_LENGTH 32

static can_msg_t can_tx_buffer[CAN_TX_LENGTH];
static can_msg_t can_rx_buffer[CAN_RX_LENGTH];


#define NUM_RX_BUFFERS  8
#define RX_BUFFER_LENGTH 128

#define RX_QUEUE_LENGTH (NUM_RX_BUFFERS * RX_BUFFER_LENGTH)

static uint8_t rx_buffer[RX_BUFFER_LENGTH];
static uint8_t rx_worker_buffer[RX_BUFFER_LENGTH];
static uint8_t rx_queue[RX_QUEUE_LENGTH];

#define NUM_TX_BUFFERS 256       // large buffer for tx
#define TX_BUFFER_LENGTH 32

#define TX_QUEUE_LENGTH (NUM_TX_BUFFERS * TX_BUFFER_LENGTH)

static uint8_t tx_buffer[TX_BUFFER_LENGTH];
static uint8_t tx_worker_buffer[TX_BUFFER_LENGTH];
static uint8_t tx_queue[TX_QUEUE_LENGTH];

static uart_config_t uart_config = {
  .uart_number = 1,
  .rx_buffer = rx_buffer,
  .rx_worker_buffer = rx_worker_buffer,
  .rx_length = RX_BUFFER_LENGTH,
  .tx_buffer = tx_buffer,
  .tx_worker_buffer = tx_worker_buffer,
  .tx_length = TX_BUFFER_LENGTH,
  .rx_pin = rpi_rpi46,
  .tx_pin = RP56R,
  .eol_char = '\r',
  .flags = UART_EOL_CHAR,
  .callback.uart_callback = process_serial,
  };

static uint16_t idle_stack[IDLE_STACK_SIZE];
static uint16_t can_tx_stack[DEFAULT_STACK_SIZE];
static uint16_t can_rx_stack[DEFAULT_STACK_SIZE];
static uint16_t rx_stack[DEFAULT_STACK_SIZE];
static uint16_t tx_stack[DEFAULT_STACK_SIZE];
static uint16_t publisher_stack[DEFAULT_STACK_SIZE];
static uint16_t eeprom_stack[DEFAULT_STACK_SIZE];
static uint16_t i2c_stack[DEFAULT_STACK_SIZE];

// should match the number of stacks.
#define NUM_TASKS 8
tcb_t tasks[NUM_TASKS];

#define NUM_I2C_TASKS 32
static i2c_task_t i2c_tasks[NUM_I2C_TASKS];

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
  } can_state_t;

#ifdef DEBUG
  // HACK: so we don't have to restart slcan all the time, remove in release
static can_state_t can_state = can_open;
#else
static can_state_t can_state = can_closed;
#endif

typedef enum _config_state {
  config_can,
  config_serial,
  config_complete
  }config_state;

static config_state init_state = config_can;

static can_parameter_type_1_t nodeid_defn = 
  {
    .hdr.data_type = p1t_short, 
    .hdr.length = SIZE_SHORT_SETTING,
    .value.SHORT = slcan_id
  };

static can_parameter_type_1_t baud_rate_defn = 
  { 
  .hdr.data_type = p1t_short,
  .hdr.length = SIZE_ULONG_SETTING,
  .value.ULONG = 57600
  };

static parameter_definition_t parameters[memid_last] = {
  {
  .parameter = (can_parameter_definition_t *) &nodeid_defn,
  .nis.memid = memid_nodeid,
  .nis.name = "Node ID",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_1_t),
  .nis.parameter_type = 1
  },
  {
  .parameter = (can_parameter_definition_t *) &baud_rate_defn,
  .nis.memid = memid_baudrate,
  .nis.name = "BAUD RATE",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_1_t),
  .nis.parameter_type = 1
  },
  };

static ee_definition_t eeprom_definitions[memid_last];

static eeprom_params_t ee_params =
  {
  .worker_stack = eeprom_stack,
  .stack_size = numelements(eeprom_stack),
  .i2c_buffer = i2c_tasks,
  .i2c_queue_len = NUM_I2C_TASKS,
  .i2c_worker_stack = i2c_stack,
  .i2c_worker_stack_length = numelements(i2c_stack),
  .definitions = eeprom_definitions,
  .number_parameters = memid_last,
  };

can_parameter_definition_t *get_parameter(memid_values memid)
  {
  if(memid >= memid_last)
    return 0;
  
  return parameters[memid].parameter;
  }

can_parameter_type_0_t *get_parameter_type_0(memid_values memid)
  {
  if(memid >= memid_last)
    return 0;
  
  return (can_parameter_type_0_t *) parameters[memid].parameter;
  }

can_parameter_type_1_t *get_parameter_type_1(memid_values memid)
  {
  if(memid >= memid_last)
    return 0;
  
  return (can_parameter_type_1_t *) parameters[memid].parameter;
  }
/**
 * Called back from the eeprom worker to initialize the eeprom.  Should only
 * be called once.
 * @param memid     memid to retrieve
 * @param nis       header structure to fill in.
 * @param length    length of the parameter data
 * @param p_data    data to be stored
 */
void define_parameter(uint16_t memid, mis_message_t *nis, 
                           uint16_t *length, void **p_data)
  {
  if(nis != 0 && length != 0 && p_data != 0 && memid < memid_last)
    {
    parameter_definition_t *param = parameters + memid;
    
    *length = param->nis.parameter_length;
    nis->memid = param->nis.memid;
    nis->parameter_type = param->nis.parameter_type;
    
    uint8_t len = param->nis.name_length;
    if(len == 0)
      len = strlen(param->nis.name);
    
    if(len > 16)
      len = 16;
    // clear the string buffer
    memset(nis->name, 0, sizeof(nis->name));
    strncpy(nis->name, param->nis.name, len);
    
    nis->name_length = len;
    
    *p_data = (void *) param->parameter;
    }
  }


static void idle_task(void *parg)
  {
  while(true)
    {
    exit_critical();
    yield();
    }
    //asm volatile("pwrsav #1");
  }

static message_listener_t listener =
  {
  .handler = process_can
  };

#define I_PLLPRE 0
#define I_PLLPOST 0
#define I_FOSC 140000000
#define I_FIN 10000000
#define I_PLLDIV 54 // ((I_OSC/I_FIN)*(I_PLLPRE+2)*(2*(I_PLLPOST+1)))-2

int main(void)
  {
  int8_t idle_task_id;
  
  // set up the hardware.  Library dependent.
  hw_init();
  // allow input to settle while clock switch in progress
  TRISCbits.TRISC5 = 1;     // input port
  CNPUCbits.CNPUC5 = 1;     // with a pull-up
  
  // set up the PLL for 70Mips
  // FOSC = 10mHz
  // FPLLI = 5Mhz
  // FSYS = 280Mhz
  // FOSC = 140Mhz
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
    // map pins
  map_rpo(RP42R, rpo_c1tx);
  map_rpi(rpi_rpi44, rpi_c1rx);

  // set up uart data early
  init_deque(&uart_config.rx_queue, RX_BUFFER_LENGTH, rx_queue, NUM_RX_BUFFERS);
  init_deque(&uart_config.tx_queue, TX_BUFFER_LENGTH, tx_queue, NUM_TX_BUFFERS);
  
  idle_task_id = scheduler_init(tasks, NUM_TASKS, idle_task, idle_stack, numelements(idle_stack));
  resume(idle_task_id);
  
  // create the canbus task
  can_init(HARDWARE_REVISION, SOFTWARE_REVISION,
           can_tx_buffer, CAN_TX_LENGTH,
           can_rx_buffer, CAN_RX_LENGTH,
           can_tx_stack, numelements(can_tx_stack), 0,
           can_rx_stack, numelements(can_rx_stack), 0,
           publisher_stack, numelements(publisher_stack), 0);
  
  subscribe(&listener);

  ee_params.init_mode = PORTCbits.RC5 == 0;
  // initialize the eeprom as we need our settings first
  eeprom_init(&ee_params);
  // run with an idle task
  run(idle_task_id);

  return 0;
  }


// this is called by the eeprom init task
bool eeprom_init_done(semaphore_t *worker_task)
  {
  switch(init_state)
    {
    case config_can :
      if(can_config(memid_nodeid, worker_task))
        init_state = config_serial;
      break;
    case config_serial :
      // TODO: read other config values that are set....
      
      uart_config.rate = get_parameter_type_1(memid_baudrate)->value.ULONG;
  
      init_uart(&uart_config, rx_stack, numelements(rx_stack), tx_stack, numelements(tx_stack));
      
      signal(worker_task);
      init_state = config_complete;
      break;
    default :
      return true;          // init is done
    }
  
  return false;             // more work
  }

void panic()
  {
  // restart the system using the brown-out
  __builtin_disi(0x3FFF); /* disable interrupts */
  asm volatile("RESET");
  }


/**
 * Parse hex value of given string
 *
 * @param line Input string
 * @param len Count of characters to interpret
 * @param value Pointer to variable for the resulting decoded value
 * @return 0 on error, 1 on success
 */
static uint8_t parse_hex(uint8_t * line, uint8_t len, unsigned long * value)
  {
  *value = 0;
  while (len--)
    {
    if (*line == 0) return 0;
    *value <<= 4;
    if ((*line >= '0') && (*line <= '9'))
      *value += *line - '0';
    else if ((*line >= 'A') && (*line <= 'F'))
      *value += *line - 'A' + 10;
    else if ((*line >= 'a') && (*line <= 'f'))
      *value += *line - 'a' + 10;
    else return 0;
    line++;
    }
  return 1;
  }

static void send_nibble(uart_config_t *config, uint8_t value)
  {
  value = value & 0x0f;
  if (value > 9)
    value = value - 10 + 'A';
  else
    value = value + '0';
  
  write_uart(config, value);
  }

/**
 * Send given byte value as hexadecimal string
 *
 * @param value Byte value to send over UART
 */
static void send_hex(uart_config_t *config, uint8_t value)
  {
  send_nibble(config, value >> 4);
  send_nibble(config, value);
  }

/**
 * Interprets given line and transmit can message
 *
 * @param line Line string which contains the transmit command
 * @returns true if the message was posted, false if queue full
 */
static bool transmit_can(uart_config_t *config, uint8_t *line)
  {
  unsigned long temp;
  uint8_t idlen;
  uint8_t *ptr;
  
  can_msg_t canmsg;
  ptr = (uint8_t *)&canmsg;
  
  for(temp = 0; temp < sizeof(can_msg_t); temp++)
    *ptr++ = 0;

  if(((line[0] == 'r') || (line[0] == 'R')))
    canmsg.flags |= REPLY_MSG;

  // upper case -> extended identifier
  if (line[0] < 'Z')
    {
    canmsg.flags |= EXTENDED_ADDRESS;
    idlen = 8;
    }
  else
    idlen = 3;

  if (!parse_hex(&line[1], idlen, &temp))
    return 0;
  canmsg.id = temp;

  if (!parse_hex(&line[1 + idlen], 1, &temp))
    return 0;
  
  canmsg.flags |= temp & DLC_MASK;

  if ((canmsg.flags & REPLY_MSG) == 0)
    {
    uint8_t i;
    uint8_t length = canmsg.flags & DLC_MASK;
    if (length > 8)
      length = 8;
    for (i = 0; i < length; i++)
      {
      if (!parse_hex(&line[idlen + 2 + i * 2], 2, &temp))
        return 0;
      canmsg.msg.raw[i] = temp;
      }
    }

  publish_raw(&canmsg);
  
  return true;
  }

// Setup with user defined timing settings for CNF1/CNF2/CNF3
static uint8_t set_userdefined_timing(uart_config_t *config, uint8_t cmd, uint8_t *buffer, uint8_t len)
  {
  
  return CR;
  }

// Setup with standard CAN bitrates
static uint8_t setup_standard_timing(uart_config_t *config, uint8_t cmd, uint8_t *buffer, uint8_t len)
  {
  // since this is a CanFLY device we always run at 125k
  return buffer[1] == '4' ? CR : BELL;
  }

// Get hardware version
static uint8_t get_hardware_version(uart_config_t *config, uint8_t cmd, uint8_t *buffer, uint8_t len)
  {
  write_uart(config, 'V');
  send_hex(config, 1);
  send_hex(config, 0);
  return CR;
  }

// Get firmware version
static uint8_t get_firmware_version(uart_config_t *config, uint8_t cmd, uint8_t *buffer, uint8_t len)
  {
  write_uart(config, 'v');
  send_hex(config, 1);
  send_hex(config, 0);
  return CR;
  }

// Get serial number
static uint8_t get_serial_number(uart_config_t *config, uint8_t cmd, uint8_t *buffer, uint8_t len)
  {
  write_uart(config, 'N');
  send_hex(config, 0xFF);
  send_hex(config, 0xFF);
  return CR;
  }

// Open CAN channel
static uint8_t set_state(uart_config_t *config, uint8_t cmd, uint8_t *buffer, uint8_t len)
  {
  // O = open
  // l = loop-back mode
  // L = listen only mode
  // C = close
  switch(cmd)
    {
    case 'O' :
      can_state = can_open;
      break;
    case 'l' :
      can_state = can_loopback;
      break;
    case 'L' :
      can_state = can_listen;
      break;
    case 'C' :
      can_state = can_closed;
      break;
    }
  return CR;
  }

// Read status flags
static uint8_t read_status_flags(uart_config_t *config, uint8_t cmd, uint8_t *buffer, uint8_t len)
  {
      uint8_t status = 0;
      
      if(overflow_warning)
        status |= 0x08;
/*
      if (flags & 0x01) status |= 0x04; // error warning
      if (flags & 0xC0) status |= 0x08; // data overrun
      if (flags & 0x18) status |= 0x20; // passive error
      if (flags & 0x20) status |= 0x80; // bus error
*/
      write_uart(config, 'F');
      send_hex(config, status);
      
      overflow_warning = false;
      
      return CR;
  }


static uint8_t process_transmit_command(uart_config_t *config, uint8_t cmd, uint8_t *buffer, uint8_t len)
  {
  // r -> transmit rtr 11 bits
  // t -> transmit 11 bit id
  // R -> transmit RTR 29 bits
  // T -> transmit 29 bit id
  if (transmit_can(config, buffer))
    {
    write_uart(config, 'z');
    return CR;
    }
  
  return BELL;
  }

static uint8_t set_timestamping(uart_config_t *config, uint8_t cmd, uint8_t *buffer, uint8_t len)
  {
      unsigned long stamping;
      if (parse_hex(&buffer[1], 1, &stamping))
        {
        timestamping = (stamping != 0);
        return CR;
        }
      

  return BELL;
  }

static uint8_t process_query_command(uart_config_t *config, uint8_t cmd, uint8_t *buffer, uint8_t len)
  {
  // q = 11 bit identifier query
      // Q = 29 bit identifier query
      // set the data logging protocol
      // Format is 
      //  [q|Q]sssssssseeeeeeeerrr[admM]
      //
      // ssssssss Start timestamp in seconds since 1/1/2000
      // eeeeeeee End timestamp in seconds since 1/1/2000
      // rrr  Sample window in seconds
      // admM   a = avg of sample window
      //        d = stddev of sample window
      //        m = min of sample window
      //        M = max of sample window
  
  return CR;
  }

// playback rate of report in MS
static uint8_t set_playback_rate(uart_config_t *config, uint8_t cmd, uint8_t *buffer, uint8_t len)
  {
  
  return CR;
  }

static uint8_t define_report(uart_config_t *config, uint8_t cmd, uint8_t *buffer, uint8_t len)
  {
      // 11 bit attribute format
      // 29 bit attribute format
      // Add attribute to report.
      //
      // [aA][CR] will clear the report definition
      // [a]000fff[CR] add all logged attributes to the report
      // [A]0000ffff[CR] add all logged attributes to the report (29 bit)
      // [a]nnn   add the specific ID to the report
      // [A]nnnn  add the specific 29 bit attribute to the report
      // [a]nnnlll  add the range of attributes to the report
      // [A]nnnnllll  add the range of 29 bit attributes to the report
  
  return CR;
  }

static uint8_t configure_logging(uart_config_t *config, uint8_t cmd, uint8_t *buffer, uint8_t len)
  {
      // add 11 bit attribute to logging
      // add a 29 bit attribute to logging
      // Add a tag to be data logged.  maximum number is 128 tags
      //
      //  [g]nnn[CR]    add a specific 11 bit id to the logger
      //  [G]nnnn[CR]   add a specific 29 bit id to the logger
  // immm[CR]   Set the sample rate in milliseconds
  // g[CR]      Return all logged tags with last known value (11 bit format)
  // G[CR]      Return all logged tags with last known value (29 bit format)
  return CR;
  }

static uint8_t set_acceptance_mask(uart_config_t *config, uint8_t cmd, uint8_t *buffer, uint8_t len)
  {
  
  return CR;
  }

static void process_serial(uart_config_t *config, uint8_t *buffer, uint8_t len)
  {
  if (buffer == 0)
    return;
  
  // we have a serial line
  uint8_t result = BELL;

  switch(buffer[0])
    {
    case 'S': 
      result = setup_standard_timing(config, buffer[0], buffer+1, len-1);
      break;
    case 's': 
      result = set_userdefined_timing(config, buffer[0], buffer+1, len-1);
      break;
    case 'V': 
      result = get_hardware_version(config, buffer[0], buffer+1, len-1);
      break;
    case 'v': 
      result = get_firmware_version(config, buffer[0], buffer+1, len-1);
      break;
    case 'N': 
      result = get_serial_number(config, buffer[0], buffer+1, len-1);
      break;
    case 'O': 
      result = set_state(config, buffer[0], buffer+1, len-1);
      break;
    case 'l': 
      result = set_state(config, buffer[0], buffer+1, len-1);
      break;
    case 'L': 
      result = set_state(config, buffer[0], buffer+1, len-1);
      break;
    case 'C': 
      result = set_state(config, buffer[0], buffer+1, len-1);
      break;
    case 'r': // Transmit standard RTR (11 bit) frame
    case 't': // Transmit standard (11 bit) frame
    case 'R': // Transmit extended RTR (29 bit) frame
    case 'T': // Transmit extended (29 bit) frame
      result = process_transmit_command(config, buffer[0], buffer+1, len-1);
      break;
    case 'F': 
      result = read_status_flags(config, buffer[0], buffer+1, len-1);
      break;
    case 'Z': // Set time stamping
      result = set_timestamping(config, buffer[0], buffer+1, len-1);
      break;
    case 'q' :
    case 'Q' :
      result = process_query_command(config, buffer[0], buffer+1, len-1);
      break;
    case 'P' :
      result = set_playback_rate(config, buffer[0], buffer+1, len-1);
      break;
    case 'a' :
    case 'A' :
      result = define_report(config, buffer[0], buffer+1, len-1);
      break;
    case 'g' :
    case 'G' :
      result = configure_logging(config, buffer[0], buffer+1, len-1);
      break;
    case 'm':
    case 'M':
      result = set_acceptance_mask(config, buffer[0], buffer+1, len-1);
      break;

    }

  // send the command reponse
  write_uart(config, result);
  }

static void process_can(const can_msg_t *msg)
  {
  int i;
  
  if(msg == 0)
    return;         // nothing there
  
  // see if this can be sent.  Don't want to overflow buffer
  // as it makes things worse!
  // overhead is taaal<CR> or 5 char
  uint16_t buffer_avail = write_buffer_avail(&uart_config);
  if(buffer_avail < (((msg->flags & DLC_MASK) << 1) + 5))
    {
    overflow_warning = true;
    return;
    }
  else
  
  write_uart(&uart_config, 't');
  send_nibble(&uart_config, msg->id >> 8);  // upper 3 bits
  send_hex(&uart_config, msg->id);     // lower 8 bits
  send_nibble(&uart_config, msg->flags & DLC_MASK);   // length
  
  for(i = 0; i < (msg->flags & DLC_MASK); i++)
    send_hex(&uart_config, msg->msg.raw[i]);
  
  write_uart(&uart_config, '\r');
  }
