#include "../../include/annunciator_widget.h"

result_t on_interval_msg(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  datetime_annunciator_t *data = (datetime_annunciator_t *)wnddata;
  bool changed = false;

  tm_t tm;
  if (get_can_id(msg) == data->base.can_id)
    {
    // this is time in seconds
    uint32_t value;
    get_param_uint32(msg, &value);

    // convert to a tm structure
    memset(&tm, 0, sizeof(tm_t));

    tm.second = value % 60;
    value /= 60;
    tm.minute = value % 60;
    value /= 60;
    tm.hour = value % 60;

    changed = memcmp(&data->clock, &tm, sizeof(tm_t))!= 0;
    }

  if (changed)
    {
    memcpy(&data->clock, &tm, sizeof(tm_t));
    invalidate(hwnd);
    }

  return s_false;
  }
