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
#include <stdint.h>
#include "../dspic33f-lib/eeprom.h"

#include "../can-aerospace/can_aerospace.h"
#include "../dspic-lib/publisher.h"

#define FILTER_IMU 0

static void filter_worker(void *parg);

static semaphore_t filter_semp;

int8_t filter_init(uint16_t *stack, uint16_t stack_size) // pretty much just a place holder for now
  {
  int8_t id;

  ahrs_state.magnetic_variation = 0.0f;


  id = create_task("FILTER", stack, stack_size, filter_worker, 0, NORMAL_PRIORITY - 2);

  resume(id);

  return id;
  }

bool filter_config(semaphore_t *worker_task)
  {
  // TODO: should all of the config params be in the config memory?
  read_parameter_definition(memid_hard_iron_correction, get_parameter(memid_hard_iron_correction), worker_task);
  return true;
  }

#define PI_F 3.14159265F

// this holds the orientation in a ECEF frame
static xyz_t orientation;
static xyz_t compass;
static short hdgd;

#define betaDef		0.1f		// 2 * proportional gain

static qtn_t ahrs_orientation ={
                                .q0 = -1.0,
                                .q1 = 0.0,
                                .q2 = 0.0,
                                .q3 = 0.0
  };
static float beta = betaDef; // filter gain
static void ahrs_update(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz, float interval);
static void ahrs_update_imu(float gx, float gy, float gz, float ax, float ay, float az, float interval);
static float inv_sqrt(float x);

