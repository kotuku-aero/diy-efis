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
#include "ion_proxy.h"

result_t attach_ion(handle_t hndl, memid_t key, const char *startup_script)
  {
  result_t result;
  screen_t *screen;

  if(failed(result = as_screen(hndl, &screen)))
    return result;

  if(failed(result = ion_create(key, startup_script, 0, 0, 0, &screen->context)))
    return result;

  return s_ok;
  }

result_t detach_ion(handle_t hndl)
  {

  }

result_t attach_proxy(handle_t handle)
  {
  result_t result;
  window_t *window;
  if(failed(result = as_window(handle, &window)))
    return result;

  // we work over the keys now

  return s_ok;
  }

result_t release_proxy(handle_t handle)
  {

  }

result_t add_event(handle_t hwnd, uint16_t id, void *parg, const char *func, event_handler_fn callback)
  {
  result_t result;
  event_proxy_t *previous = 0;
  window_t *window;
  if(failed(result = as_window(hwnd, &window)))
    return result;

  uint16_t count;
  if(failed(result = vector_count(window->events, &count)))
    return result;

  uint16_t index;
  for (index = 0; index < count; index++)
    {
    if(failed(vector_at(window->events, index, &previous)))
      return result;

    if(previous->msg_id == id)
      break;

    previous = 0;
    }

  event_proxy_t *proxy = (event_proxy_t *)neutron_malloc(sizeof(event_proxy_t));
  proxy->callback = callback;
  proxy->func = func;
  proxy->parg = parg;
  proxy->msg_id = id;
  proxy->previous = previous;

  if(previous != 0)
    {
    if (failed(result = vector_set(window->events, index, proxy)))
      {
      neutron_free(proxy);
      return result;
      }
    }

  return vector_push_back(window->events, proxy);
  }

result_t remove_event(handle_t hwnd, uint16_t id, bool head_only)
  {
  result_t result;
  event_proxy_t *proxy = 0;
  window_t *window;
  if (failed(result = as_window(hwnd, &window)))
    return result;

  uint16_t count;
  if (failed(result = vector_count(window->events, &count)))
    return result;

  uint16_t index;
  for (index = 0; index < count; index++)
    {
    if (failed(vector_at(window->events, index, &proxy)))
      return result;

    if (proxy->msg_id == id)
      break;

    proxy = 0;
    }

  if(proxy == 0)
    return e_not_found;

  event_proxy_t *next = proxy->previous;
  if (head_only)
    {
    if(next != 0)
      result = vector_set(window->events, index, next);
    else
      result = vector_erase(window->events, index);

    neutron_free(proxy);
    return result;
    }

  // erase the id
  result = vector_erase(window->events, index);
  while (next != 0)
    {
    neutron_free(proxy);
    proxy = next;
    next = proxy->previous;
    }

  return s_ok;
  }

result_t find_event(handle_t hwnd, uint16_t id, event_proxy_t **details)
  {
  result_t result;
  window_t *window;
  if (failed(result = as_window(hwnd, &window)))
    return result;

  uint16_t count;
  if (failed(result = vector_count(window->events, &count)))
    return result;

  *details = 0;
  uint16_t index;
  for (index = 0; index < count; index++)
    {
    if (failed(vector_at(window->events, index, details)))
      return result;

    if ((*details)->msg_id == id)
      break;

    *details = 0;
    }

  return *details == 0 ? e_not_found : s_ok;
  }

static result_t ion_handler(handle_t hwnd, const event_proxy_t *proxy, const canmsg_t *msg)
  {
  result_t result;
  handle_t hscreen;
  screen_t *screen;
  if (failed(result = get_screen(&hscreen)) ||
    failed(result = as_screen(hscreen, &screen)))
    return result;

  ion_context_t *context = (ion_context_t *)screen->context;

  // get the function
  if(!duk_get_global_string(context->ctx, proxy->func))
    return e_not_found;

  duk_push_pointer(context->ctx, msg);
  duk_int_t rc = duk_pcall(context->ctx, 1);
  if (rc == DUK_EXEC_SUCCESS)
    {
    result = duk_get_int(context->ctx, -1);
    }
  else
    {
    result = e_unexpected;
    }
  duk_pop(context->ctx);

  if (result == s_false && proxy->previous != 0)
    {
    // was not processed so call the previous event
    return (*proxy->previous->callback)(hwnd, proxy->previous, msg);
    }

  return result;
  }

result_t add_handler(handle_t hwnd, uint16_t id, const char *func)
  {
  result_t result;
  handle_t hscreen;
  screen_t *screen;
  if(failed(result = get_screen(&hscreen)) ||
     failed(result = as_screen(hscreen, &screen)))
    return result;

  // link in a script handler
  return add_event(hwnd, id, screen->context, func, ion_handler);
  }

result_t remove_handler(handle_t hwnd, uint16_t id)
  {
  result_t result;
  // we need to check to see if the head of the chain is ourselves.  If not then we
  // do not remove the script handler.
  event_proxy_t the_proxy;
  if(failed(result = find_event(hwnd, id, &the_proxy)))
    return result;

  if(the_proxy.callback != ion_handler)
    return e_not_found;

  return remove_event(hwnd, id, true);
  }
