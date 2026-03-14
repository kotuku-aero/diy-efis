#include "../../include/panel_widget.h"

static void on_paint(handle_t canvas, const rect_t *wnd_rect,
                     const canmsg_t *msg, void *wnddata)
  {
  panel_widget_t *wnd = (panel_widget_t *)wnddata;

  rectangle(canvas, wnd_rect, color_hollow, dialog_theme->background_color, wnd_rect);
  }

result_t create_panel_widget(handle_t parent, uint32_t flags, panel_widget_t* wnd,
                             handle_t* out)
  {
  result_t result;
  handle_t hndl;
  if (failed(result = create_widget(parent, flags, widget_wndproc, &wnd->base.base.base, &hndl)))
    return result;

  wnd->base.base.base.on_paint = on_paint;

  if (out != 0)
    *out = hndl;

  return s_ok;
  }