static void filter_worker(void *parg)
  {
  uint32_t last_run = ticks();
  while (true)
    {
    // we update the filter at 10hz
    uint32_t next_run = last_run + 10;
    uint32_t delay;
    uint32_t update_delay;

    // determine how long to wait
    delay = ticks();

    if (delay > next_run) // see if already expired
      delay = 0;
    else
      delay = next_run - delay; // calculate how many MS to wait

    if (delay > 0)
      // we update the filter every 200msec
      wait(&filter_semp, delay);

    delay = ticks();
    update_delay = delay - last_run;
    last_run = delay;

    if (is_set(SENSORUPDATES_mag) && is_set(SENSORUPDATES_accel) && is_set(SENSORUPDATES_gyro))
      {
      /* magnetic field readings corrected for hard iron effects and PCB orientation */
      xyz_add(&ahrs_state.mag, &get_parameter_type_1(memid_hard_iron_correction)->value.XYZ);
      /* magnetic field readings corrected for soft iron efects and PCB orientation */
      transform(&ahrs_state.mag, get_parameter_type_1(memid_soft_iron_correction)->value.MATRIX);

      publish_float(ahrs_state.mag.x, id_roll_angle_magnetic); /* Eq 13 */
      publish_float(ahrs_state.mag.y, id_pitch_angle_magnetic); /* Eq 15 */
      publish_float(ahrs_state.mag.z, id_yaw_angle_magnetic); /* Eq 22 */

      publish_float(ahrs_state.accel.x, id_roll_acceleration);
      publish_float(ahrs_state.accel.y, id_pitch_acceleration);
      publish_float(ahrs_state.accel.z, id_yaw_acceleration);

      publish_float(ahrs_state.gyro.y, id_pitch_rate);
      publish_float(ahrs_state.gyro.x, id_roll_rate);
      publish_float(ahrs_state.gyro.z, id_yaw_rate);

#ifdef SIMPLE_AHRS
      // check to see if the kalman filter has started generating information
      if (get_datapoint_short(id_ahrs_status) < 2)
        {
        // roll: Rotation around the X-axis. -180 <= roll <= 180                                          
        // a positive roll angle is defined to be a clockwise rotation about the positive X-axis          
        //                                                                                                
        //                    y                                                                           
        //      roll = atan2(---)                                                                         
        //                    z                                                                           
        //                                                                                                
        // where:  y, z are returned value from accelerometer sensor                                      
        orientation.x = (float) atan2(ahrs_state.accel.y, ahrs_state.accel.z);

        // pitch: Rotation around the Y-axis. -180 <= roll <= 180                                         
        // a positive pitch angle is defined to be a clockwise rotation about the positive Y-axis         
        //                                                                                                
        //                                 -x                                                             
        //      pitch = atan(-------------------------------)                                             
        //                    y * sin(roll) + z * cos(roll)                                               
        //                                                                                                
        // where:  x, y, z are returned value from accelerometer sensor                                   
        if (ahrs_state.accel.y * sin(orientation.x) + ahrs_state.accel.z * cos(orientation.x) == 0)
          orientation.y = ahrs_state.accel.x > 0 ? (PI_F / 2) : (-PI_F / 2);
        else
          orientation.y = (float) atan(-ahrs_state.accel.x / (ahrs_state.accel.y * sin(orientation.x) + \
                                                                       ahrs_state.accel.z * cos(orientation.x)));

        // heading: Rotation around the Z-axis. -180 <= roll <= 180                                       
        // a positive heading angle is defined to be a clockwise rotation about the positive Z-axis       
        //                                                                                                
        //                                       z * sin(roll) - y * cos(roll)                            
        //   heading = atan2(--------------------------------------------------------------------------)  
        //                    x * cos(pitch) + y * sin(pitch) * sin(roll) + z * sin(pitch) * cos(roll))   
        //                                                                                                
        // where:  x, y, z are returned value from magnetometer sensor                                    
        orientation.z = (float) atan2(ahrs_state.mag.z * sin(orientation.x) - ahrs_state.mag.y * cos(orientation.x), \
                                            ahrs_state.mag.x * cos(orientation.y) + \
                                            ahrs_state.mag.y * sin(orientation.y) * sin(orientation.x) + \
                                            ahrs_state.mag.z * sin(orientation.y) * cos(orientation.x));
        }
#else
      ahrs_update(ahrs_state.gyro.x, ahrs_state.gyro.y, ahrs_state.gyro.z,
                  ahrs_state.accel.x, ahrs_state.accel.y, ahrs_state.accel.z,
                  ahrs_state.mag.x, ahrs_state.mag.y, ahrs_state.mag.z,
                  ((float) update_delay) / 1000);

      float q0q0 = ahrs_orientation.q0 * ahrs_orientation.q0;
      float q1q1 = ahrs_orientation.q1 * ahrs_orientation.q1;
      float q2q2 = ahrs_orientation.q2 * ahrs_orientation.q2;
      float q3q3 = ahrs_orientation.q3 * ahrs_orientation.q3;

      orientation.y = -atan2(2 * ahrs_orientation.q1 * ahrs_orientation.q3 - 2 * ahrs_orientation.q0 * ahrs_orientation.q2,
                             1 - 2 * q1q1 - 2 * q2q2);
      orientation.x = asin(2 * ahrs_orientation.q0 * ahrs_orientation.q1 + 2 * ahrs_orientation.q2 * ahrs_orientation.q3);

      orientation.z = atan2(2 * ahrs_orientation.q0 * ahrs_orientation.q3 - 2 * ahrs_orientation.q1 * ahrs_orientation.q2,
                            1 - 2 * q0q0 - 2 * q2q2);
#endif

      publish_float(0, id_yaw_angle);
      publish_float(orientation.y, id_pitch_angle);
      publish_float(orientation.x, id_roll_angle);

      // indicate that IMU values are published
      publish_short(2, id_ahrs_status);

      /*      
            float cos_pitch = cos(orientation.y);
            float sin_pitch = sin(orientation.y);
            float sin_roll = sin(orientation.x);
            float cos_roll = cos(orientation.x);

            compass.x = ahrs_state.mag.x * cos_pitch +
                        ahrs_state.mag.y * sin_roll * sin_pitch +
                        ahrs_state.mag.z * cos_roll * sin_pitch;
            compass.y = ahrs_state.mag.y * cos_roll -
                        ahrs_state.mag.z * sin_roll;

            compass.z = -atan2(-compass.y, compass.x);

            hdgd = (short) radians_to_degrees(compass.z);
       * */
      hdgd = (short) radians_to_degrees(orientation.z);
      while (hdgd < 0)
        hdgd += 360;
      while (hdgd >= 360)
        hdgd -= 360;

      publish_short(hdgd, id_magnetic_heading);

      hdgd += ahrs_state.magnetic_variation;
      // and adjust for magnetic variation
      publish_short(hdgd, id_heading_angle);
      }

    }
  }

