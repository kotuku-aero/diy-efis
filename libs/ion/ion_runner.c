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
#include "ion.h"

#include <ctype.h>

#define ION_QUEUE_SIZE 16
typedef struct _ion_request {
  char function_name[REG_NAME_MAX + 1];
  canmsg_t msg;
  } ion_request_t;

#define NUM_SCRIPTS 8
static uint16_t num_scripts;
static ion_context_t **scripts = 0;
static handle_t ion_mutex;

extern void register_ion_functions(duk_context *ctx, handle_t co);
extern result_t ion_split_path(const char *id, memid_t *parent, char *filename);

static void ion_worker(void *parg)
  {
  ion_context_t *ion = (ion_context_t *)parg;

  ion_request_t request;
  while (true)
    {
    if (failed(pop_front(ion->message_queue, &request, INDEFINITE_WAIT)))
      continue;

    duk_push_global_object(ion->ctx);
    if (!duk_get_prop_string(ion->ctx, -1 /*index*/, request.function_name))
      {
      if (ion->console_err != 0)
        stream_printf(ion->console_err, "Cannot find %s\n", request.function_name);
      }
    else
      {
      duk_push_pointer(ion->ctx, &request.msg);
      if (duk_pcall(ion->ctx, 1 /*nargs*/) != 0)
        {
        // figure out error handling!
        }
      }
    }
  }

result_t ion_init()
  {
  result_t result;
  if (failed(result = semaphore_create(&ion_mutex)) ||
    failed(semaphore_signal(ion_mutex)))
    return result;

  scripts = (ion_context_t **)neutron_malloc(sizeof(ion_context_t *) * NUM_SCRIPTS);
  if (scripts == 0)
    return e_not_enough_memory;

  memset(scripts, 0, sizeof(ion_context_t *) * NUM_SCRIPTS);
  num_scripts = NUM_SCRIPTS;

  return s_ok;
  }

extern result_t ion_close(struct _ion_context_t *ion)
  {
  semaphore_wait(ion_mutex, INDEFINITE_WAIT);

  uint16_t i;
  for (i = 0; i < num_scripts; i++)
    {
    if (scripts[i] == ion)
      {
      scripts[i] = 0;
      break;
      }
    }

  semaphore_signal(ion_mutex);
  // stop the worker
  close_task(ion->worker);
  deque_close(ion->message_queue);
  duk_destroy_heap(ion->ctx);

  neutron_free(ion);
  return s_ok;
  }

result_t ion_queue_message(struct _ion_context_t *ion, const char *handler, const canmsg_t *msg)
  {
  if (ion->message_queue == 0)
    return e_bad_handle;

  if (handler == 0 || msg == 0)
    return e_bad_parameter;

  ion_request_t request;
  strncpy(request.function_name, handler, REG_NAME_MAX);
  request.function_name[REG_NAME_MAX] = 0;
  memcpy(&request.msg, msg, sizeof(canmsg_t));

  return push_back(ion->message_queue, &request, INDEFINITE_WAIT);
  }

// TODO: at present these use the global heap, they should use a runner
// thread specific heap so that if the thread is terminated all memory is
// freed at the same time.
// this will also be very important when the js interpreter runs as
// a user space using a thunk.

static void *neutron_alloc_function(void *udata, duk_size_t size)
  {
  return neutron_malloc(size);
  }

static void *neutron_realloc_function(void *udata, void *ptr, duk_size_t size)
  {
  return neutron_realloc(ptr, size);
  }

static void neutron_free_function(void *udata, void *ptr)
  {
  neutron_free(ptr);
  }

static void neutron_fatal_function(void *udata, const char *msg)
  {
  // fatal error has occured
  ion_context_t *ion = (ion_context_t *)udata;

  if (ion->console_err != 0)
    stream_printf("ION terminates with error: %s", msg);

  // terminate the calling thread.  Should call thread_terminate()
  //
  // not sure about memory leaks!

  // kill the process...
  }

