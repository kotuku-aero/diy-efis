#include "../../include/dialog_widget.h"

void on_paint_dialog_widget_decorations(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, dialog_widget_t* widget)
  {
  if (widget->focused)
    draw_control_focus(canvas, wnd_rect, wnd_rect);
  }

result_t dialog_wndproc(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  dialog_widget_t *widget = (dialog_widget_t*)wnddata;
  uint16_t id = get_can_id(msg);

  bool changed = false;
  bool send_event = false;
  if (id == id_setfocus)
    {
    changed = !widget->focused;
    widget->focused = true;
    send_event = true;
    }
  else if (id == id_loosefocus)
    {
    changed = widget->focused;
    widget->focused = false;
    send_event = true;
    }

  if (changed)
    invalidate(hwnd);

  if (send_event && widget->on_focus != nullptr)
    {
    event_args_t args = { .version = sizeof(event_args_t) };
    widget->on_focus(&widget->base, &args);
    }

  return widget_wndproc(hwnd, msg, wnddata);
  }
