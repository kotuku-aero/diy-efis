#include "../../include/button_widget.h"

static void on_paint(handle_t canvas, const rect_t *wnd_rect,
                     const canmsg_t *msg, void *wnddata)
  {
  button_widget_t *wnd = (button_widget_t *)wnddata;

  on_paint_widget_background(canvas, wnd_rect, msg, wnddata);
  point_t pt;
  rect_top_left(wnd_rect, &pt);

  draw_text(canvas, wnd_rect, wnd->base.base.base.name_font,
            dialog_theme->label_color, dialog_theme->background_color, 0,
            wnd->base.base.base.name, &pt, wnd_rect,
            wnd->base.base.base.style, 0);

  // outline the focused box with a grey line dashed line
  if (wnd->base.base.focused)
    draw_control_focus(canvas, wnd_rect, wnd_rect);
  }

result_t create_button_widget(handle_t parent, uint32_t flags, button_widget_t *wnd, handle_t *out)
  {
  result_t result;
  handle_t hndl;
  if (failed(result =
                 create_widget(parent, flags, dialog_wndproc, &wnd->base.base.base, &hndl)))
    return result;

  wnd->base.base.base.on_paint = on_paint;

  if (out != 0)
    *out = hndl;

  return s_ok;
  }