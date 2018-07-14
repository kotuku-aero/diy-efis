#include "../../diy-efis/libs/neutron/neutron.h"
#include "../../libs/atom/uart_device.h"
#include "../../libs/atom/microkernel.h"
#include "../../libs/atom/eeprom.h"
#include "../../libs/atom/i2c_mmx_driver.h"
#include "../../libs/atom16/pps_maps.h"
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
/**
 * Receive a buffer from the serial interface and convert to a can message
 * @param config
 * @param 
 * @param len
 */
static void process_serial(uart_config_t *uart, uint8_t *buffer, uint8_t len);

#define RX_BUFFER_LENGTH 128
static uint8_t rx_buffer[RX_BUFFER_LENGTH];

#define TX_BUFFER_LENGTH 32

static bool factory_reset = false;

static uart_config_t uart_config = {
                                    .uart_number = 1,
                                    .rate = 57600,
                                    .eol_char = '\n',
                                    .rx_buffer = rx_buffer,
                                    .rx_length = RX_BUFFER_LENGTH,
                                    .eol_char = '\r',
                                    .flags = UART_EOL_CHAR,
                                    .callback.uart_callback = process_serial,
  };

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

static can_state_t can_state;

typedef enum _config_state
  {
  config_can,
  config_serial,
  config_complete
  } config_state;

static neutron_parameters_t init_params = {
                                           .node_name = "diy-efis",
                                           .node_type = 0,
                                           .hardware_revision = 0x30,
                                           .software_revision = 0x10,
  };

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

#define MSG_BUFFER_LEN  256
static deque_p can_msg_buffer;


#define I_PLLPRE 0
#define I_PLLPOST 0
#define I_PLLDIV ((((FCY * 2)/I_OSC)*(I_PLLPRE+2)*(2*(I_PLLPOST+1)))-2)

static bool process_can(const canmsg_t *msg, void *parg)
  {
  if(msg == 0)
    return false;
  // push it onto the worker queue
  if(failed(push_back(can_msg_buffer, msg, 0)))
    {
    // set the overflow flag
    overflow_warning = true;
    }
  
  return true;
  }
static msg_hook_t hook = { 0, 0, process_can };

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

/**
 * Interprets given line and transmit can message
 *
 * @param line Line string which contains the transmit command
 * @returns true if the message was posted, false if queue full
 */
static bool transmit_can(uart_config_t *config, uint8_t *line)
  {
  if (can_state == can_closed)
    return true;

  unsigned long temp;
  uint8_t idlen;
  uint8_t *ptr;

  canmsg_t canmsg;
  ptr = (uint8_t *) & canmsg;

  for (temp = 0; temp < sizeof (canmsg_t); temp++)
    *ptr++ = 0;

  if (((line[0] == 'r') || (line[0] == 'R')))
    canmsg.reply = 1;

  // upper case -> extended identifier
  if (line[0] < 'Z')
    {
    idlen = 8;
    }
  else
    idlen = 3;

  if (!parse_hex(&line[1], idlen, &temp))
    return 0;
  canmsg.id = temp;

  if (!parse_hex(&line[1 + idlen], 1, &temp))
    return 0;

  canmsg.length = temp;

  if (canmsg.reply == 0)
    {
    uint8_t i;
    uint8_t length = canmsg.length;
    if (length > 8)
      length = 8;
    for (i = 0; i < length; i++)
      {
      if (!parse_hex(&line[idlen + 2 + i * 2], 2, &temp))
        return 0;
      canmsg.raw[i] = temp;
      }
    }

  can_send_raw(&canmsg);

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
  return CR;
  }

// Read status flags

static uint8_t read_status_flags(uart_config_t *config, uint8_t cmd, uint8_t *buffer, uint8_t len)
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
  write_uart(config, 'F');
  send_hex(config, status);

  overflow_warning = false;

  return CR;
  }

