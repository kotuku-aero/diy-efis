#include "photon_priv.h"

const canmsg_t paint_background_msg = { id_paint_background };
const canmsg_t paint_foreground_msg = { id_paint_foreground };
const canmsg_t paint_overlay_msg = { id_paint_overlay };

const typeid_t window_type;

static result_t window_close(handle_t hndl)
  {
  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hndl, &window_type, (void**)&window)))
    return result;

  if (window->parent->child == window)
    window->parent->child = window->next;

  if (window->previous != 0)
    window->previous->next = window->next;

  if (window->next != 0)
    window->next->previous = window->previous;

  // TODO: cleaup wnd data ?? need marshalling callback!
  window_t* child = window->child;
  while (child != 0)
    {
    window_t* next = child->next;
    // release all children...
    if (failed(result = window_close((handle_t)child)))
      return result;

    child = next;
    }

  if (failed(result = close_handle(window->background_canvas)) ||
    failed(result = close_handle(window->foreground_canvas)) ||
    failed(result = close_handle(window->overlay_canvas)))
    return result;

  memset(window, 0, sizeof(window_t));
  return neutron_free(window);
  }

const typeid_t window_type =
  {
  .name = "window",
  .etherealize = window_close
  };

result_t window_create(handle_t hparent, const rect_t* bounds, wndproc_fn cb, void* wnddata, uint16_t id, handle_t* hndl)
  {
  if (bounds == 0 || hndl == 0)
    return e_bad_pointer;

  result_t result;
  window_t* parent = 0;
  if (failed(result = is_typeof(hparent, &window_type, (void**)&parent)))
    return result;

  window_t* window;
  if (failed(result = neutron_malloc(sizeof(window_t), (void**)&window)))
    return result;

  memset(window, 0, sizeof(window_t));

  window->base.type = &window_type;
  window->id = id;
  window->wndproc = cb;
  window->parent = parent;
  window->wnddata = wnddata;
  window->visible = true;

  rect_copy(bounds, &window->position);

  framebuffer_t* fb;
  canvas_t* canvas;
  // create the canvas's
  if (parent->background_canvas != 0 &&
    (failed(result = is_typeof(parent->background_canvas, &canvas_type, (void**)&canvas)) ||
      failed(result = bsp_framebuffer_create_child(canvas->fb, bounds, &fb)) ||
      failed(result = create_canvas_from_framebuffer(fb, &window->background_canvas))))
    return result;

  if (parent->foreground_canvas != 0 &&
    (failed(result = is_typeof(parent->foreground_canvas, &canvas_type, (void**)&canvas)) ||
      failed(result = bsp_framebuffer_create_child(canvas->fb, bounds, &fb)) ||
      failed(result = create_canvas_from_framebuffer(fb, &window->foreground_canvas))))
    return result;

  if (parent->overlay_canvas != 0 &&
    (failed(result = is_typeof(parent->overlay_canvas, &canvas_type, (void**)&canvas)) ||
      failed(result = bsp_framebuffer_create_child(canvas->fb, bounds, &fb)) ||
      failed(result = create_canvas_from_framebuffer(fb, &window->overlay_canvas))))
    return result;


  // link window
  if (parent != 0)
    {
    if (parent->child != 0)
      parent->child->previous = window;

    window->next = parent->child;
    parent->child = window;
    }

  if (hndl != 0)
    *hndl = (handle_t)window;

  return s_ok;
  }

result_t get_window_by_id(handle_t hndl, uint16_t id, handle_t* hchild)
  {
  if (hchild == 0)
    return e_bad_pointer;

  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hndl, &window_type, (void**)&window)))
    return result;

  if (id == window->id)
    {
    *hchild = hndl;
    return s_ok;
    }

  for (window_t* child = window->child; child != 0; child = child->next)
    {
    if (child->id == id)
      {
      *hchild = (handle_t)child;
      return s_ok;
      }
    }

  return e_not_found;
  }

