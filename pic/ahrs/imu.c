#include "ahrs.h"

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
 
static i2c_task_t task;

static uint8_t ctrl_reg_set0 = 0;
static uint8_t l3gd20_reg39 = 0x01;			// set low odr
static uint8_t l3gd20_reg23 = 0x00;
static uint8_t l3gd20_reg20 = 0x8f; // DR = 10 (50 Hz ODR); BW = 10 (50 Hz bandwidth); PD = 1 (normal mode); Zen = Yen = Xen = 1 (all axes enabled)

/* The magnetometer is +/- 2gauss
 * 
 */
#define MAG_SCALE 0.00008
//#define MAG_SCALE 1.0
/* The Gyro has 245 dps full scale or the signed
 * To convert to dps = value *0.00875
 * To Radians = 0.000152716309
 * the result is in radians/sec
 */

#define GYRO_SCALE 0.000152716309
//#define GYRO_SCALE 1.0

/* The Accelerometer has +/- 16G full scale
 * value * 0.00048828125
 * or 16 / 32768
 */
#define ACCEL_SCALE 0.000479
//#define ACCEL_SCALE 1.0
static xyz_t hard_iron_correction = {
  .x = 0.0,
  .y = 0.0,
  .z = 0.0
  };

static matrix_t soft_iron_correction = {
  .v[0][0] = 1.0, .v[0][1] = 0.0, .v[0][2] = 0.0,
  .v[1][0] = 0.0, .v[1][1] = 1.0, .v[1][2] = 0.0,
  .v[2][0] = 0.0, .v[2][1] = 0.0, .v[2][2] = 1.0
  };

static qtn_t orientation ={
                                .q0 = -1.0,
                                .q1 = 0.0,
                                .q2 = 0.0,
                                .q3 = 0.0
  };
 
static bool send_imu_msg = false;

static float beta = 0.1; // 0.1f		// 2 * proportional filter gain

static void imu_worker(void *);
static void filter_worker(void *);

static semaphore_t imu_semp;

static inline void matrix_mult(float x, float y, float z, matrix_t matrix, xyz_t *result)
  {
  result->x = x * matrix.v[0][0] + y * matrix.v[0][1] + z * matrix.v[0][2];
  result->y = x * matrix.v[1][0] + y * matrix.v[1][1] + z * matrix.v[1][2];
  result->z = x * matrix.v[2][0] + y * matrix.v[2][1] + z * matrix.v[2][2];
  }

static matrix_t gyro_orientation = {
  GYRO_SCALE, 0.0, 0.0,
  0.0, GYRO_SCALE, 0.0,
  0.0, 0.0, GYRO_SCALE
  };

static matrix_t accel_orientation = {
  ACCEL_SCALE, 0.0, 0.0,
  0.0, ACCEL_SCALE, 0.0,
  0.0, 0.0, ACCEL_SCALE
  };

static matrix_t mag_orientation = {
  MAG_SCALE, 0.0, 0.0,
  0.0, MAG_SCALE, 0.0,
  0.0, 0.0, MAG_SCALE
  };

#define PI_F 3.14159265F


static canmsg_t msg;
static float recipNorm;
static float s0;
static float s1;
static float s2;
static float s3;
static float qDot1;
static float qDot2;
static float qDot3;
static float qDot4;
static float hx;
static float hy;
static float _2q0mx;
static float _2q0my;
static float _2q0mz;
static float _2q1mx;
static float _2bx;
static float _2bz;
static float _4bx;
static float _4bz;
static float _2q0;
static float _2q1;
static float _2q2;
static float _2q3;
static float _2q0q2;
static float _2q2q3;
static float q0q0;
static float q0q1;
static float q0q2;
static float q0q3;
static float q1q1;
static float q1q2;
static float q1q3;
static float q2q2;
static float q2q3;
static float q3q3;

static const char *imu_str = "imu";
static const char *rate = "rate";
static const char *gyro_orient_str = "gyro-orient";
static const char *accel_orient_str = "accel-orient";
static const char *mag_orient_str = "mag-orient";
static const char *channel_str = "channel";
static const char *hard_iron_str = "hard-iron";
static const char *soft_iron_str = "soft-iron";
static const char *orient_str = "orient";
static const char *gain_str = "gain";
static const char *send_imu_msg_str = "imu-data";

