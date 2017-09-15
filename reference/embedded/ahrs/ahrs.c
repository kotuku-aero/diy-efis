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
#include "ahrs.h"
#include "../dspic33f-lib/can_driver.h"
#include "../dspic33f-lib/analog_devices.h"
#include "../dspic33f-lib/can_driver.h"
#include "../dspic33f-lib/eeprom.h"
#include "../dspic33f-lib/i2c_mmx_driver.h"
#include "../dspic33f-lib/pps_maps.h"

#include <math.h>
#include <string.h>

#include "../can-aerospace/data_types.h"
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


#define HARDWARE_REVISION 10
#define SOFTWARE_REVISION 1

typedef unsigned char byte_t;

int main(void);

extern void hsi_changed();

static float calculate_airspeed(analog_channel_definition_t *channel, float value);
static float calculate_altitude(analog_channel_definition_t *channel, float value);

ahrs_data_t ahrs_state;

static updated_sensors_t updated_sensors;

/* These are all of the parameters that are published by
 * the AHRS along with the publish frequency in milli-seconds
 */
static published_datapoint_t published_parameter[] = {
  { id_heading_angle, 200 },
  { id_qnh, 200 },
  { id_baro_corrected_altitude, 200 },
  { id_pressure_altitude, 200 },
  { id_altitude_rate, 200 },
  { id_indicated_airspeed, 200 },
  { id_true_airspeed, 200 },
  { id_air_time, 60000 },
  { id_track, 200},
  { id_deviation, 200},
  { id_track_angle_error, 200},
  { id_estimated_time_to_next, 200},
  { id_estimated_time_of_arrival, 200},
  { id_estimated_enroute_time, 200},
  { id_desired_track_angle, 200},
  { id_distance_to_next, 200},
  { id_distance_to_destination, 200},
  { id_roll_angle_magnetic, 200},
  { id_pitch_angle_magnetic, 200},
  { id_yaw_angle_magnetic, 200},
  { id_magnetic_heading, 200},
  { id_heading_angle, 200},
  { id_yaw_angle, 200},
  { id_pitch_angle, 200},
  { id_roll_angle, 200},
  { id_pitch_acceleration, 200},
  { id_roll_acceleration, 200},
  { id_yaw_acceleration, 200},
  { id_pitch_rate, 200},
  { id_roll_rate, 200},
  { id_yaw_rate, 200},
  { id_gps_aircraft_lattitude, 200 },
  { id_gps_aircraft_longitude, 200 },
  { id_gps_groundspeed, 200 },
  { id_wind_speed, 200 },
  { id_wind_direction, 200 },
  { id_nav_valid, 1000 },
  { id_ahrs_status, 1000 },
};

static ee_definition_t eeprom_definitions[memid_last];

#define TX_LENGTH 16
#define RX_LENGTH 16

static can_msg_t tx_buffer[TX_LENGTH];
static can_msg_t rx_buffer[RX_LENGTH];

typedef enum _ahrs_config_state {
  config_can,
  config_imu,
  config_gps,
  config_parameters,
  config_read_orientation,
  config_update_orientation,
  config_navigation,
  config_autopilot,
  config_wmm,
  config_filter,
  config_complete
  } ahrs_config_state;
  
static ahrs_config_state config_state = config_can;
static uint16_t init_memid = 0;

#define NUM_I2C_TASKS 32
static i2c_task_t i2c_tasks[NUM_I2C_TASKS];

#define WMM_STACK_SIZE 1024

static uint16_t idle_stack[DEFAULT_STACK_SIZE];
static uint16_t can_tx_stack[DEFAULT_STACK_SIZE];
static uint16_t can_rx_stack[DEFAULT_STACK_SIZE];
static uint16_t publisher_stack[DEFAULT_STACK_SIZE];
static uint16_t eeprom_stack[DEFAULT_STACK_SIZE];
static uint16_t i2c_stack[DEFAULT_STACK_SIZE];
static uint16_t rx_gps_stack[DEFAULT_STACK_SIZE];
static uint16_t tx_gps_stack[DEFAULT_STACK_SIZE];
static uint16_t imu_stack[DEFAULT_STACK_SIZE];
static uint16_t filter_stack[DEFAULT_STACK_SIZE];
static uint16_t navigation_stack[DEFAULT_STACK_SIZE];
static uint16_t wmm_stack[WMM_STACK_SIZE];
static uint16_t analog_stack[DEFAULT_STACK_SIZE];
static uint16_t autopilot_stack[DEFAULT_STACK_SIZE];