result_t get_z_order(handle_t hndl, uint8_t* value)
  {
  if (value == 0)
    return e_bad_pointer;

  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hndl, &window_type, (void**)&window)))
    return result;

  *value = window->z_order;

  return s_ok;
  }

result_t get_first_child(handle_t hndl, handle_t* value)
  {
  if (value == 0)
    return e_bad_pointer;

  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hndl, &window_type, (void**)&window)))
    return result;

  *value = (handle_t)window->child;

  return s_ok;
  }

result_t get_next_sibling(handle_t hndl, handle_t* value)
  {
  if (value == 0)
    return e_bad_pointer;

  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hndl, &window_type, (void**)&window)))
    return result;

  *value = (handle_t)window->next;

  return s_ok;
  }

result_t get_previous_sibling(handle_t hndl, handle_t* value)
  {
  if (value == 0)
    return e_bad_pointer;

  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hndl, &window_type, (void**)&window)))
    return result;

  *value = (handle_t)window->previous;

  return s_ok;
  }

result_t set_z_order(handle_t hndl, uint8_t value)
  {
  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hndl, &window_type, (void**)&window)))
    return result;

  window->z_order = value;

  return s_ok;
  }

result_t set_wnddata(handle_t hndl, void* wnddata)
  {
  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hndl, &window_type, (void**)&window)))
    return result;

  window->wnddata = wnddata;
  return s_ok;
  }

result_t get_wnddata(handle_t hndl, void** wnddata)
  {
  if (wnddata == 0)
    return e_bad_parameter;

  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hndl, &window_type, (void**)&window)))
    return result;

  *wnddata = window->wnddata;
  return s_ok;
  }

result_t get_wndproc(handle_t hndl, wndproc_fn* wndproc, void** wnddata)
  {
  if (wnddata == 0 || wndproc == 0)
    return e_bad_parameter;

  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hndl, &window_type, (void**)&window)))
    return result;

  *wnddata = window->wnddata;
  *wndproc = window->wndproc;

  return s_ok;
  }

result_t get_window_id(handle_t hndl, uint16_t* id)
  {
  if (id == 0)
    return e_bad_parameter;

  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hndl, &window_type, (void**)&window)))
    return result;

  *id = window->id;
  return s_ok;
  }

result_t set_window_id(handle_t hndl, uint16_t id)
  {
  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hndl, &window_type, (void**)&window)))
    return result;

  window->id = id;
  return s_ok;
  }

result_t show_window(handle_t hndl)
  {
  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hndl, &window_type, (void**)&window)))
    return result;

  window->visible = true;
  return s_ok;
  }

result_t hide_window(handle_t hndl)
  {
  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hndl, &window_type, (void**)&window)))
    return result;

  window->visible = false;
  return s_ok;
  }

result_t is_visible(handle_t hndl)
  {
  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hndl, &window_type, (void**)&window)))
    return result;

  return window->visible ? s_ok : s_false;
  }

result_t window_getpos(handle_t hndl, rect_t* pos)
  {
  if (pos == 0)
    return e_bad_parameter;

  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hndl, &window_type, (void**)&window)))
    return result;

  rect_copy(&window->position, pos);
  return s_ok;
  }

result_t window_setpos(handle_t hndl, const rect_t* pos)
  {
  if (pos == 0)
    return e_bad_parameter;

  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hndl, &window_type, (void**)&window)))
    return result;

  rect_copy(pos, &window->position);

  canvas_t* canvas;
  if (window->background_canvas != 0)
    {
    if (failed(result = is_typeof(window->background_canvas, &canvas_type, (void**)&canvas)))
      return result;
    rect_copy(pos, &canvas->fb->position);
    }

  if (window->foreground_canvas != 0)
    {
    if (failed(result = is_typeof(window->foreground_canvas, &canvas_type, (void**)&canvas)))
      return result;

    rect_copy(pos, &canvas->fb->position);
    }

  if (window->overlay_canvas != 0)
    {
    if (failed(result = is_typeof(window->overlay_canvas, &canvas_type, (void**)&canvas)))
      return result;

    rect_copy(pos, &canvas->fb->position);
    }

  return s_ok;
  }

