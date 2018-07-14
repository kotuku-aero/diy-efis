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

#include <math.h>

#define FILTER_IMU 0

static void filter_worker(void *parg);

static semaphore_p filter_semp;


// this is the rate the imu is sampling the sensors
extern uint16_t imu_rate;

result_t filter_init(bool init_eeprom) // pretty much just a place holder for now
  {
  
  semaphore_create(&filter_semp);
  
  memid_t key;
  if(init_eeprom)
    {
    reg_create_key(0, filter_str, &key);
    }
  else
    {
    reg_open_key(0, filter_str, &key);
    }

  task_create("FILTER", DEFAULT_STACK_SIZE << 2, filter_worker, 0, NORMAL_PRIORITY - 2, 0);

  return s_ok;
  }

static void filter_worker(void *parg)
  {
  uint32_t last_run = ticks();
  while (true)
    {
    // we update the filter at 50hz
    uint32_t now = ticks();
    uint32_t delay = now - last_run;
    
    if(delay < imu_rate)
      {
      semaphore_wait(filter_semp, imu_rate - delay);
      continue;
      }
    
    last_run = now;

    if (is_set(SENSORUPDATES_mag) && is_set(SENSORUPDATES_accel) && is_set(SENSORUPDATES_gyro))
      {

      enter_critical();
      // ensure these are not clobbered if we are pre-empted
      memcpy(&mag, &ahrs_state.mag, sizeof(xyz_t));
      memcpy(&accel, &ahrs_state.accel, sizeof(xyz_t));
      memcpy(&gyro, &ahrs_state.gyro, sizeof(xyz_t));
      exit_critical();

      }
    }
  }