#define SENSOR_SCALE 1.0

// These are read from the eeprom and are the run-time values however
// if we are being reset then the values defined here will be written
// to the eeprom to allow for run-time modification

// refer to PressureCalcs.ods
static can_parameter_type_0_t airspeed_defn =
  {
  .hdr.data_type = CANAS_DATATYPE_FLOAT,
  .hdr.length = sizeof(can_parameter_type_0_t),
  .can_id = id_differential_pressure,
  .publish_rate = 500,
  .scale = 111.1111111,
  .offset = -4.444444444
  };

static can_parameter_type_0_t altitude_defn =
    { 
    .hdr.data_type = CANAS_DATATYPE_FLOAT,
    .hdr.length = sizeof(can_parameter_type_0_t),
    .can_id = id_static_pressure,
    .publish_rate = 500,
    .scale = 1111.111111,
    .offset = 105.5555556
    };

static can_parameter_type_1_t nodeid_defn = 
  {
    .hdr.data_type = p1t_short, 
    .hdr.length = SIZE_SHORT_SETTING,
    .value.SHORT = ahrs_node_id
  };

static can_parameter_type_1_t qnh_defn = 
  { 
  .hdr.data_type = p1t_short,
  .hdr.length = SIZE_SHORT_SETTING,
  .value.SHORT = 1013
  };

static can_parameter_type_1_t hdg_defn =
  {
  .hdr.data_type = p1t_short,
  .hdr.length = SIZE_SHORT_SETTING,
  .value.SHORT = 0
  };

static can_parameter_type_1_t accel_orientation_defn =
  {
  .hdr.data_type = p1t_matrix,
  .hdr.length = SIZE_MATRIX_SETTING,
  .value.MATRIX[0][0] = -SENSOR_SCALE, .value.MATRIX[0][1] = 0.0, .value.MATRIX[0][2] = 0.0,
  .value.MATRIX[1][0] = 0.0, .value.MATRIX[1][1] = -SENSOR_SCALE, .value.MATRIX[1][2] = 0.0,
  .value.MATRIX[2][0] = 0.0, .value.MATRIX[2][1] = 0.0, .value.MATRIX[2][2] = SENSOR_SCALE
  };

static can_parameter_type_1_t gyro_orientation_defn =
  {
  .hdr.data_type = p1t_matrix,
  .hdr.length = SIZE_MATRIX_SETTING,
  .value.MATRIX[0][0] = -SENSOR_SCALE, .value.MATRIX[0][1] = 0.0, .value.MATRIX[0][2] = 0.0,
  .value.MATRIX[1][0] = 0.0, .value.MATRIX[1][1] = -SENSOR_SCALE, .value.MATRIX[1][2] = 0.0,
  .value.MATRIX[2][0] = 0.0, .value.MATRIX[2][1] = 0.0, .value.MATRIX[2][2] = SENSOR_SCALE
  };

static can_parameter_type_1_t mag_orientation_defn =
  {
  .hdr.data_type = p1t_matrix,
  .hdr.length = SIZE_MATRIX_SETTING,
  .value.MATRIX[0][0] = -SENSOR_SCALE, .value.MATRIX[0][1] = 0.0, .value.MATRIX[0][2] = 0.0,
  .value.MATRIX[1][0] = 0.0, .value.MATRIX[1][1] = -SENSOR_SCALE, .value.MATRIX[1][2] = 0.0,
  .value.MATRIX[2][0] = 0.0, .value.MATRIX[2][1] = 0.0, .value.MATRIX[2][2] = SENSOR_SCALE
  };

static can_parameter_type_1_t soft_iron_correction_defn = {
  .hdr.data_type = p1t_matrix,
  .hdr.length = SIZE_MATRIX_SETTING,
  .value.MATRIX[0][0] = 1.0, .value.MATRIX[0][1] = 0.0, .value.MATRIX[0][2] = 0.0,
  .value.MATRIX[1][0] = 0.0, .value.MATRIX[1][1] = 1.0, .value.MATRIX[1][2] = 0.0,
  .value.MATRIX[2][0] = 0.0, .value.MATRIX[2][1] = 0.0, .value.MATRIX[2][2] = 1.0
  };