extern result_t window_rect(handle_t hndl, rect_t* rect)
  {
  if (rect == 0)
    return e_bad_parameter;

  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hndl, &window_type, (void**)&window)))
    return result;

  rect->left = 0;
  rect->top = 0;
  rect->right = rect_width(&window->position);
  rect->bottom = rect_height(&window->position);

  return s_ok;
  }

result_t window_parent(handle_t hndl, handle_t* parent)
  {
  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hndl, &window_type, (void**)&window)))
    return result;

  *parent = window->parent;

  return s_ok;
  }

result_t insert_before(handle_t hndl, handle_t hsibling)
  {
  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hndl, &window_type, (void**)&window)))
    return result;

  window_t* sibling;
  if (failed(result = is_typeof(hsibling, &window_type, (void**)&sibling)))
    return result;

  // root window cannot have siblings
  if (window->parent == 0 || sibling->parent != window->parent)
    return e_bad_parameter;

  // first unlink this
  if (window->previous != 0 && window->next != 0)
    {
    // unlink us
    window->next->previous = window->previous;
    window->previous->next = window->next;
    }
  else if (window->previous != 0 && window->next == 0)
    {
    window->previous->next = 0;
    }
  else if (window->previous == 0 && window->next != 0)
    {
    if (window->parent != 0)
      window->parent->child = window->next;
    window->next->previous = 0;
    }

  // then link
  if (sibling->previous == 0)
    {
    window->parent->child = window;
    sibling->previous = window;
    window->previous = 0;
    }
  else
    {
    window->previous = sibling->previous;
    window->next = sibling;
    window->previous->next = window;
    sibling->previous = window;
    }

  return s_ok;
  }

result_t insert_after(handle_t hndl, handle_t hsibling)
  {
  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hndl, &window_type, (void**)&window)))
    return result;

  window_t* sibling;
  if (failed(result = is_typeof(hsibling, &window_type, (void**)&sibling)))
    return result;

  // root window cannot have siblings
  if (window->parent == 0 || sibling->parent != window->parent)
    return e_bad_parameter;

  // unlink this
  if (window->previous != 0 && window->next != 0)
    {
    // unlink us
    window->next->previous = window->previous;
    window->previous->next = window->next;
    }
  else if (window->previous != 0 && window->next == 0)
    {
    window->previous->next = 0;
    }
  else if (window->previous == 0 && window->next != 0)
    {
    if (window->parent != 0)
      window->parent->child = window->next;
    window->next->previous = 0;
    }

  // and link
  window->next = sibling->next;
  window->previous = sibling;
  sibling->next = window;
  if (window->next != 0)
    window->next->previous = window;

  return s_ok;
  }

result_t get_background_canvas(handle_t hwnd, handle_t* canvas)
  {
  window_t* window;
  result_t result;
  if (failed(result = is_typeof(hwnd, &window_type, (void**)&window)))
    return result;

  *canvas = window->background_canvas;

  return s_ok;
  }

result_t get_foreground_canvas(handle_t hwnd, handle_t* canvas)
  {
  window_t* window;
  result_t result;
  if (failed(result = is_typeof(hwnd, &window_type, (void**)&window)))
    return result;

  *canvas = window->foreground_canvas;

  return s_ok;
  }

result_t get_overlay_canvas(handle_t hwnd, handle_t* canvas)
  {
  window_t* window;
  result_t result;
  if (failed(result = is_typeof(hwnd, &window_type, (void**)&window)))
    return result;

  *canvas = window->overlay_canvas;

  return s_ok;
  }


result_t is_invalid(handle_t hndl)
  {
  result_t result;
  canvas_t* canvas;
  if (failed(result = is_typeof(hndl, &canvas_type, (void**)&canvas)))
    return result;

  return canvas->fb->invalid ? s_ok : s_false;
  }

