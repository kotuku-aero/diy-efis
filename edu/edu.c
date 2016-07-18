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
#include "../dspic33f-lib/microkernel.h"
#include "../dspic33f-lib/analog_devices.h"
#include "../dspic33f-lib/input_capture_devices.h"
#include "../dspic33f-lib/can_driver.h"
#include "../dspic33f-lib/uart_device.h"
#include "../dspic33f-lib/eeprom.h"
#include "../can-aerospace/parameter_definitions.h"

#include <string.h>
#include <stdlib.h>

// DSPIC33EP512GP506 Configuration Bit Settings

#include <xc.h>

// FICD
#pragma config ICS = PGD2               // ICD Communication Channel Select bits (Communicate on PGEC1 and PGED1)
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

typedef unsigned char byte_t;

int main (void);

#define HARDWARE_REVISION 20
#define SOFTWARE_REVISION 1

static uint16_t rpm_callback(struct _capture_channel_definition_t *channel, uint16_t value);
static uint16_t flow_callback(struct _capture_channel_definition_t *channel, uint16_t value);

typedef enum _memid_values
  {
  memid_edu_left_fuel_quantity,
  memid_edu_right_fuel_quantity,
  memid_cylinder_head_temperature1,
  memid_cylinder_head_temperature2,
  memid_cylinder_head_temperature3,
  memid_cylinder_head_temperature4,
  memid_exhaust_gas_temperature1,
  memid_exhaust_gas_temperature2,
  memid_exhaust_gas_temperature3,
  memid_exhaust_gas_temperature4,
  memid_manifold_pressure,
  memid_fuel_pressure,
  memid_dc_voltage,
  memid_dc_current,
  memid_oil_temperature,
  memid_oil_pressure,
  memid_fuel_flow_rate,
  memid_engine_rpm_a,
  memid_engine_rpm_b,
  memid_fuel_consumed,
  memid_node_id,
  memid_engine_time,
  memid_last
  } memid_values;

/* These are all of the parameters that are published by
 * the EDU along with the publish frequency in milli-seconds
 */
static published_datapoint_t published_parameter[] = {
  { id_left_fuel_quantity, 200 },
  { id_right_fuel_quantity, 200 },
  { id_cylinder_head_temperature1, 500 },
  { id_cylinder_head_temperature2, 500 },
  { id_cylinder_head_temperature3, 500 },
  { id_cylinder_head_temperature4, 500 },
  { id_exhaust_gas_temperature1, 500 },
  { id_exhaust_gas_temperature2, 500 },
  { id_exhaust_gas_temperature3, 500 },
  { id_exhaust_gas_temperature4, 500 },
  { id_manifold_pressure, 500 },
  { id_fuel_pressure, 500 },
  { id_dc_voltage, 500 },
  { id_dc_current, 500 },
  { id_oil_temperature, 500 },
  { id_oil_pressure, 500 },
  { id_fuel_flow_rate, 500 },
  { id_engine_rpm, 500 },
  { id_engine_rpm_a, 500 },
  { id_engine_rpm_b, 200 },
  { id_fuel_consumed, 200 },
  { id_engine_rpm, 200 },
  { id_tach_time, 6000 },
  { id_fuel_flow_rate, 500 },
};
 
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
  
#define CHT_CAL 815
#define EGT_CAL 845

static uint16_t idle_stack[IDLE_STACK_SIZE];
static uint16_t can_tx_stack[DEFAULT_STACK_SIZE];
static uint16_t can_rx_stack[DEFAULT_STACK_SIZE];
static uint16_t publisher_stack[DEFAULT_STACK_SIZE];
static uint16_t eeprom_stack[DEFAULT_STACK_SIZE];
static uint16_t i2c_stack[DEFAULT_STACK_SIZE];
static uint16_t analog_stack[DEFAULT_STACK_SIZE];
static uint16_t rx_pmag1_stack[DEFAULT_STACK_SIZE];
static uint16_t tx_pmag1_stack[DEFAULT_STACK_SIZE];
static uint16_t rx_pmag2_stack[DEFAULT_STACK_SIZE];
static uint16_t tx_pmag2_stack[DEFAULT_STACK_SIZE];

// should match the number of stacks.
#define NUM_TASKS 13
tcb_t tasks[NUM_TASKS];

#define NUM_I2C_TASKS 32
static i2c_task_t i2c_tasks[NUM_I2C_TASKS];
  
