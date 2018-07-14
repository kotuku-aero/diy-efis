#include "../../libs/neutron/neutron.h"
#include "../../libs/atom/analog_devices.h"
#include "../../libs/atom/input_capture_devices.h"
#include "../../libs/atom/uart_device.h"
#include "../../libs/atom/microkernel.h"
#include "../../libs/atom/eeprom.h"
#include "../../libs/atom/i2c.h"
#include "../../libs/muon/muon.h"
#include "../../libs/atom/memory.h"
#include "../../libs/atom16/pps_maps.h"
#include "edu_cli.h"

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include <xc.h>


#include <xc.h>

/*** DEVCFG0 ***/

#pragma config DEBUG =      OFF
#pragma config JTAGEN =     OFF
#pragma config ICESEL =     ICS_PGx1
#pragma config TRCEN =      OFF
#pragma config BOOTISA =    MIPS32
#pragma config FECCCON =    OFF_UNLOCKED
#pragma config FSLEEP =     OFF
#pragma config DBGPER =     PG_ALL
#pragma config EJTAGBEN =   NORMAL
#pragma config CP =         OFF

/*** DEVCFG1 ***/

#pragma config FNOSC =      SPLL
#pragma config DMTINTV =    WIN_127_128
#pragma config FSOSCEN =    OFF
#pragma config IESO =       OFF
#pragma config POSCMOD =    EC
#pragma config OSCIOFNC =   OFF
#pragma config FCKSM =      CSDCMD
#pragma config WDTPS =      PS1048576
#pragma config WDTSPGM =    STOP
#pragma config FWDTEN =     OFF
#pragma config WINDIS =     NORMAL
#pragma config FWDTWINSZ =  WINSZ_50
#pragma config DMTCNT =     DMT31
#pragma config FDMTEN =     OFF
/*** DEVCFG2 ***/

#pragma config FPLLIDIV =   DIV_2
#pragma config FPLLRNG =    RANGE_5_10_MHZ
#pragma config FPLLICLK =   PLL_POSC
#pragma config FPLLMULT =   MUL_80
#pragma config FPLLODIV =   DIV_2
#pragma config UPLLFSEL =   FREQ_24MHZ
/*** DEVCFG3 ***/

#pragma config USERID =     0xffff
#pragma config FMIIEN =     ON
#pragma config FETHIO =     ON
#pragma config PGL1WAY =    ON
#pragma config PMDL1WAY =   ON
#pragma config IOL1WAY =    ON
#pragma config FUSBIDIO =   ON

/*** BF1SEQ0 ***/

#pragma config TSEQ =       0x0000
#pragma config CSEQ =       0xffff

int main (void);

#define HARDWARE_REVISION 20
#define SOFTWARE_REVISION 1



static const char *node_id_name = "node-id";
static const char *tach_time_name = "tach-time";
 
#define CHT_CAL 815
#define EGT_CAL 845

#define CAN_TX_LENGTH 64
#define CAN_RX_LENGTH 32
#define NUM_I2C_TASKS 32
#define CAN_TX_LENGTH 64
#define CAN_RX_LENGTH 32
#define RX_BUFFER_LENGTH 128

static uint8_t pmag1_rx_buffer[RX_BUFFER_LENGTH];
static uint8_t pmag2_rx_buffer[RX_BUFFER_LENGTH];

/*
 * I/O Assignments
 * 
 * EGT1 - AN14
 * EGT2 - AN15
 * EGT3 - AN16
 * EGT4 - AN17
 * CHT1 - AN13
 * CHT2 - AN12
 * CHT3 - AN11
 * CHT4 - AN45
 * RIGHT_FUEL - AN4 **** CHECK
 * LEFT_FUEL - AN3
 * ISENSE - AN2
 * VSENSE - AN48
 * OILT - AN49
 * OILP - AN9
 * FUELP - AN10
 * OAT - AN18
 * MAP - AN7
 * 
 * U1TX - RPF5 = 1
 * U1RX - RF4 = U1RXR = 2
 * U2TX - RPD5 = 2
 * U2RX - RD4 = U2RXR = 4
 * 
 * FUEL_FLOW = IC4 = IC4R = 4 (RF0)
 * LEFT_TACH = IC2 = IC2R = 3 (RD0)
 * RIGHT_TACH = IC6 = IC6R = 0 (RD1)
 */