static can_parameter_type_1_t hard_iron_correction_defn = {
  .hdr.data_type = p1t_xyz,
  .hdr.length = SIZE_XYZ_SETTING,
  .value.XYZ.x = 0.0,
  .value.XYZ.y = 0.0,
  .value.XYZ.z = 0.0
  };

static can_parameter_type_1_t airtime_defn =
  {
  .hdr.data_type = p1t_ulong,
  .hdr.length = SIZE_ULONG_SETTING,
  .value.ULONG = 4910      // 49.1 hrs 
  };

static parameter_definition_t parameters[memid_last] = {
  {
  .parameter = (can_parameter_definition_t *) &altitude_defn,
  .nis.memid = memid_altitude,
  .nis.name = "Altitude",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_0_t),
  .nis.parameter_type = 0
  },
  {
  .parameter = (can_parameter_definition_t *) &airspeed_defn,
  .nis.memid = memid_airspeed,
  .nis.name = "Airspeed",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_0_t),
  .nis.parameter_type = 0
  },
  {
  .parameter = (can_parameter_definition_t *) &nodeid_defn,
  .nis.memid = memid_nodeid,
  .nis.name = "Node ID",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_1_t),
  .nis.parameter_type = 1
  },
  {
  .parameter = (can_parameter_definition_t *) &qnh_defn,
  .nis.memid = memid_qnh,
  .nis.name = "QNH",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_1_t),
  .nis.parameter_type = 1
  },
  {
  .parameter = (can_parameter_definition_t *) &hdg_defn,
  .nis.memid = memid_hdg,
  .nis.name = "HDG",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_1_t),
  .nis.parameter_type = 1
  },
  {
  .parameter = (can_parameter_definition_t *) &accel_orientation_defn,
  .nis.memid = memid_accel_orientation,
  .nis.name = "Accel Orient.",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_1_t),
  .nis.parameter_type = 1
  },
  {
  .parameter = (can_parameter_definition_t *) &gyro_orientation_defn,
  .nis.memid = memid_gyro_orientation,
  .nis.name = "Gyro Orient.",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_1_t),
  .nis.parameter_type = 1
  },
  {
  .parameter = (can_parameter_definition_t *) &mag_orientation_defn,
  .nis.memid = memid_mag_orientation,
  .nis.name = "Mag Orient.",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_1_t),
  .nis.parameter_type = 1
  },
  {
  .parameter = (can_parameter_definition_t *) &soft_iron_correction_defn,
  .nis.memid = memid_soft_iron_correction,
  .nis.name = "Soft Iron Corr.",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_1_t),
  .nis.parameter_type = 1
  },
  {
  .parameter = (can_parameter_definition_t *) &hard_iron_correction_defn,
  .nis.memid = memid_hard_iron_correction,
  .nis.name = "Hard Iron Corr.",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_1_t),
  .nis.parameter_type = 1
  },
  {
  .parameter = (can_parameter_definition_t *) &airtime_defn,
  .nis.memid = memid_airtime,
  .nis.name = "Air Time",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_1_t),
  .nis.parameter_type = 1
  },
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

// Note first entry must be airspeed, see gps.c
analog_channel_definition_t channel_definitions[] = {
  {  0, &altitude_defn, calculate_altitude },          // altitude
  {  1, &airspeed_defn, calculate_airspeed },          // airspeed
  };
  
static analog_channels_t channels =
  {
  .channel_definition = channel_definitions,
  .num_channels = numelements(channel_definitions),             // number of channels
  .selector = 0,                 // no selector
  .prescaler = 3,                // prescaler = / 256 = 273 khz
  .divisor = 273 / numelements(channel_definitions), // sample rate is 1khz
  .analog_factor = 0.000244140625 // 12 bit ADC
};

// should match the number of stacks.
#define NUM_TASKS 16
tcb_t tasks[NUM_TASKS];

#define I_PLLPRE 0
#define I_PLLPOST 0
#define I_FOSC 140000000
#define I_FIN 10000000
#define I_PLLDIV 54 // ((I_OSC/I_FIN)*(I_PLLPRE+2)*(2*(I_PLLPOST+1)))-2

