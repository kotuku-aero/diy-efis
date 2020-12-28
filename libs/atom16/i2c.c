#include <xc.h>
#include <stdbool.h>
#include <string.h>
#include "../atom/i2c.h"
#include "../atom/microkernel.h"
#include "pps_maps.h"

//#define DEBUG_I2C

#ifndef NUM_I2C_CHANNELS
#define NUM_I2C_CHANNELS 2
#endif

#ifndef I2C_CHANNEL_1
#define I2C_CHANNEL_1 0
#endif

#ifndef I2C_CHANNEL_2
#define I2C_CHANNEL_2 1
#endif


static deque_p deque[NUM_I2C_CHANNELS];
static semaphore_p i2c_ready_semp[NUM_I2C_CHANNELS];
i2c_task_t current_i2c_task[NUM_I2C_CHANNELS];

bool enqueue_i2c_task(int channel, const i2c_task_t *task)
  {
  push_back(deque[channel], task, INDEFINITE_WAIT);
  
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
  st_i2c_stop
  } i2c_msg_state;
  
i2c_msg_state current_i2c_state[NUM_I2C_CHANNELS];

#ifdef DEBUG_I2C
i2c_msg_state state_buffer[NUM_I2C_CHANNELS][32];
uint16_t state_ptr[NUM_I2C_CHANNELS]; 
uint8_t write_log[NUM_I2C_CHANNELS][1024];
uint16_t write_ptr[NUM_I2C_CHANNELS];
uint8_t read_log[NUM_I2C_CHANNELS][1024];
uint16_t read_ptr[NUM_I2C_CHANNELS];
#endif

static void change_state(int channel, i2c_msg_state _state)
  {
  current_i2c_state[channel] = _state;
  
  if(_state == st_i2c_idle)
    signal_from_isr(i2c_ready_semp[channel]);
    
#ifdef DEBUG_I2C
  if(state_ptr < 32)
    state_buffer[state_ptr++] = current_i2c_state;
#endif
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
    case I2C_CHANNEL_2 :
      I2C2TRN = byte_to_write;
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
    case I2C_CHANNEL_2 :
      result = I2C2RCV;
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
    case I2C_CHANNEL_2 :
      I2C2CONbits.SEN = 1;
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
    case I2C_CHANNEL_2 :
      I2C2CONbits.RCEN = 1;
      break;
    }
  }

static void set_pen(int channel)
  {
  switch(channel)
    {
    case I2C_CHANNEL_1 :
      I2C1CONbits.PEN = 1;
      break;
    case I2C_CHANNEL_2 :
      I2C2CONbits.PEN = 1;
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
    case I2C_CHANNEL_2 :
      I2C2CONbits.RSEN = 1;
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
    case I2C_CHANNEL_2 :
      I2C2CONbits.ACKDT = value;
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
    case I2C_CHANNEL_2 :
      I2C2CONbits.ACKEN = value;
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
    case I2C_CHANNEL_2 :
      I2C2STATbits.I2COV = 0;
      break;
    }
  }

static bool get_ackstat(int channel)
  {
  switch(channel)
    {
    case I2C_CHANNEL_1 :
      return I2C1STATbits.ACKSTAT == 1;
    case I2C_CHANNEL_2 :
      return I2C2STATbits.ACKSTAT == 1;
    }
  
  return false;
  }

static bool get_rbf(int channel)
  {
  switch(channel)
    {
    case I2C_CHANNEL_1 :
      return I2C1STATbits.RBF == 1;
    case I2C_CHANNEL_2 :
      return I2C2STATbits.RBF == 1;
    }
  
  return false;
  }

static void I2C_master_int(int channel)
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
    }
  
  client_state[channel] = next_state;
  
  clear_ov(channel);     // clear in case of overflow on
                              // last error
  }

/**
 * Worker to send a new i2c command
 * @param parg not used
 */
