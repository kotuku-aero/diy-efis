#ifndef __interpreter_h__
#define __interpreter_h__

#include "../neutron/neutron.h"

#ifdef __cplusplus
extern "C" {
#endif
  struct _ion_context_t;

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
    handle_t ci, handle_t co, handle_t cerr, struct _ion_context_t **ion);
  /**
   * Queue a message to the worker to process.
   * @param ion     Context for the interpreter
   * @param handler Registered handler for the message
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
   * Run the ion event handler code.  Usually the last thing to do
   */
  extern result_t ion_run();
  
#ifdef __cplusplus
  }
#endif

#endif