static short adjust(uint16_t memid,
                    can_parameter_type_1_t *param,
                    uint16_t datapoint,
                    short min_value,
                    short max_value,
                    short delta, bool wrap_around)
  {
  uint8_t len;
  short _value = get_datapoint_short(datapoint);
  _value += delta;

  if(_value < min_value)
    _value = wrap_around ? max_value : min_value;
  else if(_value > max_value)
    _value = wrap_around ? min_value : max_value;
  
  // store the parameter in the config memory
  len = build_parameter1_short(param, _value);
  // and persist it, we don't need notification when done
  write_parameter_data(memid, len, (const uint8_t *)param, 0);
  
  return _value;
  }

void receive_command(const can_msg_t *msg)
  {
  switch(msg->id)
    {
    case id_qnh_up :
      publish_short(adjust(memid_qnh, get_parameter_type_1(memid_qnh), id_qnh, 800, 1050, get_param_short(msg), false), id_qnh);
      break;
    case id_qnh_dn :
      publish_short(adjust(memid_qnh, get_parameter_type_1(memid_qnh), id_qnh, 800, 1050, -get_param_short(msg), false), id_qnh);
      break;
    case id_heading_up :
      publish_short(adjust(memid_hdg, get_parameter_type_1(memid_hdg), id_heading_angle, 0, 359, get_param_short(msg), true), id_heading_angle);
      break;
    case id_heading_dn :
      publish_short(adjust(memid_hdg, get_parameter_type_1(memid_hdg), id_heading_angle, 0, 359, -get_param_short(msg), true), id_heading_angle);
      break;
    }
  }

static message_listener_t command_listener = { 0, receive_command };

static void idle_task(void *);

#define NUM_PARAM_UPDATES ((memid_last | 15)+1)
static uint16_t updated_parameters[NUM_PARAM_UPDATES];
static deque_t params_update_queue;

static bool parameter_updated(semaphore_t *worker_task, uint16_t memid)
  {
  switch(memid)
    {
    case memid_altitude :
      return memid_altitude_changed(worker_task);
    case memid_airspeed :
      return memid_airspeed_changed(worker_task);
    case memid_nodeid :
      return memid_nodeid_changed(worker_task);
    case memid_qnh :
      return memid_qnh_changed(worker_task);
    case memid_hdg :
      return memid_hdg_changed(worker_task);
    case memid_accel_orientation :
      return memid_accel_orientation_changed(worker_task);
    case memid_gyro_orientation :
      return memid_gyro_orientation_changed(worker_task);
    case memid_mag_orientation :
      return memid_mag_orientation_changed(worker_task);
    }
  
  return true;
  }

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
  .ee_update_queue = &params_update_queue,
  .updated = parameter_updated
  };


int main(void)
  {
  int8_t idle_task_id;
  
  // set up ports
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
  map_rpo(RP43R, rpo_c1tx);
  map_rpi(rpi_rpi44, rpi_c1rx);
  
  //map_rpo(RP42R, rpo_u1tx);
  //map_rpi(rpi_rpi25, rpi_u1rx);
  
  map_rpi(rpi_rpi45, rpi_int1);
   
  // lock the preripherals.
  //__builtin_write_OSCCONL(OSCCON | (1<<6));
  
  idle_task_id = scheduler_init(tasks, NUM_TASKS, idle_task, idle_stack, numelements(idle_stack));
  resume(idle_task_id);
  
  // create the canbus task
  can_init(HARDWARE_REVISION, SOFTWARE_REVISION,
           tx_buffer, TX_LENGTH,
           rx_buffer, RX_LENGTH,
           can_tx_stack, numelements(can_tx_stack), 0,
           can_rx_stack, numelements(can_rx_stack), 0,
           publisher_stack, numelements(publisher_stack), 0);

  config_state = config_can;
  
  // see if the init setting is made on the board
  ee_params.init_mode = PORTCbits.RC5 == 0;
  
  // create a deque that the worker task can use to get notifications of
  // updates from the can upload handler.
  init_deque(&params_update_queue, sizeof(uint16_t), updated_parameters, NUM_PARAM_UPDATES);
  // initialize the eeprom as we need our settings first
  eeprom_init(&ee_params);
  
  // run with an idle task
  run(idle_task_id);

  return 0;
  }