//---------------------------------------------------------------------------------------------------
// AHRS algorithm update

void ahrs_update(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz, float interval)
  {
  float recipNorm;
  float s0, s1, s2, s3;
  float qDot1, qDot2, qDot3, qDot4;
  float hx, hy;
  float _2q0mx, _2q0my, _2q0mz, _2q1mx, _2bx, _2bz, _4bx, _4bz, _2q0, _2q1, _2q2, _2q3, _2q0q2, _2q2q3, q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;

  // Use IMU algorithm if magnetometer measurement invalid (avoids NaN in magnetometer normalisation)
  if ((mx == 0.0f) && (my == 0.0f) && (mz == 0.0f))
    {
    ahrs_update_imu(gx, gy, gz, ax, ay, az, interval);
    return;
    }

  // Rate of change of quaternion from gyroscope
  qDot1 = 0.5f * (-ahrs_orientation.q1 * gx - ahrs_orientation.q2 * gy - ahrs_orientation.q3 * gz);
  qDot2 = 0.5f * (ahrs_orientation.q0 * gx + ahrs_orientation.q2 * gz - ahrs_orientation.q3 * gy);
  qDot3 = 0.5f * (ahrs_orientation.q0 * gy - ahrs_orientation.q1 * gz + ahrs_orientation.q3 * gx);
  qDot4 = 0.5f * (ahrs_orientation.q0 * gz + ahrs_orientation.q1 * gy - ahrs_orientation.q2 * gx);

  // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
  if (!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f)))
    {

    // Normalise accelerometer measurement
    recipNorm = inv_sqrt(ax * ax + ay * ay + az * az);
    ax *= recipNorm;
    ay *= recipNorm;
    az *= recipNorm;

    // Normalise magnetometer measurement
    recipNorm = inv_sqrt(mx * mx + my * my + mz * mz);
    mx *= recipNorm;
    my *= recipNorm;
    mz *= recipNorm;

    // Auxiliary variables to avoid repeated arithmetic
    _2q0mx = 2.0f * ahrs_orientation.q0 * mx;
    _2q0my = 2.0f * ahrs_orientation.q0 * my;
    _2q0mz = 2.0f * ahrs_orientation.q0 * mz;
    _2q1mx = 2.0f * ahrs_orientation.q1 * mx;
    _2q0 = 2.0f * ahrs_orientation.q0;
    _2q1 = 2.0f * ahrs_orientation.q1;
    _2q2 = 2.0f * ahrs_orientation.q2;
    _2q3 = 2.0f * ahrs_orientation.q3;
    _2q0q2 = 2.0f * ahrs_orientation.q0 * ahrs_orientation.q2;
    _2q2q3 = 2.0f * ahrs_orientation.q2 * ahrs_orientation.q3;
    q0q0 = ahrs_orientation.q0 * ahrs_orientation.q0;
    q0q1 = ahrs_orientation.q0 * ahrs_orientation.q1;
    q0q2 = ahrs_orientation.q0 * ahrs_orientation.q2;
    q0q3 = ahrs_orientation.q0 * ahrs_orientation.q3;
    q1q1 = ahrs_orientation.q1 * ahrs_orientation.q1;
    q1q2 = ahrs_orientation.q1 * ahrs_orientation.q2;
    q1q3 = ahrs_orientation.q1 * ahrs_orientation.q3;
    q2q2 = ahrs_orientation.q2 * ahrs_orientation.q2;
    q2q3 = ahrs_orientation.q2 * ahrs_orientation.q3;
    q3q3 = ahrs_orientation.q3 * ahrs_orientation.q3;

    // Reference direction of Earth's magnetic field
    hx = mx * q0q0 - _2q0my * ahrs_orientation.q3 + _2q0mz * ahrs_orientation.q2 + mx * q1q1 + _2q1 * my * ahrs_orientation.q2 + _2q1 * mz * ahrs_orientation.q3 - mx * q2q2 - mx * q3q3;
    hy = _2q0mx * ahrs_orientation.q3 + my * q0q0 - _2q0mz * ahrs_orientation.q1 + _2q1mx * ahrs_orientation.q2 - my * q1q1 + my * q2q2 + _2q2 * mz * ahrs_orientation.q3 - my * q3q3;
    _2bx = sqrt(hx * hx + hy * hy);
    _2bz = -_2q0mx * ahrs_orientation.q2 + _2q0my * ahrs_orientation.q1 + mz * q0q0 + _2q1mx * ahrs_orientation.q3 - mz * q1q1 + _2q2 * my * ahrs_orientation.q3 - mz * q2q2 + mz * q3q3;
    _4bx = 2.0f * _2bx;
    _4bz = 2.0f * _2bz;

    // Gradient decent algorithm corrective step
    s0 = -_2q2 * (2.0f * q1q3 - _2q0q2 - ax) +
      _2q1 * (2.0f * q0q1 + _2q2q3 - ay) -
      _2bz * ahrs_orientation.q2 * (_2bx * (0.5f - q2q2 - q3q3) +
                                    _2bz * (q1q3 - q0q2) - mx) +
      (-_2bx * ahrs_orientation.q3 + _2bz * ahrs_orientation.q1) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) +
      _2bx * ahrs_orientation.q2 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
    s1 = _2q3 * (2.0f * q1q3 - _2q0q2 - ax) +
      _2q0 * (2.0f * q0q1 + _2q2q3 - ay) -
      4.0f * ahrs_orientation.q1 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) +
      _2bz * ahrs_orientation.q3 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) +
      (_2bx * ahrs_orientation.q2 + _2bz * ahrs_orientation.q0) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) +
      (_2bx * ahrs_orientation.q3 - _4bz * ahrs_orientation.q1) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
    s2 = -_2q0 * (2.0f * q1q3 - _2q0q2 - ax) +
      _2q3 * (2.0f * q0q1 + _2q2q3 - ay) -
      4.0f * ahrs_orientation.q2 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) +
      (-_4bx * ahrs_orientation.q2 - _2bz * ahrs_orientation.q0) * (_2bx * (0.5f - q2q2 - q3q3) +
                                                                    _2bz * (q1q3 - q0q2) - mx) +
      (_2bx * ahrs_orientation.q1 + _2bz * ahrs_orientation.q3) * (_2bx * (q1q2 - q0q3) +
                                                                   _2bz * (q0q1 + q2q3) - my) +
      (_2bx * ahrs_orientation.q0 - _4bz * ahrs_orientation.q2) * (_2bx * (q0q2 + q1q3) +
                                                                   _2bz * (0.5f - q1q1 - q2q2) - mz);
    s3 = _2q1 * (2.0f * q1q3 - _2q0q2 - ax) +
      _2q2 * (2.0f * q0q1 + _2q2q3 - ay) +
      (-_4bx * ahrs_orientation.q3 + _2bz * ahrs_orientation.q1) * (_2bx * (0.5f - q2q2 - q3q3) +
                                                                    _2bz * (q1q3 - q0q2) - mx) +
      (-_2bx * ahrs_orientation.q0 + _2bz * ahrs_orientation.q2) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) +
      _2bx * ahrs_orientation.q1 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
    recipNorm = inv_sqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
    s0 *= recipNorm;
    s1 *= recipNorm;
    s2 *= recipNorm;
    s3 *= recipNorm;

    // Apply feedback step
    qDot1 -= beta * s0;
    qDot2 -= beta * s1;
    qDot3 -= beta * s2;
    qDot4 -= beta * s3;
    }

  // Integrate rate of change of quaternion to yield quaternion
  ahrs_orientation.q0 += qDot1 * interval; // TODO: this is based on MS since last tun
  ahrs_orientation.q1 += qDot2 * interval;
  ahrs_orientation.q2 += qDot3 * interval;
  ahrs_orientation.q3 += qDot4 * interval;

  // Normalise quaternion
  recipNorm = inv_sqrt(ahrs_orientation.q0 * ahrs_orientation.q0 + ahrs_orientation.q1 * ahrs_orientation.q1 + ahrs_orientation.q2 * ahrs_orientation.q2 + ahrs_orientation.q3 * ahrs_orientation.q3);
  ahrs_orientation.q0 *= recipNorm;
  ahrs_orientation.q1 *= recipNorm;
  ahrs_orientation.q2 *= recipNorm;
  ahrs_orientation.q3 *= recipNorm;
  }

