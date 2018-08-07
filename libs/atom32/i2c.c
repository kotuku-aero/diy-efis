#include <stdbool.h>
#include <string.h>
#include "../atom/i2c.h"
#include "../atom/microkernel.h"

//#define DEBUG_I2C

/*
 * This code uses Timer4 to implement the I2C timout error
 */

deque_p i2c_worker_queues[NUM_I2C_CHANNELS];
static semaphore_p i2c_ready_semp[NUM_I2C_CHANNELS];
i2c_task_t current_i2c_task[NUM_I2C_CHANNELS];

bool enqueue_i2c_task(int channel, const i2c_task_t *task)
  {
  push_back(i2c_worker_queues[channel], task, INDEFINITE_WAIT);
  
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
  
static i2c_state client_state[NUM_I2C_CHANNELS];

typedef enum _i2c_msg_state
  {
  st_i2c_idle,
  st_i2c_start,
  st_i2c_write_byte,
  st_i2c_repeat_start,
  st_i2c_read_byte,
  st_i2c_ack,
  st_i2c_nack,
  st_i2c_stop,
  st_i2c_timeout,       // current operation has timed out
  } i2c_msg_state;
  
i2c_msg_state current_i2c_state[NUM_I2C_CHANNELS];

typedef struct _i2c_init_port_t {
  volatile unsigned int *addr;
  uint32_t mask;
  uint32_t value;
  } i2c_init_port_t;

// TODO this is for the 064 package
static i2c_init_port_t i2c1_fix[] = {
  &I2C1CON, 0x00008000, 0x00000000,    // I2C off
  &I2C1CON, 0x00008000, 0x00008000,    // I2C on
  0,0
  };

#if !defined(__32MZ2048EFH064__)
static i2c_init_port_t i2c2_fix[] = {
  0,0
  };
#endif

static i2c_init_port_t i2c3_fix[] = {
  &I2C3CON, 0x00008000, 0x00000000,    // I2C off
  &I2C3CON, 0x00008000, 0x00008000,    // I2C on
  0,0
  };
  
static i2c_init_port_t i2c4_fix[] = {
  &I2C4CON, 0x00008000, 0x00000000,    // I2C off
  &I2C4CON, 0x00008000, 0x00008000,    // I2C on
  0,0
  };

  
static i2c_init_port_t i2c5_fix[] = {
  &I2C5CON, 0x00008000, 0x00000000,    // I2C off
  &I2C5CON, 0x00008000, 0x00008000,    // I2C on
  0,0
  };

// these get set as required...
i2c_init_port_t *i2c_fixups[NUM_I2C_CHANNELS] = {
  i2c1_fix,
#if !defined(__32MZ2048EFH064__)
  i2c2_fix,
#endif
  i2c3_fix,
  i2c4_fix,
  i2c5_fix,
  };

int32_t i2c_state_time[NUM_I2C_CHANNELS];
int32_t i2c_state_timeout[NUM_I2C_CHANNELS];

#ifdef DEBUG_I2C
i2c_msg_state state_buffer[NUM_I2C_CHANNELS][32];
uint16_t state_ptr[NUM_I2C_CHANNELS]; 
uint8_t write_log[NUM_I2C_CHANNELS][1024];
uint16_t write_ptr[NUM_I2C_CHANNELS];
uint8_t read_log[NUM_I2C_CHANNELS][1024];
uint16_t read_ptr[NUM_I2C_CHANNELS];
#endif

static void t4_on()
  {
  T4CONbits.TON = 1;
  }

static void t4_off()
  {
  T4CONbits.TON = 0;
  }

static void change_state(int channel, i2c_msg_state _state)
  {
  current_i2c_state[channel] = _state;
  
  if(_state == st_i2c_idle)
    {
    i2c_fixups[channel] = 0;        // reset the timeout counter
    signal_from_isr(i2c_ready_semp[channel]);
    }
  else if(_state = st_i2c_start)
    {
    // reset the timeout counter
    i2c_state_time[channel] = 0;
    i2c_state_timeout[channel] = 0;
    
    t4_on();
    }
  else
    i2c_state_time[channel]++;      // signal controller running
    
#ifdef DEBUG_I2C
  if(state_ptr < 32)
    state_buffer[state_ptr++] = current_i2c_state;
#endif
  }

static void i2c_interrupt(int channel);

void __attribute__( (interrupt(IPL0AUTO), vector(_TIMER_4_VECTOR))) _t4_interrupt( void );
void t4_interrupt(void)
  {
  IFS0bits.T4IF = 0;
  
  bool is_idle = true;
  int i;
  for(i = 0; i < NUM_I2C_CHANNELS; i++)
    {
    // see if the controller is running.
    if(current_i2c_state[i] == st_i2c_idle ||
       current_i2c_state[i] == st_i2c_timeout)
      continue;
    
    is_idle = false;
    
    i2c_state_timeout[i]++;
    
    // see if our timeout exceeds the state for the last command
    // we expect up to 1 count as the timer is not sync'd to the commands
    int32_t state_duration = i2c_state_timeout[i] - i2c_state_time[i];
    if(state_duration < 8)
      continue;         // check the next channel

    // get the command to reset the i2c device
    i2c_init_port_t *fixup = i2c_fixups[i];
    volatile unsigned int *port = fixup->addr;
    while(port != 0)
      {
      uint32_t value = *port;
      value &= ~fixup->mask;
      value |= fixup->value;
      *port = value;
   
      fixup++;
      port = fixup->addr;
      }

    // signal 
    i2c_fixups[i] = 0;
    change_state(i, st_i2c_timeout);    // signal that the device has timed out
    i2c_interrupt(i);         // simulate the stop irq
    }
  
  if(is_idle)
    t4_off();
  }

static void send_byte(int channel, uint8_t byte_to_write)
  {
#ifdef DEBUG_I2C
  if(write_ptr < 1024)
    write_log[write_ptr++] = byte_to_write;
#endif
  
  switch(channel)
    {
    case I2C_CHANNEL_1 :
      I2C1TRN = byte_to_write;
      break;
#if !defined(__32MZ2048EFH064__)
    case I2C_CHANNEL_2 :
      I2C2TRN = byte_to_write;
      break;
#endif
    case I2C_CHANNEL_3 :
      I2C3TRN = byte_to_write;
      break;
    case I2C_CHANNEL_4 :
      I2C4TRN = byte_to_write;
      break;
    case I2C_CHANNEL_5 :
      I2C5TRN = byte_to_write;
      break;
    }
  
  }

static uint8_t read_byte(int channel)
  {
  uint8_t result;
  switch(channel)
    {
    case I2C_CHANNEL_1 :
      result = I2C1RCV;
      break;
#if !defined(__32MZ2048EFH064__)
    case I2C_CHANNEL_2 :
      result = I2C2RCV;
      break;
#endif
    case I2C_CHANNEL_3 :
      result = I2C3RCV;
      break;
    case I2C_CHANNEL_4 :
      result = I2C4RCV;
      break;
    case I2C_CHANNEL_5 :
      result = I2C5RCV;
      break;
    }
#ifdef DEBUG_I2C
  if(read_ptr < 1024)
    read_log[read_ptr++] = result;
#endif
  
  return result;
  }

static void set_sen(int channel)
  {
  switch(channel)
    {
    case I2C_CHANNEL_1 :
      I2C1CONbits.SEN = 1;
      break;
#if !defined(__32MZ2048EFH064__)
    case I2C_CHANNEL_2 :
      I2C2CONbits.SEN = 1;
      break;
#endif
    case I2C_CHANNEL_3 :
      I2C3CONbits.SEN = 1;
      break;
    case I2C_CHANNEL_4 :
      I2C4CONbits.SEN = 1;
      break;
    case I2C_CHANNEL_5 :
      I2C5CONbits.SEN = 1;
      break;
    }
  }

static void set_rcen(int channel)
  {
  switch(channel)
    {
    case I2C_CHANNEL_1 :
      I2C1CONbits.RCEN = 1;
      break;
#if !defined(__32MZ2048EFH064__)
    case I2C_CHANNEL_2 :
      I2C2CONbits.RCEN = 1;
      break;
#endif
    case I2C_CHANNEL_3 :
      I2C3CONbits.RCEN = 1;
      break;
    case I2C_CHANNEL_4 :
      I2C4CONbits.RCEN = 1;
      break;
    case I2C_CHANNEL_5 :
      I2C5CONbits.RCEN = 1;
      break;
    }
  }

static void i2c_interrupt(int channel);

static void set_pen(int channel)
  {
  switch(channel)
    {
    case I2C_CHANNEL_1 :
      I2C1CONbits.PEN = 1;
      break;
#if !defined(__32MZ2048EFH064__)
    case I2C_CHANNEL_2 :
      I2C2CONbits.PEN = 1;
      break;
#endif
    case I2C_CHANNEL_3 :
      I2C3CONbits.PEN = 1;
      break;
    case I2C_CHANNEL_4 :
      I2C4CONbits.PEN = 1;
      break;
    case I2C_CHANNEL_5 :
      I2C5CONbits.PEN = 1;
      break;
    }
  }

static void set_rsen(int channel)
  {
  switch(channel)
    {
    case I2C_CHANNEL_1 :
      I2C1CONbits.RSEN = 1;
      break;
#if !defined(__32MZ2048EFH064__)
    case I2C_CHANNEL_2 :
      I2C2CONbits.RSEN = 1;
      break;
#endif
    case I2C_CHANNEL_3 :
      I2C3CONbits.RSEN = 1;
      break;
    case I2C_CHANNEL_4 :
      I2C4CONbits.RSEN = 1;
      break;
    case I2C_CHANNEL_5 :
      I2C5CONbits.RSEN = 1;
      break;
    }
  }

static void set_ackdt(int channel, int value)
  {
  switch(channel)
    {
    case I2C_CHANNEL_1 :
      I2C1CONbits.ACKDT = value;
      break;
#if !defined(__32MZ2048EFH064__)
    case I2C_CHANNEL_2 :
      I2C2CONbits.ACKDT = value;
      break;
#endif
    case I2C_CHANNEL_3 :
      I2C3CONbits.ACKDT = value;
      break;
    case I2C_CHANNEL_4 :
      I2C4CONbits.ACKDT = value;
      break;
    case I2C_CHANNEL_5 :
      I2C5CONbits.ACKDT = value;
      break;
    }
  }

static void set_acken(int channel, int value)
  {
  switch(channel)
    {
    case I2C_CHANNEL_1 :
      I2C1CONbits.ACKEN = value;
      break;
#if !defined(__32MZ2048EFH064__)
    case I2C_CHANNEL_2 :
      I2C2CONbits.ACKEN = value;
      break;
#endif
    case I2C_CHANNEL_3 :
      I2C3CONbits.ACKEN = value;
      break;
    case I2C_CHANNEL_4 :
      I2C4CONbits.ACKEN = value;
      break;
    case I2C_CHANNEL_5 :
      I2C5CONbits.ACKEN = value;
      break;
    }
  }

static void clear_ov(int channel)
  {
  switch(channel)
    {
    case I2C_CHANNEL_1 :
      I2C1STATbits.I2COV = 0;
      break;
#if !defined(__32MZ2048EFH064__)
    case I2C_CHANNEL_2 :
      I2C2STATbits.I2COV = 0;
      break;
#endif
    case I2C_CHANNEL_3 :
      I2C3STATbits.I2COV = 0;
      break;
    case I2C_CHANNEL_4 :
      I2C4STATbits.I2COV = 0;
      break;
    case I2C_CHANNEL_5 :
      I2C5STATbits.I2COV = 0;
      break;
    }
  }

static bool get_ackstat(int channel)
  {
  switch(channel)
    {
    case I2C_CHANNEL_1 :
      return I2C1STATbits.ACKSTAT == 1;
#if !defined(__32MZ2048EFH064__)
    case I2C_CHANNEL_2 :
      return I2C2STATbits.ACKSTAT == 1;
#endif
    case I2C_CHANNEL_3 :
      return I2C3STATbits.ACKSTAT == 1;
    case I2C_CHANNEL_4 :
      return I2C4STATbits.ACKSTAT == 1;
    case I2C_CHANNEL_5 :
      return I2C5STATbits.ACKSTAT == 1;
    }
  
  return false;
  }

static bool get_rbf(int channel)
  {
  switch(channel)
    {
    case I2C_CHANNEL_1 :
      return I2C1STATbits.RBF == 1;
#if !defined(__32MZ2048EFH064__)
    case I2C_CHANNEL_2 :
      return I2C2STATbits.RBF == 1;
#endif
    case I2C_CHANNEL_3 :
      return I2C3STATbits.RBF == 1;
    case I2C_CHANNEL_4 :
      return I2C4STATbits.RBF == 1;
    case I2C_CHANNEL_5 :
      return I2C5STATbits.RBF == 1;
    }
  
  return false;
  }

static void i2c_interrupt(int channel)
  {
  // operation completed so update state to next state
  i2c_state current_state = client_state[channel];
  i2c_state next_state = current_state;
  i2c_task_t *task = &current_i2c_task[channel];
  
  // ask the client what we are to do next.
  if(current_state != i2c_idle)
    next_state = (*task->get_state_handler)(task, current_state);
  
  switch(current_i2c_state[channel])
    {
    case st_i2c_idle :              // i2c is idle
      break;
    case st_i2c_start :           // SEN bit sent
      // get the address byte
      send_byte(channel, (*task->write_byte_handler)(task));
      change_state(channel, st_i2c_write_byte);
      break;
    case st_i2c_write_byte :
      if(get_ackstat(channel))
        {
        change_state(channel, st_i2c_nack);
        if((*task->error_handler)(task, current_state) == i2c_idle)
          change_state(channel, st_i2c_idle);
        else
          {
          //write_ptr--;
          // retry the whole command.
          set_sen(channel);
          client_state[channel] = i2c_write_command;     // in sen mode
          change_state(channel, st_i2c_start);
          }
        break;
        }
      // not an error so we decide what next
      switch(next_state)
        {
        case i2c_idle :
          set_pen(channel);   // and done.
          change_state(channel, st_i2c_stop);
          break;
        case i2c_write_command :
          set_rsen(channel);    // issue next command
          change_state(channel, st_i2c_repeat_start);
          break;
        case i2c_write_byte :
          send_byte(channel, (*task->write_byte_handler)(task));
          change_state(channel, st_i2c_write_byte);
          break;
        case i2c_read_byte :
          if(get_rbf(channel))
            {
            // empty the buffer.
            read_byte(channel);
            }
          set_rcen(channel);    // enable receive
          change_state(channel, st_i2c_read_byte);
          break;
        }
      break;
    case st_i2c_read_byte :
      if(!get_rbf(channel))
        {
        // error as we expect an rbf
        change_state(channel, st_i2c_nack);
        (*task->error_handler)(task, current_state);
        change_state(channel, st_i2c_idle);
        break;
        }
      (*task->read_byte_handler)(task, read_byte(channel));
      // update the next state.
      next_state = (*task->get_state_handler)(task, current_state);
      switch(next_state)
        {
        case i2c_idle :
          set_ackdt(channel, 1);
          set_acken(channel, 1);
          change_state(channel, st_i2c_nack);
          break;
        case i2c_read_byte :
          set_ackdt(channel, 0);
          set_acken(channel, 1);
          change_state(channel, st_i2c_ack);
          break;
        default:
          break;
        }
      break;
    case st_i2c_repeat_start :
      send_byte(channel, (*task->write_byte_handler)(task));
      change_state(channel, st_i2c_write_byte);
      break;
    case st_i2c_nack :
      set_pen(channel);       // and done.
      change_state(channel, st_i2c_stop);
      break;
    case st_i2c_ack :
      switch(next_state)
        {
        case i2c_idle :
          set_pen(channel);     // and done.
          change_state(channel, st_i2c_stop);
          break;
        case i2c_read_byte :
          set_rcen(channel); // enable receive
          change_state(channel, st_i2c_read_byte);
          break;
        default:
          break;
        }
      break;
    case st_i2c_stop :
      set_ackdt(channel, 0);
      change_state(channel, st_i2c_idle);
      break;
    case st_i2c_timeout :
      // the hardware has failed :-( so we have to restart the
      // current task
      task->bytes_xfered = 0;
      set_sen(channel);
      client_state[channel] = i2c_write_command;     // in sen mode
      change_state(channel, st_i2c_start);
      break;
    }
  
  client_state[channel] = next_state;
  
  clear_ov(channel);     // clear in case of overflow on
                              // last error
  }

void __attribute__( (interrupt(IPL0AUTO), vector(_I2C1_MASTER_VECTOR))) _mi2c1_interrupt( void );
void mi2c1_interrupt(void)
  {
  i2c_interrupt(I2C_CHANNEL_1);
  IFS3bits.I2C1MIF = 0;
  }

#ifndef __32MZ2048EFH064__
void __attribute__( (interrupt(IPL0AUTO), vector(_I2C2_MASTER_VECTOR))) _mi2c2_interrupt( void );
void mi2c2_interrupt(void)
  {
  i2c_interrupt(I2C_CHANNEL_2);
  IFS4bits.I2C2MIF = 0;
  }
#endif

void __attribute__( (interrupt(IPL0AUTO), vector(_I2C3_MASTER_VECTOR))) _mi2c3_interrupt( void );
void mi2c3_interrupt(void)
  {
  i2c_interrupt(I2C_CHANNEL_3);
  IFS5bits.I2C3MIF = 0;
  }

void __attribute__( (interrupt(IPL0AUTO), vector(_I2C4_MASTER_VECTOR))) _mi2c4_interrupt( void );
void mi2c4_interrupt(void)
  {
  i2c_interrupt(I2C_CHANNEL_4);
  IFS5bits.I2C4MIF= 0;
  }

void __attribute__( (interrupt(IPL0AUTO), vector(_I2C5_MASTER_VECTOR))) _mi2c5_interrupt( void );
void mi2c5_interrupt(void)
  {
  i2c_interrupt(I2C_CHANNEL_5);
  IFS5bits.I2C5MIF = 0;
  }

/**
 * Worker to send a new i2c command
 * @param parg not used
 */
static void i2c_worker(int channel)
  {
  while(true)
    {
    // wait for a msg
    pop_front(i2c_worker_queues[channel], &current_i2c_task[channel], INDEFINITE_WAIT);
#ifdef DEBUG_I2C
    state_ptr = 0;
#endif
    enter_critical();
    set_sen(channel);
    client_state[channel] = i2c_write_command;     // in sen mode
    change_state(channel, st_i2c_start);
    exit_critical();
    
    // we now wait for the interrupt that returns the controller
    // to idle
    semaphore_wait(i2c_ready_semp[channel], INDEFINITE_WAIT);
    }
  }

// this assumes PBCLK is 50Mhz (PBCLK2)
#define CALC_I2CBRG 0x037

static result_t init_channel1(uint16_t len, uint16_t i2c_worker_stack_length)
  {
  result_t result;
  
  if(failed(result = deque_create(sizeof(i2c_task_t), len, &i2c_worker_queues[I2C_CHANNEL_1])))
    return result;
  
  if(failed(result = semaphore_create(&i2c_ready_semp[I2C_CHANNEL_1])))
    return result;

  // set up the worker task
  if(failed(result = task_create("I2C1", 
                                 i2c_worker_stack_length,
                                 (task_callback) i2c_worker,
                                 (void *)I2C_CHANNEL_1,
                                 NORMAL_PRIORITY+1,
                                 0)))
    return result;
  
  // set mask to allow all data
  I2C1MSK = 0;
  I2C1BRG = CALC_I2CBRG;
  
  IFS3bits.I2C1MIF = 0;
  IPC29bits.I2C1MIP = 4;
  IPC29bits.I2C1MIS = 0;
  IEC3bits.I2C1MIE = 1;
  
  // enable I2C
  I2C1CONbits.ON = 1;
  
  return s_ok;
  }

#if !defined(__32MZ2048EFH064__)

static result_t init_channel2(uint16_t len, uint16_t i2c_worker_stack_length)
  {
  result_t result;
  
  if(failed(result = deque_create(sizeof(i2c_task_t), len, &i2c_worker_queues[I2C_CHANNEL_2])))
    return result;
  
  if(failed(result = semaphore_create(&i2c_ready_semp[I2C_CHANNEL_2])))
    return result;
  
  TRISAbits.TRISA2 = 1;
  TRISAbits.TRISA3 = 1;
  
  // disable the i2c controller
  I2C2CONbits.I2CEN = 0;
  
   // we set up the baud rate to 400khz
  I2C2BRG = CALC_I2CBRG;
  
  // set up the worker task
  if(failed(result = task_create("I2C2", 
                                 i2c_worker_stack_length,
                                 (task_callback) i2c_worker,
                                 (void *)I2C_CHANNEL_2,
                                 NORMAL_PRIORITY+1,
                                 0)))
    return result;
  
  // set mask to allow all data
  I2C2MSK = 0;
  
  IFS4bits.I2C2MIF = 0;
  IPC37bits.I2C2MIP = 4;
  IPC37bits.I2C2MIS = 0;
  IEC4bits.I2C2MIE = 1;
  
  // enable I2C
  I2C2CONbits.I2CEN = 1;
  return s_ok;
  }
#endif

static result_t init_channel3(uint16_t len, uint16_t i2c_worker_stack_length)
  {
  result_t result;
  
  if(failed(result = deque_create(sizeof(i2c_task_t), len, &i2c_worker_queues[I2C_CHANNEL_3])))
    return result;
  
  if(failed(result = semaphore_create(&i2c_ready_semp[I2C_CHANNEL_3])))
    return result;
  
  // set up the worker task
  if(failed(result = task_create("I2C3", 
                                 i2c_worker_stack_length,
                                 (task_callback) i2c_worker,
                                 (void *)I2C_CHANNEL_3,
                                 NORMAL_PRIORITY+1,
                                 0)))
    return result;
  
  I2C3BRG = CALC_I2CBRG;
  I2C3MSK = 0;
  
  IFS5bits.I2C5MIF = 0;
  IPC40bits.I2C3MIP = 4;
  IPC40bits.I2C3MIS = 0;
  IEC5bits.I2C3MIE = 1;
  
  // enable I2C
  I2C3CONbits.ON = 1;
  
  return s_ok;
  }

static result_t init_channel4(uint16_t len, uint16_t i2c_worker_stack_length)
  {
  result_t result;
  
  if(failed(result = deque_create(sizeof(i2c_task_t), len, &i2c_worker_queues[I2C_CHANNEL_4])))
    return result;
  
  if(failed(result = semaphore_create(&i2c_ready_semp[I2C_CHANNEL_4])))
    return result;
  
  // set up the worker task
  if(failed(result = task_create("I2C4", 
                                 i2c_worker_stack_length,
                                 (task_callback) i2c_worker,
                                 (void *)I2C_CHANNEL_4,
                                 NORMAL_PRIORITY+1,
                                 0)))
    return result;
  
  I2C4BRG = CALC_I2CBRG;
  I2C4MSK = 0x00000000;
  
  IFS5bits.I2C4MIF = 0;
  IPC43bits.I2C4MIP = 4;
  IPC46bits.I2C5MIS = 0;
  IEC5bits.I2C4MIE = 1;

  // enable I2C
  I2C4CONbits.ON = 1;
  
  return s_ok;
  }

static result_t init_channel5(uint16_t len, int16_t i2c_worker_stack_length)
  {
  result_t result;
  
  if(failed(result = deque_create(sizeof(i2c_task_t), len, &i2c_worker_queues[I2C_CHANNEL_5])))
    return result;
  
  if(failed(result = semaphore_create(&i2c_ready_semp[I2C_CHANNEL_5])))
    return result;

  // set up the worker task
  if(failed(result = task_create("I2C5", 
                                 i2c_worker_stack_length,
                                 (task_callback) i2c_worker,
                                 (void *)I2C_CHANNEL_5,
                                 NORMAL_PRIORITY+1,
                                 0)))
    return result;
  
  I2C5BRG = CALC_I2CBRG;
  I2C5MSK = 0;
  
  IFS5bits.I2C5MIF = 0;
  IPC46bits.I2C5MIP = 4;
  IPC46bits.I2C5MIS = 0;
  IEC5bits.I2C5MIE = 1;
  
  // enable I2C
  I2C5CONbits.ON = 1;
  
  return s_ok;
  }

static bool is_init = false;

static void setup_timer()
  {
  // check the I2c state change = 1.5 a transmit
  PR4 = 41;
  TMR4 = 0;
  T4CONbits.TCKPS = 0;                // clk is 1:1
  T4CONbits.TCS = 0;                  // select clock source to free-run
  T4CONbits.TGATE = 0;

  // set up timer 4
  IPC4bits.T4IP = 2;
  IFS0bits.T4IF = 0;
  IEC0bits.T4IE = 1;
  
  // start the i2c timeout counter running
  //T4CONbits.TON = 1;

  is_init = true;
  }

result_t i2cm_init(int channel, uint16_t len, uint16_t i2c_worker_stack_length)
  {
  result_t result = e_bad_parameter;
  
  if(!is_init)
    setup_timer();
  
  switch(channel)
    {
    case I2C_CHANNEL_1 :
      result = init_channel1(len, i2c_worker_stack_length);
      break;
#if !defined(__32MZ2048EFH064__)
    case I2C_CHANNEL_2 :
      result = init_channel2(len, i2c_worker_stack_length);
      break;
#endif
    case I2C_CHANNEL_3 :
      result = init_channel3(len, i2c_worker_stack_length);
      break;
    case I2C_CHANNEL_4 :
      result = init_channel4(len, i2c_worker_stack_length);
      break;
    case I2C_CHANNEL_5 :
      result = init_channel5(len, i2c_worker_stack_length);
      break;
    default :
      break;
    }
  
  return result;
  }
