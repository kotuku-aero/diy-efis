#include <math.h>
#include <string.h>
#include <stdio.h>

#include "../atom/analog_devices.h"

// called from the interrupt routine when the conversion is completed

#define SAMPLE_TICKS 1

#define scale_name "Scale"
#define offset_name "Offset"
#define publish_name "Publish Rate"
#define filter_name "Filter Rate"
#define id_name "Can ID"

static analog_channels_t *channels; // this holds all of the definitions
static int channel = 0;
static semaphore_p analog_conversion_complete;

#if defined(__dsPIC33EP512GP504__) | defined(__dsPIC33EP512GP506__)

extern void yield(void);

void __attribute__((interrupt, auto_psv)) _AD1Interrupt(void)
  {
  channels->channel_definition[channel].analog_value = ADC1BUF0;

  IFS0bits.AD1IF = 0; // clear IRQ
  signal_from_isr(analog_conversion_complete);
  
  yield();
  }

void __attribute__((interrupt, auto_psv)) _T3Interrupt(void)
  {
  T3CONbits.TON = 0; // turn off timer
  IFS0bits.T3IF = 0;
  
  yield();
  }

static void start_conversion(uint8_t next_channel)
  {
  analog_channel_definition_t *channel_definition = &(channels->channel_definition[next_channel]);

  // if we have a call-back to start the capture we call it now.
  // assign the channel number to the ADC
  if(channels->channel_select != 0)
    AD1CHS0bits.CH0SA = (*channels->channel_select)(channel_definition->channel);
  else
    AD1CHS0bits.CH0SA = channel_definition->channel;
  }

// this structure holds a tristate control so we can disable the tristates on
// anlog pins

typedef struct _tris_t
  {
  uint8_t channel;
  volatile unsigned int *port;
  uint16_t mask;
  volatile unsigned int *ansel;
  } tris_t;

// size is dependent on the device...
#if defined(__dsPIC33EP512GP504__)
#define NUMCHANNELS 9
#endif
#if defined(__dsPIC33EP512GP506__)
#define NUMCHANNELS 16
#endif
static const tris_t tristate[NUMCHANNELS] = {
  { 0,  &TRISA, 0x0001, &ANSELA },
  { 1,  &TRISA, 0x0002, &ANSELA },
  { 2,  &TRISB, 0x0001, &ANSELB },
  { 3,  &TRISB, 0x0002, &ANSELB },
  { 4,  &TRISB, 0x0004, &ANSELB },
  { 5,  &TRISB, 0x0008, &ANSELB },
  { 6,  &TRISC, 0x0001, &ANSELC },
  { 7,  &TRISC, 0x0002, &ANSELC },
  { 8,  &TRISC, 0x0004, &ANSELC },
#if defined(__dsPIC33EP512GP506__)
  { 9,  &TRISA, 0x0800, &ANSELA },
  { 10, &TRISA, 0x1000, &ANSELA  },
  { 11, &TRISC, 0x0800, &ANSELC },
  { 12, &TRISE, 0x1000, &ANSELE },
  { 13, &TRISE, 0x2000, &ANSELE },
  { 14, &TRISE, 0x4000, &ANSELE },
  { 15, &TRISE, 0x8000, &ANSELE },
#endif
  };

void setup_hardware(analog_channels_t *init_channels)
  {
  analog_channel_definition_t *channel_definition;
  channels = init_channels;
  
  ANSELA = 0;
  ANSELB = 0;
  ANSELC = 0;
#if defined(__dsPIC33EP512GP506__)
  ANSELE = 0;
#endif

  // publish all of the channels that we will process.
  for(channel = 0; channel < channels->num_channels; channel++)
    {
    uint8_t ch_num = 0;

    channel_definition = &(channels->channel_definition[channel]);

    ch_num = channel_definition->channel;

    if(tristate[ch_num].channel == ch_num)
      {
      *(tristate[ch_num].port) |= tristate[ch_num].mask;
      *(tristate[ch_num].ansel) |= tristate[ch_num].mask;
      }
    }

  // set interrupt priority = 5 for ADC
  IPC3bits.AD1IP = 4;
  IFS0bits.AD1IF = 0; // reset int
  IEC0bits.AD1IE = 1; // and enable the interrupt

  // TODO: check me
  PR5 = 273 / channels->num_channels;

  AD1CON1bits.SSRCG = 0;    // Not using PWM to start ADC
  AD1CON1bits.SSRC = 2;     // Timer3 is the start of conversion
  AD1CON1bits.FORM = 0;     // integer data
  AD1CON1bits.ASAM = 1;     // sampling when conversion complete
  AD1CON1bits.AD12B = 1;    // 12 bits
  AD1CON1bits.SIMSAM = 0;   // sample channels in sequence

  AD1CON2bits.SMPI = 0;     // 1 sample
  AD1CON2bits.CSCNA = 0;    // Do not scan inputs
  AD1CON2bits.VCFG = 0;     // use VSS
  AD1CON2bits.ALTS = 0;     // always use MUX A

  AD1CON3bits.SAMC = 31;    // auto sample time bits
  AD1CON3bits.ADRC = 0;     // system clock
  AD1CON3bits.ADCS = 7;     // Tad = 200ns @ 20Mhz
  
  AD1CON4bits.ADDMAEN = 0;  // No DMA

  AD1CHS0bits.CH0NA = 0;
  AD1CHS0bits.CH0NB = 0;

  TMR3 = 0;
  T3CONbits.TCKPS = 3;
  // load the comparator registers
  PR3 = 273 / channels->num_channels;

  IFS0bits.T3IF = 0;
  IEC0bits.T3IE = 0;

  T3CONbits.TON = 1; // start timer
  AD1CON1bits.ADON = 1;
  }
