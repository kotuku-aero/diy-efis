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
#include <stdbool.h>
#include <string.h>
#include <p33Exxxx.h>
#include "i2c_mmx_driver.h"
#include "microkernel.h"
#include "pps_maps.h"

//#define DEBUG_I2C

static deque_t deque;
static semaphore_t i2c_ready_semp;
i2c_task_t current_i2c_task;

bool enqueue_i2c_task(const i2c_task_t *task)
  {
  push_back(&deque, task, INDEFINITE_WAIT);
  
  return true;
  }

void init_defaults(i2c_task_t *task)
  {
  memset(task, 0, sizeof(i2c_task_t));
  
  task->get_state_handler = i2c_get_state_handler;
  task->write_byte_handler = i2c_write_byte_handler;
  task->read_byte_handler = i2c_read_byte_handler;
  task->error_handler = i2c_error_handler;
  }

uint16_t last_read_length = 0;
  
i2c_state i2c_get_state_handler(i2c_task_t *task, i2c_state last_state)
  {
  if(task->bytes_xfered < task->command_length)
    return (task->repeat_start > 0 &&
            task->bytes_xfered == task->repeat_start) ? i2c_write_command : i2c_write_byte;
  
  if((task->bytes_xfered - task->command_length) >= task->xfer_length)
    {
    if(task->read_length != 0)
      *task->read_length = task->xfer_length;
    
    if(task->task_callback != 0)
      signal_from_isr(task->task_callback);
    
   
    return i2c_idle;
    }
  
  return task->read_length == 0 ? i2c_write_byte : i2c_read_byte;
  }

uint8_t i2c_write_byte_handler(i2c_task_t *task)
  {
  uint8_t result = 0;
  
  if(task->bytes_xfered >= task->command_length)
    result = ((uint8_t *)task->p_data)[task->bytes_xfered - task->command_length];
  else
    result = task->command[task->bytes_xfered];
  
  task->bytes_xfered++;
  
  return result;
  }

void i2c_read_byte_handler(i2c_task_t *task, uint8_t data)
  {
  ((uint8_t *)task->p_data)[task->bytes_xfered - task->command_length] = data;
  task->bytes_xfered++;
  }

i2c_state i2c_error_handler(i2c_task_t *task, i2c_state last_state)
  {
  // ignore the command.  Should be handled better...
  return i2c_idle;
  }
  
static i2c_state client_state = i2c_idle;

typedef enum _i2c_msg_state
  {
  st_i2c_idle,
  st_i2c_start,
  st_i2c_write_byte,
  st_i2c_repeat_start,
  st_i2c_read_byte,
  st_i2c_ack,
  st_i2c_nack,
  st_i2c_stop
  } i2c_msg_state;
  
i2c_msg_state current_i2c_state = st_i2c_idle;

#ifdef DEBUG_I2C
i2c_msg_state state_buffer[32];
uint16_t state_ptr = 0; 
uint8_t write_log[1024];
uint16_t write_ptr = 0;
uint8_t read_log[1024];
uint16_t read_ptr = 0;
#endif

static void change_state(i2c_msg_state _state)
  {
  current_i2c_state = _state;
  
  if(current_i2c_state == st_i2c_idle)
    signal_from_isr(&i2c_ready_semp);
    
#ifdef DEBUG_I2C
  if(state_ptr < 32)
    state_buffer[state_ptr++] = current_i2c_state;
#endif
  }

static void send_byte(uint8_t byte_to_write)
  {
#ifdef DEBUG_I2C
  if(write_ptr < 1024)
    write_log[write_ptr++] = byte_to_write;
#endif
  
  I2C2TRN = byte_to_write;
  }

static uint8_t read_byte()
  {
  uint8_t result = I2C2RCV;
#ifdef DEBUG_I2C
  if(read_ptr < 1024)
    read_log[read_ptr++] = result;
#endif
  
  return result;
  }