// create a context to run a script within and load it
result_t ion_create_worker(memid_t home,
  const char *name,
  handle_t ci,
  handle_t co,
  handle_t cerr,
  bool create_worker,
  uint8_t priority,
  ion_context_t **ion)
  {
  result_t result;

  memid_t parent = home;
  char filename[REG_NAME_MAX + 1];
  memid_t child;
  field_datatype dt;

  // if the name is 0 then we are creating a script runner
  // that needs a queue etc.
  // if it is 0 then an interactive engine is needed
  if (name != 0)
    {
    if (failed(result = ion_split_path(name, &parent, filename)))
      return result;

    // if the key is not there then we are done
    if (failed(result = reg_query_child(parent, filename, &child, &dt, 0)))
      return result;

    // must be a script type
    if (dt != field_stream)
      return e_bad_parameter;
    }

  ion_context_t *ion_ctx;
  // TODO: is this ok? should we time-out?
  semaphore_wait(ion_mutex, INDEFINITE_WAIT);

  ion_context_t **scr;
  uint16_t n;
  for (n = num_scripts, scr = scripts; n > 0; n--, scr++)
    {
    ion_ctx = *scr;

    // see if we have a free slot.
    if (ion_ctx == 0)
      continue;

    if (ion_ctx->script == child)
      {
      semaphore_signal(ion_mutex);
      *ion = ion_ctx;
      return s_ok;            // existing context so we don't need to create it.
      }
    }

  // context is not found so we create it.
  for (n = num_scripts, scr = scripts; n > 0; n--, scr++)
    {
    ion_ctx = *scr;
    if (ion_ctx == 0)
      break;              // found a free slot
    }

  if (ion_ctx != 0)
    {
    // means the last ptr != 0
    ion_context_t **new_scripts = (ion_context_t **)neutron_realloc(scripts, num_scripts + NUM_SCRIPTS);   // allocate new pointers...
    if (new_scripts == 0)
      {
      semaphore_signal(ion_mutex);
      return e_not_enough_memory;
      }
    scripts = new_scripts;
    memset(scripts + num_scripts, 0, sizeof(ion_context_t *) * NUM_SCRIPTS);
    num_scripts += NUM_SCRIPTS;
    }

  ion_ctx = (ion_context_t *)neutron_malloc(sizeof(ion_context_t));
  if (*ion == 0)
    {
    semaphore_signal(ion_mutex);
    return e_not_enough_memory;
    }

  // create the context.
  duk_context *ctx = duk_create_heap(neutron_alloc_function,
    neutron_realloc_function,
    neutron_free_function,
    ion_ctx,
    neutron_fatal_function);

  if (ctx == 0)
    {
    semaphore_signal(ion_mutex);
    return e_not_enough_memory;
    }

  memset(ion_ctx, 0, sizeof(ion_context_t));

  scripts[n] = ion_ctx;     // set the context.
  ion_ctx->home = home;
  ion_ctx->console_in = ci;
  ion_ctx->console_out = co;
  ion_ctx->console_err = cerr;
  ion_ctx->ctx = ctx;                        // store the context

                                             // release as we now have a new context.
  semaphore_signal(ion_mutex);

  if (create_worker)
    deque_create(sizeof(ion_request_t), ION_QUEUE_SIZE, &ion_ctx->message_queue);

  *ion = ion_ctx;

  register_ion_functions(ctx, co);

  // load the script if labeled
  if (name != 0)
    {
    // load the script.
    handle_t script;
    if (failed(result = stream_open(parent, name, &script)))
      {
      // release the context..
      ion_close(ion_ctx);
      return result;
      }

    uint16_t len;
    if (failed(result = stream_length(script, &len)))
      {
      stream_close(script);
      // release the context..
      ion_close(ion_ctx);
      return result;
      }

    char *text = (char *)neutron_malloc(len + 1);
    if (text == 0)
      {
      stream_close(script);
      // release the context..
      ion_close(ion_ctx);
      return e_not_enough_memory;
      }

    if (failed(result = stream_read(script, text, len, &len)))
      {
      neutron_free(text);
      stream_close(script);
      // release the context..
      ion_close(ion_ctx);
      return result;
      }

    text[len] = 0;

    duk_push_lstring(ctx, text, (duk_size_t)len);
    neutron_free(text);
    stream_close(script);

    if (duk_peval(ctx) != 0)
      {
      stream_printf(cerr, "%s\r\n", duk_safe_to_string(ctx, -1));
      ion_close(ion_ctx);
      return e_unexpected;
      }
    duk_pop(ctx);  /* ignore result */
    }

  // create the worker thread
  if (create_worker &&
    failed(result = task_create(name, DEFAULT_STACK_SIZE, ion_worker,
      ion, priority, &ion_ctx->worker)))
    {
    // release the context..
    ion_close(ion_ctx);
    return result;
    }

  return s_ok;
  }

result_t ion_create(memid_t home, const char *path,
    handle_t ci, handle_t co, handle_t cerr, struct _ion_context_t **ion)
  {
  return ion_create_worker(home, path, ci, co, cerr, false, 0, ion);
  }

#define WORKER_QUEUE_LENGTH 32
static handle_t ion_queue;

static bool ion_hook_handler(const canmsg_t *msg, void *parg)
  {
  // the hook handler discards messages so that if
  // the dispatcher is hung up the publisher is not stopped.
  if(ion_queue != 0)
    {
    push_back(ion_queue, msg, 0);
    return true;
    }
  
  return false;
  }

static msg_hook_t ion_hook = { 0, 0, ion_hook_handler };

// pool of all names that can be used.  often just ev_msg
handle_t atoms;
handle_t handlers;

typedef struct _handler_t {
  const char *handler;        // held in the string pool
  struct _ion_context_t *ctx;
  } handler_t;
  
// handlers for the map.  The key is a uint16 so just compare pointers...
static void dup_key(const void *src, void **dst)
  {
  *dst = (void *)src;
  }

