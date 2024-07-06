#include "softkey_widget.h"

static void on_paint_background(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnddata)
  {
  }

static void on_paint(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnddata)
  {
  softkey_window_t *wnd = (softkey_window_t *)wnddata;
  extent_t ex;
  rect_extents(wnd_rect, &ex);

  if (wnd->background_canvas == nullptr)
    {
    canvas_create(&ex, &wnd->background_canvas);

    on_paint_widget_background(wnd->background_canvas, wnd_rect, msg, wnddata);

    // create the button shape
    rect_t btn_rect = { wnd_rect->left + 1, wnd_rect->top + 1, wnd_rect->right - 1, wnd_rect->bottom - 1 };

    round_rect(wnd->background_canvas, wnd_rect, wnd->base.border_color, color_black, &btn_rect, 5);
    }

  point_t pt = { 0, 0 };
  bit_blt(canvas, wnd_rect, wnd_rect, wnd->background_canvas, wnd_rect, &pt, src_copy);

  rect_t btn_rect = { wnd_rect->left + 3, wnd_rect->top + 3, wnd_rect->right - 3, wnd_rect->bottom - 3 };

  if (wnd->glyph != 0)
    {
    handle_t glyph = wnd->is_selected && wnd->selected_glyph != 0 ? wnd->selected_glyph : wnd->glyph;


    // copy the image
    rect_create(0, 0, ex.dx, ex.dy, &btn_rect);
    bit_blt(canvas, wnd_rect, &btn_rect,  glyph, 0, point_create(0, 0, &pt), src_copy);
    }
  else
    {
    // TODO: cache these????
    // draw the widget text inside the key
    extent_t text_ex;
    text_extent(wnd->base.name_font, 0, wnd->base.name, &text_ex);
    // get the center of the window
    point_create(ex.dx >> 1, ex.dy >> 1, &pt);

    // offset to top left of text
    pt.x -= text_ex.dx >> 1;
    pt.y -= text_ex.dy >> 1;

    round_rect(canvas, wnd_rect, color_hollow, wnd->base.background_color, &btn_rect, 3);

    rect_t text_rect;

    rect_create(pt.x, pt.y, pt.x + text_ex.dx, pt.y + text_ex.dy, &text_rect);
    draw_text(canvas, wnd_rect, wnd->base.name_font, wnd->base.name_color,
      color_hollow, 0, wnd->base.name, &pt, &text_rect, 0, 0);
    }
  }

static result_t softkey_wndproc(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  softkey_window_t* wnd = (softkey_window_t*)wnddata;
  const touch_msg_t* touch_msg;

  switch (get_can_id(msg))
    {
    case id_paint:
      on_paint_widget(hwnd, msg, wnddata);
      break;
    case id_touch_tap:
      if (succeeded(is_window_tap(hwnd, msg, 0)) &&
        succeeded(get_touch_msg(msg, &touch_msg)))
        {
        // based on begin/end
        if ((touch_msg->flags & TOUCH_BEGIN) != 0)
          {
          // post the tap event to the message queue
          if (get_can_id(&wnd->tapped_msg) != 0)
            post_message(0, &wnd->tapped_msg, INDEFINITE_WAIT);
          wnd->is_selected = true;
          // repaint our window as selected.
          invalidate(hwnd);
          }
        else if ((touch_msg->flags & TOUCH_END) != 0)
          {
          wnd->is_selected = false;
          invalidate(hwnd);
          }
        }
      break;
    case id_touch_press:
      break;
    }


  return defwndproc(hwnd, msg, wnddata);
  }

result_t create_softkey_window(handle_t parent, uint16_t id, aircraft_t* aircraft, softkey_window_t* wnd, handle_t* out)
  {
  result_t result;
  handle_t hndl;

  if (failed(result = create_widget(parent, id, softkey_wndproc, &wnd->base, &hndl)))
    return result;

  wnd->base.on_paint = on_paint;

  if(failed(result = register_touch_window(hndl, 0)))
    return result;

  if (out != 0)
    *out = hndl;

  return s_ok;

  }