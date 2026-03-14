#include "../../include/annunciator_widget.h"

result_t on_hp_msg(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  hp_annunciator_t* data = (hp_annunciator_t*)wnddata;

  bool changed = false;
  if (get_can_id(msg) == data->base.can_id)
    {
    uint16_t value;
    get_param_uint16(msg, &value);

    changed = data->hp != value;
    data->hp = value;
    }

  // default is to broadcast the messages
  return s_false;
  }

void on_paint_hp(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnddata)
  {
  hp_annunciator_t* data = (hp_annunciator_t*)wnddata;

  char txt[10];

  sprintf(txt, "%d %%", data->hp);
  on_draw_text(canvas, wnd_rect, wnddata, txt);
  }