result_t invalidate_background_rect(handle_t hndl, const rect_t* rect)
  {
  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hndl, &window_type, (void**)&window)))
    return result;

  if (window->background_canvas == 0)
    return e_invalid_operation;

  // if this canvas is not our own we need to tell the owner about this
  // but this may recurse, hence the _invalid = true being set
  canvas_t* canvas;
  if (failed(result = is_typeof(window->background_canvas, &canvas_type, (void**)&canvas)))
    return result;

  //if (succeeded(window_is_invalid(canvas->fb)))
  //  return s_ok;

  window_mark_invalid(canvas->fb);

  rect_t wnd_rect;

  // now we need to invalidate our children
  window_t* child = window->child;

  while (child != 0)
    {
    handle_t child_canvas;
    if (succeeded(get_background_canvas(child, &child_canvas)) &&
      failed(is_invalid(child_canvas)))
      {
      if (failed(result = window_rect((handle_t)child, &wnd_rect)) ||
        failed(result = invalidate_background_rect((handle_t)child, &wnd_rect)))
        return result;
      }

    child = child->next;
    }

  if (!canvas->fb->is_surface)
    {
    // ask the parent to redraw, it will then pass the message down
    // based on the correct z-order
    if (failed(result = window_rect((handle_t)window->parent, &wnd_rect)) ||
      failed(result = invalidate_background_rect((handle_t)window->parent, &wnd_rect)))
      return result;
    }

  // post a paint message to the window.  This actually just sets the
  // paint request, but this is the only way to send a message
  // between threads.
  return post_message(hndl, &paint_background_msg, 0);
  }

result_t begin_background_paint(handle_t hwnd, handle_t* paint_canvas)
  {
  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hwnd, &window_type, (void**)&window)))
    return result;

  if (window->background_canvas == 0)
    return e_invalid_operation;

  if (paint_canvas != 0)
    *paint_canvas = window->background_canvas;

  // if this canvas is not our own we need to tell the owner about this
  // but this may recurse, hence the _invalid = true being set

  canvas_t* canvas;
  do
    {
    if (failed(result = is_typeof(window->background_canvas, &canvas_type, (void**)&canvas)))
      return result;

    if (canvas->fb->is_surface)
      break;

    window = window->parent;
    } while (window != 0);

    if (window == 0)
      return s_ok;

    return window_begin_paint(canvas->fb);
  }

result_t end_background_paint(handle_t hwnd)
  {
  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hwnd, &window_type, (void**)&window)))
    return result;

  if (window->background_canvas == 0)
    return e_invalid_operation;

  // if this canvas is not our own we need to tell the owner about this
  // but this may recurse, hence the _invalid = true being set

  int depth = 0;
  canvas_t* canvas;
  do
    {
    if (failed(result = is_typeof(window->background_canvas, &canvas_type, (void**)&canvas)))
      return result;

    if (canvas->fb->is_surface)
      break;

    if(depth++ == 0)
      canvas->fb->invalid = false;
      
    window = window->parent;
    } while (window != 0);

    if (window == 0)
      return s_ok;

  return window_end_paint(canvas->fb);
  }

result_t invalidate_foreground_rect(handle_t hndl, const rect_t* rect)
  {
  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hndl, &window_type, (void**)&window)))
    return result;

  if (window->foreground_canvas == 0)
    return e_invalid_operation;

  // if this canvas is not our own we need to tell the owner about this
  // but this may recurse, hence the _invalid = true being set
  canvas_t* canvas;
  if (failed(result = is_typeof(window->foreground_canvas, &canvas_type, (void**)&canvas)))
    return result;

  // if already invalid then exit
  //if (succeeded(window_is_invalid(canvas->fb)))
  //  return s_ok;

  window_mark_invalid(canvas->fb);

  rect_t wnd_rect;

  // now we need to invalidate our children
  window_t* child = window->child;

  while (child != 0)
    {
    handle_t child_canvas;
    if (succeeded(get_foreground_canvas(child, &child_canvas)) &&
      failed(is_invalid(child_canvas)))
      {
      if (failed(result = window_rect((handle_t)child, &wnd_rect)) ||
        failed(result = invalidate_foreground_rect((handle_t)child, &wnd_rect)))
        return result;
      }

    child = child->next;
    }

  if (!canvas->fb->is_surface)
    {
    // ask the parent to redraw, it will then pass the message down
    // based on the correct z-order
    if (failed(result = window_rect((handle_t)window->parent, &wnd_rect)) ||
      failed(result = invalidate_foreground_rect((handle_t)window->parent, &wnd_rect)))
      return result;
    }

  // post a paint message to the window.  This actually just sets the
  // paint request, but this is the only way to send a message
  // between threads.
  return post_message(hndl, &paint_foreground_msg, 0);
  }