//---------------------------------------------------------------------------------------------------
// IMU algorithm update

void ahrs_update_imu(float gx, float gy, float gz, float ax, float ay, float az, float interval)
  {
  float recipNorm;
  float s0, s1, s2, s3;
  float qDot1, qDot2, qDot3, qDot4;
  float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2, _8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

  // Rate of change of quaternion from gyroscope
  qDot1 = 0.5f * (-ahrs_orientation.q1 * gx - ahrs_orientation.q2 * gy - ahrs_orientation.q3 * gz);
  qDot2 = 0.5f * (ahrs_orientation.q0 * gx + ahrs_orientation.q2 * gz - ahrs_orientation.q3 * gy);
  qDot3 = 0.5f * (ahrs_orientation.q0 * gy - ahrs_orientation.q1 * gz + ahrs_orientation.q3 * gx);
  qDot4 = 0.5f * (ahrs_orientation.q0 * gz + ahrs_orientation.q1 * gy - ahrs_orientation.q2 * gx);

  // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
  if (!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f)))
    {

    // Normalise accelerometer measurement
    recipNorm = inv_sqrt(ax * ax + ay * ay + az * az);
    ax *= recipNorm;
    ay *= recipNorm;
    az *= recipNorm;

    // Auxiliary variables to avoid repeated arithmetic
    _2q0 = 2.0f * ahrs_orientation.q0;
    _2q1 = 2.0f * ahrs_orientation.q1;
    _2q2 = 2.0f * ahrs_orientation.q2;
    _2q3 = 2.0f * ahrs_orientation.q3;
    _4q0 = 4.0f * ahrs_orientation.q0;
    _4q1 = 4.0f * ahrs_orientation.q1;
    _4q2 = 4.0f * ahrs_orientation.q2;
    _8q1 = 8.0f * ahrs_orientation.q1;
    _8q2 = 8.0f * ahrs_orientation.q2;
    q0q0 = ahrs_orientation.q0 * ahrs_orientation.q0;
    q1q1 = ahrs_orientation.q1 * ahrs_orientation.q1;
    q2q2 = ahrs_orientation.q2 * ahrs_orientation.q2;
    q3q3 = ahrs_orientation.q3 * ahrs_orientation.q3;

    // Gradient decent algorithm corrective step
    s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
    s1 = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * ahrs_orientation.q1 - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
    s2 = 4.0f * q0q0 * ahrs_orientation.q2 + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
    s3 = 4.0f * q1q1 * ahrs_orientation.q3 - _2q1 * ax + 4.0f * q2q2 * ahrs_orientation.q3 - _2q2 * ay;
    recipNorm = inv_sqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
    s0 *= recipNorm;
    s1 *= recipNorm;
    s2 *= recipNorm;
    s3 *= recipNorm;

    // Apply feedback step
    qDot1 -= beta * s0;
    qDot2 -= beta * s1;
    qDot3 -= beta * s2;
    qDot4 -= beta * s3;
    }

  // Integrate rate of change of quaternion to yield quaternion
  ahrs_orientation.q0 += qDot1 * interval;
  ahrs_orientation.q1 += qDot2 * interval;
  ahrs_orientation.q2 += qDot3 * interval;
  ahrs_orientation.q3 += qDot4 * interval;

  // Normalise quaternion
  recipNorm = inv_sqrt(ahrs_orientation.q0 * ahrs_orientation.q0 + ahrs_orientation.q1 * ahrs_orientation.q1 + ahrs_orientation.q2 * ahrs_orientation.q2 + ahrs_orientation.q3 * ahrs_orientation.q3);
  ahrs_orientation.q0 *= recipNorm;
  ahrs_orientation.q1 *= recipNorm;
  ahrs_orientation.q2 *= recipNorm;
  ahrs_orientation.q3 *= recipNorm;
  }

//---------------------------------------------------------------------------------------------------
// Fast inverse square-root
// See: http://en.wikipedia.org/wiki/Fast_inverse_square_root

float inv_sqrt(float x)
  {
  float halfx = 0.5f * x;
  float y = x;
  long i = *(long*) &y;
  i = 0x5f3759df - (i >> 1);
  y = *(float*) &i;
  y = y * (1.5f - (halfx * y * y));
  return y;
  }