#define CAN_TX_LENGTH 64
#define CAN_RX_LENGTH 32

static can_msg_t can_tx_buffer[CAN_TX_LENGTH];
static can_msg_t can_rx_buffer[CAN_RX_LENGTH];


#define NUM_RX_BUFFERS  8
#define RX_BUFFER_LENGTH 128

#define RX_QUEUE_LENGTH (NUM_RX_BUFFERS * RX_BUFFER_LENGTH)

static uint8_t pmag1_rx_buffer[RX_BUFFER_LENGTH];
static uint8_t pmag1_rx_worker_buffer[RX_BUFFER_LENGTH];
static uint8_t pmag1_rx_queue[RX_QUEUE_LENGTH];

static uint8_t pmag2_rx_buffer[RX_BUFFER_LENGTH];
static uint8_t pmag2_rx_worker_buffer[RX_BUFFER_LENGTH];
static uint8_t pmag2_rx_queue[RX_QUEUE_LENGTH];

static void process_line(uart_config_t *config, unsigned char *, uint8_t length);

static uart_config_t pmag1_config = {
  .uart_number = 0,
  .rate = 9600,
  .flags = UART_EOL_CHAR,
  .eol_char = '\n',
  .rx_buffer = pmag1_rx_buffer,
  .rx_worker_buffer = pmag1_rx_worker_buffer,
  .rx_length = RX_BUFFER_LENGTH,
  .rx_pin = rpi_rpi18,
  .tx_pin = RP43R,
  .callback.uart_callback = process_line
  };

static uart_config_t pmag2_config = {
  .uart_number = 1,
  .rate = 9600,
  .eol_char = '\n',
  .flags = UART_EOL_CHAR,
  .rx_buffer = pmag2_rx_buffer,
  .rx_worker_buffer = pmag2_rx_worker_buffer,
  .rx_length = RX_BUFFER_LENGTH,
  .rx_pin = rpi_rpi19,
  .tx_pin = RP42R,
  .callback.uart_callback = process_line
  };

// These are read from the eeprom and are the run-time values however
// if we are being reset then the values defined here will be written
// to the eeprom to allow for run-time modification

static can_parameter_type_1_t nodeid_defn = 
  {
    .hdr.data_type = p1t_short, 
    .hdr.length = SIZE_SHORT_SETTING,
    .value.SHORT = slcan_id
  };

static can_parameter_type_1_t engine_time_defn = 
  {
    .hdr.data_type = p1t_ulong, 
    .hdr.length = SIZE_ULONG_SETTING,
    .value.ULONG = 0
  };

static can_parameter_type_0_t left_fuel_quantity_defn =
  {
  .hdr.data_type = CANAS_DATATYPE_SHORT,
  .hdr.length = sizeof(can_parameter_type_0_t),
  .can_id = id_edu_left_fuel_quantity,
  .publish_rate = 1000,
  .scale = 80.0,
  .offset = 0
  };

static can_parameter_type_0_t right_fuel_quantity_defn =
  {
  .hdr.data_type = CANAS_DATATYPE_SHORT,
  .hdr.length = sizeof(can_parameter_type_0_t),
  .can_id = id_edu_right_fuel_quantity,
  .publish_rate = 1000,
  .scale = 80.0,
  .offset = 0
  };

// CHT probes are based on an Type J probe
// The maximum temperature is 750 degrees which is 8.8181v
// the voltage divider will generate a voltage of 4.85v 
// range of ADC = 0-5vdc / 1024 = 0.0048828125 mv/step
// 1 = 765 degrees c
static can_parameter_type_0_t cylinder_head_temperature1_defn =
  {
  .hdr.data_type = CANAS_DATATYPE_SHORT,
  .hdr.length = sizeof(can_parameter_type_0_t),
  .can_id = id_cylinder_head_temperature1,
  .publish_rate = 1000,
  .scale = CHT_CAL,
  .offset = 273.15
  };

static can_parameter_type_0_t cylinder_head_temperature2_defn =
  {
  .hdr.data_type = CANAS_DATATYPE_SHORT,
  .hdr.length = sizeof(can_parameter_type_0_t),
  .can_id = id_cylinder_head_temperature2,
  .publish_rate = 1000,
  .scale = CHT_CAL,
  .offset = 273.15
  };

