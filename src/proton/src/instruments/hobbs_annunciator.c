#include "../../include/annunciator_widget.h"

result_t on_hobbs_msg(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  hobbs_annunciator_t* data = (hobbs_annunciator_t*)wnddata;

  bool changed = false;

  if (get_can_id(msg) == data->base.can_id)
    {
    uint32_t value;
    get_param_int32(msg, &value);

    changed = data->hobbs != value;
    data->hobbs = value;
    }

  if (changed)
    invalidate(hwnd);

  return s_false;
  }

void on_paint_hobbs(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnddata)
  {
  hobbs_annunciator_t* data = (hobbs_annunciator_t*)wnddata;

  char txt[10];

  // time is in seconds
  uint32_t hours = data->hobbs / 3600;
  uint32_t mins = (data->hobbs % 3600) / 360;

  sprintf(txt, "%04.4d.%d", hours, mins);
  on_draw_text(canvas, wnd_rect, wnddata, txt);
  }
