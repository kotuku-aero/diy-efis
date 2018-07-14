#include "ahrs.h"
#include "../../libs/muon/muon.h"
#include "../../libs/atom/memory.h"
#include "../../libs/atom16/pps_maps.h"
#include "ahrs_cli.h"

#include <math.h>
#include <string.h>

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

#define DIFF_PRESSURE_CHANNEL 2
#define STATIC_PRESSURE_CHANNEL 1

#define HARDWARE_REVISION 3
#define SOFTWARE_REVISION 6

const char *node_name = "ahrs";

ahrs_data_t ahrs_state;

#define ANALOG_PUBLISH_RATE 1000
#define NUM_ANALOG_CHANNELS 2

// storage is allocated here, but will be filled in when analog_init is called.
static analog_channel_definition_t channel_definitions[NUM_ANALOG_CHANNELS] = {
  {  
    .name = "diff-pres",
    .channel = DIFF_PRESSURE_CHANNEL, 
    .scale = 111.1111111,
    .offset = -4.444444444,
    .can_id = id_differential_pressure,
    .loopback = true,
    .publish_rate = 50
  },          // altitude
  {  
    .name = "static-pres",
    .channel = STATIC_PRESSURE_CHANNEL,
    .can_id = id_static_pressure,
    .scale = 1111.111111,
    .offset = 90.0,
    .loopback = true,
    .publish_rate = 50
  },          // airspeed
  };

  
static analog_channels_t channels =
  {
  .channel_definition = channel_definitions,
  .num_channels = NUM_ANALOG_CHANNELS,  // number of channels
  .analog_factor = 0.000244140625       // 12 bit ADC
};


static neutron_parameters_t init_params = {
  .hardware_revision = HARDWARE_REVISION,
  .software_revision = SOFTWARE_REVISION,
  .node_type = unit_ahrs,
  .node_id = ahrs_node_id,
  };

static const char *node_id_name = "node-id";
static const char *airtime_name = "airtime";
static const char *qnh_name = "qnh";
static const char *hdg_name = "hdg";

#define EEPROM_SIZE (128L * 1024L)    // eeprom is 128k bytes

static void adjust(const char *name,
                   uint16_t can_id,
                   uint16_t min_value,
                   uint16_t max_value,
                   uint16_t delta,
                   bool wrap_around)
  {
  canmsg_t msg;
  uint16_t old_value = min_value;
  reg_get_uint16(0, name, &old_value);
  
  uint16_t value = old_value;
  value += delta;

  if (value < min_value)
    value = wrap_around ? max_value : min_value;
  else if (value > max_value)
    value = wrap_around ? min_value : max_value;

  if(old_value != value)
    {
    can_send(create_can_msg_uint16(&msg, can_id, 0, value));
    reg_set_uint16(0, name, value);
    }
 
  }

#define air_density  1.225
#define speed_of_sound 340.29471111
#define isa_pressure  1013.25
#define std_air_temp 288.15
#define speed_f_sound 340.29471111

// hpa per 1000ft at 15 deg c = 36.0843
#define hpa_per_m  0.118386811023622
static uint32_t last_tick = 0;

// how long the aircraft has been flying (hobbs meter)
static uint32_t airtime;

static uint16_t oat = 18 + 273;

