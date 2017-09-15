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
#include "../can-aerospace/data_types.h"
#include "../dspic33f-lib/eeprom.h"
#include "../dspic33f-lib/i2c_mmx_driver.h"
#include <math.h>

typedef enum {
  imu_init_l3gd20h,
  imu_init_lsm303d,
  imu_init_done,
  imu_read_gyro_status,
  imu_read_gyro_temp,
  imu_read_gyro,
  imu_read_accel_status,
  imu_read_accel,
  imu_read_mag_status,
  imu_read_mag_temp,
  imu_read_mag,
  } imu_state;

static imu_state state = imu_init_l3gd20h;
static uint16_t bytes_read;       // now many were read
static uint8_t buffer[16];        // maximum bytes to read
static int8_t mag_temp;
static int8_t gyro_temp;
 
static i2c_task_t task;
static uint8_t ctrl_reg_set0 = 0x00;
static uint8_t ctrl_reg1 = 0x06f; // DR = 01 (200 Hz ODR); BW = 10 (50 Hz bandwidth); PD = 1 (normal mode); Zen = Yen = Xen = 1 (all axes enabled)

static uint8_t lsm303_ctrl1 = 0x57;
static uint8_t lsm303_ctrl6 = 0x20;

/* The magnetometer is niormalised to 0-1.0
 */
#define MAG_SCALE 0.000030517578125

/* The Gyro has 245 dps full scale or the signed
 * value * degrees_to_radians(0.0074770348216193)
 * 
 * or value * 1.304988759234993e-4
 * 
 * the result is degrees/second in radins
 */

//#define GYRO_SCALE 0.0001304988759234993f
#define GYRO_SCALE MAG_SCALE

/* The Accelerometer has +/- 8G full scale
 * value * 0.000244140625
 */

//#define ACCEL_SCALE 0.000244140625f
#define ACCEL_SCALE MAG_SCALE

static void imu_worker(void *);
static semaphore_t imu_semp;

static void matrix_mult(float x, float y, float z, matrix_t matrix, xyz_t *result)
  {
  result->x = x * matrix[0][0] + y * matrix[0][1] + z * matrix[0][2];
  result->y = y * matrix[1][0] + y * matrix[1][1] + z * matrix[1][2];
  result->z = z * matrix[2][0] + z * matrix[2][1] + z * matrix[2][2];
  }

int8_t imu_init(uint16_t *stack, uint16_t stack_size)
  {
  int8_t task_id;
  // create an ahrs_state task that will wake up every 10msec or when a
  // byte is read from the i2c
  resume(task_id = create_task("IMU", stack, stack_size, imu_worker, 0, NORMAL_PRIORITY));
  
  // ensure the state machine starts
  signal(&imu_semp);
  
  return task_id;
  }

bool imu_config(semaphore_t *worker_task)
  {
  signal(worker_task);
  return true;
  }

// set up the simplest read-register command
static void create_i2c_read_reg(uint8_t cmd, uint8_t address, uint8_t len)
  {
  init_defaults(&task);
  task.task_callback = &imu_semp;
  task.command[0] = cmd & 0xfe;
  task.command[1] = address;
  task.command[2] = cmd | 0x01;
  task.command_length = 3;
  task.p_data = (void *)&buffer;
  task.xfer_length = len ;
  task.read_length = &bytes_read;
  task.repeat_start = 2;
  
  enqueue_i2c_task(&task);         // queue the task
  }

/**
 * Initialize the Gyroscope device
 * @return next state to enter when i2c completed
 */
static imu_state init_l3gd20h()
  {
  init_defaults(&task);

  task.command[0] = 0xD6;
  task.command[1] = 0x39;
  task.command_length = 2;
  task.xfer_length = 1;
  task.p_data = &ctrl_reg_set0; // LOW_ODR
  enqueue_i2c_task(&task);

  // set for 245 deg/sec
  // 3 wire SPI normal mode
  task.command[1] = 0x01; // CTRL4
  enqueue_i2c_task(&task);

  // output is at 12.5 hz
  task.command[1] = 0x20; // CTRL1
  task.p_data = &ctrl_reg1;
  task.task_callback = &imu_semp;
  enqueue_i2c_task(&task);

  return imu_init_lsm303d;
  }

/**
 * Initialize the Accelerometer/Gyroscope
 * @return  next state to enter when i2c completed
 */