static uint8_t process_transmit_command(uart_config_t *config, uint8_t cmd, uint8_t *buffer, uint8_t len)
  {
  if (can_state != can_open &&
      can_state != can_loopback)
    return BELL;

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

  switch (buffer[0])
    {
    case 'S':
      result = setup_standard_timing(config, buffer[0], buffer + 1, len - 1);
      break;
    case 's':
      result = set_userdefined_timing(config, buffer[0], buffer + 1, len - 1);
      break;
    case 'V':
      result = get_hardware_version(config, buffer[0], buffer + 1, len - 1);
      break;
    case 'v':
      result = get_firmware_version(config, buffer[0], buffer + 1, len - 1);
      break;
    case 'N':
      result = get_serial_number(config, buffer[0], buffer + 1, len - 1);
      break;
    case 'O':
      result = set_state(config, buffer[0], buffer + 1, len - 1);
      break;
    case 'l':
      result = set_state(config, buffer[0], buffer + 1, len - 1);
      break;
    case 'L':
      result = set_state(config, buffer[0], buffer + 1, len - 1);
      break;
    case 'C':
      result = set_state(config, buffer[0], buffer + 1, len - 1);
      break;
    case 'r': // Transmit standard RTR (11 bit) frame
    case 't': // Transmit standard (11 bit) frame
    case 'R': // Transmit extended RTR (29 bit) frame
    case 'T': // Transmit extended (29 bit) frame
      result = process_transmit_command(config, buffer[0], buffer + 1, len - 1);
      break;
    case 'F':
      result = read_status_flags(config, buffer[0], buffer + 1, len - 1);
      break;
    case 'Z': // Set time stamping
      result = set_timestamping(config, buffer[0], buffer + 1, len - 1);
      break;
    case 'q':
    case 'Q':
      result = process_query_command(config, buffer[0], buffer + 1, len - 1);
      break;
    case 'P':
      result = set_playback_rate(config, buffer[0], buffer + 1, len - 1);
      break;
    case 'a':
    case 'A':
      result = define_report(config, buffer[0], buffer + 1, len - 1);
      break;
    case 'g':
    case 'G':
      result = configure_logging(config, buffer[0], buffer + 1, len - 1);
      break;
    case 'm':
    case 'M':
      result = set_acceptance_mask(config, buffer[0], buffer + 1, len - 1);
      break;

    }

  // send the command reponse
  write_uart(config, result);
  }

static uint16_t keys = 0;

static void test_key(uint16_t portb, uint16_t mask, uint16_t id)
  {
  uint16_t old_keys = keys & mask;
  uint16_t masked_bits = portb & mask;

  if(old_keys != masked_bits)       // test for change
    {
    keys &= ~mask;            // kill bit
    keys |= masked_bits;      // new key
    
    canmsg_t msg;
    
    // if the key is pressed portb will be 0, so send a 1
    process_can(create_can_msg_int16(&msg, id, 0, masked_bits == 0 ? 1 : 0), 0);
    }
  }

static const int16_t transitions[16] = 
  {
   0, -1,  1,  0, 
   1,  0,  0, -1,
  -1,  0,  0,  1,
   0,  1, -1,  0
  };

static void test_encoder(uint16_t portb, uint16_t a_mask, uint16_t b_mask, uint16_t id)
  {
  // encoder is as follows
  //    A       B
  //    0       0
  //    1       0
  //    1       1
  //    0       1
  uint16_t old_keys = keys & (a_mask | b_mask);
  uint16_t masked_bits = portb & (a_mask | b_mask);
  
  if(old_keys != masked_bits)
    {
    uint16_t state = 0;
    
    if(old_keys & a_mask)
      state |= 8;
    if(old_keys & b_mask)
      state |= 4;
    
    if(masked_bits & a_mask)
      state |= 2;
    if(masked_bits & b_mask)
      state |= 1;
    
    keys &= ~(a_mask | b_mask);
    keys |= masked_bits;
    
    if(transitions[state] != 0)
      {
      canmsg_t msg;
      process_can(create_can_msg_int16(&msg, id, 0, transitions[state]), 0);
      }
    }
  }