static bool memid_update = false;

static void update_parameter(memid_values memid, semaphore_t *worker_task)
  {
  read_parameter_definition(memid, get_parameter(memid), worker_task);
  }

bool memid_airspeed_changed(semaphore_t *worker_task)
  {
  if(!memid_update)
    {
    memid_update = true;
    update_parameter(memid_airspeed, worker_task);
    return false;
    }

  memid_update = false;
  return true;          // and done
  }

bool memid_altitude_changed(semaphore_t *worker_task)
  {
  if(!memid_update)
    {
    memid_update = true;
    update_parameter(memid_altitude, worker_task);
    return false;
    }

  memid_update = false;
  return true;          // and done
  }

bool memid_qnh_changed(semaphore_t *worker_task)
  {
  if(!memid_update)
    {
    memid_update = true;
    update_parameter(memid_qnh, worker_task);
    return false;
    }
  
  memid_update = false;
  return true;
  }

bool memid_hdg_changed(semaphore_t *worker_task)
  {
  if(!memid_update)
    {
    memid_update = true;
    update_parameter(memid_hdg, worker_task);
    return false;
    }
  
  memid_update = false;
  return true;
  }

bool memid_accel_orientation_changed(semaphore_t *worker_task)
  {
  if(!memid_update)
    {
    memid_update = true;
    update_parameter(memid_accel_orientation, worker_task);
    return false;
    }
  
  memid_update = false;
  return true;
  }

bool memid_gyro_orientation_changed(semaphore_t *worker_task)
  {
  if(!memid_update)
    {
    memid_update = true;
    update_parameter(memid_gyro_orientation, worker_task);
    return false;
    }
  
  memid_update = false;
  return true;
  }

bool memid_mag_orientation_changed(semaphore_t *worker_task)
  {
  if(!memid_update)
    {
    memid_update = true;
    update_parameter(memid_mag_orientation, worker_task);
    return false;
    }
  
  memid_update = false;
  return true;
  }

bool memid_nodeid_changed(semaphore_t *worker_task)
  {
  
  return true;
  }

// this is called by the eeprom init task
bool eeprom_init_done(semaphore_t *worker_task)
  {
  switch(config_state)
    {
    case config_can :
      // and publish our values
      register_datapoints(published_parameter, numelements(published_parameter));
      if(can_config(memid_nodeid, worker_task))
        config_state = config_imu;
      break;
    case config_imu :
      if(imu_config(worker_task))
        config_state = config_gps;
      break;
    case config_gps :     // not defined yet...
      if(gps_config(worker_task))
        config_state = config_parameters;
      break;
    case config_parameters :
      if(init_memid < memid_last)
        {
        // read the definition from the eeprom
        update_parameter(init_memid, worker_task);
        // do the next memid
        init_memid++;
        }
      else
        {
        // all memid's read, start publishing them
        publish_short(get_parameter_type_1(memid_qnh)->value.SHORT, id_qnh);
        publish_short(get_parameter_type_1(memid_hdg)->value.SHORT, id_heading_angle);
        publish_uint32(get_parameter_type_1(memid_airtime)->value.ULONG, id_air_time);
        
        // move to next state
        config_state = config_navigation;

        // set up the op amps
        // OA2 is used
        CM2CONbits.COE = 1;                 // enable output
        CM2CONbits.CON = 1;                // enable op-amp
        signal(worker_task);
        }
      
      break;
    case config_navigation :
      if(navigation_config(worker_task))
        config_state = config_autopilot;
      break;
    case config_autopilot :
      if(autopilot_config(worker_task))
        config_state = config_wmm;
      break;
    case config_wmm :
      if(wmm_config(worker_task))
        config_state = config_filter;
      break;
    case config_filter :
      if(filter_config(worker_task))
        config_state = config_complete;
      break;
    case config_complete :
    default :
      // create the imu task
      imu_init(imu_stack, numelements(imu_stack));
      
      // create the gps task
      gps_init(rx_gps_stack, numelements(rx_gps_stack), 0,
               tx_gps_stack, numelements(tx_gps_stack), 0);
      // create the analog worker task
      analog_init(&channels, analog_stack, numelements(analog_stack));
      
      // create the publisher worker task that will publish the computed values
      // publisher_init();
      // create the navigation worker task
      navigation_init(navigation_stack, numelements(navigation_stack));
      
      // create the autopilot task
      autopilot_init(autopilot_stack, numelements(autopilot_stack));
      // create a task to update the mag variation
      wmm_init(wmm_stack, numelements(wmm_stack));
      
      // create a task to do the dsp filtering of the state
      filter_init(filter_stack, numelements(filter_stack));

      // subscribe to the qnh and mag variation commands
      subscribe(&command_listener);
      
      return true;          // init is done
    }
  
  return false;             // more work
  }