result_t begin_foreground_paint(handle_t hwnd, handle_t* paint_canvas)
  {
  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hwnd, &window_type, (void**)&window)))
    return result;


  if (window->foreground_canvas == 0)
    return e_invalid_operation;

  if (paint_canvas != 0)
    *paint_canvas = window->foreground_canvas;

  // if this canvas is not our own we need to tell the owner about this
  // but this may recurse, hence the _invalid = true being set

  canvas_t* canvas;
  do
    {
    if (failed(result = is_typeof(window->foreground_canvas, &canvas_type, (void**)&canvas)))
      return result;

    if (canvas->fb->is_surface)
      break;

    window = window->parent;
    } while (window != 0);

    if (window == 0)
      return s_ok;

    return window_begin_paint(canvas->fb);
  }

result_t end_foreground_paint(handle_t hwnd)
  {
  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hwnd, &window_type, (void**)&window)))
    return result;


  if (window->foreground_canvas == 0)
    return e_invalid_operation;

  // if this canvas is not our own we need to tell the owner about this
  // but this may recurse, hence the _invalid = true being set

  int depth = 0;
  canvas_t* canvas;
  do
    {
    if (failed(result = is_typeof(window->foreground_canvas, &canvas_type, (void**)&canvas)))
      return result;

    if (canvas->fb->is_surface)
      break;

    if (depth++ == 0)
      canvas->fb->invalid = false;

    window = window->parent;
    } while (window != 0);

    if (window == 0)
      return s_ok;

    return window_end_paint(canvas->fb);
  }

result_t invalidate_overlay_rect(handle_t hndl, const rect_t* rect)
  {
  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hndl, &window_type, (void**)&window)))
    return result;

  if (window->overlay_canvas == 0)
    return e_invalid_operation;

  // if this canvas is not our own we need to tell the owner about this
  // but this may recurse, hence the _invalid = true being set
  canvas_t* canvas;
  if (failed(result = is_typeof(window->overlay_canvas, &canvas_type, (void**)&canvas)))
    return result;

  //if (succeeded(window_is_invalid(canvas->fb)))
  //  return s_ok;

  window_mark_invalid(canvas->fb);

  rect_t wnd_rect;

  // now we need to invalidate our children
  window_t* child = window->child;

  while (child != 0)
    {
    handle_t child_canvas;
    if (succeeded(get_overlay_canvas(child, &child_canvas)) &&
      failed(is_invalid(child_canvas)))
      {
      if (failed(result = window_rect((handle_t)child, &wnd_rect)) ||
        failed(result = invalidate_overlay_rect((handle_t)child, &wnd_rect)))
        return result;
      }

    child = child->next;
    }

  if (!canvas->fb->is_surface)
    {
    // ask the parent to redraw, it will then pass the message down
    // based on the correct z-order
    if (failed(result = window_rect((handle_t)window->parent, &wnd_rect)) ||
      failed(result = invalidate_overlay_rect((handle_t)window->parent, &wnd_rect)))
      return result;
    }

  // post a paint message to the window.  This actually just sets the
  // paint request, but this is the only way to send a message
  // between threads.
  return post_message(hndl, &paint_overlay_msg, 0);
  }

