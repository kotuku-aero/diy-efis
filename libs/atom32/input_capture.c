#include "../atom/microkernel.h"
#include "../atom/input_capture_devices.h"
#include <sys/attribs.h>

#define NUM_CAPTURE_CHANNELS 9

static capture_channels_t *channels; // this holds all of the definitions
static semaphore_p capture_complete;

static capture_channel_definition_t *channel_defn[NUM_CAPTURE_CHANNELS];
static void init_channel(capture_channel_definition_t *channel_definition);
static void init_hardware();
static void update_channel(capture_channel_definition_t *channel, uint32_t value);

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

static void init_channel(capture_channel_definition_t *channel_definition)
  {
  channel_defn[channel_definition->channel-1] = channel_definition;
  
  switch(channel_definition->channel & CHANNEL_MASK)
    {
    case 1 :
      IC1CONbits.C32 = 1;
      IC1CONbits.ICM = 3;
      IC1CONbits.ON = 1;
      IFS0bits.IC1IF = 0;
      IEC0bits.IC1IE = 1;
      IPC1bits.IC1IP = 4;
      break;
    case 2 :
      IC2CONbits.C32 = 1;
      IC2CONbits.ICM = 3;
      IC2CONbits.ON = 1;
      IFS0bits.IC2IF = 0;
      IEC0bits.IC2IE = 1;
      IPC2bits.IC2IP = 4;
      break;
    case 3 :
      IC3CONbits.C32 = 1;
      IC3CONbits.ICM = 3;
      IC3CONbits.ON = 1;
      IFS0bits.IC3IF = 0;
      IEC0bits.IC3IE = 1;
      IPC4bits.IC3IP = 4;
      break;
    case 4 :
      IC4CONbits.C32 = 1;
      IC4CONbits.ICM = 3;
      IC4CONbits.ON = 1;
      IFS0bits.IC4IF = 0;
      IEC0bits.IC4IE = 1;
      IPC5bits.IC4IP = 4;
      break;
    case 5 :
      IC5CONbits.C32 = 1;
      IC5CONbits.ICM = 3;
      IC5CONbits.ON = 1;
      IFS0bits.IC5IF = 0;
      IEC0bits.IC5IE = 1;
      IPC6bits.IC5IP = 4;
      break;
    case 6 :
      IC6CONbits.C32 = 1;
      IC6CONbits.ICM = 3;
      IC6CONbits.ON = 1;
      IFS0bits.IC6IF = 0;
      IEC0bits.IC6IE = 1;
      IPC7bits.IC6IP = 4;
      break;
    case 7 :
      IC7CONbits.C32 = 1;
      IC7CONbits.ICM = 3;
      IC7CONbits.ON = 1;
      IFS1bits.IC7IF = 0;
      IEC1bits.IC7IE = 1;
      IPC8bits.IC7IP = 4;
      break;
    case 8 :
      IC8CONbits.C32 = 1;
      IC8CONbits.ICM = 3;
      IC8CONbits.ON = 1;
      IFS1bits.IC8IF = 0;
      IEC1bits.IC8IE = 1;
      IPC9bits.IC8IP = 4;
      break;
    case 9 :
      IC9CONbits.C32 = 1;
      IC9CONbits.ICM = 3;
      IC9CONbits.ON = 1;
      IFS1bits.IC9IF = 0;
      IEC1bits.IC9IE = 1;
      IPC10bits.IC9IP = 4;
      break;
    }
  }

static void update_channel(capture_channel_definition_t *channel, uint32_t value)
  {
  if (channel == 0)
    return;

  if(channel->timer_value > value)
  {
    // wrapped around, ignore this sample
    channel->timer_value = value;
    return;
    }
  
  uint32_t period = value - channel->timer_value;
  channel->timer_value = value;

  // take the average period
    channel->result += period;
  channel->result /= 2;
    channel->capture_count++;


  // TODO: handle accumulator
  // calculate the result first
  if(channel->capture_count >= channel->publish_rate)
    {
    float result = channel->result;
    result *= channel->scale;
    result += channel->offset;
    
    if(result != 0)
      // convert to frequency
      result = 1 / result;

    // if we have a result processor then we just pass the filtered value to it to be processed.
    // publish the result to the publisher.
    publish_float(channel->can_id, result);

    // reset counter so that we calculate the next period
    channel->capture_count = 0;
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

  // the timer 2/3 is used for the input capture.  It runs on PBCLK3 which is
  // a 10mhz clock.
  // the 10mhz clock is / 16 so the clock is 625khz or 1.6ns/count.
  //
  // this yields counts of up to 6871 sec/interval
  T2CONbits.T32 = 1;    // 32 bit timer.
  T2CONbits.TCKPS = 4;  // prescaller of 16
  T2CONbits.TON = 1;    // start the timer.
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