static bool airtime_updated = false;

static void idle_task(void *parg)
  {
  while(true)
    {
    if(airtime_updated)
      {
      // save the new hobbs time
      write_parameter_definition(memid_airtime, get_parameter(memid_airtime), 0);
      airtime_updated = false;
      }
    exit_critical();
    yield();
    }
    //asm volatile("pwrsav #1");
  }

static const float air_density = 1.225;
static const float speed_of_sound = 340.29471111;
static const float isa_pressure = 1013.25;
static const float std_air_temp = 288.15;         // kelvin

// our altitude is updated every 500msec  we calculate the
// rate from the last
static float last_altitude = 0;
static uint32_t last_altitude_time = 0;

static float calculate_altitude(analog_channel_definition_t *channel, float value)
  {
  float f_qnh = get_datapoint_short(id_qnh);
  // round to value
  float _value = floor(value + 0.5);
  uint32_t last_vs_time = last_altitude_time;
  uint32_t now = ticks();
  
  //ahrs_state.pressure_altitude = (1-pow(_value/1013.25, 0.190284)) * 44307.694;
  ahrs_state.pressure_altitude = (1-pow(_value * 0.000987167, 0.190284)) * 44307.694;
  ahrs_state.baro_corrected_altitude = (1-pow(_value / f_qnh, 0.190284)) * 44307.694;
  
  set_mask(SENSORUPDATES_alt);
  // we calculate the id_baro_corrected_altitude
  publish_float(ahrs_state.baro_corrected_altitude, id_baro_corrected_altitude);
  publish_float(ahrs_state.pressure_altitude, id_pressure_altitude);

  uint32_t num_vs_ticks = now - last_vs_time;
  // update vertical speed every 500msec
  if(num_vs_ticks > 500)
    {
    // calculate how may 500msec slots we are calculating for.
    uint32_t num_incr = num_vs_ticks / 500;
    
    ahrs_state.vertical_speed = ahrs_state.baro_corrected_altitude - last_altitude;

    // vertical speed is m/sec, calculate to num 0.5 secs
    ahrs_state.vertical_speed /= num_incr;
    ahrs_state.vertical_speed *= 2;       // convert to m/s
    
    num_incr = num_vs_ticks;
    num_incr %= 500;          // how much time of the last tick to calc
    
    last_altitude_time = now - num_incr;
    last_altitude = ahrs_state.baro_corrected_altitude;
    
    set_mask(SENSORUPDATES_vs);
  
    // should be a parameter!
    publish_float(ahrs_state.vertical_speed, id_altitude_rate);
    }

  return value;
  }

static uint32_t last_tick;
static bool aircraft_flying = false;

