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
#ifndef __input_sense_h__
#define __input_sense_h__

#include <stdbool.h>
#include <stdint.h>
#include "arinc.h"

typedef enum
{
  reg_b = 0x02C6,
  reg_c = 0x02CC,
  reg_d = 0x02D2,
  reg_e = 0x02D8,
  reg_f = 0x02DE
} dspic_port_t;

typedef bool (*preprocess_input_channel)(bool value);

typedef struct
{
  dspic_port_t port;      // base address of sense channel
  uint16_t mask;            // bit 0..15
  uint8_t config_memid;    // offset into config channel for this input
  preprocess_input_channel result_proc;
  published_msg_t can_channel;  // details and storage of the publish
  int publish_count;        // number of ticks the signal has been active
                            // once this == publish_frequency the signal is sent
                            // if the config scale is < 0 then the signal
                            // will repeat
} input_sense_channel_t;

typedef struct _input_sense_channels_t
{
  input_sense_channel_t *channels;
  uint8_t num_channels;
} input_sense_channels_t;

extern void input_sense_init(input_sense_channels_t *);

#endif
