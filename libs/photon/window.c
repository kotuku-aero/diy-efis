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
#include "window.h"

result_t add_event(handle_t hwnd, uint16_t id, void *parg, event_handler_fn callback)
  {
  result_t result;
  event_proxy_t *previous = 0;
  uint16_t index = 0;
  window_t *window;
  if(failed(result = as_window(hwnd, &window)))
    return result;

  // if our window has no events attached create a container
  if(window->events == 0)
    {
    if(failed(result = vector_create(sizeof(event_proxy_t *), &window->events)))
      return result;
    }
  else
    {
    uint16_t count;
    if(failed(result = vector_count(window->events, &count)))
      return result;

    for (index = 0; index < count; index++)
      {
      if(failed(vector_at(window->events, index, &previous)))
        return result;

      if(previous->msg_id == id)
        break;

      previous = 0;
      }
    }

  event_proxy_t *proxy = (event_proxy_t *)neutron_malloc(sizeof(event_proxy_t));
  memset(proxy, 0, sizeof(event_proxy_t));
  proxy->callback = callback;
  proxy->parg = parg;
  proxy->msg_id = id;
  proxy->previous = previous;

  if(previous != 0)
    {
    if (failed(result = vector_set(window->events, index, &proxy)))
      {
      neutron_free(proxy);
      return result;
      }
    }

  return vector_push_back(window->events, &proxy);
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
      result = vector_set(window->events, index, &next);
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

result_t on_paint(handle_t hwnd, struct _event_proxy_t *proxy, const canmsg_t *msg)
  {
  result_t result;
  
  // we assume the widget has painted its canvas, we work over our children
  // in z-order
  uint16_t painting_order = 0;
  uint16_t next_z_order = 0;
  uint16_t max_order = 0;
  handle_t child;

  uint8_t z_order;
  if (failed(result = get_z_order(hwnd, &z_order)))
    return result;

  max_order = z_order;
  painting_order = z_order;

  begin_paint(hwnd);

  do
    {
    for (get_first_child(hwnd, &child); child != 0; get_next_sibling(child, &child))
      {
      get_z_order(child, &z_order);

      if (z_order > painting_order)
        {
        // get the next highest order after our own
        if (z_order < next_z_order || next_z_order == painting_order)
          next_z_order = z_order;

        // figure out what the last one to do is.
        if (z_order > max_order)
          max_order = z_order;
        }

      if (z_order == painting_order)
        send_message(child, msg);         // is ok to paint
      }

    if (painting_order >= max_order)
      break;

    painting_order = next_z_order;        // lowest next paint

    } while (true);

  end_paint(hwnd);
  return s_ok;      // processed
  }

result_t defwndproc(handle_t hwnd, const canmsg_t *msg)
  {
  uint16_t count;
  uint16_t item;
  window_t *window;
  if (succeeded(as_window(hwnd, &window)))
    {
    if (window->events != 0)
      {
      vector_count(window->events, &count);
      event_proxy_t *proxy;
      for (item = 0; item < count; item++)
        {
        vector_at(window->events, item, &proxy);
        if(proxy != 0 && proxy->msg_id == get_can_id(msg))
          {
          // special case for a paint event
          if(get_can_id(msg) == id_paint)
            {
            if(window->invalid)
              {
              (*proxy->callback)(hwnd, proxy, msg);
              window->invalid = false;
              }
            }
          else
            (*proxy->callback)(hwnd, proxy, msg);
          }
        }
      }
    }

  // paint is handled with z-order not child order
  if (get_can_id(msg) == id_paint)
    return s_ok;

  handle_t child;
  for (get_first_child(hwnd, &child); child != 0; get_next_sibling(child, &child))
    {
    // send the message to the children of this window
    send_message(child, msg);
    }
  return s_ok;
  }

static result_t make_window(handle_t hwnd_parent, const rect_t *bounds, wndproc cb,
  uint16_t id, canvas_t *canvas, handle_t *hwnd)
  {
  result_t result;
  window_t *wnd;

  window_t *parent;
  if (failed(result = as_window(hwnd_parent, &parent)))
    return result;

  wnd = (window_t *)neutron_malloc(sizeof(window_t));
  memset(wnd, 0, sizeof(window_t));

  wnd->parent = parent;
  wnd->version = sizeof(window_t);
  copy_rect(bounds, &wnd->position);
  wnd->canvas = canvas;
  wnd->window_proc = cb;
  wnd->id = id;
  wnd->invalid = true;

  // attach the default event handlers to the window
  add_event(hwnd, id_paint, wnd, 0, on_paint);

  *hwnd = wnd;

  if (parent->child != 0 && failed(result = insert_before(wnd, parent->child)))
    return result;

  // first child.
  parent->child = wnd;
  return s_ok;
  }

static canmsg_t close_msg = {
  .flags = id_close
  };

result_t close_window(handle_t hwnd)
  {
  result_t result;
  
  window_t *window;
  if (failed(result = as_window(hwnd, &window)))
    return result;

  /*
  if (failed(detach_ion_from_window(hwnd)))
    return result;
   * */

  // send an id_close to the window so it can remove all of the
  // data from the window
  if (failed(result = send_message(hwnd, &close_msg)))
    return result;

  if (window->parent->child == window)
    {
    if (window->previous != 0)
      window->parent->child = window->previous;
    else if (window->next != 0)
      window->parent->child = window->next;
    else
      window->parent->child = 0;
    }

  if (window->previous != 0)
    window->previous->next = window->next;

  if (window->next != 0)
    window->next->previous = window->previous;

  canvas_t *canvas = window->canvas;

  neutron_free(window);

  // and delete the canvas
  canvas_close(canvas);

  return s_ok;
  }

result_t create_window(handle_t hwnd_parent, const rect_t *bounds, wndproc cb, uint16_t id, handle_t *hwnd)
  {
  result_t result;
  canvas_t *canvas;

  if (hwnd_parent == 0 || bounds == 0 || cb == 0 || hwnd == 0)
    return e_bad_parameter;

  extent_t size =
    {
    .dx = rect_width(bounds),.dy = rect_height(bounds)
    };

  if (failed(result = bsp_canvas_create_rect(&size, &canvas)))
    return result;

  if (failed(result = make_window(hwnd_parent, bounds, cb, id, canvas, hwnd)))
    return result;

  return s_ok; // attach_ion_to_window(0, 0, *hwnd);
  }

result_t create_child_window(handle_t hwnd_parent,
  const rect_t *bounds,
  wndproc cb, uint16_t id,
  handle_t *hwnd)
  {
  result_t result;
  canvas_t *canvas;

  if (hwnd_parent == 0 || bounds == 0 || cb == 0 || hwnd == 0)
    return e_bad_parameter;

  window_t *parent;
  if (failed(result = as_window(hwnd_parent, &parent)))
    return result;

  canvas_t *parent_canvas;

  if (failed(result = get_canvas(hwnd_parent, &parent_canvas)))
    return result;

  if (failed(result = bsp_canvas_create_child(parent_canvas, bounds, &canvas)))
    return result;

  if (failed(result = make_window(hwnd_parent, bounds, cb, id, canvas, hwnd)))
    return result;

  return s_ok; // attach_ion_to_window(prototype, key, *hwnd);
  }

result_t get_parent(handle_t window, handle_t *parent)
  {
  result_t result;
  if (parent == 0)
    return e_bad_parameter;

  window_t *wnd;
  if (failed(result = as_window(window, &wnd)))
    return result;

  *parent = wnd->parent;

  return s_ok;
  }

result_t get_window_by_id(handle_t parent, uint16_t id, handle_t *child)
  {
  result_t result;
  if (child == 0)
    return e_bad_parameter;

  window_t *wnd;
  if (failed(result = as_window(parent, &wnd)))
    return result;


  // see if the child is the parent!
  if (wnd->id == id)
    {
    *child = parent;

    return s_ok;
    }

  handle_t hwnd_child;
  if (failed(result = get_first_child(parent, &hwnd_child)) ||
    failed(as_window(hwnd_child, &wnd)))
    return result;        // possibly e_not_found

  do
    {
    wnd = (window_t *)hwnd_child;
    if (wnd->id == id)
      {
      *child = hwnd_child;
      return s_ok;
      }

    // check the children of this window.
    if (succeeded(result = get_window_by_id(hwnd_child, id, child)))
      return result;

    if (failed(result = get_next_sibling(hwnd_child, &hwnd_child)))
      return result;

    } while (hwnd_child != 0);

    return e_not_found;
  }

result_t get_first_child(handle_t parent, handle_t *child)
  {
  result_t result;
  if (child == 0)
    return e_bad_parameter;

  *child = 0;

  window_t *wnd;
  if (failed(result = as_window(parent, &wnd)))
    return result;


  *child = wnd->child;

  if (wnd->child == 0)
    return e_not_found;
  return s_ok;
  }

result_t get_next_sibling(handle_t wnd, handle_t *sibling)
  {
  result_t result;
  if (sibling == 0)
    return e_bad_parameter;

  *sibling = 0;

  window_t *window;
  if (failed(result = as_window(wnd, &window)))
    return result;


  *sibling = window->next;

  if (window->next == 0)
    return e_not_found;

  return s_ok;
  }

result_t get_previous_sibling(handle_t wnd, handle_t *sibling)
  {
  result_t result;
  if (sibling == 0)
    return e_bad_parameter;

  *sibling = 0;

  window_t *window;
  if (failed(result = as_window(wnd, &window)))
    return result;

  *sibling = window->previous;

  if (window->next == 0)
    return e_not_found;

  return s_ok;
  }

result_t insert_before(handle_t wnd, handle_t sibling)
  {
  result_t result;
  window_t *next;
  window_t *child;
  if (failed(result = as_window(wnd, &child)) || failed(result = as_window(sibling, &next)))
    return result;

  child->previous = next->previous;

  if (next->previous != 0)
    next->previous->next = child;

  next->previous = child;

  child->next = next;

  return s_ok;
  }

result_t insert_after(handle_t wnd, handle_t sibling)
  {
  result_t result;
  window_t *previous;
  window_t *child;
  if (failed(result = as_window(wnd, &child)) || failed(result = as_window(sibling, &previous)))
    return result;

  child->next = previous->next;

  if (previous->next != 0)
    previous->next->previous = child;

  previous->next = child;
  child->previous = previous;

  return s_ok;
  }

result_t get_window_rect(handle_t hwnd, rect_t *rect)
  {
  result_t result;

  window_t *window;
  if (failed(result = as_window(hwnd, &window)))
    return result;

  rect->left = 0;
  rect->top = 0;
  rect->right = rect_width(&window->position);
  rect->bottom = rect_height(&window->position);

  return s_ok;
  }

result_t get_window_pos(handle_t hwnd, rect_t *rect)
  {
  result_t result;

  window_t *window;
  if (failed(result = as_window(hwnd, &window)))
    return result;

  memcpy(rect, &window->position, sizeof(rect_t));

  return s_ok;
  }

result_t set_window_pos(handle_t hwnd, rect_t *rect)
  {
  result_t result;

  window_t *window;
  if (failed(result = as_window(hwnd, &window)))
    return result;

  // check that the rect fits on the canvas
  if (rect->left >= window->canvas->width ||
     rect->left < 0 ||
     rect->right > window->canvas->width ||
    rect->right < 0 ||
    rect->right <= rect->left)
    return e_bad_parameter;

  if (rect->top >= window->canvas->height ||
    rect->top < 0 ||
    rect->bottom > window->canvas->height ||
    rect->bottom < 0 ||
    rect->bottom <= rect->top)
    return e_bad_parameter;

  memcpy(&window->position, rect, sizeof(rect_t));

  return s_ok;
  }

result_t invalidate_rect(handle_t hwnd, const rect_t *rect)
  {
  result_t result;
  window_t *wnd;

  if (failed(result = as_window(hwnd, &wnd)))
    return result;

  // see if the canvas is a parent draw canvas
  canvas_t *canvas;
  if (failed(result = get_canvas(hwnd, &canvas)))
    return result;

  wnd->invalid = true;

  // now we need to invalidate our children
  handle_t child = 0;
  if(succeeded(get_first_child(hwnd, &child)))
    while (child != 0)
      {
      if(failed(is_invalid(child)))
        invalidate_rect(child, 0);      // don't yet clip the rect.  TODO

      if (failed(get_next_sibling(child, &child)))
        break;

      }

  // if this canvas is not our own we need to tell the owner about this
  if (!canvas->own_buffer)
    {
    // keep going up the tree
    window_t *parent = wnd->parent;
    if (parent == 0)
      return e_unexpected;

    rect_t rect_p;
    rect_p.left = canvas->offset.x + ((rect != 0) ? rect->left : 0);
    rect_p.top = canvas->offset.y + ((rect != 0) ? rect->top : 0);
    rect_p.right = canvas->offset.x + ((rect != 0) ? rect->right : canvas->width);
    rect_p.bottom = canvas->offset.y + ((rect != 0) ? rect->bottom : canvas->height);

    // ask the parent to redraw, it will then pass the message down
    // based on the correct z-order
    return invalidate_rect(parent, &rect_p);
    }

  return s_ok;
  }

result_t get_z_order(handle_t hwnd, uint8_t *z_order)
  {
  result_t result;
  if (hwnd == 0 || z_order == 0)
    return e_bad_parameter;

  window_t *wnd;
  if (failed(result = as_window(hwnd, &wnd)))
    return result;


  *z_order = wnd->z_order;

  return s_ok;
  }

result_t set_z_order(handle_t hwnd, uint8_t z_order)
  {
  result_t result;

  window_t *wnd;
  if (failed(result = as_window(hwnd, &wnd)))
    return result;


  wnd->z_order = z_order;

  return s_ok;
  }

