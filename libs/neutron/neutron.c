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
#include "neutron.h"
#include "bsp.h"

static stream_p trace_stream;
#ifdef _DEBUG
static uint16_t trace_level = 7;
#else
static uint16_t trace_level = 3;
#endif

static const char *dmesg_str = "dmesg";
static const char *trace_str = "trace-level";

// the trace stream is an event stream
void trace_init()
  {
  if (failed(stream_open(0, dmesg_str, &trace_stream)))
    {
    stream_create(0, dmesg_str, &trace_stream);
    }
  else
    stream_truncate(trace_stream, 0);

  if (failed(reg_get_uint16(0, trace_str, &trace_level)))
#ifdef _DEBUG
    trace_level = 7;
#else
    trace_level = 3;
#endif
  }

static const char *levels[8] = {
  "emergency",
  "alert",
  "critical",
  "error",
  "warning",
  "notice",
  "info",
  "debug"
  };

void trace(uint16_t level, const char *msg, ...)
  {
  va_list args;
  va_start(args, msg);

  if (trace_stream != 0 && trace_level <= level)
    {
    // send the timestamp
    stream_printf(trace_stream, "%ld %s ", ticks(), levels[level]);
    // send the level
    stream_vprintf(trace_stream, msg, args);
    stream_puts(trace_stream, "\r\n");
    }

  platform_trace(level, msg, args);
  }
