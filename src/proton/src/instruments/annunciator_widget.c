#include "../../include/annunciator_widget.h"

result_t annunciator_wndproc(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  annunciator_t* wnd = wnddata;

  uint16_t can_id = get_can_id(msg);

  if (wnd != 0 && succeeded((*wnd->base.on_message)(hwnd, msg, wnd)))
    return s_ok;

  // pass to default
  return widget_wndproc(hwnd, msg, wnddata);
  }

result_t paint_annunciator_background(const rect_t *wnd_rect,
                                         annunciator_t *ann)
  {
  result_t result;
  if (ann->background_canvas == nullptr)
    {
    extent_t ex;
    rect_extents(wnd_rect, &ex);
    if (failed(result = canvas_create(&ex, &ann->background_canvas)))
      return result;
    
    // if the background paint is overloaded then call it, otherwise call the widget paint
    if (ann->on_paint_background != 0)
      {
      // firstly make sure the flag is not set as there is a custom background
      ann->base.style &= ~FILL_BACKGROUND;
      ann->on_paint_background(ann->background_canvas, wnd_rect, nullptr, ann);
      }

    // draw the background and borders if the flags are set
    on_paint_widget_background(ann->background_canvas, wnd_rect, nullptr,
                               &ann->base);      
    }

  return s_ok;
  }

result_t create_annunciator_widget(handle_t parent, uint32_t flags, annunciator_t* wnd, handle_t* out)
  {
  result_t result;
  handle_t hndl;
  if (failed(result = create_widget(parent, flags, annunciator_wndproc, &wnd->base, &hndl)))
    return result;

  if (out != 0)
    *out = hndl;

  return s_ok;
  }