static can_parameter_type_0_t cylinder_head_temperature3_defn =
  {
  .hdr.data_type = CANAS_DATATYPE_SHORT,
  .hdr.length = sizeof(can_parameter_type_0_t),
  .can_id = id_cylinder_head_temperature3,
  .publish_rate = 1000,
  .scale = CHT_CAL,
  .offset = 273.15
  };

static can_parameter_type_0_t cylinder_head_temperature4_defn =
  {
  .hdr.data_type = CANAS_DATATYPE_SHORT,
  .hdr.length = sizeof(can_parameter_type_0_t),
  .can_id = id_cylinder_head_temperature4,
  .publish_rate = 1000,
  .scale = CHT_CAL,
  .offset = 273.15
  };

  // EGT probles are based on an AD595 (Type K) with a voltage divider operating
  // on an 9v supply rail allowing for an 880 decgree C offset
  // 5v = 880 deg
static can_parameter_type_0_t exhaust_gas_temperature1_defn =
  {
  .hdr.data_type = CANAS_DATATYPE_SHORT,
  .hdr.length = sizeof(can_parameter_type_0_t),
  .can_id = id_exhaust_gas_temperature1,
  .publish_rate = 1000,
  .scale = EGT_CAL,
  .offset = 273.15
  };

static can_parameter_type_0_t exhaust_gas_temperature2_defn =
  {
  .hdr.data_type = CANAS_DATATYPE_SHORT,
  .hdr.length = sizeof(can_parameter_type_0_t),
  .can_id = id_exhaust_gas_temperature2,
  .publish_rate = 1000,
  .scale = EGT_CAL,
  .offset = 273.15
  };
static can_parameter_type_0_t exhaust_gas_temperature3_defn =
  {
  .hdr.data_type = CANAS_DATATYPE_SHORT,
  .hdr.length = sizeof(can_parameter_type_0_t),
  .can_id = id_exhaust_gas_temperature3,
  .publish_rate = 1000,
  .scale = EGT_CAL,
  .offset = 273.15
  };

static can_parameter_type_0_t exhaust_gas_temperature4_defn =
  {
  .hdr.data_type = CANAS_DATATYPE_SHORT,
  .hdr.length = sizeof(can_parameter_type_0_t),
  .can_id = id_exhaust_gas_temperature4,
  .publish_rate = 1000,
  .scale = EGT_CAL,
  .offset = 273.15
  };

// Manifold pressure calculation:
// range of channel = 15-115kpa == 0.2v - 4.7v
// range of ADC = 0-5vdc / 1024 = 0.0048828125 mv/step
// converted to 0.2v = 0.04 4.7v = 0.94
// 4.7v = 1150hpa
// 0.2v = 150hpa
// scale = 1000/.90 = 1111.11111111111
// offset = 150-(.04 * 1111.11111111111) = 105.5555555555556

static can_parameter_type_0_t manifold_pressure_defn =
  {
  .hdr.data_type = CANAS_DATATYPE_SHORT,
  .hdr.length = sizeof(can_parameter_type_0_t),
  .can_id = id_manifold_pressure,
  .publish_rate = 1000,
  .scale = 1111.11111111111,
  .offset = 105.5555555555556
  };

// based on a 0-35 psi pressure gauge
// 35 psi = 241.3165048 kpa
// 0.2-4.7v
// 4.5/0.0048828125 = 922
// (1/1024)*922 = 0.900390625
// 0.900390625 * N = 241.3165048
// scale = 268.0131246368764
// offset = (241.3165048/4.5)*.2
// 0.2v = 10.72517799 KPa
static can_parameter_type_0_t fuel_pressure_defn =
  {
  .hdr.data_type = CANAS_DATATYPE_SHORT,
  .hdr.length = sizeof(can_parameter_type_0_t),
  .can_id = id_fuel_pressure,
  .publish_rate = 1000,
  .scale = 268.0131246368764,
  .offset = -10.72517799
  };

// Voltage calculations
static can_parameter_type_0_t dc_voltage_defn =
  {
  .hdr.data_type = CANAS_DATATYPE_SHORT,
  .hdr.length = sizeof(can_parameter_type_0_t),
  .can_id = id_dc_voltage,
  .publish_rate = 1000,
  .scale = 19.00,
  .offset = 0
  };
 