result_t begin_overlay_paint(handle_t hwnd, handle_t* paint_canvas)
  {
  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hwnd, &window_type, (void**)&window)))
    return result;


  if (window->overlay_canvas == 0)
    return e_invalid_operation;

  if (paint_canvas != 0)
    *paint_canvas = window->overlay_canvas;

  // if this canvas is not our own we need to tell the owner about this
  // but this may recurse, hence the _invalid = true being set

  canvas_t* canvas;
  do
    {
    if (failed(result = is_typeof(window->overlay_canvas, &canvas_type, (void**)&canvas)))
      return result;

    if (canvas->fb->is_surface)
      break;

    window = window->parent;
    } while (window != 0);

    if (window == 0)
      return s_ok;

    return window_begin_paint(canvas->fb);
  }

result_t end_overlay_paint(handle_t hwnd)
  {
  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hwnd, &window_type, (void**)&window)))
    return result;

  if (window->overlay_canvas == 0)
    return e_invalid_operation;

  // if this canvas is not our own we need to tell the owner about this
  // but this may recurse, hence the _invalid = true being set

  int depth = 0;
  canvas_t* canvas;
  do
    {
    if (failed(result = is_typeof(window->overlay_canvas, &canvas_type, (void**)&canvas)))
      return result;

    if (canvas->fb->is_surface)
      break;

    if (depth++ == 0)
      canvas->fb->invalid = false;

    window = window->parent;
    } while (window != 0);

    if (window == 0)
      return s_ok;

    return window_end_paint(canvas->fb);
  }

result_t defwndproc(handle_t wnd, const canmsg_t* msg, void* wnddata)
  {
  wndproc_fn wndproc;

  handle_t hchild;
  uint16_t id = get_can_id(msg);

  if (id == id_paint_foreground ||
    id == id_paint_background ||
    id == id_paint_overlay)
    {
    // we assume the widget has painted its canvas, we work over our children
    // in z-order
    uint16_t painting_order = 0;
    uint16_t next_z_order = 0;
    uint16_t max_order = 0;

    if (failed(get_first_child(wnd, &hchild)) || hchild == 0)
      return s_ok;

    switch (id)
      {
      case id_paint_background:
        begin_background_paint(wnd, 0);
        break;
      case id_paint_foreground:
        begin_foreground_paint(wnd, 0);
        break;
      case id_paint_overlay:
        begin_overlay_paint(wnd, 0);
        break;
      }

    // paint in lowest order to highest, note if there is
    // no window with painting order of 0 then the first
    // pass does nothing other than finding the first child
    // to paint
    while (true)
      {
      handle_t painting_wnd = hchild;
      do
        {
        uint8_t z_order;
        get_z_order(painting_wnd, &z_order);

        // if the next order is more than the current one, then
        // cache it, but if the next_z_order is greater then
        // reduce it to the current one
        if (z_order > painting_order)
          {
          // get the next highest order
          if (z_order < next_z_order || next_z_order == painting_order)
            next_z_order = z_order;

          // figure out what the last one to do is.
          if (z_order > max_order)
            max_order = z_order;
          }

        if (z_order == painting_order && succeeded(is_visible(painting_wnd)))
          {
          get_wndproc(painting_wnd, &wndproc, &wnddata);

          // call the window
          (*wndproc)(painting_wnd, msg, wnddata);
          }


        get_next_sibling(painting_wnd, &painting_wnd);
        } while (painting_wnd != 0);

        // if the painting z_order is the maximum then the loop is done
        if (painting_order >= max_order)
          break;

        painting_order = next_z_order;        // lowest paint z-order
      }

    switch (id)
      {
      case id_paint_background:
        end_background_paint(wnd);
        break;
      case id_paint_foreground:
        end_foreground_paint(wnd);
        break;
      case id_paint_overlay:
        end_overlay_paint(wnd);
        break;
      }
    }
  else
    {
    // send the message to all children of this window.
    if (failed(get_first_child(wnd, &hchild)) || hchild == 0)
      return s_ok;

    do
      {
      if (succeeded(get_wndproc(hchild, &wndproc, &wnddata)))
        (*wndproc)(hchild, msg, wnddata);

      get_next_sibling(hchild, &hchild);
      } while (hchild != 0);
    }

  return s_ok;
  }