// the value is the dynamic pressure in HPA.
static float calculate_airspeed(analog_channel_definition_t *channel, float value)
  {
  float oat = 0.0;      // todo implement this
  /*
   This is calculated from a reduction of bernoulis theorum
  */
  if(value < 0)
    value = 0;
  
  // publish the differential pressure in hPa
  publish_float(value, id_differential_pressure);
  
  // TODO: copy the hobbs meter code

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
  ahrs_state.indicated_airspeed = sqrt(value * 163.265306122449);
  
  // if speed is < 40kts then we trim it
  if(ahrs_state.indicated_airspeed < 20)
    {
    ahrs_state.indicated_airspeed = 0;
    ahrs_state.true_airspeed = 0;
    }
  else
    {
    if(last_tick != ticks())
      {
      if(!aircraft_flying)
        {
        aircraft_flying = true;
        last_tick = ticks();
        }
      else
        {
        uint32_t now = ticks();
        uint32_t runtime = now - last_tick;
        // calculate the time in MS since the last tick
        
        // the engine runtime is in hours, only update each 1/100 hour
        // which is 36 seconds
        if(runtime > 35999)
          {
          uint32_t run_secs = runtime / 1000;
          uint32_t run_hours = run_secs / 3600;
          // remove whole hours (normally 0)
          run_secs -= run_hours * 3600;
          // calculate 1/100's of an hour to add
          uint32_t run_parts = run_secs / 36;
          
          can_parameter_type_1_t *param = get_parameter_type_1(memid_airtime);
          param->value.ULONG += run_hours * 100;
          param->value.ULONG += run_parts;
          
          // remove our calculated hours
          run_secs = (run_parts * 36) + (run_hours * 3600);
          // back to MS
          run_secs *= 1000;
          
          // and remove from accumulator
          runtime -= run_secs;

          // keep the overflow seconds accumulating by adding back the
          // amount accumulates
          last_tick = now - runtime;
          
          airtime_updated = true;
          
          // and send the new value as a published value
          publish_uint32(param->value.ULONG, id_air_time);
          }
        }
      }
    else
      aircraft_flying = false;

    // calculate CAS as
    ahrs_state.true_airspeed = speed_of_sound * 
      sqrt(((5 * oat) / std_air_temp) *
      (pow(value / 1013.25 + 1, 0.28571428571423) - 1));

    }
  
  publish_float(ahrs_state.indicated_airspeed, id_indicated_airspeed);
  publish_float(ahrs_state.true_airspeed, id_true_airspeed);
  
  set_mask(SENSORUPDATES_tas | SENSORUPDATES_ias);

  return value;
  }

char *skip_param(char *ptr)
  {
  while(*ptr != 0)
    ptr++;

  ptr++;
  return ptr;
  }

typedef enum _error_type
  {
  e_oscillator_fail,
  e_address_error,
  e_stack_error,
  e_math_error,
  e_dmac_error
  } error_type;

static void *err_addr;
static error_type error;

void panic()
  {
  // restart the system using the brown-out
  __builtin_disi(0x3FFF); /* disable interrupts */
  asm volatile("RESET");
  }

void (*getErrLoc(void))(void); // Get Address Error Loc


void __attribute__((__interrupt__)) _OscillatorFail(void);
void __attribute__((__interrupt__)) _AddressError(void);
void __attribute__((__interrupt__)) _StackError(void);
void __attribute__((__interrupt__)) _MathError(void);
void __attribute__((__interrupt__)) _DMACError(void);

/*
Primary Exception Vector handlers:
These routines are used if INTCON2bits.ALTIVT = 0.
All trap service routines in this file simply ensure that device
continuously executes code within the trap service routine. Users
may modify the basic framework provided here to suit to the needs
of their application.
 */
void __attribute__((interrupt, no_auto_psv)) _OscillatorFail(void)
  {
  INTCON1bits.OSCFAIL = 0; //Clear the trap flag
  error = e_oscillator_fail;
  panic();
  }

void __attribute__((interrupt, no_auto_psv)) _AddressError(void)
  {
  err_addr = getErrLoc();
  INTCON1bits.ADDRERR = 0; //Clear the trap flag
  error = e_address_error;
  panic();
  }

static void *error_sp;
static uint16_t error_stack[32];

void __attribute__((interrupt, no_auto_psv)) _StackError(void)
  {
  // get a new stack as the stack has failed
  asm volatile("MOV W15, _error_sp");
  asm volatile("MOV _error_stack, W15");
  
  INTCON1bits.STKERR = 0; //Clear the trap flag
  error = e_stack_error;
  panic();
  }

void __attribute__((interrupt, no_auto_psv)) _MathError(void)
  {
  INTCON1bits.MATHERR = 0; //Clear the trap flag
  error = e_math_error;
  panic();
  }

void __attribute__((interrupt, no_auto_psv)) _DMACError(void)
  {
  INTCON1bits.DMACERR = 0; //Clear the trap flag
  error = e_dmac_error;
  panic();
  }

updated_sensors_t unset_mask(updated_sensors_t mask)
  {
  updated_sensors &= ~mask;
  return updated_sensors;
  }

updated_sensors_t set_mask(updated_sensors_t mask)
  {
  updated_sensors |= mask;
  return updated_sensors;
  }

bool is_set(updated_sensors_t mask)
  {
  return (updated_sensors & mask) != 0;
  }