static void I2CInterrupt()
  {
  // operation completed so update state to next state
  i2c_state next_state = client_state;
  
  // ask the client what we are to do next.
  if(client_state != i2c_idle)
    next_state = (*current_i2c_task.get_state_handler)(&current_i2c_task, client_state);
  
  switch(current_i2c_state)
    {
    case st_i2c_idle :              // i2c is idle
      break;
    case st_i2c_start :           // SEN bit sent
      // get the address byte
      send_byte((*current_i2c_task.write_byte_handler)(&current_i2c_task));
      change_state(st_i2c_write_byte);
      break;
    case st_i2c_write_byte :
      if(I2C2STATbits.ACKSTAT == 1)
        {
        change_state(st_i2c_nack);
        if((*current_i2c_task.error_handler)(&current_i2c_task, client_state) == i2c_idle)
          change_state(st_i2c_idle);
        else
          {
          //write_ptr--;
          // retry the whole command.
          I2C2CONbits.SEN = 1;
          client_state = i2c_write_command;     // in sen mode
          change_state(st_i2c_start);
          }
        break;
        }
      // not an error so we decide what next
      switch(next_state)
        {
        case i2c_idle :
          I2C2CONbits.PEN = 1;   // and done.
          change_state(st_i2c_stop);
          break;
        case i2c_write_command :
          I2C2CONbits.RSEN = 1;    // issue next command
          change_state(st_i2c_repeat_start);
          break;
        case i2c_write_byte :
          send_byte((*current_i2c_task.write_byte_handler)(&current_i2c_task));
          change_state(st_i2c_write_byte);
          break;
        case i2c_read_byte :
          if(I2C2STATbits.RBF == 1)
            {
            // empty the buffer.
            uint8_t buf = I2C2RCV;
            }
          I2C2CONbits.RCEN = 1;    // enable receive
          change_state(st_i2c_read_byte);
          break;
        }
      break;
    case st_i2c_read_byte :
      if(I2C2STATbits.RBF == 0)
        {
        // error as we expect an rbf
        change_state(st_i2c_nack);
        (*current_i2c_task.error_handler)(&current_i2c_task, client_state);
        change_state(st_i2c_idle);
        break;
        }
      (*current_i2c_task.read_byte_handler)(&current_i2c_task, read_byte());
      // update the next state.
      next_state = (*current_i2c_task.get_state_handler)(&current_i2c_task, client_state);
      switch(next_state)
        {
        case i2c_idle :
          I2C2CONbits.ACKDT = 1;
          I2C2CONbits.ACKEN = 1;
          change_state(st_i2c_nack);
          break;
        case i2c_read_byte :
          I2C2CONbits.ACKDT = 0;
          I2C2CONbits.ACKEN = 1;
          change_state(st_i2c_ack);
          break;
        }
      break;
    case st_i2c_repeat_start :
      send_byte((*current_i2c_task.write_byte_handler)(&current_i2c_task));
      change_state(st_i2c_write_byte);
      break;
    case st_i2c_nack :
      I2C2CONbits.PEN = 1;   // and done.
      change_state(st_i2c_stop);
      break;
    case st_i2c_ack :
      switch(next_state)
        {
        case i2c_idle :
          I2C2CONbits.PEN = 1;   // and done.
          change_state(st_i2c_stop);
          break;
        case i2c_read_byte :
          I2C2CONbits.RCEN = 1;    // enable receive
          change_state(st_i2c_read_byte);
          break;
        }
      break;
    case st_i2c_stop :
      I2C2CONbits.ACKDT = 0;
      change_state(st_i2c_idle);
      break;
    }
  
  client_state = next_state;
  
  I2C2STATbits.I2COV = 0;     // clear in case of overflow on
                              // last error
  }

void __attribute__((interrupt, no_auto_psv)) _MI2C2Interrupt(void)
  {
  I2CInterrupt();
  IFS3bits.MI2C2IF = 0;
  }

/**
 * Worker to send a new i2c command
 * @param parg not used
 */
static void i2c_worker(void *parg)
  {
  while(true)
    {
    // wait for a msg
    pop_front(&deque, &current_i2c_task, INDEFINITE_WAIT);
#ifdef DEBUG_I2C
    state_ptr = 0;
#endif
    enter_critical();
    I2C2CONbits.SEN = 1;
    client_state = i2c_write_command;     // in sen mode
    change_state(st_i2c_start);
    exit_critical();
    
    do
      {
      // we now wait for the interrupt that returns the controller
      // to idle
      wait(&i2c_ready_semp, INDEFINITE_WAIT);
      } while(current_i2c_state != st_i2c_idle);
    }
  }

#define FCY 70000000
#define FSCL  400000
#define CALC_I2CBRG 165 // (((1/FSCL)-0.000000012)*(FCY/2))-2

int8_t i2c_init(i2c_task_t *buffer,
              uint16_t len,
              uint16_t *i2c_worker_stack,
              uint16_t i2c_worker_stack_length)
  {
  int8_t pid;
  
  init_deque(&deque, sizeof(i2c_task_t), buffer, len);
  
  TRISBbits.TRISB4 = 1;
  TRISAbits.TRISA8 = 1;
  
  // disable the i2c controller
  I2C2CONbits.I2CEN = 0;
  //I2C2CONHbits.SBCDE = 0;
  
   // we set up the baud rate to 400khz
  I2C2BRG = CALC_I2CBRG;
  
  // set up the worker task
  resume(pid = create_task("I2C", i2c_worker_stack, i2c_worker_stack_length,
                           i2c_worker, 0, NORMAL_PRIORITY));
  
  // set mask to allow all data
  I2C2MSK = 0;
   // enable interrupts
  IFS3bits.MI2C2IF = 0;
  //IFS10bits.I2C2BCIF = 0;
  IEC3bits.MI2C2IE = 1;
  //IEC10bits.I2C2BCIE = 1;
  IPC12bits.MI2C2IP = 6;
  //IPC43bits.I2C2BCIP = 6;
  // enable I2C
  I2C2CONbits.I2CEN = 1;
  
  return pid;
  }
