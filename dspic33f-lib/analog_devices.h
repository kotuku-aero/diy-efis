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
#ifndef __analog_devices_h__
#define __analog_devices_h__

#include "../can-aerospace/parameter_definitions.h"

// this structure defines the dsPIC analog channel ports
// in our designs we have systems that have up to 4 mux'd inputs
// controlled by RE0 and RE1
// There are up to 9 analog channels that can be used.
// in addition a low pass filter can be assigned to filter the analog signal.
// the nominal sample rate is 4Khz, although the conversion is done at 600kp/s
// with up to 16 channels this equates to 37500 max samples.

// this code uses TMR3

struct _channel_definition_t;
typedef float (*publish_analog_result)(struct _channel_definition_t *channel, float value);

// routine that can be provided to de-multiplex a channel.
// the value returned must be the DSPIC channel number that is managed
// by the device.  The sub-channel is then set
// e.g. PORTE = sub_channel_num;
typedef uint8_t (*select_channel)(uint8_t channel);

typedef struct _channel_definition_t
  {
  uint8_t channel_num;
  can_parameter_type_0_t *definition;   // parameters that manage this channel
  publish_analog_result result_proc;    // pre-process result to be published.
  int publish_count;
  short analog_value;                   // read from the ADC
  float result;                         // filtered result
  } analog_channel_definition_t;

typedef struct
  {
  analog_channel_definition_t *channel_definition;
  int num_channels;
  select_channel selector;      // if non-0 then allows for a sub-channel select
  uint8_t prescaler;            // used to calculate the timing accuracy.  must be 0 = 1, 1 = 8, 2 = 64, 3 = 256
  uint16_t divisor;             // divisor to set the sample rate
  float analog_factor;          // set to 0.0009765625 for 10Bit devices
                                // dsPIC30F4011 dsPIC30F4012
                                // set to 0.000244140625 for 12 bit devices
                                // dsPIC30F4013
  } analog_channels_t;

/**
 * Start the Analog subsystem
 * @param channels    definitions for the analog channels
 */
extern int8_t analog_init(analog_channels_t *channels, uint16_t *stack, uint16_t stack_length);

#endif