// Current Calculations
// range of channel = 0-9v for 0-72A
// 4.5v = 0
// -72 = 1.0728v
// +72 = 3.4272v
// set up for 2 turns so 0-36A
// 0-5v = 89v
static can_parameter_type_0_t dc_current_defn =
  {
  .hdr.data_type = CANAS_DATATYPE_SHORT,
  .hdr.length = sizeof(can_parameter_type_0_t),
  .can_id = id_dc_current,
  .publish_rate = 1000,
  .scale = 89.0,
  .offset = -46.5
  };

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
static can_parameter_type_0_t oil_temperature_defn =
  {
  .hdr.data_type = CANAS_DATATYPE_SHORT,
  .hdr.length = sizeof(can_parameter_type_0_t),
  .can_id = id_oil_temperature,
  .publish_rate = 1000,
  .scale = -129.129886507,
  .offset = 373
  };

  // based on a 0-150 psi UMA gauge
  // 0-1034.2135935 KPa
  // 0.2-4.7v
  // 4.5/0.0048828125 = 922
  // (1/1024)*922 = 0.900390625
  // 0.900390625 * N = 1034.2135935
  // scale = 1148.6276786811280
  // offset = (1034.2135935/2.5)*.2
  // 0.2v = 82.73708748 KPa
static can_parameter_type_0_t oil_pressure_defn =
  {
  .hdr.data_type = CANAS_DATATYPE_SHORT,
  .hdr.length = sizeof(can_parameter_type_0_t),
  .can_id = id_oil_pressure,
  .publish_rate = 1000,
  .scale = 1148.6276786811280,
  .offset = -82.73708748
  };

static can_parameter_type_0_t fuel_flow_rate_defn =
  {
  .hdr.data_type = CANAS_DATATYPE_SHORT,
  .hdr.length = sizeof(can_parameter_type_0_t),
  .can_id = id_fuel_flow_rate,
  .publish_rate = 1000,
  .scale = 1800,
  .offset = 0
  };


static can_parameter_type_0_t engine_rpm_a_defn =
  {
  .hdr.data_type = CANAS_DATATYPE_SHORT,
  .hdr.length = sizeof(can_parameter_type_0_t),
  .can_id = id_engine_rpm_a,
  .publish_rate = 25,
  .scale = 30,
  .offset = 0
  };

static can_parameter_type_0_t engine_rpm_b_defn =
  {
  .hdr.data_type = CANAS_DATATYPE_SHORT,
  .hdr.length = sizeof(can_parameter_type_0_t),
  .can_id = id_engine_rpm_b,
  .publish_rate = 25,
  .scale = 30,
  .offset = 0
  };

  // publish every 1 seconds
  // the value is based on the 'K' factor of the sensor.  This assumes
  // A sensor with a k-factor of 32000
  // generates 8421 pulses/l so 0.0001187507422
  // we publish l*10 so value is 10x lager
static can_parameter_type_0_t fuel_consumed_defn =
  {
  .hdr.data_type = CANAS_DATATYPE_SHORT,
  .hdr.length = sizeof(can_parameter_type_0_t),
  .can_id = id_fuel_consumed,
  .publish_rate = 1,
  .scale = 0.001187507422,
  .offset = 0
  };

