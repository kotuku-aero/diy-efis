#include "../../include/annunciator_widget.h"
#include "../../include/strftm.h"
#include <string.h>

result_t on_datetime_msg(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  datetime_annunciator_t* data = (datetime_annunciator_t*)wnddata;
  bool changed = false;

  if (get_can_id(msg) == data->base.can_id)
    {
    tm_t tm;
    if (succeeded(get_param_utc(msg, &tm)))
      {
      changed = tm.year != data->clock.year ||
        tm.month != data->clock.month ||
        tm.day != data->clock.day ||
        tm.hour != data->clock.hour ||
        tm.minute != data->clock.minute ||
        tm.second != data->clock.second;

      memcpy(&data->clock, &tm, sizeof(tm_t));

      data->is_null = false;
      }
    }

  if (changed)
    invalidate(hwnd);

  return s_false;
  }

void on_paint_datetime(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnd)
  {
  datetime_annunciator_t* data = (datetime_annunciator_t*)wnd;

  if (data->is_null)
    on_draw_text(canvas, wnd_rect, wnd, "---");
  else
    {
    strftm(data->txt, sizeof(data->txt), data->format, &data->clock);

    on_draw_text(canvas, wnd_rect, wnd, data->txt);
    }
  }
