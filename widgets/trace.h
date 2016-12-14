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
#ifndef __trace_h__
#define __trace_h__

#include "canfly.h"

#include <stdarg.h>

#ifdef trace
#undef trace
#endif

extern void __trace(const char *msg, char **params);

// trace levels are 1..7
extern void __trace(int level, const char *msg, va_list params);

// debug only version of a trace message

inline void trace(int level, const char *msg, ...)
  {
  if (__tron)
    {
    va_list va;
    va_start(va, msg);

    __trace(level, msg, va);
    }
  }

#define trace_emergency(...)  trace(0, __VA_ARGS__)
#define trace_alert(...)      trace(1, __VA_ARGS__)
#define trace_critical(...)   trace(2, __VA_ARGS__)
#define trace_error(...)      trace(3, __VA_ARGS__)
#define trace_warning(...)    trace(4, __VA_ARGS__)
#define trace_notice(...)     trace(5, __VA_ARGS__)
#define trace_info(...)       trace(6, __VA_ARGS__)
#define trace_debug(...)      trace(7, __VA_ARGS__)

#endif