static void process_line(uart_config_t *config, unsigned char *, uint8_t length);

static uart_config_t pmag1_config = {
  .uart_number = 0,
  .rate = 9600,
  .flags = UART_EOL_CHAR,
  .eol_char = '\n',
  .rx_buffer = pmag1_rx_buffer,
  .rx_length = RX_BUFFER_LENGTH,
  .callback.uart_callback = process_line
  };

static uart_config_t pmag2_config = {
  .uart_number = 1,
  .rate = 9600,
  .eol_char = '\n',
  .flags = UART_EOL_CHAR,
  .rx_buffer = pmag2_rx_buffer,
  .rx_length = RX_BUFFER_LENGTH,
  .callback.uart_callback = process_line
  };

#define NUM_CAPTURE_CHANNELS 3

static capture_channel_definition_t capture_channel_definitions[NUM_CAPTURE_CHANNELS] = {
  { 
    .name = "fuel-flow",
    .channel = 4 | ACCUMULATOR_CHANNEL,
    .can_id = id_fuel_flow_rate,
    .publish_rate = 1000,
    .scale = 1800,
    .offset = 0,
  },
  { 
    .name = "left-tach",
    .channel = 1, 
    .can_id = id_engine_rpm_a,
    .publish_rate = 25,
    .scale = 30,
    .offset = 0,
  },
  { 
    .name = "right-tach",
    .channel = 6,
    .can_id = id_engine_rpm_b,
    .publish_rate = 25,
    .scale = 30,
    .offset = 0,
  },
};

/*  Capture channels are relatively slow.
 *
 * Given this we select a prescale and divisor that will give us a
 * 50 hz publish multiple
 *
 * 20,000,000 / 256 = 78125 hz
 * 78125 / 15625    = 5hz
 * 
 * 39063
 */
static capture_channels_t capture_channels = { capture_channel_definitions, numelements(capture_channel_definitions), 3, 15625 };

