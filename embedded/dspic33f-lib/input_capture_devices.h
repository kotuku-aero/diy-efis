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
#ifndef INPUT_CAPTURE_DEVICES_H
#define	INPUT_CAPTURE_DEVICES_H

#include "../can-aerospace/can_msg.h"
#include "../dspic-lib/publisher.h"

#define ACCUMULATOR_CHANNEL    0x80
#define CHANNEL_MASK 0x0F

/* Input capture devices are connected to one of the 16 possible DSPIC
 * input capture inputs.  Depending on the processor type, the pin functions
 * will be multiplexed.
 * This is a parameter driven input driver
 *
 * This code uses TMR2
 */
struct _capture_channel_definition_t;
typedef uint16_t (*capture_callback)(struct _capture_channel_definition_t *channel, uint16_t value);
typedef struct _capture_channel_definition_t
  {
  uint8_t channel_num;          // this is the input capture channel number
  uint16_t config_memid;        // offset into eeprom config slot for this channel
  uint8_t capture_mode;         // 1 = every edge
                                // 2 = falling edge
                                // 3 = rising edge
                                // 4 = 4th rising edge
                                // 5 = 16th rising edge
  capture_callback int_proc;    // called when a capture event interrupt is called
  capture_callback result_proc; // pre-process result to be published.
  unsigned int publish_count;   // number of samples received
  unsigned int capture_count;   // number of samples captured
  unsigned int timer_value;     // last value read from the timer capture register, if 0 then resync
  float result;                 // filter result
  } capture_channel_definition_t;

typedef struct
  {
  capture_channel_definition_t *channel_definition;
  int num_channels;
  uint8_t prescaler;            // used to calculate the timing accuracy.  must be 0 = 1, 1 = 8, 2 = 64, 3 = 256
  uint16_t divisor;             // divisor to set the sample rate
  } capture_channels_t;

// Must be created in a startup task.
extern void capture_init(capture_channels_t *channels);

// this routine can be used to generate a quadrature up/down for a pair
// of channels.
// channel_a_event assumes that the channel is the first of the pair.
// channel_b_event assumes that the channel is the second of a pair.
extern unsigned int channel_a_event(struct _capture_channel_definition_t *channel, unsigned int value);
extern unsigned int channel_b_event(struct _capture_channel_definition_t *channel, unsigned int value);

#endif	/* INPUT_CAPTURE_DEVICES_H */
