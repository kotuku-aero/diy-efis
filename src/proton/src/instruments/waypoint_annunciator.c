#include "../../include/annunciator_widget.h"
#include "../../include/strftm.h"

result_t on_waypoint_msg(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  waypoint_annunciator_t* data = (waypoint_annunciator_t*)wnddata;
  bool changed = false;

  if (get_can_id(msg) == data->base.can_id &&
      get_can_type(msg) == CANFLY_CHARS)
    {
    // first byte is type so this is number of chars + 1
    uint32_t len = get_can_len(msg);
    uint32_t i;
    // limit to 7 chars + null
    for(i = 1; i < len && i < sizeof(data->ident)-1; i++)
      {
      char c = (char)msg->data[i];
      if (c < 32 || c > 126)
        c = ' ';

      if (data->ident[i-1] != c)
        {
        data->ident[i] = c;
        changed = true;
        }
      }

    // null terminate (is assigned above if len < 8)
    if(data->ident[i] != 0)
      changed = true;

    data->ident[i] = 0;
    }

  if (changed)
    invalidate(hwnd);

  return s_false;
  }

void on_paint_waypoint(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnd)
  {
  waypoint_annunciator_t* data = (waypoint_annunciator_t*)wnd;
  if(strlen(data->ident) == 0)
    on_draw_text(canvas, wnd_rect, wnd, "-------");
  else
    on_draw_text(canvas, wnd_rect, wnd, data->ident);
  }
