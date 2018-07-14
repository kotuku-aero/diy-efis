#include "../atom/microkernel.h"
#include "../atom/input_capture_devices.h"
#include <sys/attribs.h>

#define NUM_CAPTURE_CHANNELS 9

static capture_channels_t *channels; // this holds all of the definitions
static semaphore_p capture_complete;

static capture_channel_definition_t *channel_defn[NUM_CAPTURE_CHANNELS];
static void init_channel(capture_channel_definition_t *channel_definition);
static void init_hardware();
static void update_channel(capture_channel_definition_t *channel, int value);

void __attribute__( (interrupt(IPL0AUTO), vector(_INPUT_CAPTURE_1_VECTOR))) _ic1_interrupt( void );
void ic1_interrupt(void)
  {
  IFS0bits.IC1IF = 0;
  while (IC1CONbits.ICBNE)
    update_channel(channel_defn[0], IC1BUF);
  }

void __attribute__( (interrupt(IPL0AUTO), vector(_INPUT_CAPTURE_2_VECTOR))) _ic2_interrupt( void );
void ic2_interrupt(void)
  {
  IFS0bits.IC2IF = 0;
  while (IC2CONbits.ICBNE)
    update_channel(channel_defn[1], IC2BUF);
  }

void __attribute__( (interrupt(IPL0AUTO), vector(_INPUT_CAPTURE_3_VECTOR))) _ic3_interrupt( void );
void ic3_interrupt(void)
  {
  IFS0bits.IC3IF = 0;
  while (IC3CONbits.ICBNE)
    update_channel(channel_defn[2], IC3BUF);
  }

void __attribute__( (interrupt(IPL0AUTO), vector(_INPUT_CAPTURE_4_VECTOR))) _ic4_interrupt( void );
void ic4_interrupt(void)
  {
  IFS0bits.IC4IF = 0;
  while (IC4CONbits.ICBNE)
    update_channel(channel_defn[3], IC4BUF);
  }

void __attribute__( (interrupt(IPL0AUTO), vector(_INPUT_CAPTURE_5_VECTOR))) _ic5_interrupt( void );
void ic5_interrupt(void)
  {
  IFS0bits.IC5IF = 0;
  while (IC5CONbits.ICBNE)
    update_channel(channel_defn[4], IC5BUF);
  }

void __attribute__( (interrupt(IPL0AUTO), vector(_INPUT_CAPTURE_6_VECTOR))) _ic6_interrupt( void );
void ic6_interrupt(void)
  {
  IFS0bits.IC6IF = 0;
  while (IC6CONbits.ICBNE)
    update_channel(channel_defn[5], IC6BUF);
  }

void __attribute__( (interrupt(IPL0AUTO), vector(_INPUT_CAPTURE_7_VECTOR))) _ic7_interrupt( void );
void ic7_interrupt(void)
  {
  IFS1bits.IC7IF = 0;
  while (IC7CONbits.ICBNE)
    update_channel(channel_defn[6], IC7BUF);
  }

void __attribute__( (interrupt(IPL0AUTO), vector(_INPUT_CAPTURE_8_VECTOR))) _ic8_interrupt( void );
void ic8_interrupt(void)
  {
  IFS1bits.IC8IF = 0;
  while (IC8CONbits.ICBNE)
    update_channel(channel_defn[7], IC8BUF);
  }

void __attribute__( (interrupt(IPL0AUTO), vector(_INPUT_CAPTURE_9_VECTOR))) _ic9_interrupt( void );
void ic9_interrupt(void)
  {
  IFS1bits.IC9IF = 0;
  while (IC9CONbits.ICBNE)
    update_channel(channel_defn[8], IC9BUF);
  }

// called from the interrupt routine when the publish tick is made
void __attribute__( (interrupt(IPL0AUTO), vector(_TIMER_2_VECTOR))) _t2_interrupt( void );
void t2_interrupt(void)
  {
  int i;
  IFS0bits.T2IF = 0;

  for(i = 0; i < NUM_CAPTURE_CHANNELS; i++)
    if(channel_defn[i] != 0)
      channel_defn[i]->timer_value = 0;

  // set the flag
  signal_from_isr(capture_complete);
  }

static void init_channel(capture_channel_definition_t *channel_definition)
  {
  channel_defn[channel_definition->channel-1] = channel_definition;
  
  switch(channel_definition->channel)
    {
    case 1 :
      IFS0bits.IC1IF = 0;
      IEC0bits.IC1IE = 1;
      IPC1bits.IC1IP = 4;
      break;
    case 2 :
      IFS0bits.IC2IF = 0;
      IEC0bits.IC2IE = 1;
      IPC2bits.IC2IP = 4;
      break;
    case 3 :
      IFS0bits.IC3IF = 0;
      IEC0bits.IC3IE = 1;
      IPC4bits.IC3IP = 4;
      break;
    case 4 :
      IFS0bits.IC4IF = 0;
      IEC0bits.IC4IE = 1;
      IPC5bits.IC4IP = 4;
      break;
    case 5 :
      IFS0bits.IC5IF = 0;
      IEC0bits.IC5IE = 1;
      IPC6bits.IC5IP = 4;
      break;
    case 6 :
      IFS0bits.IC6IF = 0;
      IEC0bits.IC6IE = 1;
      IPC7bits.IC6IP = 4;
      break;
    case 7 :
      IFS1bits.IC7IF = 0;
      IEC1bits.IC7IE = 1;
      IPC8bits.IC7IP = 4;
      break;
    case 8 :
      IFS1bits.IC8IF = 0;
      IEC1bits.IC8IE = 1;
      IPC9bits.IC8IP = 4;
      break;
    case 9 :
      IFS1bits.IC9IF = 0;
      IEC1bits.IC9IE = 1;
      IPC10bits.IC9IP = 4;
      break;
    }
  }

static void init_hardware()
  {
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
  float result;
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
    publish_float(channel_definition->can_id, channel_definition->result);

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
  return task_create("CAPTURE", stack_length, capture_callback_proc, 0, NORMAL_PRIORITY, 0);
  }

static const char *scale_name = "scale";
static const char *offset_name = "offset";
static const char *publish_name = "publish-rate";
static const char *filter_name = "filter-rate";
static const char *id_name = "can-id";


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
  
  return s_ok;
  }
