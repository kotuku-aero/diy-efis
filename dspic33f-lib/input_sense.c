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
#include "input_sense.h"
#include "../dspic-lib/microkernel.h"
#include "../dspic-lib/can_app_cfg.h"

static void input_sense_worker(void *);

static void input_sense_worker(void *parg)
  {
  input_sense_channels_t *channels = (input_sense_channels_t *)parg;

  }

void input_sense_init(input_sense_channels_t *channels)
  {
  /*
  TRISBbits.TRISB5 = 1;     // enable RB5 as input
  TRISBbits.TRISB4 = 1;     // enable RB4 input
  TRISEbits.TRISE5 = 1;     // enable RE5 as input
  TRISEbits.TRISE4 = 1;     // enable RE4 as input
  TRISDbits.TRISD0 = 1;     // enable RD0 as input
  TRISDbits.TRISD1 = 1;     // enable RD1 as input

  // and ptr processing the data on this task
  create_task(TRIM_TASK, input_sense_worker, channels);

  // create a scheduled task that will be called every 4000 msec
  schedule_task(TRIM_TASK, 4);
   */
  }
