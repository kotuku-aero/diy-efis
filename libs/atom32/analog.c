#include <string.h>
#include <stdio.h>
#include <math.h>
#include "../atom/analog_devices.h"

// called from the interrupt routine when the conversion is completed

#define SAMPLE_TICKS 1

#define scale_name "Scale"
#define offset_name "Offset"
#define publish_name "Publish Rate"
#define filter_name "Filter Rate"
#define id_name "Can ID"

static analog_channels_t *channels; // this holds all of the definitions
static semaphore_t analog_conversion_complete;

static void setup_hardware(analog_channels_t *init_channels);

#include <sys/attribs.h>

//void __attribute__( (interrupt(IPL0AUTO), vector(_ADC_VECTOR))) _adc_interrupt( void );
//void adc_interrupt(void)
void __attribute__( (interrupt(IPL0AUTO), vector(_ADC_EOS_VECTOR))) _adc_eos_interrupt( void );
void adc_eos_interrupt(void)
  {
   int i;
  
  /*
    // read the active channels
   for(i = 0; i < channels->num_channels; i++)
     {
     bool read_channel = false;
     uint32_t mask;
     if(channels->channel_definition[i].channel > 32)
       {
       mask = 1 << (channels->channel_definition[i].channel -32 );
       read_channel = (ADCDSTAT2 & mask) != 0;
       }
     else
       {
       mask = 1 << channels->channel_definition[i].channel;
       read_channel = (ADCDSTAT1 & mask) != 0;
       }

     if(read_channel)
       {
      uint32_t channel_data = ADCDATA0 + channels->channel_definition[i].channel;

      // we read in unsigned integer format.
      channels->channel_definition[i].analog_value = (uint16_t) channel_data;
       }
     }
   */
  
 if(ADCCON2bits.EOSRDY)
    {
    for(i = 0; i < channels->num_channels; i++)
      {
      // we read in unsigned integer format.
      uint32_t chnum = channels->channel_definition[i].channel;
      uint32_t channel_data = ADCDATA0 + chnum;
      channels->channel_definition[i].analog_value = (uint16_t) channel_data;
      }
    
    signal_from_isr(&analog_conversion_complete);
    }
   
  IFS6bits.ADCEOSIF = 0;
  }


// called back from the microkernel when a conversion is complete

static void conversion_complete_callback(void *parg)
  {
  while(true)
    {
    float value;

    semaphore_wait(&analog_conversion_complete, INDEFINITE_WAIT);
    
    int i;
    for(i = 0; i < channels->num_channels; i++)
      {
      analog_channel_definition_t *channel_definition = &channels->channel_definition[i];

      // convert the integer to a floating point valie (0 .. 1.0)
      value = channel_definition->analog_value;

      value *= channels->analog_factor;
 
      // otherwise use the parameter driven conversions.
      if(channel_definition->scale != 1.0)
        value *= channel_definition->scale;

      if(channel_definition->offset != 0.0)
        value += channel_definition->offset;

      // publish the result
      publish_float(channel_definition->can_id, value);
      }
    }
  }

static const char *analog_str = "analog";
static const char *name_str = "name";
static const char *scale_str = "scale";
static const char *offset_str = "offset";
static const char *can_id_str = "can-id";
static const char *filter_rate_str = "filter-rate";
static char tmp[32];

// this is called every 1 msec and all of the analog channels are
// sampled.  When the scan is completed the published values will be sent
static void analog_hook_fn(uint32_t ticks)
  {
  // start the next scan
  ADCCON3bits.GSWTRG = 1;
  }

static timer_hook_t analog_hook = { 0, 0, analog_hook_fn };

