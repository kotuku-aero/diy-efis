#include "../../include/annunciator_widget.h"

result_t on_apmode_msg(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  apmode_annunciator_t* data = (apmode_annunciator_t*)wnddata;
  bool changed = false;

  if (get_can_id(msg) == data->base.can_id)
    {
    uint16_t mode;
    if (succeeded(get_param_uint16(msg, &mode)))
      {
      if (changed = mode != data->mode)
        data->mode = mode;
      }
    }

  if (changed)
    invalidate(hwnd);

  return s_false;
  }

void on_paint_apmode(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnd)
  {
  apmode_annunciator_t* data = (apmode_annunciator_t*)wnd;

  switch (data->mode)
    {
    default:
      on_draw_text(canvas, wnd_rect, wnd, "OFF");
      break;
    case AP_MODE_HDG_ALT:
      on_draw_text(canvas, wnd_rect, wnd, "HDG ALT");
      break;
    case AP_MODE_CRS_ALT:
      on_draw_text(canvas, wnd_rect, wnd, "CRS ALT");
      break;
    case AP_MODE_HDG_VS:
      on_draw_text(canvas, wnd_rect, wnd, "HDG VS");
      break;
    case AP_MODE_CRS_VS:
      on_draw_text(canvas, wnd_rect, wnd, "CRS VS");
      break;
    case AP_MODE_HDG:
      on_draw_text(canvas, wnd_rect, wnd, "HDG");
      break;
    case AP_MODE_CRS:
      on_draw_text(canvas, wnd_rect, wnd, "CRS");
      break;
    case AP_MODE_ALT:
      on_draw_text(canvas, wnd_rect, wnd, "ALT");
      break;
    case AP_MODE_VS:
      on_draw_text(canvas, wnd_rect, wnd, "VS");
      break;
    case AP_MODE_REVERSE_LEFT:
      on_draw_text(canvas, wnd_rect, wnd, "REV L");
      break;
    case AP_MODE_REVERSE_RIGHT:
      on_draw_text(canvas, wnd_rect, wnd, "REV R");
      break;
    }


  }