static void calculate_airspeed(float dynamic_pressure)
  {
  canmsg_t msg;
  // This is calculated from a reduction of bernoulis theorum
  if (dynamic_pressure < 0)
    dynamic_pressure = 0;

  // the value is Hpa which is 100pa
  // This is calculated from a reduction of bernoulis theorum
  // pd = 0.5*p*(v*v)
  //
  // where pd is the dynamic pressure
  //  p is the density of air or 1.225 kg/m3
  //  v is the velocity in m/sec
  // replacing for pd yields
  // v = sqrt((2*pd)/1.225)
  // v = sqrt(2*pd*0.8163265306122449)
  // v = sqrt(pd*1.63265306122449)
  // where pd is in pascals which is 100x hpa so
  float indicated_airspeed = sqrt(dynamic_pressure * 163.265306122449);
  float true_airspeed;
  
  // if speed is < 40kts then we trim it
  if (indicated_airspeed < 20)
    {
    indicated_airspeed = 0;
    true_airspeed = 0;
    }
  else
  {
    // aircraft going > 20m/s
    // get the 1ms tick count
    uint32_t now = ticks();

    uint32_t runtime = now - last_tick;
    // calculate the time in MS since the last tick

    // the engine runtime is in hours, only update each 1/100 hour
    // which is 36 seconds
    if (runtime > 35999)
    {
      uint32_t run_secs = runtime / 1000;
      uint32_t run_hours = run_secs / 3600;
      // remove whole hours (normally 0)
      run_secs -= run_hours * 3600;
      // calculate 1/100's of an hour to add
      uint32_t run_parts = run_secs / 36;

      airtime += run_hours * 100;
      airtime += run_parts;

      // send to the can bus
      can_send(create_can_msg_uint32(&msg, id_air_time, 0, airtime));
      reg_set_uint32(0, airtime_name, airtime);

      // remove our calculated hours
      run_secs = (run_parts * 36) + (run_hours * 3600);
      // back to MS
      run_secs *= 1000;

      // and remove from accumulator
      runtime -= run_secs;

      // keep the overflow seconds accumulating by adding back the
      // amount accumulates
      last_tick = now - runtime;
    }
  

    // calculate CAS
    true_airspeed = speed_of_sound *
      sqrt(((5 * oat) / 15) *
      pow(dynamic_pressure / isa_pressure + 1, 0.28571428571423) - 1);
    }

  can_send(create_can_msg_float(&msg, id_indicated_airspeed, 0, indicated_airspeed));
  can_send(create_can_msg_float(&msg, id_true_airspeed, 0, true_airspeed));
  }

static float last_altitude = 0;
static uint32_t last_altitude_time = 0;
static uint16_t qnh = 1013;

static void calculate_altitude(float value)
  {
  canmsg_t msg;

  // round to value
  value = floor(value + 0.5);

  uint32_t now = ticks();

  //pressure_altitude = (1-pow(_value/1013.25, 0.190284)) * 44307.694;
  float pressure_altitude = (1 - pow(value * 0.000987167, 0.190284)) * 44307.694;
  float baro_corrected_altitude = (1 - pow(value / qnh, 0.190284)) * 44307.694;

  // we calculate the id_baro_corrected_altitude
  can_send(create_can_msg_float(&msg, id_baro_corrected_altitude, 0, baro_corrected_altitude));
  can_send(create_can_msg_float(&msg, id_pressure_altitude, 0, pressure_altitude));

  uint32_t num_vs_ticks = now - last_altitude_time;
  // update vertical speed every 500msec
  if (num_vs_ticks > 500) 
    {
    // calculate how may 500msec slots we are calculating for.
    uint32_t num_incr = num_vs_ticks / 500;

    float vertical_speed = baro_corrected_altitude - last_altitude;

    // vertical speed is m/sec, calculate to num 0.5 secs
    vertical_speed /= num_incr;
    vertical_speed *= 2;       // convert to m/s

    num_incr = num_vs_ticks;
    num_incr %= 500;          // how much time of the last tick to calc

    last_altitude_time = now - num_incr;
    last_altitude = baro_corrected_altitude;

    can_send(create_can_msg_float(&msg, id_altitude_rate, 0, vertical_speed));
  }
}

// called when a msg arrives
bool ev_msg(const canmsg_t *msg, void *parg)
  {
  if(msg == 0)
    return false;

  switch(msg->id)
    {
    case id_qnh_up :
      adjust(qnh_name, id_qnh, 800, 1050, 1, false);
      break;
    case id_qnh_dn :
      adjust(qnh_name, id_qnh, 800, 1050, -1, false);
      break;
    case id_heading_up :
      adjust(hdg_name, id_heading_angle, 0, 359, 1, true);
      break;
    case id_heading_dn :
      adjust(hdg_name, id_heading_angle, 0, 359, -1, true);
      break;
    case id_differential_pressure :
      {
      float pressure;
      if(succeeded(get_param_float(msg, &pressure)))
        calculate_airspeed(pressure);
      }
      break;
    case id_static_pressure :
      {
      float pressure;
      if(succeeded(get_param_float(msg, &pressure)))
        calculate_altitude(pressure);
      }
      break;
    case id_outside_air_temperature :
      {
      uint16_t temp;
      if(succeeded(get_param_uint16(msg, 0, &temp)))
        oat = temp;
      }
      break;
    }
  
  return true;
  }

