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
#ifndef __interpreter_h__
#define __interpreter_h__

#include "../neutron/neutron.h"
#include "duktape.h"

#ifdef __cplusplus
extern "C" {
#endif
  struct _ion_context_t;
  
  /**
   * @function ion_register_fn(duk_context *ctx, handle_t co)
   * callback to register application specific library functions
   * @param ctx   Context of the newly created script engine
   * @param co    Optional console output to log function output to
   * @return s_ok if functions registered ok
   */
  typedef result_t (*ion_register_fn)(duk_context *ctx, handle_t co);

  extern result_t ion_init();
  /**
   * Setup the ECMA Script 5 interpreter
   * @param home      home key to refer all load funcs to
   * @param path      path to the script to run
   * @param ci        console in
   * @param co        console out handler
   * @param cerr      console error
   * @param lib_funcs Optional library function
   * @param ion       resulting interactive interpreter
   * @return 
   */
  extern result_t ion_create(memid_t home, const char *path,
                             handle_t ci,
                             handle_t co,
                             handle_t cerr,
                             ion_register_fn lib_funcs,
                             struct _ion_context_t **ion);
  /**
   * Queue a message to the worker to process.
   * @param ion     Context for the interpreter
   * @param handler Registered name for the message (MUST be static variable)
   * @param msg     Message to queue
  */
  extern result_t ion_queue_message(struct _ion_context_t *ion, const char *handler, const canmsg_t *msg);
  /**
   * Execute an interactive command in the shell
   * @param ion     Context to use
   */
  extern result_t ion_exec(struct _ion_context_t *ion);

  extern result_t ion_close(struct _ion_context_t *ion);
  
  
  /**
   * @function ion_run(ion_register_fn lib_funcs)
   * Run the ion event handler code.  Usually the last thing to do
   * Never returns
   * @param lib_funcs Optional library functions to register
   * 
   */
  extern result_t ion_run(ion_register_fn lib_funcs);
  
#ifdef __cplusplus
  }
#endif

#endif