static int compare_key(const void *left, const void *right)
  {
  uint16_t v1 = (uint16_t)(uint32_t)left;
  uint16_t v2 = (uint16_t)(uint32_t)right;
  
  if (v1 > v2)
    return 1;
  else if(v1 < v2)
    return -1;
  return 0;
  }

static void destroy_key(void *key)
  {
  
  }

static void dup_value(const void *src, void **dst)
  {
  *dst = (void *) src;
  }

static void destroy_value(void *value)
  {
  vector_close(value);
  }

result_t ion_run()
  {
  result_t result;
  
  if(failed(result = deque_create(sizeof(canmsg_t), WORKER_QUEUE_LENGTH, &ion_queue)))
    return result;
  
  if(failed(result = vector_create(sizeof(const char *), &atoms)))
    return result;
  
  if(failed(result = map_create(dup_key, dup_value, compare_key, destroy_key,
                                destroy_value, &handlers)))
    return result;
  
  // enumerate the ion keys
  memid_t ion_home;
  if(failed(result = reg_open_key(0, ion_key, &ion_home)))
    {
    if(failed(result = reg_create_key(0, ion_key, &ion_home)))
      return result;
    }
  
  memid_t parent;
  if(failed(result = reg_open_key(ion_home, event_key, &parent)))
    {
    if(failed(result = reg_create_key(ion_home, event_key, &parent)))
      return result;
    }
  
  field_datatype dt = field_key;
  char event_name[REG_NAME_MAX+1];
  memid_t child = 0;

  while (succeeded(reg_enum_key(parent, &dt, 0, 0, REG_NAME_MAX, event_name, &child)))
    {
    event_name[REG_NAME_MAX] = 0;
    // check for errors
    const char *str = event_name;
    bool valid_num = true;
    while (*str != 0)
      {
      if (!isdigit(*str))
        {
        valid_num = false;
        break;
        }
      str++;
      }

    if (!valid_num)
      continue;             // just ignore it

    uint16_t event_id = atoi(event_name);

    // iterate over the handlers.
    field_datatype str_type = field_string;
    memid_t str_child = 0;
    char handler_name[REG_NAME_MAX + 1];
    while (succeeded(reg_enum_key(child, &str_type, 0, 0, REG_NAME_MAX, handler_name, &str_child)))
      {
      handler_name[REG_NAME_MAX] = 0;
      char func_name[REG_STRING_MAX + 1];

      if (failed(reg_get_string(str_child, handler_name, func_name, 0)) ||
        func_name[0] == 0)
        {
        trace_error("Ion event handler %s does not have a function", handler_name);
        continue;
        }
      func_name[REG_STRING_MAX] = 0;

      // firstly we try to load the handler.
      // TODO: js errors to trace log?????
      struct _ion_context_t *ctx;
      if (failed(result = ion_create_worker(ion_home, handler_name, 0, 0, 0, true, BELOW_NORMAL, &ctx)))
        {
        trace_error("Cannot create ion worker for script %s : %d", handler_name, result);
        continue;
        }

      // we have the handler, now we see if the map holds a key to the value
      handle_t event_handlers;
      if (failed(map_find(handlers, (void *)event_id, &event_handlers)))
        {
        // create a vector to hold the handlers for this id
        if (failed(result = vector_create(sizeof(handler_t), &event_handlers)))
          {
          trace_error("Cannot create script handlers");
          continue;
          }
        }
      handler_t the_handler;

      the_handler.ctx = ctx;

      // see if the string exists
      uint16_t idx;
      uint16_t len;
      char *atom = 0;
      vector_count(atoms, &len);
      for (idx = 0; idx < len; idx++)
        {
        vector_at(atoms, idx, &atom);
        if (strcmp(atom, func_name) == 0)
          break;

        atom = 0;
        }

      if (atom == 0)
        {
        atom = neutron_strdup(func_name);
        vector_append(atoms, 1, atom);
        }

      the_handler.handler = atom;
      vector_append(event_handlers, 1, &the_handler);
      }

    dt = field_key;
    }
  
  //
  if (failed(result = subscribe(&ion_hook)))
    {
    trace_error("Ion cannot install hook");
    return result;
    }

  while (true)
    {
    canmsg_t the_message;

    if (failed(pop_front(ion_queue, &the_message, INDEFINITE_WAIT)))
      continue;

    // see if there is a handler.
    handle_t event_handlers;
    if (failed(map_find(handlers, (void *)the_message.id, &event_handlers)))
      continue;

    // send each handler a message...
    uint16_t len= 0;
    uint16_t idx;
    handler_t the_handler;
    vector_count(event_handlers, &len);
    for (idx = 0; idx < len; idx++)
      {
      vector_at(event_handlers, idx, &the_handler);

      // execute the handler
      ion_queue_message(the_handler.ctx, the_handler.handler, &the_message);
      }
    }
  return s_ok;
  }