static msg_hook_t functions_hook = { 0, 0, ev_msg, 0 };

static void ahrs_task(void *parg)
  {
  result_t result;
#if defined(__dsPIC33EP512GP504__)
  bool init_eeprom = PORTCbits.RC5 == 0;
#else
  bool init_eeprom = PORTDbits.RD9 == 0;
#endif

  
  semaphore_create(&ahrs_state.gps_position_updated);
#if defined(__dsPIC33EP512GP504__)
  result = eeprom_init(init_eeprom, I2C_CHANNEL_2, EEPROM_SIZE);
#else
  result = eeprom_init(init_eeprom, I2C_CHANNEL_4, EEPROM_SIZE);
#endif

  if(result == e_not_initialized)
    {
    init_eeprom = true;
    }
  
  can_aerospace_init(&init_params, false, true);
  // we are nearly ready to run, now we set default values...

  if(init_eeprom)
    {
    reg_set_uint16(0, hdg_name, 0);
    reg_set_uint16(0, qnh_name, 1013);
    
    }
  
  uint16_t node_id = ahrs_node_id;
  
  if(succeeded(reg_get_uint16(0, node_id_name, &node_id)))
    init_params.node_id = node_id;
  
  if(failed(reg_get_uint32(0, airtime_name, &airtime)))
    airtime = 0;

  analog_init(&channels, DEFAULT_STACK_SIZE, init_eeprom);
  gps_init(init_eeprom);
  imu_init(init_eeprom);
  // hook the publisher
  subscribe(&functions_hook);

  // start the command interpreter running
  muon_initialize_cli(&ahrs_cli_root);
  
  // start ion running
  publish_task(0);
  }

/* Hardware config:
 *  RPB3(13) -> GPSRX (UART4TX)
 *  RPB2(14) -> GPSTX (UART4RX)
 *  SDA4(5) -> SDA4
 *  SCL4(6) -> SCL5
 *  RPB8(22)(41) -> CANTX
 *  RPB9(15)(42) -> CANRX
 * 
 *  AN46 -> static pressure
 *  AN4 -> dynamic pressure
 */

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
  &TRISD,  0x0000FFFF, 0x0000FFFF,
  &CNPUD,  0x0000FFFF, 0x0000FFFF,          // Pullups on Port D
  &ANSELE, 0x0000FFFF, 0x00000000,          // all pins digital
  &ANSELG, 0x0000FFFF, 0x00000000,
  &U4RXR,  0x0000003F, 0x00000006,          // RPB2   U4RX
  &PORTB,  0x00000004, 0x00000004,          // force UART high
  &C1CON,  0x00008000, 0x00008000,          // Turn ON CAN controller
  &TRISB,  0x00000006, 0x00000000,          // RPB5,6 outputs
  &TRISE,  0x00000020, 0x00000020,          // RPE5 input
  &RPB5R,  0x0000001F, 0x0000000F,          // RPB5     C1TX
  &C1RXR,  0x0000001F, 0x00000006,          // RPE5     C1RX
  &PORTD,  0x000000FF, 0x000000FF,
  &TRISG,  0x00000180, 0x00000000,          // RG7, RG8 outputs
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
  
  // pick up a small malloc amount to get the heap pointer
  uint32_t mem = (uint32_t) malloc(4);
  mem = ((mem -1) | 0x0f)+1;

  // 480k of memory
  size_t length = 0x78000;
  
  neutron_run((void *)mem, length, "AHRS", DEFAULT_STACK_SIZE, ahrs_task, 0, NORMAL_PRIORITY, 0);

  return 0;
  }

void panic()
  {
  // restart the system using the brown-out
#ifdef _DEBUG
  while(true);
#else
#if defined(__dsPIC33EP512GP504__)
  reset();
#else
  Reset();
#endif
#endif
  }

void malloc_failed_hook(uint16_t size)
  {
  panic();
  }