static imu_state init_lsm303d()
  {
  init_defaults(&task);
  // Accelerometer
  // 0x00 = 0b00000000
  // AFS = 0 (+/- 2 g full scale)
  // Accelerometer
  // AFS = 0 (+/- 2 g full scale)
  task.command[0] = 0x3A;
  task.command[1] = 0x21;
  task.command_length = 2;
  task.xfer_length = 1;
  task.p_data = &ctrl_reg_set0;
  enqueue_i2c_task(&task);
  // 0x57 = 0b01010111
  // AODR = 0101 (50 Hz ODR); AZEN = AYEN = AXEN = 1 (all axes enabled)
  task.command[1] = 0x20;
  task.p_data = &lsm303_ctrl1;
  enqueue_i2c_task(&task);
  // Magnetometer
  // 0x64 = 0b01100100
  // M_RES = 11 (high resolution mode); M_ODR = 001 (6.25 Hz ODR)
  task.command[1] = 0x25;
  task.p_data = &lsm303_ctrl6;
  enqueue_i2c_task(&task);
  // 0x00 = 0b00000000
  // MLP = 0 (low power mode off); MD = 00 (continuous-conversion mode)
  task.command[1] = 0x26;
  task.p_data = &ctrl_reg_set0;
  task.task_callback = &imu_semp;
  enqueue_i2c_task(&task);

  return imu_init_done;
  }

static void imu_worker(void *parg)
  {
  int16_t temp;
  can_parameter_type_1_t *param;
  while(true)
    {
    wait(&imu_semp, INDEFINITE_WAIT);

    switch(state)
      {
      case imu_init_l3gd20h:
        state = init_l3gd20h();
        break;
      case imu_init_lsm303d:
        state = init_lsm303d();
        break;
      case imu_read_gyro_status:
        // see if gyro ready
        if((buffer[0x00] & 0x07) != 7)
          {
          // no fetch the gyro temp
          create_i2c_read_reg(0xD7, 0x26, 1);

          state = imu_read_gyro_temp;
          }
        else
          {
          create_i2c_read_reg(0xD7, 0xa8, 6); // read gyro registers
          state = imu_read_gyro;
          }
        break;
      case imu_read_gyro:
        param = get_parameter_type_1(memid_gyro_orientation);
        // we have 6 values.  Converted them to radians/sec
        matrix_mult(((int16_t)buffer[0x00] | (buffer[0x01] << 8)) * GYRO_SCALE,
                    ((int16_t)buffer[0x02] | (buffer[0x03] << 8)) * GYRO_SCALE,
                    ((int16_t)buffer[0x04] | (buffer[0x05] << 8)) * GYRO_SCALE,
                    param->value.MATRIX, &ahrs_state.gyro);
        set_mask(SENSORUPDATES_gyro);

        // fetch the gyro temperature
        create_i2c_read_reg(0xD7, 0x26, 1);

        state = imu_read_gyro_temp;
        break;
      case imu_read_gyro_temp:
        // 8 bit value
        gyro_temp = (int8_t) buffer[0x00];

        // read the accelerometer status
        create_i2c_read_reg(0x3B, 0x27, 1);
        state = imu_read_accel_status;
        break;
      case imu_read_accel_status:
        if((buffer[0x00] & 0x07) != 7)
          {
          // fetch the magnetometer status
          create_i2c_read_reg(0x3B, 0x07, 1);

          state = imu_read_mag_status;
          }
        else
          {
          // read the accelerometer
          create_i2c_read_reg(0x3B, 0xa8, 6);
          state = imu_read_accel;
          }
        break;
      case imu_read_accel:
        param = get_parameter_type_1(memid_accel_orientation);
        // we have 6 values.
        matrix_mult(((int16_t)buffer[0x00] | (buffer[0x01] << 8)) * ACCEL_SCALE,
                    ((int16_t)buffer[0x02] | (buffer[0x03] << 8)) * ACCEL_SCALE,
                    ((int16_t)buffer[0x04] | (buffer[0x05] << 8)) * ACCEL_SCALE,
                    param->value.MATRIX, &ahrs_state.accel);
        set_mask(SENSORUPDATES_accel);

        // fetch the magnetometer status
        create_i2c_read_reg(0x3B, 0x07, 1);
        state = imu_read_mag_status;
        break;
      case imu_read_mag_status:
        if((buffer[0x00] & 0x07) != 7)
          {
          // fetch the gyro status
          create_i2c_read_reg(0xD7, 0x27, 1);
          state = imu_read_gyro_status;
          }
        else
          {
          create_i2c_read_reg(0x3B, 0x88, 6);
          state = imu_read_mag;
          }
        break;
      case imu_read_mag:
        param = get_parameter_type_1(memid_mag_orientation);
        matrix_mult(((int16_t)buffer[0x00] | (buffer[0x01] << 8)) * MAG_SCALE,
                    ((int16_t)buffer[0x02] | (buffer[0x03] << 8)) * MAG_SCALE,
                    ((int16_t)buffer[0x04] | (buffer[0x05] << 8)) * MAG_SCALE,
                    param->value.MATRIX, &ahrs_state.mag);
       
        set_mask(SENSORUPDATES_mag);

        // get the magnetometer temperature
        create_i2c_read_reg(0xD7, 0x5, 2);

        state = imu_read_mag_temp;
        break;
      case imu_read_mag_temp:
        temp = buffer[0x00] + (buffer[0x01] << 8);

        mag_temp = (int8_t) (temp >> 4);

      case imu_init_done:
        // fetch the gyro status
        create_i2c_read_reg(0xD7, 0x27, 1);

        state = imu_read_gyro_status;
        break;
      }
    }
  }