static analog_channel_definition_t analog_channel_definitions[] = {
  ///////////////////////////////////////////////
  // TODO: fix this...
  { 
    .name = "left-fuel",
    .channel = 3, 
    .can_id = id_left_fuel_quantity,
    .scale = 80.0,
    .offset = 0,
    .publish_rate=500
  },
  ///////////////////////////////////////////////
  { 
    .name = "right-fuel",
    .channel = 4, 
    .can_id = id_right_fuel_quantity,
    .scale = 80.0,
    .offset = 0,
    .publish_rate=500
  },
  ///////////////////////////////////////////////
  // EGT probles are based on an AD595 (Type K) with a voltage divider operating
  // on an 9v supply rail allowing for an 880 decgree C offset
  // 5v = 880 deg
  { 
    .name = "egt-1",
    .channel = 14,
    .can_id = id_exhaust_gas_temperature1,
    .scale = EGT_CAL,
    .offset = 273.15,
    .publish_rate=500
  },      
  {
    .name = "egt-2",
    .channel = 15,
    .can_id = id_exhaust_gas_temperature2,
     .scale = EGT_CAL,
    .offset = 273.15,
    .publish_rate=500
  },
  { 
    .name = "egt-3",
    .channel = 16,
    .can_id = id_exhaust_gas_temperature3,
     .scale = EGT_CAL,
    .offset = 273.15,
    .publish_rate=500
  },
  { 
    .name = "egt-4",
    .channel = 17,
    .can_id = id_exhaust_gas_temperature4,
     .scale = EGT_CAL,
    .offset = 273.15,
    .publish_rate=500
  },
  ///////////////////////////////////////////////
  // CHT probes are based on an Type J probe
  // The maximum temperature is 750 degrees which is 8.8181v
  // the voltage divider will generate a voltage of 4.85v 
  // range of ADC = 0-5vdc / 1024 = 0.0048828125 mv/step
  // 1 = 765 degrees c
  {
    .name = "cht-1",
    .channel = 13,
    .can_id = id_cylinder_head_temperature1,
    .scale = CHT_CAL,
    .offset = 273.15,
    .publish_rate=500
  },
  { 
    .name = "cht-2",
    .channel = 12,     
    .can_id = id_cylinder_head_temperature2,
    .scale = CHT_CAL,
    .offset = 273.15,
    .publish_rate=500
  },
  { 
    .name = "cht-3",
    .channel = 11,
    .can_id = id_cylinder_head_temperature3,
    .scale = CHT_CAL,
    .offset = 273.15,
    .publish_rate=500
  },
  { 
    .name = "cht-4",
    .channel = 45,
    .can_id = id_cylinder_head_temperature4,
    .scale = CHT_CAL,
    .offset = 273.15,
    .publish_rate=500
  },
  ///////////////////////////////////////////////
  // Manifold Pressure
  // Manifold pressure calculation:
  // range of channel = 15-115kpa == 0.2v - 4.7v
  // range of ADC = 0-5vdc / 1024 = 0.0048828125 mv/step
  // converted to 0.2v = 0.04 4.7v = 0.94
  // 4.7v = 1150hpa
  // 0.2v = 150hpa
  // scale = 1000/.90 = 1111.11111111111
  // offset = 150-(.04 * 1111.11111111111) = 105.5555555555556
  { 
    .name = "map",
    .channel = 7,
    .can_id = id_manifold_pressure,
    .scale = 1111.11111111111,
    .offset = 105.5555555555556,
    .publish_rate=500
    },
  ///////////////////////////////////////////////
  // Fuel Pressure
  // based on a 0-35 psi pressure gauge
  // 35 psi = 241.3165048 kpa
  // 0.2-4.7v
  // 4.5/0.0048828125 = 922
  // (1/1024)*922 = 0.900390625
  // 0.900390625 * N = 241.3165048
  // scale = 268.0131246368764
  // offset = (241.3165048/4.5)*.2
  // 0.2v = 10.72517799 KPa
  { 
    .name = "fuel-pressure",
    .channel = 10,
    .can_id = id_fuel_pressure,
    .scale = 268.0131246368764,
    .offset = -10.72517799,
    .publish_rate=500
  },
  ///////////////////////////////////////////////
  // DC volts
  { 
    .name = "dc-voltage",
    .channel = 48,
    .can_id = id_dc_voltage,
    .scale = 19.00,
    .offset = 0,
    .publish_rate=500
  },   
  ///////////////////////////////////////////////
  // DC Amps
  // Current Calculations
  // range of channel = 0-9v for 0-72A
  // 4.5v = 0
  // -72 = 1.0728v
  // +72 = 3.4272v
  // set up for 2 turns so 0-36A
  // 0-5v = 89v
  { 
    .name = "dc-amps",
    .channel = 2,
    .can_id = id_dc_current,
    .scale = 89.0,
    .offset = -46.5,
    .publish_rate=500
  },   
  ///////////////////////////////////////////////
  // Oil Temp
  // Temperature calculations.
  // The temp sensor has a
  // 49R4  resistance at 100 degrees
  // 1K3 resistance at 20 degrees
  // Pull-up is 680 ohm
  // Voltage @ 100 Degrees is 0.34v
  // Voltage @ 0 Degress is 4.21v
  // Volts/degree is 
  // using 0.0048828125 mv/step
  // 100 degrees = 70
  // 0 degrees = 863
  // 100 degrees = (1.0 / 1024) * 70 = 0.068359375
  // 0 degrees = (1.0 / 1024) * 863 = 0.842773438
  // difference between 0.842773438 and 0.068359375 is 0.774414063  == 100
  // so scale is -129.129886507
  // but is degrees kelvin so add 273 + 100 for offset
  { 
    .name = "oil-t",
    .channel = 49,
    .can_id = id_oil_temperature,
    .scale = -129.129886507,
    .offset = 373,
    .publish_rate=500
  }, 
  ///////////////////////////////////////////////
  // Oil Pressure
  // based on a 0-150 psi UMA gauge
  // 0-1034.2135935 KPa
  // 0.2-4.7v
  // 4.5/0.0048828125 = 922
  // (1/1024)*922 = 0.900390625
  // 0.900390625 * N = 1034.2135935
  // scale = 1148.6276786811280
  // offset = (1034.2135935/2.5)*.2
  // 0.2v = 82.73708748 KPa
  { 
    .name = "oil-p",
    .channel = 9,
    .can_id = id_oil_pressure,
    .scale = 1148.6276786811280,
    .offset = -82.73708748,
    .publish_rate=500
  },
  /*  
  ///////////////////////////////////////////////
  // OAT
  // based on 10mv/degree K
  // so 3.3v is 57 degrees
  { 
    .name = "oat",
    .channel = 18,
    .can_id = id_outside_air_temperature,
    .scale = 57,
    .offset = 0.0,
    .publish_rate=500
  },   
  */
  };