static void keys_worker(void *parg)
  {
  uint16_t key0_id;
  uint16_t decka_id;
  uint16_t deckb_id;
  
  semaphore_p worker_semp;
  
  semaphore_create(&worker_semp);
  
  if(failed(reg_get_uint16(0, "key0-id", &key0_id)))
    key0_id = id_user_defined_start + 100;
  
  if(failed(reg_get_uint16(0, "decka-id", &decka_id)))
    decka_id = id_user_defined_start + 101;
  
  if(failed(reg_get_uint16(0, "deckb-id", &deckb_id)))
    deckb_id = id_user_defined_start + 102;
  
  while(true)
    {
    // the worker tests the keys every 100msec
    semaphore_wait(worker_semp, 100);
    
    test_key(PORTB, 1 << 14, key0_id);
    test_encoder(PORTA, 1 << 0, 1 << 1, decka_id);
    test_encoder(PORTB, 1 << 0, 1 << 1, deckb_id);
    }  
  }


#define EEPROM_SIZE (128L * 1024L)    // eeprom is 128k bytes

static void main_task(void *parg)
  {
  
  // initialize the eeprom as we need our settings first
  eeprom_init(factory_reset, I2C_CHANNEL_2, EEPROM_SIZE);

  uint16_t node_id;
  if (failed(reg_get_uint16(0, "node-id", &node_id)))
    node_id = 200;

  init_params.node_id = node_id;

  // start the canbus stuff working
  neutron_init(&init_params, factory_reset);
  
  // create a queue
  deque_create(sizeof(canmsg_t), MSG_BUFFER_LEN, &can_msg_buffer);
  
  subscribe(&hook);

  int i;

  uint16_t value;
  
  task_create("KEYS", DEFAULT_STACK_SIZE, keys_worker, 0, NORMAL_PRIORITY, 0);
  
  // TODO: read other config values that are set....
  if (failed(reg_get_uint16(0, "baud-rate", &value)))
    value = 57600;

  uart_config.rate = value;

  open_uart(&uart_config, DEFAULT_STACK_SIZE, DEFAULT_STACK_SIZE);

  // and publish our values
  //register_datapoints(published_parameter, numelements(published_parameter));
  
  // now we just spin
  while(true)
    {
    canmsg_t msg;
    if(succeeded(pop_front(can_msg_buffer, &msg, INDEFINITE_WAIT)))
      {
      if(can_state > can_closed)
        {
        write_uart(&uart_config, 't');
        send_nibble(&uart_config, msg.id >> 8); // upper 3 bits
        send_hex(&uart_config, msg.id); // lower 8 bits
        send_nibble(&uart_config, msg.length); // length

        for (i = 0; i < msg.length; i++)
          send_hex(&uart_config, msg.raw[i]);

        write_uart(&uart_config, CR);  
        }
      }
    }
  }

int main(void)
  {
  TRISAbits.TRISA0 = 1;
  TRISAbits.TRISA1 = 1;
  TRISBbits.TRISB0 = 1;
  TRISBbits.TRISB1 = 1;
  TRISBbits.TRISB12 = 1;
  CNPUAbits.CNPUA0 = 1;
  CNPUAbits.CNPUA1 = 1;
  CNPUBbits.CNPUB0 = 1;
  CNPUBbits.CNPUB1 = 1;
  CNPUBbits.CNPUB12 = 1;
  
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
  // map pins
  map_rpo(RP36R, rpo_c1tx);
  map_rpi(rpi_rpi45, rpi_c1rx);
  map_rpo(RP40R, rpo_u1tx);
  map_rpi(rpi_rpi47, rpi_u1rx);
  

  factory_reset = PORTBbits.RB12 == 0;
  
  scheduler_init();
  
  task_create("MAIN", DEFAULT_STACK_SIZE, main_task, 0, NORMAL_PRIORITY, 0);
   
  // and start dispatching
  neutron_run();

  
  return 0;
  }
