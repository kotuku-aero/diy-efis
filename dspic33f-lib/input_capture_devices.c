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
#include "../dspic-lib/microkernel.h"
#include "input_capture_devices.h"
#include "../dspic-lib/can_app_cfg.h"

static capture_channels_t *channels;       // this holds all of the definitions

static capture_channel_definition_t *channel_1 = 0;
static capture_channel_definition_t *channel_2 = 0;
static capture_channel_definition_t *channel_7 = 0;
static capture_channel_definition_t *channel_8 = 0;

static void update_channel(capture_channel_definition_t *channel, int value)
  {
  if(channel == 0)
    return;

  // if the timer_value == 0 then the timer has overflowed so we can't
  // use the value.  We need to resync the input capture
  if(channel->timer_value > 0)
    {
    int period = value - channel->timer_value;
    channel->result += period;
    channel->capture_count++;

    if(channel->int_proc != 0)
      (*channel->int_proc)(channel, period);
    }

  channel->timer_value = value;
  }

void __attribute__((interrupt, no_auto_psv)) _IC1Interrupt(void)
  {
  IFS0bits.IC1IF = 0;
  while(IC1CONbits.ICBNE)
    update_channel(channel_1, IC1BUF);
  }

void __attribute__((interrupt, no_auto_psv)) _IC2Interrupt(void)
  {
  IFS0bits.IC2IF = 0;
  while(IC2CONbits.ICBNE)
    update_channel(channel_2, IC2BUF);
  }

void __attribute__((interrupt, no_auto_psv)) _IC7Interrupt(void)
  {
  IFS1bits.IC7IF = 0;
  while(IC7CONbits.ICBNE)
    update_channel(channel_7, IC7BUF);
  }

void __attribute__((interrupt, no_auto_psv)) _IC8Interrupt(void)
  {
  IFS1bits.IC8IF = 0;
  while(IC8CONbits.ICBNE)
    update_channel(channel_8, IC8BUF);
  }

// called from the interrupt routine when the publish tick is made
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void)
  {
  IFS0bits.T2IF = 0;

  // reset all timer values
  if(channel_1 != 0)
    channel_1->timer_value = 0;

  if(channel_2 != 0)
    channel_2->timer_value = 0;

  if(channel_7 != 0)
    channel_7->timer_value = 0;

  if(channel_8 != 0)
    channel_8->timer_value = 0;
  
  // set the flag
  signal(CAPTURE_COMPLETE);
  }

static void calculate_and_publish(capture_channel_definition_t *channel_definition)
  {
  // signal one more clock timeout
  channel_definition->publish_count++;
  if(channel_definition->publish_count >= channel_definition->can_channel.publish_frequency)
    {
    float result;
    // calculate the result first
    if(channel_definition->capture_count == 0)
      channel_definition->result = 0;
    else
      {
      // take the average value
      channel_definition->result /= channel_definition->capture_count;

      // the result is a count of 20mhz / 256 intervals
      channel_definition->result *= 0.0000128;

      // convert to a frequency
      channel_definition->result = 1 / channel_definition->result;
      }
    
    // otherwise use the parameter driven conversions.
    if(channel_definition->can_channel.scale != 1.0)
      channel_definition->result *= channel_definition->can_channel.scale;

    if(channel_definition->can_channel.offset != 0.0)
      channel_definition->result += channel_definition->can_channel.offset;

    // if we have a result processor then we just pass the filtered value to it to be processed.
    if(channel_definition->result_proc != 0)
      result = channel_definition->result_proc(channel_definition, channel_definition->result);
    else
      result = channel_definition->result;

    // we send the value (frequency) as a 16 bit number
    send_short(result, &(channel_definition->can_channel));

    // reset counter so that we calculate the next period
    channel_definition->result = -1;
    channel_definition->capture_count = 0;
    }
  }

// called back from the microkernel when a conversion is complete
static void capture_callback_proc(void *parg)
  {
  if(channel_1 != 0)
    calculate_and_publish(channel_1);

  if(channel_2 != 0)
    calculate_and_publish(channel_2);

  if(channel_7 != 0)
    calculate_and_publish(channel_7);

  if(channel_8 != 0)
    calculate_and_publish(channel_8);
  }

void capture_init(capture_channels_t *init_channels)
{
  capture_channel_definition_t *channel_definition;
  int channel_num;
  
  channels = init_channels;

  // publish all of the channels that we will process.
  for(channel_num = 0; channel_num < channels->num_channels; channel_num++)
    {
    channel_definition = &(channels->channel_definition[channel_num]);

    // this assignes our outpt definitions
    define_msg(channel_definition->config_memid, &(channel_definition->can_channel));

    // decide from the channel number what register we need to set up
    // int priority = 4
    switch(channel_definition->channel_num)
      {
      case 1 :
        channel_1 = channel_definition;
        IC1CONbits.ICTMR = 1;       // TMR2
        IC1CONbits.ICM = channel_definition->capture_mode;
        IFS0bits.IC1IF = 0;
        IEC0bits.IC1IE = 1;
        IPC0bits.IC1IP = 4;
        TRISDbits.TRISD0 = 1;
        break;
      case 2 :
        channel_2 = channel_definition;
        IC2CONbits.ICTMR = 1;       // TMR2
        IC2CONbits.ICM = channel_definition->capture_mode;
        IFS0bits.IC2IF = 0;
        IEC0bits.IC2IE = 1;
        IPC1bits.IC2IP = 4;
        TRISDbits.TRISD1 = 1;
        break;
      case 7 :
        channel_7 = channel_definition;
        IC7CONbits.ICTMR = 1;       // TMR2
        IC7CONbits.ICM = channel_definition->capture_mode;
        IFS1bits.IC7IF = 0;
        IEC1bits.IC7IE = 1;
        IPC4bits.IC7IP = 4;
        TRISBbits.TRISB4 = 1;
        break;
      case 8 :
        channel_8 = channel_definition;
        IC8CONbits.ICTMR = 1;       // TMR2
        IC8CONbits.ICM = channel_definition->capture_mode;
        IFS1bits.IC7IF = 0;
        IEC1bits.IC7IE = 1;
        IPC4bits.IC8IP = 4;
        TRISBbits.TRISB5 = 1;
        break;
      }
    }

  TMR2 = 0;
  T2CONbits.TCKPS = channels->prescaler;
  // load the comparator registers
  PR2 = channels->divisor;

  IFS0bits.T2IF = 0;
  IEC0bits.T2IE = 1;

  // schedule 2 tasks.
  create_task(CAPTURE_COMPLETE, capture_callback_proc, 0);

  T2CONbits.TON = 1;            // start timer
}