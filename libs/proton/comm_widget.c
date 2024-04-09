#include "comm_widget.h"


result_t comm_wndproc(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  bool changed = false;
  comm_window_t* wnd = (comm_window_t*)wnddata;

  switch (get_can_id(msg))
    {
    case id_paint:
      on_paint_widget(hwnd, msg, wnddata);
      break;
    }

  if (changed)
    invalidate(hwnd);

  return defwndproc(hwnd, msg, wnddata);
  }

result_t create_comm_window(handle_t parent, uint16_t id, aircraft_t* aircraft, comm_window_t* wnd, handle_t* out)
  {
  result_t result;
  handle_t hndl;
  if (failed(result = create_widget(parent, id, comm_wndproc, &wnd->base, &hndl)))
    return result;

  // calculate the rectangles
  rect_t wnd_rect;
  window_rect(hndl, &wnd_rect);


  if(out != 0)
    *out = hndl;

  return s_ok;
  }