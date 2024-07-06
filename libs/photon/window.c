#include "photon_priv.h"

const canmsg_t paint_msg = { id_paint };

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

  if (failed(result = close_handle(window->canvas)))
    return result;

  memset(window, 0, sizeof(window_t));
  return neutron_free(window);
  }

const typeid_t window_type =
  {
  .name = "window",
  .etherealize = window_close
  };
/**
* @brief Convert a window position to an absolute position
* @param fb    framebuffer
* @param src   source rectangle
* @param dst   calculated rectangle
* @return dst
*/
static inline const rect_t* to_framebuffer_position(gdi_t* fb, const rect_t* src, rect_t* dst)
  {
  rect_copy(src, dst);
  dst->left += fb->position.left;
  dst->top += fb->position.top;
  dst->right += fb->position.left;
  dst->bottom += fb->position.top;

  return dst;
  }

static inline result_t window_get_pixel(gdi_t* fb, const point_t* src, color_t* pix)
  {
  point_t pt;
  return fb->surface->base.get_pixel(&fb->surface->base, to_absolute_pt(src, fb, &pt), pix);
  }

static inline result_t window_set_pixel(gdi_t* fb, const point_t* dest, color_t color, color_t* pix)
  {
  point_t pt;
  return fb->surface->base.set_pixel(&fb->surface->base, to_absolute_pt(dest, fb, &pt), color, pix);
  }

static inline result_t window_fast_fill(gdi_t* fb, const rect_t* dest, color_t fill_color)
  {
  rect_t r;
  return fb->surface->base.fast_fill(&fb->surface->base, to_absolute_rect(dest, fb, &r), fill_color);
  }

static inline result_t window_fast_line(gdi_t* fb, const point_t* p1, const point_t* p2, color_t fill_color)
  {
  point_t pt1;
  point_t pt2;

  return fb->surface->base.fast_line(&fb->surface->base, to_absolute_pt(p1, fb, &pt1), to_absolute_pt(p2, fb, &pt2), fill_color);
  }

static inline result_t window_fast_copy(gdi_t* fb, const point_t* dest, const gdi_t* src_canvas, const rect_t* src, raster_operation op)
  {
  point_t p1;
  rect_t r1;

  return fb->surface->base.fast_copy(&fb->surface->base, to_absolute_pt(dest, fb, &p1), &src_canvas->surface->base, to_absolute_rect(src, src_canvas, &r1), op);
  }


static result_t create_child_framebuffer(canvas_t* parent, const rect_t* rect, gdi_t** out)
  {
  result_t result;
  // a child framebuffer shares its parent's buffer
  // check the ranges.  The window must be completely within its parent.
  if (rect->left < 0 || rect->left > rect_width(&parent->fb->position) ||
    rect->top < 0 || rect->top > rect_height(&parent->fb->position) ||
    rect->right < rect->left || rect->right > rect_width(&parent->fb->position) ||
    rect->bottom > rect_height(&parent->fb->position))
    return e_bad_parameter;       // rect exceeds bounds

  gdi_t* fb;
  if (failed(result = neutron_calloc(1, sizeof(gdi_t), (void**)&fb)))
    return result;

  // initialize the child with routines that use the parent canvas
  fb->surface = parent->fb->surface;
  fb->is_surface = false;
  fb->fast_copy = window_fast_copy;
  fb->fast_fill = window_fast_fill;
  fb->fast_line = window_fast_line;
  fb->get_pixel = window_get_pixel;
  fb->set_pixel = window_set_pixel;

  to_framebuffer_position(parent->fb, rect, &fb->position);      // store the rectange as relative to the canvas
  to_absolute_rect(&fb->position, fb, &fb->absolute_position);

  *out = fb;

  return s_ok;
  }

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

  gdi_t* fb;
  canvas_t* canvas;
  // create the canvas
  if (parent->canvas != 0 &&
    (failed(result = is_typeof(parent->canvas, &canvas_type, (void**)&canvas)) ||
      failed(result = create_child_framebuffer(canvas, bounds, &fb)) ||
      failed(result = create_canvas_from_framebuffer(fb, (handle_t *) &window->canvas))))
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

  if (!window->visible)
    {
  window->visible = true;
    // visible changed so add to invalid count
    if (window->canvas->fb->invalid)
      {
      enter_critical();
      //trace_info("show_window: invalidate window %d\n", window->id);
      window->canvas->fb->surface->invalid_count++;
      exit_critical();
      }
    }

  return s_ok;
  }

result_t hide_window(handle_t hndl)
  {
  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hndl, &window_type, (void**)&window)))
    return result;

  if (window->visible)
    {
  window->visible = false;
    // visible changed so remove invalid count
    if (window->canvas->fb->invalid)
      {
      enter_critical();
      //trace_info("hide_window: un-invalidate window %d\n", window->id);
      window->canvas->fb->surface->invalid_count--;
      exit_critical();
      }
    }

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
  if (failed(result = is_typeof(window->canvas, &canvas_type, (void**)&canvas)))
      return result;

  to_absolute_rect(&window->position, canvas->fb, &canvas->fb->absolute_position);

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

