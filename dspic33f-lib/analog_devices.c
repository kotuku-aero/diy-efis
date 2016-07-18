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
#include <p33Exxxx.h>
#include <math.h>

#include "analog_devices.h"
#include "microkernel.h"
#include "eeprom.h"

#define SAMPLE_TICKS 1

static analog_channels_t *channels; // this holds all of the definitions
static int channel_num = 0;
static semaphore_t analog_conversion_complete;

// called from the interrupt routine when the conversion is completed

void __attribute__((interrupt, no_auto_psv)) _AD1Interrupt(void)
  {
  channels->channel_definition[channel_num].analog_value = ADC1BUF0;

  IFS0bits.AD1IF = 0; // clear IRQ
  signal_from_isr(&analog_conversion_complete);
  }

void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void)
  {
  T3CONbits.TON = 0; // turn off timer
  IFS0bits.T3IF = 0;
  }

static void start_conversion(short next_channel)
  {
  analog_channel_definition_t *channel_definition = &(channels->channel_definition[next_channel]);

  // if we have a call-back to start the capture we call it now.
  if(channels->selector != 0)
    AD1CHS0 = channels->selector(channel_definition->channel_num);
  else
    // assign the channel number to the ADC
    AD1CHS0 = channel_definition->channel_num;
  }

// called back from the microkernel when a conversion is complete

static void conversion_complete_callback(void *parg)
  {
  while(true)
    {
    float value;

    wait(&analog_conversion_complete, INDEFINITE_WAIT);

    analog_channel_definition_t *channel_definition = &channels->channel_definition[channel_num];

    // and convert the next channel
    channel_num++;
    if(channel_num >= channels->num_channels)
      channel_num = 0;

    start_conversion(channel_num); // cick this off as soon as possible

    value = channel_definition->analog_value * channels->analog_factor;

    // add this value
    channel_definition->result += value;

    channel_definition->publish_count++;
    can_parameter_type_0_t *config = channel_definition->definition;
    if(channel_definition->publish_count >= config->publish_rate)
      {
      float result;
      // take the average value
      channel_definition->result /= channel_definition->publish_count;

      // otherwise use the parameter driven conversions.
      if(config->scale != 1.0)
        channel_definition->result *= config->scale;

      if(config->offset != 0.0)
        channel_definition->result += config->offset;

      // publish the result
      result = channel_definition->result_proc(channel_definition, channel_definition->result);

      channel_definition->result = 0;
      channel_definition->publish_count = 0;
      }
    }
  }

// this structure holds a tristate control so we can disable the tristates on
// anlog pins

typedef struct _tris_t
  {
  uint8_t channel;
  volatile unsigned int *port;
  uint16_t mask;
  } tris_t;

// size is dependent on the device...
#define NUMCHANNELS 10
static const tris_t tristate[NUMCHANNELS] = {
  { 0, &TRISA, 0x0001},
  { 1, &TRISA, 0x0002},
  { 2, &TRISB, 0x0001},
  { 3, &TRISB, 0x0002},
  { 4, &TRISB, 0x0004},
  { 5, &TRISB, 0x0008},
  { 24, &TRISA, 0x0010},
  { 25, &TRISB, 0x0008},
  { 26, &TRISB, 0x0010},
  { 27, &TRISB, 0x0020},
  };

int8_t analog_init(analog_channels_t *init_channels, uint16_t *stack, uint16_t stack_length)
  {
  int i;
  int8_t task_id;
  analog_channel_definition_t *channel_definition;
  channels = init_channels;

  // publish all of the channels that we will process.
  for(channel_num = 0; channel_num < channels->num_channels; channel_num++)
    {
    uint8_t ch_num = 0;

    channel_definition = &(channels->channel_definition[channel_num]);

    if(channels->selector != 0)
      ch_num = channels->selector(channel_definition->channel_num);
    else
      ch_num = channel_definition->channel_num;

    //ADPCFG &= ~(1 << ch_num);
    //TRISB |= 1 << ch_num;
    for(i = 0; i < NUMCHANNELS; i++)
      {
      if(tristate[i].channel == ch_num)
        {
        *(tristate[i].port) |= tristate[i].mask;
        break;
        }
      }
    }


  // set interrupt priority = 5 for ADC
  IPC3bits.AD1IP = 5;
  IFS0bits.AD1IF = 0; // reset int
  IEC0bits.AD1IE = 1; // and enable the interrupt

  PR5 = channels->divisor;

  AD1CON1bits.SSRCG = 0; // Not using PWM to start ADC
  AD1CON1bits.SSRC = 2; // Timer3 is the start of conversion
  AD1CON1bits.FORM = 0; // integer data
  AD1CON1bits.ASAM = 1; // sampling when conversion complete
  AD1CON1bits.AD12B = 1; // 12 bits
  AD1CON1bits.SIMSAM = 0; // sample channels in sequence

  AD1CON2bits.SMPI = 0; // 1 sample
  AD1CON2bits.CSCNA = 0; // Do not scan inputs
  AD1CON2bits.VCFG = 0; // use VSS

  AD1CON3bits.SAMC = 31; // auto sample time bits
  AD1CON3bits.ADRC = 0; // system clock
  AD1CON3bits.ADCS = 7; // Tad = 200ns @ 20Mhz

  AD1CHS0 = 0;

  TMR3 = 0;
  T3CONbits.TCKPS = channels->prescaler;
  // load the comparator registers
  PR3 = channels->divisor;

  IFS0bits.T3IF = 0;
  IEC0bits.T3IE = 0;

  // schedule 2 tasks.
  task_id = create_task("ANALOG", stack, stack_length, conversion_complete_callback, 0, NORMAL_PRIORITY);

  channel_num = 0;
  start_conversion(channel_num);

  T3CONbits.TON = 1; // start timer
  AD1CON1bits.ADON = 1;

  // start the task
  resume(task_id);
  return task_id;
  }

