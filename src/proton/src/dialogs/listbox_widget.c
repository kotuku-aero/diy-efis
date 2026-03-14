#include "../../include/listbox_widget.h"

static void on_paint(handle_t canvas, const rect_t *wnd_rect,
                     const canmsg_t *msg, void *wnddata)
  {
  listbox_widget_t *wnd = (listbox_widget_t *)wnddata;

  on_paint_widget_background(canvas, wnd_rect, msg, wnddata);
  }

result_t create_listbox_widget(handle_t parent, uint32_t flags,
                              listbox_widget_t *wnd, handle_t *out)
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