#define GYRO_VALID 0x01
#define ACCEL_VALID 0x02
#define MAG_VALID 0x04
#define GYRO_TEMP_VALID 0x08
#define MAG_TEMP_VALID 0x10

typedef struct _imu_data_t {
  uint16_t valid_flags;
  uint32_t timestamp;
  // these are the corrected IMU values
  xyz_t mag;
  xyz_t accel;
  xyz_t gyro;
  int16_t mag_temp;
  int16_t gyro_temp;
  } imu_data_t;
  
#define NUM_SAMPLES 5
static deque_p filter_queue;

uint16_t imu_rate = 20;     // 50hz
  
#if defined(__dsPIC33EP512GP504__)
static uint16_t i2c_channel = I2C_CHANNEL_2;
#else
static uint16_t i2c_channel = I2C_CHANNEL_4;
#endif

result_t imu_init(bool init_eeprom)
  {
  deque_create(sizeof(imu_data_t), NUM_SAMPLES, &filter_queue);
  memid_t key;
  if(init_eeprom)
    {
    reg_create_key(0, imu_str, &key);
    reg_set_matrix(key, gyro_orient_str, &gyro_orientation);
    reg_set_matrix(key, accel_orient_str, &accel_orientation);
    reg_set_matrix(key, mag_orient_str, &mag_orientation);
    reg_set_uint16(key, channel_str, i2c_channel);
    reg_set_xyz(key, hard_iron_str, &hard_iron_correction);
    reg_set_matrix(key, soft_iron_str, &soft_iron_correction);
    reg_set_qtn(key, orient_str, &orientation);
    reg_set_float(key, gain_str, 0.1);
    reg_set_bool(key, send_imu_msg_str, false);
    }
  else
    {
    reg_open_key(0, imu_str, &key);
    reg_get_matrix(key, gyro_orient_str, &gyro_orientation);
    reg_get_matrix(key, accel_orient_str, &accel_orientation);
    reg_get_matrix(key, mag_orient_str, &mag_orientation);
    reg_get_uint16(key, channel_str, &i2c_channel);
    reg_get_xyz(key, hard_iron_str, &hard_iron_correction);
    reg_get_matrix(key, soft_iron_str, &soft_iron_correction);
    reg_get_qtn(key, orient_str, &orientation);
    reg_get_float(key, gain_str, &beta);
    reg_get_bool(key, send_imu_msg_str, &send_imu_msg);
    }
    
  
  // create an ahrs_state task that will wake up every 10msec or when a
  // byte is read from the i2c
  task_create("IMU", DEFAULT_STACK_SIZE, imu_worker, 0, HIGH_PRIORITY, 0);
  task_create("FILTER", DEFAULT_STACK_SIZE, filter_worker, 0, NORMAL_PRIORITY, 0);
  
  // ensure the state machine starts
  semaphore_signal(&imu_semp);
  
  return s_ok;
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
  
  enqueue_i2c_task(i2c_channel, &task);         // queue the task
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
  task.p_data = &l3gd20_reg39; // LOW_ODR
  enqueue_i2c_task(i2c_channel, &task);

  // set for 245 deg/sec
  // 3 wire SPI normal mode
  task.command[1] = 0x23; // CTRL4
  task.p_data = &l3gd20_reg23;
  enqueue_i2c_task(i2c_channel, &task);
  
  // output is at 12.5 hz
  task.command[1] = 0x20; // CTRL1
  task.p_data = &l3gd20_reg20;
  task.task_callback = &imu_semp;
  enqueue_i2c_task(i2c_channel, &task);

  return imu_init_lsm303d;
  }


// Sets AODR3..0 to 0x05 which is 50hz update (20ms)
static uint8_t lsm303_ctrl1 = 0x57;
// +/- 2 gauss sensitivity of 
static uint8_t lsm303_ctrl6 = 0x00;
// Anti-Alias bandwidth: 773Hz
// +/- 16G full scale
static uint8_t lsm303_ctrl2 = 0b00100000;  // AFS set for +/- 16g
/**
 * Initialize the Accelerometer/Gyroscope
 * @return  next state to enter when i2c completed
 */
