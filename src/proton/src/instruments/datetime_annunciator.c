#include "../../include/annunciator_widget.h"
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
    char *s = data->txt;
    *s = 0;
    const char *fmt = data->format;
    for (; *fmt != 0; fmt++)
      {
      if (*fmt != '%')
        {
        // If it's a regular character, copy it directly
        *s++ = *fmt;
        *s = 0;
        continue;
        }

      fmt++;
      switch (*fmt)
        {
        case 0 :
          goto out;
        case '%' :
          // Escaped percentage sign "%%"
          *s++ = '%';
          continue;
        case 'd': // Day of the month (01-31)
          s += sprintf(s, "%02d", data->clock.day);
          break;
        case 'm': // Month number (01-12)
          s += sprintf(s, "%02.2d", data->clock.month);
          break;
        case 'Y': // Year with century
          s += sprintf(s, "%04.4d", data->clock.year);
          break;
        case 'y': // Year
          {
          int yr = data->clock.year;
          if (yr > 99)
            yr = yr % 100;
          s += sprintf(s, "%02.2d", yr);
          break;
          }
        case 'H': // Hour in 24h format (00-23)
          s += sprintf(s, "%02.2d", data->clock.hour);
          break;
        case 'M': // Minute (00-59)
          s += sprintf(s, "%02.2d", data->clock.minute);
          break;
        case 'S': // Second (00-60)
          s += sprintf(s, "%02.2d", data->clock.second);
          break;
        }
      }
out:
    on_draw_text(canvas, wnd_rect, wnd, data->txt);
    }
  }
