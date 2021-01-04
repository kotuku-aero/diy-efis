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

// the nanoFramework header
#include "CLR_Startup_Thread.h"

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

  // this is the type of message passed to the callback functions.
  typedef struct _ion_request {
    canmsg_t msg;
    } ion_request_t;

  extern const char *ion_s;
  extern const char *event_s;
  extern const char *ion_name;

extern result_t ion_init();
  /**
   * Setup the ECMA Script 5 interpreter
   * @param home      home key to refer all load funcs to
   * @param path      path to the script to run
   * @param ci        console in
   * @param co        console out handler
   * @param cerr      console error
   * @param ion       resulting interactive interpreter
   * @return 
   */
  extern result_t ion_create(memid_t home, const char *path,
                             handle_t ci,
                             handle_t co,
                             handle_t cerr,
                             struct _ion_context_t **ion);
  /**
   * Queue a message to the worker to process.
   * @param ion     Context for the interpreter
   * @param handler Registered name for the message (MUST be static variable)
   * @param msg     Message to queue
  */
  extern result_t ion_queue_message(struct _ion_context_t *ion, const canmsg_t *msg);

  extern result_t ion_close(struct _ion_context_t *ion);
  /**
   * @function ion_run(memid_t key)
   * Run the ion event handler code.  Usually the last thing to do
   * Never returns
   * @param key Optional key to the registry for ion
   * 
   */
  extern result_t ion_run(memid_t key);

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