static imu_state init_lsm303d()
  {
  init_defaults(&task);
  // Accelerometer
  // 0x00 = 0b00000000
  // AFS = 4 (+/- 16 g full scale)
  task.command[0] = 0x3A;
  task.command[1] = 0x21;
  task.command_length = 2;
  task.xfer_length = 1;
  task.p_data = &lsm303_ctrl2;
  enqueue_i2c_task(i2c_channel, &task);
  // 0x57 = 0b01010111
  // AODR = 0101 (50 Hz ODR); AZEN = AYEN = AXEN = 1 (all axes enabled)
  task.command[1] = 0x20;
  task.p_data = &lsm303_ctrl1;
  enqueue_i2c_task(i2c_channel, &task);
  // Magnetometer
  // 0x64 = 0b01100100
  // M_RES = 11 (high resolution mode); M_ODR = 001 (6.25 Hz ODR)
  task.command[1] = 0x25;
  task.p_data = &lsm303_ctrl6;
  enqueue_i2c_task(i2c_channel, &task);
  // 0x00 = 0b00000000
  // MLP = 0 (low power mode off); MD = 00 (continuous-conversion mode)
  task.command[1] = 0x26;
  task.p_data = &ctrl_reg_set0;
  task.task_callback = &imu_semp;
  enqueue_i2c_task(i2c_channel, &task);

  return imu_init_done;
  }

static inline int16_t to_int16(uint8_t lo, uint8_t hi)
  {
  return (int16_t)(((uint16_t)lo) | (((uint16_t)hi) << 8));
  }

static semaphore_t delay_semp;

static void imu_worker(void *parg)
  {
  uint32_t last_run = ticks();
  
  imu_data_t data;
  data.valid_flags = 0;
  
  while(true)
    {
    semaphore_wait(&imu_semp, INDEFINITE_WAIT);

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
        // we have 6 values.  Converted them to radians/sec
        matrix_mult(to_int16(buffer[0x00], buffer[0x01]),
                    to_int16(buffer[0x02], buffer[0x03]),
                    to_int16(buffer[0x04], buffer[0x05]),
                    gyro_orientation, &data.gyro);
        
        
        // fetch the gyro temperature
        create_i2c_read_reg(0xD7, 0x26, 1);
        
        data.valid_flags |= GYRO_VALID;
        
        state = imu_read_gyro_temp;
        break;
      case imu_read_gyro_temp:
        data.gyro_temp = buffer[0x00];
        
        data.valid_flags |= GYRO_TEMP_VALID;

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
        // we have 6 values.
        matrix_mult(to_int16(buffer[0x00], buffer[0x01]),
                    to_int16(buffer[0x02], buffer[0x03]),
                    to_int16(buffer[0x04], buffer[0x05]),
                    accel_orientation, &data.accel);
        
        data.valid_flags |= ACCEL_VALID;

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
        matrix_mult(to_int16(buffer[0x00], buffer[0x01]),
                    to_int16(buffer[0x02], buffer[0x03]),
                    to_int16(buffer[0x04], buffer[0x05]),
                    mag_orientation, &data.mag);
        
        data.valid_flags |= MAG_VALID;

        // get the magnetometer temperature
        create_i2c_read_reg(0xD7, 0x5, 2);

        state = imu_read_mag_temp;
        break;
      case imu_read_mag_temp:
        data.mag_temp = (uint16_t)to_int16(buffer[0x00], buffer[0x01]);
        
        data.valid_flags |= MAG_TEMP_VALID;
        // push the sample onto the queue and release the filter
        push_back(filter_queue, &data, 0);
        
        {
        uint32_t delay = ticks() - last_run;
        if(delay < imu_rate)
          semaphore_wait(&delay_semp, delay);
        }

      case imu_init_done:
        // init the next sample
        data.valid_flags = 0;
        data.timestamp = ticks();
        
        // fetch the gyro status
        create_i2c_read_reg(0xD7, 0x27, 1);

        state = imu_read_gyro_status;
        break;
      }
    }
  }

static inline bool isreal(float f)
{
    long l;
    l = *(long *)&f;
    return((l & 0x7F800000) != 0x7F800000);
}

