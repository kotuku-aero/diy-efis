#include "../../include/edit_widget.h"

static void on_paint(handle_t canvas, const rect_t *wnd_rect, const canmsg_t* msg, void* wnddata)
  {
  edit_widget_t* wnd = (edit_widget_t*)wnddata;

  on_paint_widget_background(canvas, wnd_rect, msg, wnddata);

  point_t pt;
  rect_top_left(wnd_rect, &pt);

  pt.x += 2;
  pt.y += 2;

  // get the edit value and format it
  char edit_value[32];
  // for now just use a float value
  float value;
  coerce_to_float(&wnd->value, &value);
  snprintf(edit_value, sizeof(edit_value), "%.2f", value);

  // TODO: in-edit handling in wndproc
  draw_text(canvas, wnd_rect, dialog_theme->edit_font, dialog_theme->edit_color,
    dialog_theme->background_color_selected, 0, edit_value, &pt,
    wnd_rect, eto_none, 0);

  // outline the focused box with a grey line dashed line
  if (wnd->base.base.focused)
    draw_control_focus(canvas, wnd_rect, wnd_rect);
  }

static result_t wndproc(handle_t hwnd, const canmsg_t *msg,
                                 void *wnddata)
  {
  uint16_t id = get_can_id(msg);
  edit_widget_t *widget = (edit_widget_t *)wnddata;

  bool changed = false;
  bool flag;

  // switch (id)
  //   {
  //   }

  if (changed)
    invalidate(hwnd);

  return dialog_wndproc(hwnd, msg, wnddata);
  }

result_t create_edit_widget(handle_t parent, uint32_t flags, edit_widget_t* wnd, handle_t* out)
  {
  result_t result;
  handle_t hndl;
  if (failed(result = create_widget(parent, flags, wndproc, &wnd->base.base.base, &hndl)))
    return result;

  create_variant_float(0.0f, &wnd->value);

  wnd->base.base.base.on_paint = on_paint;

  if (out != 0)
    *out = hndl;

  return s_ok;
  }
