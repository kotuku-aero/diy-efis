#include "../atom/microkernel.h"
#include "../atom/input_capture_devices.h"

#define NUM_CAPTURE_CHANNELS 4

static capture_channels_t *channels; // this holds all of the definitions
static semaphore_p capture_complete;

static capture_channel_definition_t *channel_defn[NUM_CAPTURE_CHANNELS];
static void init_channel(capture_channel_definition_t *channel_definition);
static void init_hardware();
static void update_channel(capture_channel_definition_t *channel, int value);

extern void yield(void);

void __attribute__((interrupt, auto_psv)) _IC1Interrupt(void)
  {
  IFS0bits.IC1IF = 0;
  while (IC1CON1bits.ICBNE)
    update_channel(channel_defn[0], IC1BUF);
  
  yield();
  }

void __attribute__((interrupt, auto_psv)) _IC2Interrupt(void)
  {
  IFS0bits.IC2IF = 0;
  while (IC2CON1bits.ICBNE)
    update_channel(channel_defn[1], IC2BUF);
  
  yield();
  }

void __attribute__((interrupt, auto_psv)) _IC3Interrupt(void)
  {
  IFS2bits.IC3IF = 0;
  while (IC3CON1bits.ICBNE)
    update_channel(channel_defn[2], IC3BUF);
  
  yield();
  }

void __attribute__((interrupt, auto_psv)) _IC4Interrupt(void)
  {
  IFS2bits.IC4IF = 0;
  while (IC4CON1bits.ICBNE)
    update_channel(channel_defn[3], IC4BUF);
  
  yield();
  }

// called from the interrupt routine when the publish tick is made
void __attribute__((interrupt, auto_psv)) _T2Interrupt(void)
  {
  int i;
  IFS0bits.T2IF = 0;

  for(i = 0; i < NUM_CAPTURE_CHANNELS; i++)
    if(channel_defn[i] != 0)
      channel_defn[i]->timer_value = 0;

  // set the flag
  signal_from_isr(capture_complete);
  
  yield();
  }

void init_channel(capture_channel_definition_t *channel_definition)
  {

    // decide from the channel number what register we need to set up
    // int priority = 4
    switch (channel_definition->channel)
      {
      case 1:
        channel_defn[0] = channel_definition;
        IC1CON1bits.ICTSEL = 1; // TMR2
        IC1CON1bits.ICM = 0;
        IFS0bits.IC1IF = 0;
        IEC0bits.IC1IE = 1;
        IPC0bits.IC1IP = 4;
        //TRISDbits.TRISD0 = 1;
        break;
      case 2:
        channel_defn[1] = channel_definition;
        IC2CON1bits.ICTSEL = 1; // TMR2
        IC2CON1bits.ICM = 0;
        IFS0bits.IC2IF = 0;
        IEC0bits.IC2IE = 1;
        IPC1bits.IC2IP = 4;
        //TRISDbits.TRISD1 = 1;
        break;
      case 3:
        channel_defn[2] = channel_definition;
        IC3CON1bits.ICTSEL = 1; // TMR2
        IC3CON1bits.ICM = 0;
        IFS2bits.IC3IF = 0;
        IEC2bits.IC3IE = 1;
        IPC9bits.IC3IP = 4;
        TRISBbits.TRISB4 = 1;
        break;
      case 4:
        channel_defn[3] = channel_definition;
        IC4CON1bits.ICTSEL = 1; // TMR2
        IC4CON1bits.ICM = 0;
        IFS2bits.IC4IF = 0;
        IEC2bits.IC4IE = 1;
        IPC9bits.IC3IP = 4;
        //TRISBbits.TRISB5 = 1;
        break;
      }
  }

static void init_hardware()
  {
  TMR2 = 0;
  T2CONbits.TCKPS = channels->prescaler;
  // load the comparator registers
  PR2 = channels->divisor;

  IFS0bits.T2IF = 0;
  IEC0bits.T2IE = 1;


  T2CONbits.TON = 1; // start timer
  }