//---------------------------------------------------------------------------------------------------
// Fast inverse square-root
static inline float inv_sqrt(float x)
  {
  float y;
  /*
  float halfx = 0.5f * x;
  y = x;
  uint32_t i = *(uint32_t*) &y;
  i = 0x5f3759df - (i >> 1);
  y = *(float*) &i;
  y = y * (1.5f - (halfx * y * y));
  
*/  
  uint32_t i = 0x5F1F1412 - (*(uint32_t *)&x >> 1);
  y = *(float*)&i;
  y = y * (1.69000231f - (0.714158168f * x * y * y));

  
  // y = 1/sqrt(x);

  return y;
  }

//---------------------------------------------------------------------------------------------------
// AHRS algorithm update
/**
 * Update the quaternion
 * @param interval  Time between updates in MS
 */

static uint32_t last_run;

static void ahrs_update(imu_data_t *data)
  {
  // work out how long the sample is for
  uint32_t interval = data->timestamp - last_run;
  last_run = data->timestamp;

  // Rate of change of quaternion from gyroscope
  qDot1 = 0.5f * (-orientation.q1 * data->gyro.x - 
                  orientation.q2 * data->gyro.y - 
                  orientation.q3 * data->gyro.z);
  qDot2 = 0.5f * (orientation.q0 * data->gyro.x + 
                  orientation.q2 * data->gyro.z - 
                  orientation.q3 * data->gyro.y);
  qDot3 = 0.5f * (orientation.q0 * data->gyro.y - 
                  orientation.q1 * data->gyro.z + 
                  orientation.q3 * data->gyro.x);
  qDot4 = 0.5f * (orientation.q0 * data->gyro.z + 
                  orientation.q1 * data->gyro.y - 
                  orientation.q2 * data->gyro.x);

  // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
  if (!((data->accel.x == 0.0f) && 
        (data->accel.y == 0.0f) &&
        (data->accel.z == 0.0f)))
    {

    // Normalise accelerometer measurement
    recipNorm = inv_sqrt(data->accel.x * data->accel.x +
                         data->accel.y * data->accel.y +
                         data->accel.z * data->accel.z);
    data->accel.x *= recipNorm;
    data->accel.y *= recipNorm;
    data->accel.z *= recipNorm;

    // Normalise magnetometer measurement
    recipNorm = inv_sqrt(data->mag.x * data->mag.x +
                         data->mag.y * data->mag.y +
                         data->mag.z * data->mag.z);
    data->mag.x *= recipNorm;
    data->mag.y *= recipNorm;
    data->mag.z *= recipNorm;

    // Auxiliary variables to avoid repeated arithmetic
    _2q0mx = 2.0f * orientation.q0 * data->mag.x;
    _2q0my = 2.0f * orientation.q0 * data->mag.y;
    _2q0mz = 2.0f * orientation.q0 * data->mag.z;
    _2q1mx = 2.0f * orientation.q1 * data->mag.x;
    _2q0 = 2.0f * orientation.q0;
    _2q1 = 2.0f * orientation.q1;
    _2q2 = 2.0f * orientation.q2;
    _2q3 = 2.0f * orientation.q3;
    _2q0q2 = 2.0f * orientation.q0 * orientation.q2;
    _2q2q3 = 2.0f * orientation.q2 * orientation.q3;
    q0q0 = orientation.q0 * orientation.q0;
    q0q1 = orientation.q0 * orientation.q1;
    q0q2 = orientation.q0 * orientation.q2;
    q0q3 = orientation.q0 * orientation.q3;
    q1q1 = orientation.q1 * orientation.q1;
    q1q2 = orientation.q1 * orientation.q2;
    q1q3 = orientation.q1 * orientation.q3;
    q2q2 = orientation.q2 * orientation.q2;
    q2q3 = orientation.q2 * orientation.q3;
    q3q3 = orientation.q3 * orientation.q3;

    // Reference direction of Earth's magnetic field
    hx = data->mag.x * q0q0 -
         _2q0my * orientation.q3 +
         _2q0mz * orientation.q2 +
         data->mag.x * q1q1 +
         _2q1 * data->mag.y * orientation.q2 +
         _2q1 * data->mag.z * orientation.q3 -
         data->mag.x * q2q2 -
         data->mag.x * q3q3;
    
    hy = _2q0mx * orientation.q3 +
         data->mag.y * q0q0 - _2q0mz * orientation.q1 +
         _2q1mx * orientation.q2 -
         data->mag.y * q1q1 +
         data->mag.y * q2q2 +
         _2q2 * data->mag.z * orientation.q3 -
         data->mag.y * q3q3;
    
    _2bx = sqrt(hx * hx + hy * hy);
    _2bz = -_2q0mx * orientation.q2 +
           _2q0my * orientation.q1 +
           data->mag.z * q0q0 +
           _2q1mx * orientation.q3 -
           data->mag.z * q1q1 +
           _2q2 * data->mag.y * orientation.q3 -
           data->mag.z * q2q2 + data->mag.z * q3q3;
    _4bx = 2.0f * _2bx;
    _4bz = 2.0f * _2bz;

    // Gradient decent algorithm corrective step
    s0 = -_2q2 * (2.0f * q1q3 - _2q0q2 - data->accel.x) +
      _2q1 * (2.0f * q0q1 + _2q2q3 - data->accel.y) -
      _2bz * orientation.q2 * (_2bx * (0.5f - q2q2 - q3q3) +
                                    _2bz * (q1q3 - q0q2) - data->mag.x) +
      (-_2bx * orientation.q3 + _2bz * orientation.q1) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - data->mag.y) +
      _2bx * orientation.q2 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - data->mag.z);
    s1 = _2q3 * (2.0f * q1q3 - _2q0q2 - data->accel.x) +
      _2q0 * (2.0f * q0q1 + _2q2q3 - data->accel.y) -
      4.0f * orientation.q1 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - data->accel.z) +
      _2bz * orientation.q3 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - data->mag.x) +
      (_2bx * orientation.q2 + _2bz * orientation.q0) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - data->mag.y) +
      (_2bx * orientation.q3 - _4bz * orientation.q1) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - data->mag.z);
    s2 = -_2q0 * (2.0f * q1q3 - _2q0q2 - data->accel.x) +
      _2q3 * (2.0f * q0q1 + _2q2q3 - data->accel.y) -
      4.0f * orientation.q2 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - data->accel.z) +
      (-_4bx * orientation.q2 - _2bz * orientation.q0) * (_2bx * (0.5f - q2q2 - q3q3) +
                                                                    _2bz * (q1q3 - q0q2) - data->mag.x) +
      (_2bx * orientation.q1 + _2bz * orientation.q3) * (_2bx * (q1q2 - q0q3) +
                                                                   _2bz * (q0q1 + q2q3) - data->mag.y) +
      (_2bx * orientation.q0 - _4bz * orientation.q2) * (_2bx * (q0q2 + q1q3) +
                                                                   _2bz * (0.5f - q1q1 - q2q2) - data->mag.z);
    s3 = _2q1 * (2.0f * q1q3 - _2q0q2 - data->accel.x) +
      _2q2 * (2.0f * q0q1 + _2q2q3 - data->accel.y) +
      (-_4bx * orientation.q3 + _2bz * orientation.q1) * (_2bx * (0.5f - q2q2 - q3q3) +
                                                                    _2bz * (q1q3 - q0q2) - data->mag.x) +
      (-_2bx * orientation.q0 + _2bz * orientation.q2) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - data->mag.y) +
      _2bx * orientation.q1 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - data->mag.z);
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
  orientation.q0 += qDot1 * interval; // TODO: this is based on MS since last tun
  orientation.q1 += qDot2 * interval;
  orientation.q2 += qDot3 * interval;
  orientation.q3 += qDot4 * interval;

  // Normalise quaternion
  recipNorm = inv_sqrt(orientation.q0 * orientation.q0 + orientation.q1 * orientation.q1 + orientation.q2 * orientation.q2 + orientation.q3 * orientation.q3);
  orientation.q0 *= recipNorm;
  orientation.q1 *= recipNorm;
  orientation.q2 *= recipNorm;
  orientation.q3 *= recipNorm;
  
  if(!isreal(orientation.q0) ||
     !isreal(orientation.q1) ||
     !isreal(orientation.q2) ||
     !isreal(orientation.q3))
    {
    orientation.q0 = -1.0;
    orientation.q1 = 0;
    orientation.q2 = 0;
    orientation.q3 = 0;
    }
  }

