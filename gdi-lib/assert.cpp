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
#include <stdio.h>
#include "assert.h"
#include "hal.h"

#include <string>

void __assert(const char *file, int line, bool check)
  {
  if(__debug_flag && !check)
    {
    char msg[256];
    sprintf(msg, "Debug assertion failed at %s:%d.\n"
                 "You may continue after this assertion by pressing the OK button\n"
                 "Press Cancel to abort", file, line);
    trace_emergency(msg);
    kotuku::the_hal()->assert_failed();
    }
  }

void __trace(int level, const char *fmt_msg, va_list va)
{
  static char msg[4096];

  vsprintf(msg, fmt_msg, va);

  kotuku::the_hal()->debug_output(level, msg);
  }

result_t __is_bad_read_pointer(const void *ptr, size_t size)
  {
  return kotuku::the_hal()->is_bad_read_pointer(ptr, size);
  }

result_t __is_bad_write_pointer(void *ptr, size_t size)
  {
  return kotuku::the_hal()->is_bad_write_pointer(ptr, size);
  }

result_t get_last_error()
  {
  return kotuku::the_hal()->get_last_error();
  }

result_t set_last_error(result_t r)
  {
  kotuku::the_hal()->set_last_error(r);
  return r;
  }

long __interlocked_increment(volatile long &l)
  {
  return kotuku::the_hal()->interlocked_increment(l);
  }

long __interlocked_decrement(volatile long &l)
  {
  return kotuku::the_hal()->interlocked_decrement(l);
  }
