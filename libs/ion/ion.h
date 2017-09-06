#ifndef __ion_h__
#define __ion_h__

#include "duktape.h"

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
    handle_t worker;              // worker script
    handle_t message_queue;       // queue of messages being handled
    } ion_context_t;

  extern const char *ion_key;
  extern const char *event_key;
  extern const char *ion_name;

#ifdef __cplusplus
  }
#endif


#endif