static void master_worker(void *parg)
  {
  int channel = (int)parg;
  
  while(true)
    {
    // wait for a msg
    pop_front(deque[channel], &current_i2c_task[channel], INDEFINITE_WAIT);
#ifdef DEBUG_I2C
    state_ptr = 0;
#endif
    enter_critical();
    set_sen(channel);
    client_state[channel] = i2c_write_command;     // in sen mode
    change_state(channel, st_i2c_start);
    exit_critical();
    
    do
      {
      // we now wait for the interrupt that returns the controller
      // to idle
      semaphore_wait(i2c_ready_semp[channel], INDEFINITE_WAIT);
      } while(current_i2c_state[channel] != st_i2c_idle);
    }
  }

#define FSCL  400000

extern uint32_t fcy;

static inline uint8_t calc_i2cbrg()
  {
  uint32_t value = 237;
  value *= fcy;
  value /= 100000;
  
  return (uint8_t) value;
  }

static result_t init_master1(uint16_t len, uint16_t i2c_worker_stack_length)
  {
  result_t result;
  if(failed(result = deque_create(sizeof(i2c_task_t), len, &deque[I2C_CHANNEL_1])))
    return result;
  
  if(failed(result = semaphore_create(&i2c_ready_semp[I2C_CHANNEL_1])))
    return result;

  // disable the i2c controller
  I2C1CONbits.I2CEN = 0;
  //I2C2CONHbits.SBCDE = 0;
  
  // we set up the baud rate to 400khz
  I2C1BRG = calc_i2cbrg();
  
  // set up the worker task
  if(failed(result = task_create("I2C1",
                                 i2c_worker_stack_length,
                                 master_worker,
                                 I2C_CHANNEL_1,
                                 NORMAL_PRIORITY,
                                 0)))
    return result;
  
  // set mask to allow all data
  I2C1MSK = 0;
   // enable interrupts
  IFS1bits.MI2C1IF = 0;
  //IFS10bits.I2C2BCIF = 0;
  IEC1bits.MI2C1IE = 1;
  //IEC10bits.I2C2BCIE = 1;
  IPC4bits.MI2C1IP = 4;
  //IPC43bits.I2C2BCIP = 6;
  // enable I2C
  I2C1CONbits.I2CEN = 1;
  
  return s_ok;
  }

static result_t init_master2(uint16_t len, uint16_t i2c_worker_stack_length)
  {
  result_t result;
  if(failed(result = deque_create(sizeof(i2c_task_t), len, &deque[I2C_CHANNEL_2])))
    return result;
  
  if(failed(result = semaphore_create(&i2c_ready_semp[I2C_CHANNEL_2])))
    return result;
  
  // disable the i2c controller
  I2C2CONbits.I2CEN = 0;
  //I2C2CONHbits.SBCDE = 0;
  
   // we set up the baud rate to 400khz
  I2C2BRG = calc_i2cbrg();
  
  // set up the worker task
  if(failed(result = task_create("I2C2", 
                                 i2c_worker_stack_length,
                                 master_worker,
                                 (void *)I2C_CHANNEL_2,
                                 NORMAL_PRIORITY,
                                 0)))
    return result;
  
  // set mask to allow all data
  I2C2MSK = 0;
   // enable interrupts
  IFS3bits.MI2C2IF = 0;
  //IFS10bits.I2C2BCIF = 0;
  IEC3bits.MI2C2IE = 1;
  //IEC10bits.I2C2BCIE = 1;
  IPC12bits.MI2C2IP = 4;
  //IPC43bits.I2C2BCIP = 6;
  // enable I2C
  I2C2CONbits.I2CEN = 1;
  
  return s_ok;
  }

result_t i2c_channel_init(int channel, uint16_t len, uint16_t i2c_worker_stack_length)
  {
  result_t result = e_bad_parameter;
  
  switch(channel)
    {
    case I2C_CHANNEL_1 :
      result = init_master1(len, i2c_worker_stack_length);
      break;
    case I2C_CHANNEL_2 :
      result = init_master2(len, i2c_worker_stack_length);
      break;
    default :
      break;
    }
  
  return result;
  }