uint16_t select_channel(uint16_t channel)
  {
  return channel;
  }
  
static analog_channels_t channels =
  {
  .channel_definition = analog_channel_definitions,
  .num_channels = numelements(analog_channel_definitions),  // number of channels
  .analog_factor = 0.000244140625,       // 12 bit ADC
  .channel_select = select_channel
};

static uint32_t last_tick;
static bool engine_running = false;
static uint32_t tach_time_value;

#define BITRATE 	125000               // 125kbs
#define PRSEG_LEN   1
#define PRSEG_VAL   (PRSEG_LEN -1)
#define SEG1PH_LEN  5
#define SEG1PH_VAL  (SEG1PH_LEN -1)
#define SEG2PH_LEN  3
#define SEG2PH_VAL  (SEG2PH_LEN -1)
#define NTQ 		(1 + PRSEG_LEN + SEG1PH_LEN + SEG2PH_LEN) // Number of Tq cycles which will make the
//CAN Bit Timing .
//#define BRP_VAL		((FCY/(2* NTQ * BITRATE))-1)  //Formulae used for C1CFG1bits.BRP
#define BRP_VAL 27


static neutron_parameters_t init_params = {
  .hardware_revision = HARDWARE_REVISION,
  .software_revision = SOFTWARE_REVISION,
  .node_type = unit_edu,
  .node_id = edu_node_id
  };

#define EEPROM_SIZE (128L * 1024L)    // eeprom is 128k bytes


// called when a msg arrives
bool ev_msg(const canmsg_t *msg, void *parg)
  {
  if(msg == 0)
    return false;

  switch(msg->id)
    {
    }
  
  return true;
  }

static msg_hook_t functions_hook = { 0, 0, ev_msg, 0 };


static void edu_task(void *parg)
  {
  result_t result;
  
  // TODO: pull-ups!
  
  bool init_eeprom = PORTBbits.RB6 == 0;
  result = eeprom_init(init_eeprom, I2C_CHANNEL_1, EEPROM_SIZE);

  int i;
  if(result == e_not_initialized)
    {
    // fuel consumption
    // node id
    init_eeprom = true;
    reg_set_uint16(0, node_id_name, init_params.node_id);
    reg_set_uint32(0, tach_time_name, 0);
    }

  // fuel consumption
  // node id
  uint16_t value;
  if(succeeded(reg_get_uint16(0, node_id_name, &value)))
    init_params.node_id = (uint8_t)value;

  // read the 
  reg_get_uint32(0, tach_time_name, &tach_time_value);
  
  can_aerospace_init(&init_params, init_eeprom, true);
  
  // TODO: read other config values that are set....
  deque_create(sizeof(char), RX_BUFFER_LENGTH, &pmag1_config.rx_queue);

  // create the left mag worker
  open_uart(&pmag1_config, DEFAULT_STACK_SIZE, DEFAULT_STACK_SIZE);

  send_str(&pmag1_config, "/I1\r");
 // TODO: read other config values that are set....
  deque_create(sizeof(char), RX_BUFFER_LENGTH, &pmag2_config.rx_queue);

  // create the left mag worker
  open_uart(&pmag2_config, DEFAULT_STACK_SIZE, DEFAULT_STACK_SIZE);

  send_str(&pmag2_config, "/I1\r");
  
  // create the analog worker task
  analog_init(&channels, DEFAULT_STACK_SIZE, init_eeprom);
  // create the capture channels
  capture_init(&capture_channels, DEFAULT_STACK_SIZE);
  
  // hook the publisher
  subscribe(&functions_hook);

  // start the command interpreter running
  muon_initialize_cli(&edu_cli_root);
  
  // start ion running
  publish_task(0);
  }

const char *node_name = "edu";

typedef struct init_port_t {
  volatile unsigned int *addr;
  uint32_t mask;
  uint32_t value;
  } init_port_t;
  