#endif

// called back from the microkernel when a conversion is complete
static canmsg_t msg;

static void conversion_complete_callback(void *parg)
  {
  while(true)
    {
    float value;

    semaphore_wait(analog_conversion_complete, INDEFINITE_WAIT);

    analog_channel_definition_t *channel_definition = &channels->channel_definition[channel];

    // and convert the next channel
    channel++;
    if(channel >= channels->num_channels)
      channel = 0;

    start_conversion(channel); // kick this off as soon as possible
    
    value = channel_definition->analog_value;

    value *= channels->analog_factor;

    // otherwise use the parameter driven conversions.
    if(channel_definition->scale != 1.0)
      value *= channel_definition->scale;

    if(channel_definition->offset != 0.0)
      value += channel_definition->offset;
    
    channel_definition->value += value;
    channel_definition->num_samples++;
    
    if(channel_definition->num_samples >= channel_definition->publish_rate)
      {
      value = channel_definition->value;
      value /= channel_definition->num_samples;

      can_send(create_can_msg_float(&msg, channel_definition->can_id, 0, value));
      
      if(channel_definition->loopback)
        publish_local(&msg);

      channel_definition->num_samples = 0;
      channel_definition->value = 0;
      }
    }
  }

static const char *analog_str = "analog";
static const char *channel_str = "channel";
static const char *scale_str = "scale";
static const char *offset_str = "offset";
static const char *can_id_str = "can-id";
static const char *publish_rate_str = "publish_rate";
static const char *loopback_str = "loopback";
static char tmp[32];

result_t analog_init(analog_channels_t *init_channels, uint16_t stack_length, bool factory_reset)
  {
  result_t result;
  uint16_t i;
  
  if(failed(result = semaphore_create(&analog_conversion_complete)))
    return result;
  
  // read the registry values.
  memid_t key;
  memid_t cfg_key;
  if(factory_reset || failed(reg_open_key(0, analog_str, &key)))
    {
    // must be initial phase.
    if(failed(result = reg_create_key(0, analog_str, &key)))
      return result;
    
    // create a basic channel
    for(i = 0; i < init_channels->num_channels; i++)
      {
      snprintf(tmp, 32, "%d", i);
      reg_create_key(key, tmp, &cfg_key);
      
      analog_channel_definition_t *channel = &init_channels->channel_definition[i];
      
      reg_set_uint16(cfg_key, channel_str, channel->channel);
      reg_set_float(cfg_key, scale_str, channel->scale);
      reg_set_float(cfg_key, offset_str, channel->offset);
      reg_set_uint16(cfg_key, can_id_str, channel->can_id);
      reg_set_uint16(cfg_key, publish_rate_str, channel->publish_rate);
      reg_set_bool(cfg_key, loopback_str, channel->loopback);
      }
    // initial registry is created.
    }
  
  // read the reg values
  for(i = 0; i < init_channels->num_channels; i++)
    {
    snprintf(tmp, 32, "%d", i);
    reg_open_key(key, tmp, &cfg_key);
      
      analog_channel_definition_t *channel = &init_channels->channel_definition[i];

    reg_get_uint16(cfg_key, channel_str, &channel->channel);
    reg_get_float(cfg_key, scale_str, &channel->scale);
    reg_get_float(cfg_key, offset_str, &channel->offset);
    reg_get_uint16(cfg_key, can_id_str, &channel->can_id);
    reg_get_uint16(cfg_key, publish_rate_str, &channel->publish_rate);
    reg_get_bool(cfg_key, loopback_str, &channel->loopback);
    }
  
  setup_hardware(init_channels);

  channel = 0;
  start_conversion(channel);

  // schedule 2 tasks.
  return task_create("ANALOG", stack_length, conversion_complete_callback, 0, NORMAL_PRIORITY, 0);
  }

result_t persist_analog_channel(memid_t parent,
                                   const char *name,
                                   analog_channel_definition_t *channel)
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


result_t load_analog_channel(memid_t parent,
                                     const char *name,
                                     analog_channel_definition_t *channel)
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