////////////////////////////////////////////////////////////////////////////////
//
//  Slave channels

typedef enum {
  sst_idle,             // waiting for a command
  sst_get_address,          // start of msg, address has been set, read it
  sst_read_reg,         // address received, reading multiple, wait ack
  sst_write_reg,        // waiting for next register to write
  } i2c_slave_state;
 
typedef struct _i2c_slave_t {
  i2c_read_reg read_reg;
  i2c_write_reg write_reg;
  i2c_valid_reg valid_reg;
  i2c_slave_state state;
  uint8_t reg_address;  // if high bit set then multiple
  uint16_t *stat_reg;
  uint16_t *rcv_reg;
  uint16_t *trn_reg;
  uint16_t *ctl_reg;
  } i2c_slave_t;
  
static i2c_slave_t slave_channels[2];

// these channels run at a special IRQ level above all others
// this is the stack.

void I2C_slave_int(int chnum)
  {
  switch(chnum)
    {
    case 0 :
      if(IFS1bits.SI2C1IF != 0)
        IFS1bits.SI2C1IF = 0;
      else
        return;
      break;
    case 1 :
      if(IFS3bits.SI2C2IF != 0)
        IFS3bits.SI2C2IF = 0;
      else
        return;
      break;
    default:
      return;
    }

  i2c_slave_t *channel = &slave_channels[chnum];
  
  if(((*channel->stat_reg) & 0x0020)== 0)
    {
    channel->state = sst_idle;      // flag start of next transaction
    // read the address from the channel
    uint8_t dummy = *channel->rcv_reg;
    }
 
  switch(channel->state)
    {
    case sst_idle :
      // read the address to clear rbf
      if(((*channel->stat_reg)& 0x0004)== 0)  // if this is a write get the reg
        {
        channel->state = sst_get_address;     // address matched, wait for reg
        break;
        }
      
      // fall through and load first data
      channel->state = sst_read_reg;
    case sst_read_reg :
      {
      // todo: handle errors
      // send the next register, only if a valid address
      uint8_t value;
      if(succeeded((*channel->valid_reg)(chnum, channel->reg_address)) &&
        succeeded((*channel->read_reg)(chnum, channel->reg_address, &value)))
        {
        *channel->trn_reg = value;
        channel->reg_address++;
        }
      }
      break;
    case sst_get_address :
      channel->reg_address = *channel->rcv_reg;
      channel->state = sst_write_reg;
      break;
    case sst_write_reg :
      // send the next register, only if valid
      if(succeeded((*channel->valid_reg)(chnum, channel->reg_address)) &&
        succeeded((*channel->write_reg)(chnum, channel->reg_address, *channel->rcv_reg)))
        channel->reg_address++;
      break;
    }
  
  *channel->ctl_reg |= 0x1000;    // release the clock.
  }

extern void SI2C1Setup();

