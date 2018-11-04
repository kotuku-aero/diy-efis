#include "../../diy-efis/libs/neutron/bsp.h"

#include "i2c.h"
#include <string.h>
#include <stdint.h>

#include "microkernel.h"

#define NUM_I2C_TASKS 32
 
static i2c_state eeprom_get_state(i2c_task_t *task, i2c_state last_state)
  {
  if(task->bytes_xfered < task->command_length)
    return ((task->bytes_xfered == 3 || task->bytes_xfered == 0) &&
            last_state != i2c_write_command) ? i2c_write_command : i2c_write_byte;
  
  if(task->bytes_xfered >= (task->xfer_length + task->command_length))
    {
    if(task->command_length == 4 && task->read_length != 0)
      *task->read_length = task->xfer_length - task->command_length;
    
    if(task->task_callback != 0)
      signal_from_isr(task->task_callback);
    
   
    return i2c_idle;
    }
  
  return task->command_length == 3 ? i2c_write_byte : i2c_read_byte;
  }

static i2c_state eeprom_error(i2c_task_t *task, i2c_state last_state)
  {
  // if the write was busy then we poll for the start of the command
  if(task->bytes_xfered == 1)
    {
    task->bytes_xfered = 0;       // restart cmd
    return i2c_write_command;
    }
  
  // ignore the command.  Should be handled better...
  return i2c_idle;
  }

// a memid descrbes a block which id 32 bytes
#define BLOCK_SHIFT 5
#define BLOCK_SIZE (1 << BLOCK_SHIFT)
#define BLOCK_MASK (BLOCK_SIZE -1)
// a page is 64k bytes
#define PAGE_MASK (0x07FF)

// The MEMID is a reference to a 32 byte area of memory
// so we use 
static uint8_t create_command(uint32_t address)
	{
	// base command is A0 for 24LC1025
	uint8_t cmd = 0xA0;
  memid_t block = (memid_t)(address >> BLOCK_SHIFT);
  
	// get the bank select
	cmd |= block & 0x0800 ? 0x08 : 0x00;
	
	// get the chip select
	cmd |= (block & 0x3000) >> 11;
	
	return cmd;
	}

static int i2c_channel;

result_t bsp_reg_read_block(uint32_t address,
                            uint16_t bytes_to_read,
                            void *buffer)
  {
  result_t result;
  uint8_t command_byte = create_command(address);
 
  i2c_task_t task;
  
  // make page offset from start off the bank
  semaphore_t lock = { { 0, 0}, 0 };
  
  // now we read the ee_params->definitions for the parameters (if any)
  task.bytes_xfered = 0;
  task.command[0] = command_byte;       // command byte, send address high
  task.command[1] = (uint8_t)(address >> 8);
  task.command[2] = (uint8_t)(address);
  task.command[3] = command_byte | 0x01;             // read data
  task.command_length = 4;
  task.p_data = buffer;
  task.xfer_length = bytes_to_read;
  task.read_length = 0;
  task.get_state_handler = eeprom_get_state;
  task.write_byte_handler = i2c_write_byte_handler;
  task.read_byte_handler = i2c_read_byte_handler;
  task.error_handler = eeprom_error;
  task.repeat_start = 0;
  task.task_callback = &lock;
  
  if(failed(result = enqueue_i2c_task(i2c_channel, &task)))        // queue the task
    return result;
  
  return semaphore_wait(&lock, INDEFINITE_WAIT);
  }

result_t bsp_reg_write_block(uint32_t address,
                             uint16_t bytes_to_write,
                             const void *buffer)
  {
  uint8_t command_byte = create_command(address);
 
  i2c_task_t task;
  result_t result;
  
  // make page offset from start off the bank
  semaphore_t lock = { {0, 0}, 0 };
  
  // now we read the ee_params->definitions for the parameters (if any)
  task.bytes_xfered = 0;
  task.command[0] = command_byte;       // command byte, send address high
  task.command[1] = (uint8_t)(address >> 8);
  task.command[2] = (uint8_t)(address);
  task.command_length = 3;
  task.p_data = (void *)buffer;
  task.xfer_length = bytes_to_write ;
  task.get_state_handler = eeprom_get_state;
  task.write_byte_handler = i2c_write_byte_handler;
  task.read_byte_handler = i2c_read_byte_handler;
  task.error_handler = eeprom_error;
  task.repeat_start = 0;
  task.task_callback = &lock;
  
  if(failed(result = enqueue_i2c_task(i2c_channel, &task)))        // queue the task
    return result;
  
  return semaphore_wait(&lock, INDEFINITE_WAIT);
  }

result_t eeprom_init(bool factory_reset, int _i2c_channel, uint32_t eeprom_size)
  {
	eeprom_size >>= BLOCK_SHIFT;
  
  if(eeprom_size > 65535)
    return e_bad_parameter;
  
  i2c_channel = _i2c_channel;
  
  // we read the settings from the external eeprom.
  // this is done using the i2c_mmx driver 
  i2cm_init(_i2c_channel, NUM_I2C_TASKS, DEFAULT_STACK_SIZE);         // set up driver
  
	// and initialize the registry functions
	return bsp_reg_init(factory_reset, (memid_t)eeprom_size, 128);
  }