static capture_channel_definition_t capture_channel_definitions[] = {
  { 2 | ACCUMULATOR_CHANNEL, memid_fuel_consumed, 3, 0, flow_callback },    // fuel flow capture channel == 2
  { 7, memid_engine_rpm_a, 3, 0, rpm_callback },    // left tachometer capture channel == 1
  { 8, memid_engine_rpm_b, 3, 0, rpm_callback },    // right tachometer capture channel
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

static analog_channel_definition_t channel_definitions[] = {
  { 0x02, &left_fuel_quantity_defn, 0 },    // left fuel
  { 0x03, &right_fuel_quantity_defn, 0 },    // right fuel
  { 0x08, &exhaust_gas_temperature1_defn, 0 },     // EGT 1
  { 0x09, &exhaust_gas_temperature2_defn, 0 },     // EGT 2
  { 0x0A, &exhaust_gas_temperature3_defn, 0 },     // EGT 3
  { 0x0B, &exhaust_gas_temperature4_defn, 0 },     // EGT 4
  { 0x0C, &cylinder_head_temperature1_defn, 0 },     // CHT 1
  { 0x0D, &cylinder_head_temperature2_defn, 0 },     // CHT 2
  { 0x0E, &cylinder_head_temperature3_defn, 0 },     // CHT 3
  { 0x0F, &cylinder_head_temperature4_defn, 0 },     // CHT 4
  { 0x07, &manifold_pressure_defn, 0 },    // Manifold pressure
  { 0x06, &fuel_pressure_defn, 0 },    // Fuel pressure
  { 0x00, &dc_voltage_defn, 0 },   // DC volts
  { 0x01, &dc_current_defn, 0 },   // DC Amps
  { 0x04, &oil_temperature_defn, 0 },   // Oil Temp
  { 0x05, &oil_pressure_defn, 0 },   // Oil Pressure
  };

static uint8_t select_sub_channel(uint8_t channel)
  {
  return channel & 0x0f;        // remove selector bits
  }

static analog_channels_t channels = 
  {
  channel_definitions, 
  numelements(channel_definitions), 
  select_sub_channel,
  0, 
  5000 / numelements(channel_definitions), /* set for 4khz samples */ 
  0.000244140625                 // 12 bit ADC
  };

static parameter_definition_t parameters[memid_last] = {
  {
  .parameter = (can_parameter_definition_t *) &left_fuel_quantity_defn,
  .nis.memid = memid_edu_left_fuel_quantity,
  .nis.name = "Left Fuel",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_0_t),
  .nis.parameter_type = 0
  },
  {
  .parameter = (can_parameter_definition_t *) &right_fuel_quantity_defn,
  .nis.memid = memid_edu_right_fuel_quantity,
  .nis.name = "Right Fuel",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_0_t),
  .nis.parameter_type = 0
  },
  {
  .parameter = (can_parameter_definition_t *) &cylinder_head_temperature1_defn,
  .nis.memid = memid_cylinder_head_temperature1,
  .nis.name = "CHT 1",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_0_t),
  .nis.parameter_type = 0
  },
  {
  .parameter = (can_parameter_definition_t *) &cylinder_head_temperature2_defn,
  .nis.memid = memid_cylinder_head_temperature2,
  .nis.name = "CHT 2",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_0_t),
  .nis.parameter_type = 0
  },
  {
  .parameter = (can_parameter_definition_t *) &cylinder_head_temperature3_defn,
  .nis.memid = memid_cylinder_head_temperature3,
  .nis.name = "CHT 3",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_0_t),
  .nis.parameter_type = 0
  },
  {
  .parameter = (can_parameter_definition_t *) &cylinder_head_temperature4_defn,
  .nis.memid = memid_cylinder_head_temperature4,
  .nis.name = "CHT 4",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_0_t),
  .nis.parameter_type = 0
  },
  {
  .parameter = (can_parameter_definition_t *) &exhaust_gas_temperature1_defn,
  .nis.memid = memid_exhaust_gas_temperature1,
  .nis.name = "EGT 1",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_0_t),
  .nis.parameter_type = 0
  },
  {
  .parameter = (can_parameter_definition_t *) &exhaust_gas_temperature2_defn,
  .nis.memid = memid_exhaust_gas_temperature2,
  .nis.name = "EGT 2",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_0_t),
  .nis.parameter_type = 0
  },
  {
  .parameter = (can_parameter_definition_t *) &exhaust_gas_temperature3_defn,
  .nis.memid = memid_exhaust_gas_temperature3,
  .nis.name = "EGT 3",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_0_t),
  .nis.parameter_type = 0
  },
  {
  .parameter = (can_parameter_definition_t *) &exhaust_gas_temperature4_defn,
  .nis.memid = memid_exhaust_gas_temperature4,
  .nis.name = "EGT 4",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_0_t),
  .nis.parameter_type = 0
  },
  {
  .parameter = (can_parameter_definition_t *) &manifold_pressure_defn,
  .nis.memid = memid_manifold_pressure,
  .nis.name = "Manifold Pres.",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_0_t),
  .nis.parameter_type = 0
  },
  {
  .parameter = (can_parameter_definition_t *) &fuel_pressure_defn,
  .nis.memid = memid_fuel_pressure,
  .nis.name = "Fuel Pressure",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_0_t),
  .nis.parameter_type = 0
  },
  {
  .parameter = (can_parameter_definition_t *) &dc_voltage_defn,
  .nis.memid = memid_dc_voltage,
  .nis.name = "DC Voltage",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_0_t),
  .nis.parameter_type = 0
  },
  {
  .parameter = (can_parameter_definition_t *) &dc_current_defn,
  .nis.memid = memid_dc_current,
  .nis.name = "DC Current",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_0_t),
  .nis.parameter_type = 0
  },
  {
  .parameter = (can_parameter_definition_t *) &oil_temperature_defn,
  .nis.memid = memid_oil_temperature,
  .nis.name = "Oil Temperature",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_0_t),
  .nis.parameter_type = 0
  },
  {
  .parameter = (can_parameter_definition_t *) &oil_pressure_defn,
  .nis.memid = memid_oil_pressure,
  .nis.name = "Oil Pressure",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_0_t),
  .nis.parameter_type = 0
  },
  {
  .parameter = (can_parameter_definition_t *) &fuel_flow_rate_defn,
  .nis.memid = memid_fuel_flow_rate,
  .nis.name = "Fuel Flow",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_0_t),
  .nis.parameter_type = 0
  },
  {
  .parameter = (can_parameter_definition_t *) &engine_rpm_a_defn,
  .nis.memid = memid_engine_rpm_a,
  .nis.name = "Left Tachometer",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_0_t),
  .nis.parameter_type = 0
  },
  {
  .parameter = (can_parameter_definition_t *) &engine_rpm_b_defn,
  .nis.memid = memid_engine_rpm_b,
  .nis.name = "Right Tachometer",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_0_t),
  .nis.parameter_type = 0
  },
  {
  .parameter = (can_parameter_definition_t *) &fuel_consumed_defn,
  .nis.memid = memid_fuel_consumed,
  .nis.name = "Fuel Consumption",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_0_t),
  .nis.parameter_type = 0
  },
  {
  .parameter = (can_parameter_definition_t *) &nodeid_defn,
  .nis.memid = memid_node_id,
  .nis.name = "Node ID",
  .nis.name_length = 0,
  .nis.parameter_length = sizeof(can_parameter_type_1_t),
  .nis.parameter_type = 1
  },
  {
  .parameter = (can_parameter_definition_t *) &engine_time_defn,
  .nis.memid = memid_engine_time,
  .nis.name = "Tach Time",
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


static uint32_t last_tick;
static bool engine_running = false;
static bool tach_time_updated = false;

static uint32_t read_tach_time()
  {
  can_parameter_type_1_t *param = get_parameter_type_1(memid_engine_time);
  return param->value.ULONG;
  }

static void write_tach_time(uint32_t tach_time)
  {
  can_parameter_type_1_t *param = get_parameter_type_1(memid_engine_time);
  param->value.ULONG = tach_time;
  tach_time_updated = true;
  }

static uint16_t rpm_callback(struct _capture_channel_definition_t *channel, uint16_t value)
  {
  if(value > 0)
    {
    if(last_tick != ticks())
      {
      if(!engine_running)
        {
        engine_running = true;
        last_tick = ticks();
        }
      else
        {
        uint32_t runtime = ticks() - last_tick;
        
        // the engine runtime is in minutes, so divide the 1/10 sec tick by
        // that amount
        if(runtime > 60000)
          {
          uint32_t tach_time = read_tach_time();
          uint32_t run_secs = runtime / 60000;
          tach_time += run_secs;
          write_tach_time(tach_time);
          
          publish_uint32(tach_time, id_tach_time);
          
          // keep the overflow seconds accumulating
          last_tick += run_secs * 60000;
          }
        }
      }
    else
      engine_running = false;
    
    publish_short((short) value, id_engine_rpm);
    }
  
  return value;
  }

// passed in value is l*10
static float last_value = 0.0f;
static can_parameter_type_0_t flow_param;

static uint16_t flow_callback(struct _capture_channel_definition_t *channel, uint16_t value)
  {
  // the capture count is the number of samples that comprise the value
  // we take that as being 
  if(channel->capture_count > 0)
    {
    float flow_rate = value - last_value;
    flow_rate *= flow_param.scale;
    flow_rate += flow_param.offset;

    publish_short((short) flow_rate, id_fuel_flow_rate);
    }
  
  last_value = value;
  return value;
  }

static void idle_task(void *parg)
  {
  while(true)
    {
    if(tach_time_updated)
      {
      // save the new hobbs time
      write_parameter_definition(memid_engine_time, get_parameter(memid_engine_time), 0);
      tach_time_updated = false;
      }
    exit_critical();
    yield();
    }
    //asm volatile("pwrsav #1");
  }

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

#define I_PLLPRE 0
#define I_PLLPOST 0
#define I_FOSC 140000000
#define I_FIN 10000000
#define I_PLLDIV 54 // ((I_OSC/I_FIN)*(I_PLLPRE+2)*(2*(I_PLLPOST+1)))-2


int main(void)
  {
  int8_t idle_task_id;
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

  
  idle_task_id = scheduler_init(tasks, NUM_TASKS, idle_task, idle_stack, numelements(idle_stack));
  resume(idle_task_id);
  
  // create the canbus task
  can_init(HARDWARE_REVISION, SOFTWARE_REVISION,
           can_tx_buffer, CAN_TX_LENGTH,
           can_rx_buffer, CAN_RX_LENGTH,
           can_tx_stack, numelements(can_tx_stack), 0,
           can_rx_stack, numelements(can_rx_stack), 0,
           publisher_stack, numelements(publisher_stack), 0);

  // initialize the eeprom as we need our settings first
  eeprom_init(&ee_params);
  // run with an idle task
  run(idle_task_id);

  return 0;
  }

/*
  
  // create the analog worker task
  analog_init(&channels);
  // create the timer channels
  capture_init(&capture_channels);
  
  last_tick = ticks();
  
  read_parameter_definition(memid_fuel_flow_rate, &flow_param);
 */

typedef enum _config_state {
  config_can,
  config_complete
  }config_state;

static config_state init_state = config_can;

// this is called by the eeprom init task
bool eeprom_init_done(semaphore_t *worker_task)
  {
  switch(init_state)
    {
    case config_can :
      if(can_config(memid_node_id, worker_task))
        init_state = config_complete;
      break;
    case config_complete :
    default :
      // TODO: read other config values that are set....
      init_deque(&pmag1_config.rx_queue, RX_BUFFER_LENGTH, pmag1_rx_queue, NUM_RX_BUFFERS);
  
      // create the left mag worker
      init_uart(&pmag1_config, rx_pmag1_stack, numelements(rx_pmag1_stack), tx_pmag1_stack, numelements(tx_pmag1_stack));
      
      send_str(&pmag1_config, "/I1\r");
     // TODO: read other config values that are set....
      init_deque(&pmag2_config.rx_queue, RX_BUFFER_LENGTH, pmag2_rx_queue, NUM_RX_BUFFERS);
  
      // create the left mag worker
      init_uart(&pmag2_config, rx_pmag2_stack, numelements(rx_pmag2_stack), tx_pmag2_stack, numelements(tx_pmag2_stack));
      
      send_str(&pmag2_config, "/I1\r");
      // create the analog worker task
      analog_init(&channels, analog_stack, numelements(analog_stack));
      
      // and publish our values
      register_datapoints(published_parameter, numelements(published_parameter));
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

// P-MAG Communications protocol
//------------------------------------------------------------------------------
//
//	COMMUNICATION COMMANDS:
// (Upper case = command  /  Lower case = data request)
// Format: /(chr)(CR)    for normal commands  (CR= carrage return / 0D hex)
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
  can_msg_t msg;
  char *token = (char *) buffer;
  // process the line
  if(buffer == 0)
    return;
    
  token = strtok(token, delim);
  publish(create_can_msg_short(&msg, config->uart_number == 0 ? id_left_mag_rpm : id_right_mag_rpm, 0, (short) atoi(token)));
  
  token = strtok(0, delim);
  
  token = strtok(0, delim);
  publish(create_can_msg_short(&msg, config->uart_number == 0 ? id_left_mag_adv : id_right_mag_adv, 0, (short) atoi(token)));
  
  token = strtok(0, delim);
  publish(create_can_msg_short(&msg, config->uart_number == 0 ? id_left_mag_map : id_right_mag_map, 0, (short) atoi(token)));
  
  token = strtok(0, delim);
  
  token = strtok(0, delim);
  publish(create_can_msg_short(&msg, config->uart_number == 0 ? id_left_mag_volt : id_right_mag_volt, 0, (short) atoi(token)));
  
  token = strtok(0, delim);
  publish(create_can_msg_short(&msg, config->uart_number == 0 ? id_left_mag_temp : id_right_mag_temp, 0, (short) atoi(token)));
  
  token = strtok(0, delim);
  publish(create_can_msg_short(&msg, config->uart_number == 0 ? id_left_mag_coil1 : id_right_mag_coil1, 0, (short) atoi(token)));
  
  token = strtok(0, delim);
  publish(create_can_msg_short(&msg, config->uart_number == 0 ? id_left_mag_coil2 : id_right_mag_coil2, 0, (short) atoi(token)));
  
  return;
  }