result_t invalidate(handle_t hndl)
  {
  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hndl, &window_type, (void**)&window)))
    return result;

   // invalidate the canvas
  enter_critical();
  if (!window->canvas->fb->invalid)
    {
    if (window->visible)
      {
      //trace_info("invalidate: invalidate window %d\n", window->id);
      window->canvas->fb->surface->invalid_count++;
      }

    window->canvas->fb->invalid = true;
    }
  exit_critical();

  // invalidate our children
  for (window_t* child = window->child; child != 0; child = child->next)
    {
    if (failed(is_invalid((handle_t)child)))
      invalidate((handle_t)child);
    }

      return s_ok;
  }

result_t is_invalid(handle_t hwnd)
  {
  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hwnd, &window_type, (void**)&window)))
    return result;

  canvas_t* canvas;
  if (failed(result = is_typeof(window->canvas, &canvas_type, (void**)&canvas)))
      return result;

  return canvas->fb->invalid ? s_ok : s_false;
  }

result_t begin_paint(handle_t hwnd, handle_t* paint_canvas)
  {
  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hwnd, &window_type, (void**)&window)))
    return result;

  if (window->canvas == nullptr)
    return e_invalid_operation;

  canvas_t* canvas;
  if (failed(result = is_typeof(window->canvas, &canvas_type, (void**)&canvas)))
      return result;

  // tell the surface that this is in a paint operation
  enter_critical();
  canvas->fb->surface->paint_depth++;
  exit_critical();

  if (paint_canvas != nullptr)
    *paint_canvas = window->canvas;

      return s_ok;
  }

result_t end_paint(handle_t hwnd)
  {
  result_t result;
  window_t* window;
  if (failed(result = is_typeof(hwnd, &window_type, (void**)&window)))
    return result;

  if (window->canvas == 0)
    return e_invalid_operation;

  canvas_t* canvas;
  if (failed(result = is_typeof(window->canvas, &canvas_type, (void**)&canvas)))
        return result;

  bool paint_complete = false;
  enter_critical();
  // one less invalid window
  if (canvas->fb->invalid)
    {
    //trace_info("end_paint: un-invalidate window %d\n", window->id);

    canvas->fb->surface->invalid_count--;
    canvas->fb->invalid = false;
  }
  canvas->fb->surface->paint_depth--;
  paint_complete = canvas->fb->surface->paint_depth == 0;
  exit_critical();

  if (paint_complete)
  {
    handle_t screen;
    get_screen(&screen);

    screen_surface_t *surface;
    get_screen_surface(screen, &surface);

    // if the paint is complete then sync the framebuffer
    gdi_painting_done(surface);
  }

      return s_ok;
  }

result_t defwndproc(handle_t wnd, const canmsg_t* msg, void* wnddata)
  {
  wndproc_fn wndproc;

  handle_t hchild;
  uint16_t id = get_can_id(msg);

  if (id == id_paint)
    {
    // we assume the widget has painted its canvas, we work over our children
    // in z-order
    uint16_t painting_order = 0;
    uint16_t next_z_order = 0;
    uint16_t max_order = 0;

    if (failed(get_first_child(wnd, &hchild)) || hchild == 0)
      return s_ok;

    begin_paint(wnd, 0);    // start painting, set ctr == 1 on first paint

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

        if (z_order == painting_order &&
          succeeded(is_visible(painting_wnd)))
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

    end_paint(wnd);
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
result_t get_screen_surface(handle_t hwnd, screen_surface_t** surface)
  {
  if (surface == 0)
    return e_bad_pointer;

  result_t result;
  screen_t* window;
  if (failed(result = is_typeof(hwnd, &screen_type, (void**)&window)))
    return result;

  canvas_t* canvas;
  if (failed(result = is_typeof(window->base.canvas, &canvas_type, (void**)&canvas)))
    return result;

  *surface = (screen_surface_t *) canvas->fb->surface;

  return s_ok;
  }

// called when the window has been sync'd with the video buffer
// and the window is ready to be painted.
// called within an IRQ so cannot block
result_t gdi_sync(screen_surface_t* gdi)
  {

  if (gdi->state == fbds_in_sync)
  {
    bsp_sync_framebuffer();

    // if the state has changed then we need to post a paint message
    if (gdi->state == fbds_in_sync_need_paint)
    #ifdef _WIN32
      post_message(0, &paint_msg, 0);
    #else
      post_message_from_isr(&paint_msg);
    #endif

    gdi->state = fbds_idle;
      }

  return s_ok;
    }

result_t gdi_painting_done(screen_surface_t* gdi)
  {
  gdi->state = fbds_in_sync;

  return s_ok;
  }

result_t window_queue_empty(screen_surface_t* gdi)
  {
  enter_critical();
  switch (gdi->state)
    {
    case fbds_idle:
      gdi->state = fbds_painting;          // the GDI is painting so don't update video buffer
      break;
    case fbds_in_sync:
      if (gdi->base.invalid_count > 0)
        gdi->state = fbds_in_sync_need_paint;  // the GDI is painting so don't update video buffer
      break;
    }
  exit_critical();

  return s_ok;
  }

result_t window_sync_done(screen_surface_t *gdi)
  {
  // if there was a queue empty while we are paining then
  // post another paint message
  if (gdi->state == fbds_in_sync_need_paint)
      {
#ifdef _WIN32
    post_message(0, &paint_msg, INDEFINITE_WAIT);
#else
    post_message_from_isr(&paint_msg);
#endif
    }

  gdi->state = fbds_idle;

  return s_ok;
  }