result_t send_message(handle_t hndl, const canmsg_t* msg)
  {
  result_t result;
  void* wnddata;
  wndproc_fn wndproc;

  if (failed(result = get_wndproc(hndl, &wndproc, &wnddata)))
    return result;

  if (wndproc == 0)
    return defwndproc(hndl, msg, wnddata);

  return (*wndproc)(hndl, msg, wnddata);
  }

/////////////////////////////////////////////////////////////////////////
//
// This is the double buffered framebuffer draw code.

// these are the fbds state machine transitions
// a bsp_sync is detected
result_t window_sync(framebuffer_t* fb)
  {
  // if the framebuffer 
  if (fb->state >= fbds_in_sync && fb->paint_depth == 0)
    bsp_sync_framebuffer(fb);

  return s_ok;
  }

// these should be states
result_t window_mark_invalid(framebuffer_t* fb)
  {
  fb->invalid = true;
  return s_ok;
  }

result_t window_is_invalid(framebuffer_t* fb)
  {
  return fb->invalid ? s_ok : s_false;
  }

result_t window_queue_empty(framebuffer_t* fb)
  {
  result_t result = s_false;
  enter_critical();
  switch (fb->state)
    {
    case fbds_idle:
      // fb->state = fbds_painting;          // the GDI is painting so don't update video buffer
      result = s_ok;
      break;
    case fbds_in_sync:
      fb->state = fbds_in_sync_need_paint;
      break;
    }
  exit_critical();

  return result;
  }

result_t window_begin_paint(framebuffer_t* fb)       // the application is starting to paint
  {
  fb->paint_depth++;
  if (fb->state == fbds_in_sync_need_paint)
    {
    switch (fb->type)
      {
      case lt_background:
        post_message(0, &paint_background_msg, INDEFINITE_WAIT);
        break;
      case lt_foreground:
        post_message(0, &paint_foreground_msg, INDEFINITE_WAIT);
        break;
      case lt_overlay:
        post_message(0, &paint_overlay_msg, INDEFINITE_WAIT);
        break;
      }
    }

  fb->state = fbds_painting;

  return s_ok;
  }

result_t window_end_paint(framebuffer_t* fb)         // the application has finished painting
  {
  if (--fb->paint_depth == 0)
    {
    fb->invalid = false;
    fb->state = fbds_in_sync;          // wait for vsync
    }

  return s_ok;
  }

result_t window_sync_done(framebuffer_t* fb)
  {
  // if there was a queue empty while we are paining then
  // post another paint message
  if (fb->state == fbds_in_sync_need_paint)
    {
#ifdef _WIN32
    result_t result;
    handle_t screen;
    if (failed(result = get_screen(&screen)))
      return result;
#endif

    // on embedded systems this is in an ISR 
    switch (fb->type)
      {
      case lt_background:
#ifdef _WIN32
        if (failed(result = post_message(screen, &paint_background_msg, INDEFINITE_WAIT)))
          return result;
#else
        post_message_from_isr(&paint_background_msg);
#endif
        break;
      case lt_foreground:
#ifdef _WIN32
        if (failed(result = post_message(screen, &paint_foreground_msg, INDEFINITE_WAIT)))
          return result;
#else
        post_message_from_isr(&paint_foreground_msg);
#endif
        break;
      case lt_overlay:
#ifdef _WIN32
        if (failed(result = post_message(screen, &paint_overlay_msg, INDEFINITE_WAIT)))
          return result;
#else
        post_message_from_isr(&paint_overlay_msg);
#endif
        break;
      }
    }

  fb->state = fbds_idle;
  fb->invalid = false;
  return s_ok;
  }

