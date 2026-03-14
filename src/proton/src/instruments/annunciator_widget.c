#include "../../include/annunciator_widget.h"

result_t annunciator_wndproc(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  annunciator_t* wnd = wnddata;

  if (wnd != 0)
    (*wnd->base.on_message)(hwnd, msg, wnd);

  // pass to default
  return widget_wndproc(hwnd, msg, wnddata);
  }

result_t create_annunciator_widget(handle_t parent, uint32_t flags, annunciator_t* wnd, handle_t* out)
  {
  result_t result;
  handle_t hndl;
  if (failed(result = create_widget(parent, flags, annunciator_wndproc, &wnd->base, &hndl)))
    return result;

  if (out != 0)
    *out = hndl;

  return s_ok;
  }