static init_port_t init_ports[] = {
  &PB2DIV, 0x0000007F, 0x00000003,          // 50Mhz = 200/4
  &PB5DIV, 0x0000007F, 0x00000003,          // 50Mhz = 200/4
  &PB3DIV, 0x0000007F, 0x00000013,          // 10mhz = 200/20
  &INTCON, 0x00001000, 0x00001000,          // Multi-vector mode
  &ANSELB, 0x00000040, 0x00000000,          // AN46 is digital input
  &CNPUB,  0x00002840, 0x00002840,          // RB6 pullup, RB13, RB11 pullup as not used
  &CNPUD,  0x00000E00, 0x00000E00,          // RD9, RD10, RD11 pullup as not used
  &CNPUF,  0x00000018, 0x00000018,          // RF3, RF4 pullup as not used
  &TRISF,  0x00000022, 0x00000000,          // RF5, RF1 output
  &TRISD,  0x00000020, 0x00000000,          // RD5 output
  &RPF1R,  0x0000001F, 0x0000000F,          // RPF1     C1TX
  &C1RXR,  0x0000001F, 0x00000003,          // RPD11    C1RX
  &C1CON,  0x00008000, 0x00008000,          // Turn ON CAN controller
  &RPF5R,  0x0000001F, 0x00000001,          // RPF5     U1TX
  &U1RXR,  0x0000001F, 0x00000002,          // RPF4     U1RX
  &RPD5R,  0x0000001F, 0x00000002,          // RPD5     U2TX
  &U2RXR,  0x0000001F, 0x00000004,          // RPD4     U2RX
  &IC1R,   0x0000001F, 0x00000003,          // RPD0     IC1
  &IC6R,   0x0000001F, 0x00000000,          // RPD1     IC6
  &IC4R,   0x0000001F, 0x00000004,          // RPF0     IC4
  &IC1CON, 0x00008103, 0x00008103,          // Capture rising edge, 32bit timer
  &IC4CON, 0x00008103, 0x00008103,          // Capture rising edge, 32bit timer
  &IC6CON, 0x00008103, 0x00008103,          // Capture rising edge, 32bit timer
  0, 0
  };


int main(void)
  {
  init_port_t *port_init = init_ports;
  
  SYSKEY = 0xAA996655;
  SYSKEY = 0x556699AA;
  
  while(port_init->addr != 0)
    {
    volatile unsigned int *port = port_init->addr;
    uint32_t value = *port;
    value &= ~port_init->mask;
    value |= port_init->value;
    *port = value;
    port_init++;
    }
  
  SYSKEY = 0x33333333;
 
  // lock the peripherals.
  //__builtin_write_OSCCONL(OSCCON | (1<<6));
  
  // pick up a small malloc amount to get the heap pointer
  uint32_t mem = (uint32_t) malloc(4);
  mem = ((mem -1) | 0x0f)+1;

  // 480k of memory
  size_t length = 0x78000;
  
  neutron_run((void *)mem, length, "EDU", DEFAULT_STACK_SIZE, edu_task, 0, NORMAL_PRIORITY, 0);

  return 0;
  }

