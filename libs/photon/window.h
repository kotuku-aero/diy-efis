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

#ifndef __ion_proxy_h__
#define __ion_proxy_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

#include "photon.h"
#include "../neutron/bsp.h"

#define WINDOW_QUEUE_SIZE 128

struct _event_proxy_t;

// event handler callback
typedef result_t (*event_handler_fn)(handle_t hwnd, struct _event_proxy_t *proxy,  const canmsg_t *msg);

// this structure is used to hold a callback
typedef struct _event_proxy_t {
  uint16_t msg_id;                // message id that is handled
  event_handler_fn callback;      // callback to execute
  void *parg;                     // user defined argument
  struct _event_proxy_t *previous;  // chain of event handlers.  The callback can use this
  char func[1];                   // optional callback name, always at least 1 for null
  } event_proxy_t;

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
    // window procedure used to process messages.
    wndproc window_proc;
    // id of the window
    uint16_t id;
    // painting order
    uint8_t z_order;
    // the window stream that is stored for the window
    void *wnd_data;
    // Array of event handlers for a window.
    vector_p events;
  } window_t;

typedef struct _screen_t
  {
  window_t wnd;
  // the event queue for the screen.
  deque_p event_queue;
  // callback for CAN messages
  msg_hook_t msg_hook;
  // this holds the fonts
  vector_p fonts;
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
 * @function result_t add_event(uint16_t id, void *parg, const char *func, event_handler_fn callback, proxy_t *previous)
 * Hook an event and add a callback
 * @param hwnd    Window to hook
 * @param id      ID of the event to handle
 * @param parg    Optional argument to be passed to the callback
 * @param func    Optional function name to be passed to the callback
 * @param callback  Function to call when the event occurs
 * @return s_ok if the event is hooked.
 */
extern result_t add_event(handle_t hwnd, uint16_t id, void *parg, const char *func, event_handler_fn callback);
/**
 * @function result_t remove_event(uint16_t id, proxy_t *previous)
 * Remove an event handler
 * @param hwnd      Window to release
 * @param id        ID of the handler to remove
 * @param head_only True if remove only the first handler in the chain.
 * @return s_ok if the event is removed
 */
extern result_t remove_event(handle_t hwnd, uint16_t id, bool head_only);
/**
 * @function result_t find_event(uint16_t id, proxy_t *details)
 * Find an event handler given the requested id
 * @param hwnd      Window to lookup
 * @param id        ID of handler to find
 * @param details   Details of the handler
 * @return s_ok if the handler was found ok
 */
extern result_t find_event(handle_t hwnd, uint16_t id, event_proxy_t **details);

typedef struct _variant_t {
  field_datatype dt;
  union {
    bool v_bool;
    uint16_t v_uint16;
    int16_t v_int16;
    uint32_t v_uint32;
    int32_t v_int32;
    float v_float;
    xyz_t v_xyz;
    matrix_t v_matrix;
    lla_t v_lla;
    qtn_t v_qtn;
    const char *v_string;
  };
  } variant_t;

typedef void (*free_fn)(variant_t *value);

typedef result_t (*getter_fn)(handle_t hwnd, variant_t *value);
typedef result_t (*setter_fn)(handle_t hwnd, const variant_t *value);

/**
 * @function result_t add_property(handle_t hwnd, const char *property_name, void *parg, getter_fn getter, setter_fn setter)
 * As a widget is created it can have optional ECMA Scripts attached to the event subsystem.  To enable the
 * script to access the arguments on the widget that are built-in the function allows properties to be defined
 * @param hwnd    Opaque handle to the window
 * @param property_name   Name of the property.  Must conform to javascript property type
 * @param parg    Opaque pointer passed into the function
 * @param getter  Callback to get the value of a property
 * @param setter  Callback to set the value of a property
 * @return s_ok if the property added ok.
*/
extern result_t add_property(handle_t hwnd, const char *property_name, getter_fn getter, setter_fn setter, field_datatype dt);

extern result_t get_canvas(handle_t hwnd, canvas_t **canvas);

#ifdef __cplusplus
}
#endif

#endif