#include "annunciator_widget.h"



result_t annunciator_wndproc(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  uint16_t can_id = get_can_id(msg);
  annunciator_t* wnd = (annunciator_t*)wnddata;

  if (wnd != 0 && (*wnd->base.on_message)(hwnd, can_id, msg, wnd))
    {
    invalidate(hwnd);

    return s_ok;
    }

  // pass to default
  return widget_wndproc(hwnd, msg, wnddata);
  }

result_t create_annunciator_widget(handle_t parent, uint16_t id, aircraft_t* aircraft, annunciator_t* wnd, handle_t* out)
  {
  result_t result;
  handle_t hndl;
  if (failed(result = create_widget(parent, id, annunciator_wndproc, &wnd->base, &hndl)))
    return result;

  if (out != 0)
    *out = hndl;

  return s_ok;
  }
