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
#ifndef __ion_h__
#define __ion_h__

#ifndef (__dsPIC33)
#include "duktape.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "interpreter.h"

  typedef struct _ion_context_t {
    handle_t console_in;          // console in for script engine
    handle_t console_out;         // console out for script engine
    handle_t console_err;         // error console for script engine
    memid_t home;                 // home registry key for the script engine
    duk_context *ctx;             // context, used by script engine
    memid_t script;               // handle to the script.  Used to compare running instances
    task_p worker;              // worker script
    deque_p message_queue;       // queue of messages being handled
    } ion_context_t;

  // this is the type of message passed to the callback functions.
  typedef struct _ion_request {
    const char *function_name;
    canmsg_t msg;
    } ion_request_t;

  extern const char *ion_s;
  extern const char *event_s;
  extern const char *ion_name;

typedef struct _event_registration_t {
  uint16_t can_id;
  const char *function_name;
} event_registration_t;
/**
 * @function add_event_handler(const char *script, const uint16_t *ids, uint16_t num_ids)
 * Utility function to register an event handler that will be processed by the ion engine.
 * @param name      Name of the javascript file.
 * @param script    Text of the javascript to be stored.
 * @param ids       Array of can-id's and function names to be registered
 * @param num_ids   Number of events to be hooked
 * @return s_ok if the events can be registered ok
 * @remark This is primarily used by embedded devices to register their default event scripts
 */
extern result_t add_event_handler(const char *name, const char *script, const event_registration_t *ids, uint16_t num_ids);


#ifdef __cplusplus
  }
#endif


#endif
