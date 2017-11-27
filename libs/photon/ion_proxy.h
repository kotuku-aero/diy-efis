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

#ifndef __ion_proxy_h__
#define __ion_proxy_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "../neutron/bsp.h"
#include "../ion/ion.h"

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif
#include <math.h>
#include <string.h>

#define WINDOW_QUEUE_SIZE 128

// event handler callback
typedef result_t (*event_handler_fn)(handle_t hwnd, void *parg, const char *func, const canmsg_t *msg);

// this structure is used to hold a callback
typedef struct _proxy_t {
  uint16_t msg_id;                // message id that is handled
  event_handler_fn callback;      // callback to execute
  const char *func;               // optional callback name
  void *parg;                     // user defined argument
  } proxy_t;

// shared structure for all canvas's
typedef struct _window_t
  {
    uint16_t version;
    // canvas that holds the window drawing surface
    canvas_t *canvas;
    // next canvas in the list. 0 == end
    struct _window_t *next;
    // previous canvas in the list. 0 == end
    struct _window_t *previous;
    // parent canvas, 0 = root canvas
    struct _window_t *parent;
    // first child of this window
    struct _window_t *child;
    // true if this window is invalidated and needs a redraw
    // note the invalidate function does not use the rect hint
    bool invalid;
    // dimensions of the canvas
    rect_t position;          // position of the canvas.  Relative to the parent
    // window procedure used to process messages.  If ion proxy attached
    // then is is replaced with the proxy code.
    wndproc window_proc;
    // this holds the user created wndproc
    wndproc user_wndproc;
    // id of the window
    uint16_t id;
    // painting order
    uint8_t z_order;
    // the window data that is stored for the window
    void *wnd_data;
    // Array of event handlers for a window.  The well-known ones are cached
    handle_t events;
    proxy_t *on_draw_background;   // called when a draw background event is received
    proxy_t *on_paint;             // called when a paint event occurs
  } window_t;

typedef struct _screen_t
  {
  window_t wnd;
  // the event queue for the screen.
  handle_t event_queue;
  // callback for CAN messages
  msg_hook_t msg_hook;
  // this holds the fonts
  handle_t fonts;
  // This is the script engine that is associated with the layout.. Each
  // scriptlet that is loaded has its own context so it can be released
  // separately
  ion_context_t *context;
  // array of scripts that are loaded
  handle_t scriptlets;
  } screen_t;

static inline result_t as_screen(handle_t handle, screen_t **screen)
  {
  if (handle == 0)
    return e_bad_handle;

  if (((window_t *)handle)->version != sizeof(screen_t))
    return e_bad_handle;

  if(screen != 0)
    *screen = (screen_t *)handle;

  return s_ok;
  }


static inline result_t as_window(handle_t handle, window_t **window)
  {
  if (handle == 0)
    return e_bad_handle;

  if(((window_t *)handle)->version != sizeof(window_t) &&
     ((window_t *)handle)->version != sizeof(screen_t))
    return e_bad_handle;

  if(window != 0)
    *window = (window_t *)handle;

  return s_ok;
  }

/**
 * @function result_t attach_proxy(handle_t handle)
 * set up the event handlers for the passed in window or screen
 * @return s_ok if a valid handle.
*/
extern result_t attach_proxy(handle_t handle);
/**
 * @function result_t release_proxy(handle_t handle)
 * release all handlers for a proxy
 * @return s_ok if all proxy's are released.
*/
extern result_t release_proxy(handle_t handle);
/**
 * @function result_t add_event(uint16_t id, void *parg, const char *func, event_handler_fn callback, proxy_t *previous)
 * Hook an event and add a callback
 * @param hwnd    Window to hook
 * @param id      ID of the event to handle
 * @param parg    Optional argument to be passed to the callback
 * @param func    Optional function name to be passed to the callback
 * @param callback  Function to call when the event occurs
 * @param previous  Optional structure to receive previous callback (if defined).  If not provided then the previous callback
 * is erased.
 * @return s_ok if the event is hooked.
 */
extern result_t add_event(handle_t hwnd, uint16_t id, void *parg, const char *func, event_handler_fn callback, proxy_t *previous);
/**
 * @function result_t remove_event(uint16_t id, proxy_t *previous)
 * Remove an event handler
 * @param hwnd      Window to release
 * @param id        ID of the handler to remove
 * @param previous  If provided, the event handler will be replaced with the previous handler
 * @return s_ok if the event is removed
 */
extern result_t remove_event(handle_t hwnd, uint16_t id, proxy_t *previous);
/**
 * @function result_t find_event(uint16_t id, proxy_t *details)
 * Find an event handler given the requested id
 * @param hwnd      Window to lookup
 * @param id        ID of handler to find
 * @param details   Details of the handler
 * @return s_ok if the handler was found ok
 */
extern result_t find_event(handle_t hwnd, uint16_t id, proxy_t *details);

#ifdef __cplusplus
}
#endif

#endif