static result_t init_slave1(uint16_t slave_address,
                          i2c_read_reg read_register, 
                          i2c_write_reg write_register,
                          i2c_valid_reg valid_register)
  {
  slave_channels[0].read_reg = read_register;
  slave_channels[0].write_reg = write_register;
  slave_channels[0].valid_reg = valid_register;
  slave_channels[0].ctl_reg = (uint16_t *)&I2C1CON;
  slave_channels[0].stat_reg = (uint16_t *)&I2C1STAT;
  slave_channels[0].rcv_reg = (uint16_t *)&I2C1RCV;
  slave_channels[0].trn_reg = (uint16_t *)&I2C1TRN;
  
  I2C1ADD = slave_address;
  I2C1MSK = 0x7F;           // mask 7 bits
  
  I2C1CONbits.GCEN = 0;     // no general call (yet)
  I2C1CONbits.SCLREL = 1;   // release clock
  //I2C1CONbits.STREN = 1;    // enable clock stretch
  I2C1CONbits.STREN = 0;    // disable clock stretch
  I2C1CONbits.IPMIEN = 0;   // enable address matching
  I2C1CONbits.I2CEN = 1;
  
  // set mask to allow all data
  I2C1MSK = 0;
   // enable interrupts
  IFS1bits.SI2C1IF = 0;
  //IFS10bits.I2C2BCIF = 0;
  IEC1bits.SI2C1IE = 1;
  //IEC10bits.I2C2BCIE = 1;
  //IPC4bits.SI2C1IP = 4;
  IPC4bits.SI2C1IP = 6;     // run as IRQ6
  // enable I2C
  I2C1CONbits.I2CEN = 1;
  
  SI2C1Setup();
  
  return s_ok;
  }

extern void SI2C2Setup();

static result_t init_slave2(uint16_t slave_address,
                          i2c_read_reg read_register, 
                          i2c_write_reg write_register,
                          i2c_valid_reg valid_register)
  {
  slave_channels[1].read_reg = read_register;
  slave_channels[1].write_reg = write_register;
  slave_channels[1].valid_reg = valid_register;
  slave_channels[1].ctl_reg = (uint16_t *)&I2C2CON;
  slave_channels[1].stat_reg = (uint16_t *)&I2C2STAT;
  slave_channels[1].rcv_reg = (uint16_t *)&I2C2RCV;
  slave_channels[1].trn_reg = (uint16_t *)&I2C2TRN;
  
  I2C2ADD = slave_address;
  I2C2MSK = 0x7F;           // mask 8 bits
  
  I2C2CONbits.GCEN = 0;     // no general call (yet)
  I2C2CONbits.SCLREL = 1;   // release clock
  I2C2CONbits.STREN = 1;    // enable clock stretch
  I2C2CONbits.IPMIEN = 0;   // enable address matching
  I2C2CONbits.I2CEN = 1;
  
  
  // set mask to allow all data
  I2C2MSK = 0;
   // enable interrupts
  IFS3bits.SI2C2IF = 0;
  //IFS10bits.I2C2BCIF = 0;
  IEC3bits.SI2C2IE = 1;
  //IEC10bits.I2C2BCIE = 1;
  //IPC12bits.SI2C2IP = 4;
  IPC12bits.SI2C2IP = 6;
  // enable I2C
  I2C2CONbits.I2CEN = 1;
  
  SI2C1Setup();
  
  return s_ok;
  }

result_t i2cs_init(int channel, 
                          uint16_t slave_address,
                          i2c_read_reg read_register, 
                          i2c_write_reg write_register,
                          i2c_valid_reg valid_register)
  {
  result_t result = e_bad_parameter;
  
  switch(channel)
    {
    case I2C_CHANNEL_1 :
      result = init_slave1(slave_address, 
                           read_register, 
                           write_register, 
                           valid_register);
      break;
    case I2C_CHANNEL_2 :
      result = init_slave2(slave_address, 
                           read_register, 
                           write_register,
                           valid_register);
      break;
    default :
      break;
    }
  
  return result;
  }

////////////////////////////////////////////////////////////////////////////////
//
// Interrupts

extern void yield(void);


void __attribute__((interrupt, auto_psv)) _MI2C1Interrupt(void)
  {
  if(IFS1bits.MI2C1IF != 0)
    {
    IFS1bits.MI2C1IF = 0;
    I2C_master_int(I2C_CHANNEL_1);
    }
  
  yield();
  }

void __attribute__((interrupt, auto_psv)) _MI2C2Interrupt(void)
  {
  if(IFS3bits.MI2C2IF != 0)
    {
    IFS3bits.MI2C2IF = 0;
    I2C_master_int(I2C_CHANNEL_2);
    }
  
  yield();
  }
