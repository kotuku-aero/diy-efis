#ifndef __i2cm_h__
#define	__i2cm_h__


#include <stdint.h>
#include "microkernel.h"

/**
   * This driver allows for multiplexing the I2C controller
   * amount multiple tasks.  The I2C driver knows nothing
   * about the type of device and all work is done with
   * callbacks.
 * 
 * on PIC32 this runs as IPL5
   */

  typedef enum _i2c_state
  {
    i2c_idle,           // i2c command completed
    i2c_write_command,  // sending a command.
    i2c_write_byte,     // write a byte to the i2c
    i2c_read_byte,      // read a byte from the i2c
  } i2c_state;
  
  struct _i2c_task_t;

  typedef i2c_state (*get_task_state)(struct _i2c_task_t * task, i2c_state last_state);
  typedef uint8_t (*i2c_byte_to_write)(struct _i2c_task_t * task);
  typedef void (*i2c_byte_read)(struct _i2c_task_t * task, uint8_t); 
  /** return the new state to transition to on an error.  If idle is returned the
   the command is cancelled.  otherwise can return i2c_write_command to restart
   the task, caution needed as this could result in an infinite loop */
  typedef i2c_state (*i2c_error)(struct _i2c_task_t *task, i2c_state last_state);
  
  // default routines

  typedef struct _i2c_task_t
    {
    // callback routines
    get_task_state get_state_handler;
    i2c_byte_to_write write_byte_handler;
    i2c_byte_read read_byte_handler;
    i2c_error error_handler;
    uint8_t command_length;       // total number of command bytes
    uint8_t repeat_start;         // point in command string a repeat-start is done
                                  // only used if >0
    void *p_data;
    uint16_t xfer_length;
    uint16_t bytes_xfered;        // will be xfer_length + 2
    uint16_t *read_length;
    uint8_t command[16];           // up to 8 bytes sent as a command header
    semaphore_p task_callback;   // will be signaled when complete
    } i2c_task_t;
    
extern uint16_t last_read_length;

/**
 * return a state change based on a simple write/read protocol
 * @param task
 * @return 
 */
extern i2c_state i2c_get_state_handler(i2c_task_t *task, i2c_state last_state);
/**
 * Get a byte to write, will be a command byte or data byte depending on
 * protocol
 * @param task
 * @return 
 */
extern uint8_t i2c_write_byte_handler(i2c_task_t *task);
/**
 * store a byte into memory
 * @param task
 * @param data
 */
extern void i2c_read_byte_handler(i2c_task_t *task, uint8_t data);
/**
 * Default error handler.  returns idle on error.
 * @param task
 * @param last_state
 * @return 
 */
extern i2c_state i2c_error_handler(i2c_task_t *task, i2c_state last_state);
/**
 * Set up a task with defaults that use built-in routines
 * @param task
 */
extern void init_defaults(i2c_task_t *task);

/*  Adds a task to the i2c queue.  The state machine callbacks will
 * be called when the controller is free.
 */
extern bool enqueue_i2c_task(int channel, const i2c_task_t *task);

#if defined(__dsPIC33)

#define NUM_I2C_CHANNELS 2
#define I2C_CHANNEL_1 0
#define I2C_CHANNEL_2 1
#elif defined(__32MZ2048EFH144__) | defined(__32MZ2064DAH176)
#define NUM_I2C_CHANNELS 5
#define I2C_CHANNEL_1 0
#define I2C_CHANNEL_2 1
#define I2C_CHANNEL_3 2
#define I2C_CHANNEL_4 3
#define I2C_CHANNEL_5 4
#elif defined(__32MZ2048EFH064__)
#define NUM_I2C_CHANNELS 4
#define I2C_CHANNEL_1 0
#define I2C_CHANNEL_3 1
#define I2C_CHANNEL_4 2
#define I2C_CHANNEL_5 3
#endif
/**
 * @function result_t i2c_init()
 * Initialize the i2c subsystem
 * @return s_ok if created ok
 */
extern result_t i2c_init();
/**
 * @function result_t i2c_channel_init(int channel, uint16_t buffer_length, uint16_t i2c_worker_stack_length)
 * Initialize the i2c as a master controller
 * @param channel               Channel to initialize.
 * @param buffer_length           Number of tasks
 * @param i2c_worker_stack_length Stack length
 * @return s_ok if created ok
 */
extern result_t i2c_channel_init(int channel, uint16_t buffer_length, uint16_t i2c_worker_stack_length);

/** 
 * return s_ok if register read
 * return e_no_more_information if end of a block read
 * return e_buffer_too_small if end of a block write
 */
typedef result_t (*i2c_read_reg)(int channel, uint8_t reg, uint8_t *value);
typedef result_t (*i2c_write_reg)(int channel, uint8_t reg, uint8_t value);
typedef result_t (*i2c_valid_reg)(int channel, uint8_t reg);

/**
 * Initialize an i2c channel as a slave
 * @param channel             I2C channel to set up
 * @param slave_address       Address to respond to
 * @param read_register       Callback to read a register
 * @param write_register      Callback to write a register
 * @return s_ok if channel is initialized ok
 * @remark The read and write registers are called from within interupts
 * so must not call blocking called
 */
extern result_t i2cs_init(int channel, 
                          uint16_t slave_address,
                          i2c_read_reg read_register, 
                          i2c_write_reg write_register,
                          i2c_valid_reg valid_register);

/**
 * Return true if the i2c state machine is busy
 * @return 
 */
extern bool i2c_busy();

#endif	/* I2C_MMX_DRIVER_H */