// P-MAG Communications protocol
//------------------------------------------------------------------------------
//
//	COMMUNICATION COMMANDS:
// (Upper case = command  /  Lower case = data request)
// Format: /(char)(CR)    for normal commands  (CR= carriage return / 0D hex)
//
// 	- bypass setup mode (only at startup)
//	  [this command is the same as momentarily switching the kill switch to the "run" position]
//
//	/F	- send full config info
//
//	/Ix  	- Streaming data mode 1=on-Hex (/I1) / 0=off (/I0) 2=on binary (/I2)
//			ASCII: = xx xx   xx   xx   xx    xx    xx    xx  xx  CR
//	            rpm rpm  adv  map  mode  volt  temp  C1  C2 (CR)  //  (C1,C2 = coil charge time)
//			BINARY:- r  r    a    m    m     v     t     c1  c2 (CR)
//
//	/J 	- Constellation shift enable  1=disabled / 0=enabled   (/J1 or /J0)
//------------------------------------------------------------------------------
//
//	COMMUNICATION COMMANDS:
// (Upper case = command  /  Lower case = data request)
// Format: /(chr)(CR)    for normal commands
//
//	* 	- bypass setup mode (only at startup)
//	A /   	- address assignment for multi-drop
//	    b 	- send target advance angle
//	C / c 	- constellation shift (+/- 1 digit)
//	D / d 	- cycle delay (1 digit)
//	    e 	- send temperature (degrees C - 1 byte)
//	F	- send full config info
//	  / f 	- send flash zero index position
//	G / g 	- map calibration set
//	    h 	- send run time
//	I  	- Streaming data mode 1=on-Hex / 0=off / 2=on-Binary
//			ASCII: =xx xx  xx xx xx xx xx xx xx CR
//	           =rpm rpm  adv  map  mode  volt  temp  C1  C2 (CR)  C1,C2 = coil charge time)
//			BINARY: -r r  a  m  m  v  t  c1  c2
//	J / j 	- Constellation shift enable  1=disabled / 0=enabled
//	k 	- send coil discharge time
//	L / l 	- LED mode
//				0=kill input
// 				1=start/run	(red/green)
// 				2=diagnostic
// 				3=position sensor flags  INC / DEC  (default)
//	M / m 	- max rpm set (2 digits x100rpm)
//	N / n 	- max advance
//
//  	PZ / p 	- RESET Index Zero to current pos /read angular position
//	q 	- read and send shaft position  - full 32 bit
//	R	- Set rotation direction (1=LH / 0=RH) Default=0
//        r 	- read rotation direction data
//	SM    	- set manual advance mode
//	SA	- set auto advance mode
//	S+    	- manual advance angle increment (advances one increment 1.4deg)
//	S-	- manual advance angle decrement (1.4 deg) (range limit = 0-45)
//	T / t	- tach output 0,1,2 ppr
//	u 	- firmware rev level
//	V / v 	- Tach volts (removed from version 31+)
//	W     	- write configuration flash
//	w 	- buss volts (V=6.75+(w*0.3883))
//	x 	- rpm data   (2 bytes)
//	y 	- map data   (M=     )
//	z 	- max temperature (degrees C - 1 byte)
//	Z	- WatchDogTimer test - locks up cpu (caution will cause restart)
//
//
static const char *delim = " ";

// process the returned data
static void process_line(uart_config_t *config, unsigned char *buffer, uint8_t length)
  {
  canmsg_t msg;
  char *token = (char *) buffer;
  // process the line
  if(buffer == 0)
    return;
    
  token = strtok(token, delim);
  can_send(create_can_msg_int16(&msg, config->uart_number == 0 ? id_left_mag_rpm : id_right_mag_rpm, 0, (int16_t) atoi(token)));
  
  token = strtok(0, delim);
  
  token = strtok(0, delim);
  can_send(create_can_msg_int16(&msg, config->uart_number == 0 ? id_left_mag_adv : id_right_mag_adv, 0, (int16_t) atoi(token)));
  
  token = strtok(0, delim);
  can_send(create_can_msg_int16(&msg, config->uart_number == 0 ? id_left_mag_map : id_right_mag_map, 0, (int16_t) atoi(token)));
  
  token = strtok(0, delim);
  
  token = strtok(0, delim);
  can_send(create_can_msg_int16(&msg, config->uart_number == 0 ? id_left_mag_volt : id_right_mag_volt, 0, (int16_t) atoi(token)));
  
  token = strtok(0, delim);
  can_send(create_can_msg_int16(&msg, config->uart_number == 0 ? id_left_mag_temp : id_right_mag_temp, 0, (int16_t) atoi(token)));
  
  token = strtok(0, delim);
  can_send(create_can_msg_int16(&msg, config->uart_number == 0 ? id_left_mag_coil1 : id_right_mag_coil1, 0, (int16_t) atoi(token)));
  
  token = strtok(0, delim);
  can_send(create_can_msg_int16(&msg, config->uart_number == 0 ? id_left_mag_coil2 : id_right_mag_coil2, 0, (int16_t) atoi(token)));
  
  return;
  }

void panic()
  {
  // restart the system using the brown-out
#ifdef _DEBUG
  while(true);
#else
  //Reset();
#endif
  }

void malloc_failed_hook(uint16_t size)
  {
  panic();
  }

#if defined(_DEBUG)
void platform_trace(uint16_t level, const char *msg, va_list va)
  {
  }
#endif
