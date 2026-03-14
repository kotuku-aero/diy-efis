#include "../../include/checkbox_widget.h"

static void on_paint(handle_t canvas, const rect_t *wnd_rect,
                     const canmsg_t *msg, void *wnddata)
  {
  checkbox_widget_t *wnd = (checkbox_widget_t *)wnddata;

  on_paint_widget_background(canvas, wnd_rect, msg, wnddata);

  point_t pt;
  rect_top_left(wnd_rect, &pt);

  // the left part of the control is the check-box
  pt.x += rect_height(wnd_rect) + 1;

  point_t pts[5] = {
    { wnd_rect->left + 4, wnd_rect->top + 4 },
    {pt.x - 5, wnd_rect->top + 4 },
    { pt.x - 5, wnd_rect->bottom -4 },
    { wnd_rect->left + 4, wnd_rect->bottom -4 },
   { wnd_rect->left + 4, wnd_rect->top + 4 },
  };

  polyline(canvas, wnd_rect, dialog_theme->border_color, 5, pts);

  bool checked;
  coerce_to_bool(&wnd->base.value, &checked);

  if (checked)
    {
    // draw a cross in the checkbox
    pts[1].y = wnd_rect->bottom -4;
    polyline(canvas, wnd_rect, dialog_theme->alarm_color, 2, pts);

    pts[0].x = pt.x - 5;
    pts[1].x = wnd_rect->left + 4;
    polyline(canvas, wnd_rect, dialog_theme->alarm_color, 2, pts);
    }

  draw_text(canvas, wnd_rect, dialog_theme->label_font,
            dialog_theme->foreground_color, dialog_theme->background_color, 0,
            wnd->base.base.base.name, &pt, wnd_rect,
             wnd->base.base.base.style, 0);

  // outline the focused box with a grey line dashed line
  if (wnd->base.base.focused)
    draw_control_focus(canvas, wnd_rect, wnd_rect);
  }

static result_t wndproc(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  checkbox_widget_t *wnd = (checkbox_widget_t *)wnddata;

  result_t result;
  switch (get_can_id(msg))
    {
    default:
      break;
    case id_ok :
      {
      bool checked;
      coerce_to_bool(&wnd->base.value, &checked);
      checked = !checked;
      create_variant_bool(checked, &wnd->base.value);
      invalidate(hwnd);
      }
    return s_ok;
    }

  return dialog_wndproc(hwnd, msg, wnddata);
  }

result_t create_checkbox_widget(handle_t parent, uint32_t flags, checkbox_widget_t* wnd, handle_t* out)
  {
  result_t result;
  handle_t hndl;
  if (failed(result = create_widget(parent, flags, wndproc, &wnd->base.base.base, &hndl)))
    return result;

  wnd->base.base.base.on_paint = on_paint;

  if (out != 0)
    *out = hndl;

  return s_ok;
  }