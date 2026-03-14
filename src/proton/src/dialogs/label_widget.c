#include "../../include/label_widget.h"

static void on_paint(handle_t canvas, const rect_t *wnd_rect, const canmsg_t* msg, void* wnddata)
  {
  label_widget_t* wnd = (label_widget_t*)wnddata;

  point_t pt;
  rect_top_left(wnd_rect, &pt);

  draw_text(canvas, wnd_rect, dialog_theme->label_font, dialog_theme->label_color,
    dialog_theme->background_color, 0, wnd->base.name, &pt,
    wnd_rect, wnd->base.style, 0);
  }

result_t create_label_widget(handle_t parent, uint32_t flags, label_widget_t* wnd,
                             handle_t* out)
  {
  result_t result;
  handle_t hndl;
  if (failed(result = create_widget(parent, flags, widget_wndproc, &wnd->base, &hndl)))
    return result;

  wnd->base.on_paint = on_paint;

  if (out != 0)
    *out = hndl;

  return s_ok;
  }