result_t analog_init(analog_channels_t *init_channels, uint16_t stack_length, bool factory_reset)
  {
  result_t result;
  uint16_t i;
  
  channels = init_channels;
  
  // read the registry values.
  memid_t key;
  memid_t cfg_key;
  if(factory_reset || failed(reg_open_key(0, analog_str, &key)))
    {
    // must be initial phase.
    if(failed(result = reg_create_key(0, analog_str, &key)))
      return result;
    
    // create a basic channel
    for(i = 0; i < channels->num_channels; i++)
      {
      snprintf(tmp, 32, "%d", channels->channel_definition[i].channel);
      
      reg_create_key(key, tmp, &cfg_key);
      
      reg_set_string(cfg_key, name_str, channels->channel_definition[i].name);
      reg_set_float(cfg_key, scale_str, channels->channel_definition[i].scale);
      reg_set_float(cfg_key, offset_str, channels->channel_definition[i].offset);
      reg_set_uint16(cfg_key, can_id_str, channels->channel_definition[i].can_id);
      }
    // initial registry is created.
    }
  else
    {
    // read the reg values
    for(i = 0; i < channels->num_channels; i++)
      {
      snprintf(tmp, 32, "%d", channels->channel_definition[i].channel);
      if(succeeded(reg_open_key(key, tmp, &cfg_key)))
        {
        uint16_t len = REG_STRING_MAX;
        
        reg_get_string(cfg_key, name_str, channels->channel_definition[i].name, &len);
        reg_get_float(cfg_key, scale_str, &channels->channel_definition[i].scale);
        reg_get_float(cfg_key, offset_str, &channels->channel_definition[i].offset);
        reg_get_uint16(cfg_key, can_id_str, &channels->channel_definition[i].can_id);
        }
      }
    }
  
  /* initialize ADC calibration setting */
  ADC0CFG = DEVADC0;
  ADC1CFG = DEVADC1;
  ADC2CFG = DEVADC2;
  ADC3CFG = DEVADC3;
  ADC4CFG = DEVADC4;
  ADC7CFG = DEVADC7;  
  
  //ADCCON1bits.FSPBCLKEN = 1;
  
  /* Initialize warm up time register */
  ADCANCON = 0;
  ADCCON1 = 0;                  // clear all bits
  ADCCON2 = 0;
  ADCCON3 = 0;
  
  ADCANCONbits.WKUPCLKCNT = 5;    // Wakeup exponent = 32 * TADx
  ADCCON1bits.STRGSRC = 1;        // Global software edge trigger

  ADCCON3bits.ADCSEL = 0;         // PBCLK03 (10Mhz)
  ADCCON2bits.SAMC = 5;           // set up for 40ns
  ADCCON3bits.VREFSEL = 0;        // Select AVDD and AVSS as reference source
  
  // set the prescaler
  ADCCON2bits.ADCDIV = 1;         // Tq / 2
  
  // enable the end of scan interrupt
  ADCCON2bits.EOSIEN = 1;
  
  ADCCON1bits.SELRES = 3;   // 12 bits
  
  // We set up a scan for the ADC.
  for(i = 0; i < channels->num_channels; i++)
    {
    uint16_t channel = channels->channel_definition[i].channel;
     
    switch(channel)
      {
      case 0 :
        ADCTRG1bits.TRGSRC0 = 3;          // scanned trigger
        ADC0TIMEbits.ADCDIV = 1; // ADC0 clock frequency is half of control clock = TAD0
        ADC0TIMEbits.SAMC = 5; // ADC0 sampling time = 5 * TAD0
        ADC0TIMEbits.SELRES = 3; // ADC0 resolution is 12 bits
        ADCIMCON1bits.SIGN0 = 0; // unsigned data format
        ADCIMCON1bits.DIFF0 = 0; // Single ended mode
        ADCCON3bits.DIGEN0 = 1; // Enable ADC0
        ADCANCONbits.ANEN0 = 1; // Enable the clock to analog bias
        ADCCSS1bits.CSS0 = 1;
        break;
      case 1 :
        ADCTRG1bits.TRGSRC1 = 3;
        ADC1TIMEbits.ADCDIV = 1; // ADC1 clock frequency is half of control clock = TAD1
        ADC1TIMEbits.SAMC = 5; // ADC1 sampling time = 5 * TAD1
        ADC1TIMEbits.SELRES = 3; // ADC1 resolution is 12 bits
        ADCIMCON1bits.SIGN1 = 0; // unsigned data format
        ADCIMCON1bits.DIFF1 = 0; // Single ended mode
        ADCCON3bits.DIGEN1 = 1; // Enable ADC1
        ADCANCONbits.ANEN1 = 1; // Enable the clock to analog bias
        ADCCSS1bits.CSS1 = 1;
        break;
      case 2 :
        ADCTRG1bits.TRGSRC2 = 3;
        ADC2TIMEbits.ADCDIV = 1; // ADC2 clock frequency is half of control clock = TAD2
        ADC2TIMEbits.SAMC = 5; // ADC2 sampling time = 5 * TAD2
        ADC2TIMEbits.SELRES = 3; // ADC2 resolution is 12 bits
        ADCIMCON1bits.SIGN2 = 0; // unsigned data format
        ADCIMCON1bits.DIFF2 = 0; // Single ended mode
        ADCCON3bits.DIGEN2 = 1; // Enable ADC2
        ADCANCONbits.ANEN2 = 1; // Enable the clock to analog bias
        ADCCSS1bits.CSS2 = 1;
        break;
      case 3 :
        ADCTRG1bits.TRGSRC3 = 3;
        ADC3TIMEbits.ADCDIV = 1; // ADC2 clock frequency is half of control clock = TAD2
        ADC3TIMEbits.SAMC = 5; // ADC2 sampling time = 5 * TAD2
        ADC3TIMEbits.SELRES = 3; // ADC2 resolution is 12 bits
        ADCIMCON1bits.SIGN3 = 0; // unsigned data format
        ADCIMCON1bits.DIFF3 = 0; // Single ended mode
        ADCCON3bits.DIGEN3 = 1; // Enable ADC3
        ADCANCONbits.ANEN3 = 1; // Enable the clock to analog bias
        ADCCSS1bits.CSS3 = 1;
        break;
      case 4 :
        ADCTRG2bits.TRGSRC4 = 3;
        ADC4TIMEbits.ADCDIV = 1; // ADC2 clock frequency is half of control clock = TAD2
        ADC4TIMEbits.SAMC = 5; // ADC2 sampling time = 5 * TAD2
        ADC4TIMEbits.SELRES = 3; // ADC2 resolution is 12 bits
        ADCIMCON1bits.SIGN4 = 0; // unsigned data format
        ADCIMCON1bits.DIFF4 = 0; // Single ended mode
        ADCCON3bits.DIGEN4 = 1; // Enable ADC4
        ADCANCONbits.ANEN4 = 1; // Enable the clock to analog bias
        ADCCSS1bits.CSS4 = 1;
        break;
      case 5 :
        ADCIMCON1bits.SIGN5 = 0;  // unsigned data format
        ADCIMCON1bits.DIFF5 = 0;  // Single ended mode
        ADCTRG2bits.TRGSRC5 = 3;
        ADCCSS1bits.CSS5 = 1;
        break;
      case 6 :
        ADCIMCON1bits.SIGN6 = 0;  // unsigned data format
        ADCIMCON1bits.DIFF6 = 0;  // Single ended mode
        ADCTRG2bits.TRGSRC6 = 3;
        ADCCSS1bits.CSS6 = 1;
        break;
      case 7 :
        ADCIMCON1bits.SIGN7 = 0;  // unsigned data format
        ADCIMCON1bits.DIFF7 = 0;  // Single ended mode
        ADCTRG2bits.TRGSRC7 = 3;
        ADCCSS1bits.CSS7 = 1;
        break;
      case 8 :
        ADCIMCON1bits.SIGN8 = 0;  // unsigned data format
        ADCIMCON1bits.DIFF8 = 0;  // Single ended mode
        ADCTRG3bits.TRGSRC8 = 3;
        ADCCSS1bits.CSS8 = 1;
        break;
      case 9 :
        ADCIMCON1bits.SIGN9 = 0;  // unsigned data format
        ADCIMCON1bits.DIFF9 = 0;  // Single ended mode
        ADCTRG3bits.TRGSRC9 = 3;
        ADCCSS1bits.CSS9 = 1;
        break;
      case 10 :
        ADCIMCON1bits.SIGN10 = 0;  // unsigned data format
        ADCIMCON1bits.DIFF10 = 0;  // Single ended mode
        ADCTRG3bits.TRGSRC10 = 3;
        ADCCSS1bits.CSS10 = 1;
        break;
      case 11 :
        ADCIMCON1bits.SIGN11 = 0;  // unsigned data format
        ADCIMCON1bits.DIFF11 = 0;  // Single ended mode
        ADCTRG3bits.TRGSRC11 = 3;
        ADCCSS1bits.CSS11 = 1;
        break;
      case 12 :
        ADCIMCON1bits.SIGN12 = 0;  // unsigned data format
        ADCIMCON1bits.DIFF12 = 0;  // Single ended mode
        ADCCSS1bits.CSS12 = 1;
        break;
      case 13 :
        ADCIMCON1bits.SIGN13 = 0;  // unsigned data format
        ADCIMCON1bits.DIFF13 = 0;  // Single ended mode
        ADCCSS1bits.CSS13 = 1;
        break;
      case 14 :
        ADCIMCON1bits.SIGN14 = 0;  // unsigned data format
        ADCIMCON1bits.DIFF14 = 0;  // Single ended mode
        ADCCSS1bits.CSS14 = 1;
        break;
      case 15 :
        ADCIMCON1bits.SIGN15 = 0;  // unsigned data format
        ADCIMCON1bits.DIFF15 = 0;  // Single ended mode
        ADCCSS1bits.CSS15 = 1;
        break;
      case 16 :
        ADCIMCON2bits.SIGN16 = 0;  // unsigned data format
        ADCIMCON2bits.DIFF16 = 0;  // Single ended mode
        ADCCSS1bits.CSS16 = 1;
        break;
      case 17 :
        ADCIMCON2bits.SIGN17 = 0;  // unsigned data format
        ADCIMCON2bits.DIFF17 = 0;  // Single ended mode
        ADCCSS1bits.CSS17 = 1;
        break;
      case 18 :
        ADCIMCON2bits.SIGN18 = 0;  // unsigned data format
        ADCIMCON2bits.DIFF18 = 0;  // Single ended mode
        ADCCSS1bits.CSS18 = 1;
        break;
     }
    }
  
  ADCCON3bits.DIGEN7 = 1;   // Enable ADC7
  ADCANCONbits.ANEN7 = 1;   // Enable the clock to analog bias
  
  //IFS1bits.ADCIF = 0;
  //IPC11bits.ADCIP = 6;
  //IPC11bits.ADCIS = 0;
  //IEC1bits.ADCIE = 1;
  IFS6bits.ADCEOSIF = 0;
#ifndef __32MZ2064DAH176
  IPC48bits.ADCEOSIP = 6;
  IPC48bits.ADCEOSIS = 0;
#endif
  IEC6bits.ADCEOSIE = 1;

  
  ADCCON1bits.ON = 1;
    
  /* Wait for voltage reference to be stable */
  while(!ADCCON2bits.BGVRRDY); // Wait until the reference voltage is ready
  if(ADCCON2bits.REFFLT)
    panic();
  
  
  /* Wait for ADC to be ready */
  if(ADCCON3bits.DIGEN0)
    while(!ADCANCONbits.WKRDY0); // Wait until ADC0 is ready
    
  if(ADCCON3bits.DIGEN1)
    while(!ADCANCONbits.WKRDY1); // Wait until ADC1 is ready
    
  if(ADCCON3bits.DIGEN2)
    while(!ADCANCONbits.WKRDY2); // Wait until ADC2 is ready
    
  if(ADCCON3bits.DIGEN3)
    while(!ADCANCONbits.WKRDY3); // Wait until ADC2 is ready
    
  if(ADCCON3bits.DIGEN4)
    while(!ADCANCONbits.WKRDY4); // Wait until ADC2 is ready
   
  if(ADCCON3bits.DIGEN7)
    while(!ADCANCONbits.WKRDY7); // Wait until ADC2 is ready

  // start sampling
  timer_hook(&analog_hook);

  // schedule the task.  Needs to run as soon as the IRQ happens to stop conversion jitter.
  return task_create("ANALOG", stack_length, conversion_complete_callback, 0, HIGH_PRIORITY, 0);
  }