static imu_data_t filter_sample;
static uint16_t imu_valid = 0;

static void filter_worker(void *parg)
  {
  while(true)
    {
    pop_front(filter_queue, &filter_sample, INDEFINITE_WAIT);
        // we are ready when all of these flags are set
    if((filter_sample.valid_flags & (GYRO_VALID | MAG_VALID | ACCEL_VALID))== (GYRO_VALID | MAG_VALID | ACCEL_VALID))
      {
      imu_valid = 1;

      // magnetic field readings corrected for hard iron effects and PCB orientation
      filter_sample.mag.x += hard_iron_correction.x;
      filter_sample.mag.y += hard_iron_correction.y;
      filter_sample.mag.z += hard_iron_correction.z;

      // magnetic field readings corrected for soft iron effects and PCB orientation
      matrix_mult(filter_sample.mag.x, filter_sample.mag.y, filter_sample.mag.z, soft_iron_correction, &filter_sample.mag);

      if(send_imu_msg)
        {
        can_send(create_can_msg_float(&msg, id_roll_angle_magnetic, 0, filter_sample.mag.x)); // Eq 13
        can_send(create_can_msg_float(&msg, id_pitch_angle_magnetic, 0, filter_sample.mag.y)); // Eq 15
        can_send(create_can_msg_float(&msg, id_yaw_angle_magnetic, 0, filter_sample.mag.z)); // Eq 22

        can_send(create_can_msg_float(&msg, id_roll_acceleration, 0, filter_sample.accel.x));
        can_send(create_can_msg_float(&msg, id_pitch_acceleration, 0, filter_sample.accel.y));
        can_send(create_can_msg_float(&msg, id_yaw_acceleration, 0, filter_sample.accel.z));

        can_send(create_can_msg_float(&msg, id_pitch_rate, 0, filter_sample.gyro.y));
        can_send(create_can_msg_float(&msg, id_roll_rate, 0, filter_sample.gyro.x));
        can_send(create_can_msg_float(&msg, id_yaw_rate, 0, filter_sample.gyro.z));
        }

      ahrs_update(&filter_sample);

      q1q1 = orientation.q1 * orientation.q1;
      q2q2 = orientation.q2 * orientation.q2;
      q3q3 = orientation.q3 * orientation.q3;

      // roll (x-axis rotation)
      can_send(create_can_msg_float(&msg, id_roll_angle, 0, 
                                    atan2(2.0 * (orientation.q0 * orientation.q1 + orientation.q2 * orientation.q3),
                                          1.0 - 2.0 * (q1q1 + q2q2))));

      // pitch (y-axis rotation)
      can_send(create_can_msg_float(&msg, id_pitch_angle, 0, 
                                    asin(2.0 * (orientation.q0 * orientation.q2 - orientation.q3 * orientation.q1))));

      // yaw (z-axis rotation)
      can_send(create_can_msg_float(&msg, id_yaw_angle, 0, 
                                    atan2(2.0 * (orientation.q0 * orientation.q3 + orientation.q1 * orientation.q2),
                                          1.0 - 2.0 * (q2q2 + q3q3))));

      /*
      // calculate the compass heading corrected for pitch and roll
      float roll  = atan2(accel.y, sqrt(accel.x * accel.x + accel.z * accel.z));
      float pitch = atan2(accel.x, sqrt(accel.y * accel.y + accel.z * accel.z));

      float cos_pitch = cos(pitch);
      float sin_pitch = sin(pitch);
      float sin_roll = sin(roll);
      float cos_roll = cos(roll);

      compass.y = mag.x * cos_roll +
                  mag.y * sin_roll * sin_pitch -
                  mag.z * sin_roll * cos_pitch;
      compass.x = mag.y * cos_pitch -
                  mag.z * sin_pitch;

      compass.z = -atan2(compass.y, compass.x);

      hdgd = (int16_t)(compass.z * (180/PI_F));

      while (hdgd < 0)
        hdgd += 360;
      while (hdgd >= 360)
        hdgd -= 360;

      can_send(create_can_msg_uint16(&msg, id_magnetic_heading, 0, hdgd));

      hdgd += magnetic_variation;
      // and adjust for magnetic variation
      can_send(create_can_msg_uint16(&msg, id_heading_angle, 0, hdgd));
       * */
      }
    else
      imu_valid = 0;
    
    // indicate that IMU values are published
    can_send(create_can_msg_uint16(&msg, id_imu_valid, 0, imu_valid));
    }
  }