static void update_channel(capture_channel_definition_t *channel, int value)
  {
  if (channel == 0)
    return;

  // if the timer_value == 0 then the timer has overflowed so we can't
  // use the value.  We need to resync the input capture
  if (channel->timer_value > 0)
    {
    int period = value - channel->timer_value;
    channel->result += period;
    channel->capture_count++;
    }

  channel->timer_value = value;
  }

static void calculate_and_publish(capture_channel_definition_t *channel_definition)
  {
  // signal one more clock timeout
  canmsg_t msg;

  // calculate the result first
  if (channel_definition->capture_count == 0)
    channel_definition->result = 0;
  else if(channel_definition->capture_count >= channel_definition->publish_rate)
    {
    // take the average value
    channel_definition->result /= channel_definition->capture_count;

    // the result is a count of 20mhz / 256 intervals
    channel_definition->result *= 0.0000128;

    // convert to a frequency
    channel_definition->result = 1 / channel_definition->result;

    // if we have a result processor then we just pass the filtered value to it to be processed.
    // publish the result to the publisher.
    can_send(create_can_msg_float(&msg, channel_definition->can_id, 0, channel_definition->result));
    
    if(channel_definition->loopback)
      publish_local(&msg);

    // reset counter so that we calculate the next period
    channel_definition->result = -1;
    channel_definition->capture_count = 0;
    }
  else
    channel_definition->capture_count++;
  }

// called back from the microkernel when a conversion is complete

static void capture_callback_proc(void *parg)
  {
  int i;
  while (true)
    {
    semaphore_wait(capture_complete, INDEFINITE_WAIT);

    for(i = 0; i < NUM_CAPTURE_CHANNELS; i++)
      if(channel_defn[i] != 0)
        calculate_and_publish(channel_defn[i]);
    }
  }

result_t capture_init(capture_channels_t *init_channels, uint16_t stack_length)
  {
  result_t result;
  task_p task_id;
  capture_channel_definition_t *channel_definition;
  int channel;

  channels = init_channels;
  
  if(failed(result = semaphore_create(&capture_complete)))
    return result;

  // publish all of the channels that we will process.
  for (channel = 0; channel < channels->num_channels; channel++)
    {
    channel_definition = &(channels->channel_definition[channel]);
    init_channel(channel_definition);
    }

  init_hardware();

  // schedule 2 tasks.
  return task_create("CAPTURE", stack_length, capture_callback_proc, 0, NORMAL_PRIORITY, &task_id);
  }

static const char *scale_name = "scale";
static const char *offset_name = "offset";
static const char *publish_rate_str = "publish-rate";
static const char *id_name = "can-id";
static const char *loopback_str = "loopback";


result_t persist_capture_channel(memid_t parent,
                                   const char *name,
                                   capture_channel_definition_t *channel)
  {
  result_t result;
  memid_t memid;
  
  // create the parent key, or open it
  if(failed(result = reg_create_key(parent, name, &memid)))
    return result;
  
  if(failed(result = reg_set_uint16(memid, id_name, channel->can_id)))
    return result;
  
  if(failed(result = reg_set_float(memid, scale_name, channel->scale)))
    return result;
  
  if(failed(result = reg_set_float(memid, offset_name, channel->offset)))
    return result;
  
  if(failed(result = reg_set_uint16(memid, publish_rate_str, channel->publish_rate)))
    return result;
  
  if(failed(result = reg_set_bool(memid, loopback_str, channel->loopback)))
    return result;
  
  return s_ok;
  }


result_t load_capture_channel(memid_t parent,
                                     const char *name,
                                     capture_channel_definition_t *channel)
  {
  result_t result;
  memid_t memid;
  
  if(failed(result = reg_open_key(parent, name, &memid)))
    return result;
  
  reg_get_uint16(memid, id_name, &channel->can_id);
  reg_get_float(memid, scale_name, &channel->scale);
  reg_get_float(memid, offset_name, &channel->offset);
  reg_get_uint16(memid, publish_rate_str, &channel->publish_rate);
  reg_get_bool(memid, loopback_str, &channel->loopback);
  
  return s_ok;
  }
