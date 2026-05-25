#include "../../include/annunciator_widget.h"

result_t on_hours_msg(handle_t hwnd, uint16_t can_id, const canmsg_t* msg, void* wnddata)
  {
  hours_annunciator_t* data = (hours_annunciator_t*)wnddata;
  bool changed = false;

  if (get_can_id(msg) == data->base.can_id)
    {
    uint32_t value;
    get_param_uint32(msg, &value);
    changed = data->hours != value;
    data->hours = value;
    }

  if (changed)
    invalidate(hwnd);

  return s_false;
  }

void on_paint_hours(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnddata)
  {
  hours_annunciator_t* data = (hours_annunciator_t*)wnddata;

  char txt[10];

  // time is in seconds. Convert to hours and minutes
  uint32_t hours = data->hours / 60;
  uint32_t minutes = data->hours % 60;


  sprintf(txt, "%02d:%02d", hours, minutes);
  on_draw_text(canvas, wnd_rect, wnddata, txt);
  }
