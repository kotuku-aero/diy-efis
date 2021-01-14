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
then the original copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
#ifndef __ion_h__
#define __ion_h__

#include "../neutron/neutron.h"

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct _ion_context_t {
    handle_t console_in;          // console in for script engine
    handle_t console_out;         // console out for script engine
    handle_t console_err;         // error console for script engine
    memid_t home;                 // home registry key for the script engine
    task_p worker;                // worker script.  This runs the nanoFramework
    } ion_context_t;

  /**
  * @function ion_create(ion_context_t *ion, const char *path)
   * Setup the nanoFramework interpreter
   * @param ion       resulting interactive interpreter
   * @return 
   */
  extern result_t ion_create(ion_context_t *ion);
  /**
   * Queue a message to the worker to process.
   * @param ion     Context for the interpreter
   * @param handler Registered name for the message (MUST be static variable)
   * @param hwnd    Optional handle of window to receive the message
   * @param msg     Message to queue
  */
  extern result_t ion_queue_message(ion_context_t *ion_context, uint32_t hwnd, const canmsg_t *msg);

  extern result_t ion_close(ion_context_t *ion_context);

  // this will free the context when it exits
  extern void ion_run(void *ion_context);

  /**
   * @function ion_malloc(size_t len)
   * Allocate a block of memory to be used by ion
   * @param len number of bytes to allocate
   * @return pointer to the memory block, else 0 for no memory
  */
  extern void* ion_malloc(size_t len);
  /**
  *  @function ion_free(void *buffer)
   * Free a block of memory allocated by ion
   * @param buffer previously allocated buffer
  */
  extern void ion_free(void *buffer);

#ifdef __cplusplus
  }
#endif


#endif
