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
#ifndef __proton_h__
#define	__proton_h__

#include "../ion/ion.h"

#ifdef	__cplusplus
extern "C"
  {
#endif

/**
 * @file proton.h
 * Proton widget library definitions
 */


typedef struct _proton_args_t {
  stream_p stream;
  handle_t ci;
  handle_t co;
  handle_t cerr;
  } proton_args_t;

  /**
   * @function run_proton(void *parg, handle_t ci, handle_t co, handle_t cerr)
   * Run the main window application loop
   * @param parg  Argument to pass
   */
  extern void run_proton(proton_args_t *args);

/**
 * @function load_layout(handle_t canvas, memid_t hive)
 * Load a registry hive that describes a series of windows
 * @param canvas    Parent window to create child windows within
 * @param hive      list of keys that describe each window
 * @return s_ok if loaded ok
 */
extern result_t load_layout(handle_t canvas, memid_t hive);
extern handle_t main_window;

#ifdef	__cplusplus
  }
#endif

#endif

