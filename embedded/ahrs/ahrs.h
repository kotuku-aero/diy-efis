#ifndef __ahrs_h__
#define __ahrs_h__

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

#include <stdint.h>
#include <stdbool.h>

#include "../dspic33f-lib/microkernel.h"
#include "../can-aerospace/parameter_definitions.h"
#include "../can-aerospace/data_types.h"

#define strtof(a, b) (float) strtod(a, b)
 
char *skip_param(char *);

/*
 * This enumerates the settings in the NV ram.
 */
typedef enum _memid_values {
  memid_altitude,
  memid_airspeed,
  memid_nodeid,
  memid_qnh,
  memid_hdg,
  memid_accel_orientation,
  memid_gyro_orientation,
  memid_mag_orientation,
  memid_soft_iron_correction,
  memid_hard_iron_correction,
  memid_airtime,
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

  
// these are called when the nominated parameter changes
extern bool memid_altitude_changed(semaphore_t *worker_task);
extern bool memid_airspeed_changed(semaphore_t *worker_task);
extern bool memid_nodeid_changed(semaphore_t *worker_task);
extern bool memid_qnh_changed(semaphore_t *worker_task);
extern bool memid_hdg_changed(semaphore_t *worker_task);
extern bool memid_accel_orientation_changed(semaphore_t *worker_task);
extern bool memid_gyro_orientation_changed(semaphore_t *worker_task);
extern bool memid_mag_orientation_changed(semaphore_t *worker_task);
   
extern int8_t autopilot_init(uint16_t *stack, uint16_t stack_size);
extern bool autopilot_config(semaphore_t *worker_task);
 
extern void gps_init(uint16_t *rx_stack,
                     uint16_t rx_stack_size,
                     int8_t *rx_pid,
                     uint16_t *tx_stack,
                     uint16_t tx_stack_size,
                     int8_t *tx_pid);
extern bool gps_config(semaphore_t *worker_task);

extern int8_t mm7150_init(uint16_t *stack, uint16_t stack_size);
extern bool mm7150_config(semaphore_t *worker_task);

extern int8_t imu_init(uint16_t *stack, uint16_t stack_size);
extern bool imu_config(semaphore_t *worker_task);

extern int8_t navigation_init(uint16_t *stack, uint16_t stack_size);
extern bool navigation_config(semaphore_t *worker_task);

extern int8_t autopilot_init(uint16_t *stack, uint16_t stack_size);
extern bool autopilot_config(semaphore_t *worker_task);

extern int8_t wmm_init(uint16_t *stack, uint16_t stack_size);
extern bool wmm_config(semaphore_t *worker_task);

extern int8_t filter_init(uint16_t *stack, uint16_t stack_size);
extern bool filter_config(semaphore_t *worker_task);

extern short get_qnh();


// Enumeration for filter result
typedef enum {
    FILTERRESULT_UNINITIALISED = -1,
    FILTERRESULT_OK       = 0,
    FILTERRESULT_WARNING  = 1,
    FILTERRESULT_CRITICAL = 2,
    FILTERRESULT_ERROR    = 3,
} filter_result_t;


typedef enum {
    SENSORUPDATES_gyro      = 0x0001,
    SENSORUPDATES_accel     = 0x0002,
    SENSORUPDATES_mag       = 0x0004,
    SENSORUPDATES_tas       = 0x0040,
    SENSORUPDATES_alt       = 0x0080,
    SENSORUPDATES_lla       = 0x0100,
    SENSORUPDATES_ga        = 0x0200,
    SENSORUPDATES_mv        = 0x0400,
    SENSORUPDATES_incl      = 0x0800,
    SENSORUPDATES_vs        = 0x1000,
    SENSORUPDATES_ias       = 0x2000,
} updated_sensors_t;

extern updated_sensors_t unset_mask(updated_sensors_t mask);
extern updated_sensors_t set_mask(updated_sensors_t mask);
extern bool is_set(updated_sensors_t mask);

#define MAGSTATUS_OK      1
#define MAGSTATUS_AUX     2
#define MAGSTATUS_INVALID 0

typedef struct {
  
  // These are read from the sensors
  xyz_t gyro;               // gyroscope (in radians/sec)
  xyz_t accel;              // accelerometer (in m/s accel)
  xyz_t mag;                // magnetometer (unit vector)
  
  // these are read from the world magnetic model
  xyz_t mag_flux;
  short magnetic_variation; // computed horizontal magnetic variation (degrees)

  // stores gps position as lla (lat, lng, alt)
  lla_t gps_position;
  float gps_ground_speed;           // in m/sec
  bool gps_ground_speed_valid;
  float gps_track;
  bool gps_track_valid;
  date_t date;
  interval_t time;
  float wind_speed;
  float wind_direction;
  bool gps_position_valid;
  
  // computed from the barometric sensors
  float true_airspeed;
  float indicated_airspeed;
  // barometeric pressure.
  float baro_corrected_altitude;
  float pressure_altitude;
  float vertical_speed;
  } ahrs_data_t;

extern ahrs_data_t ahrs_state;

#endif
