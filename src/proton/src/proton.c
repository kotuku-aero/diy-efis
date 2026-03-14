#include "../include/proton.h"
#include "../include/converters.h"
#include "../include/dialog_theme.h"

static const char *proton_str = "proton";

handle_t menu_handle;
handle_t right_menu_handle;

// if parg is given then is a handle to a stream which will be closed
result_t run_proton(memid_t memid, create_layout_fn create_layout, create_widgets_fn create_widgets, wndproc_fn layout_wndproc)
  {
  result_t result;
  handle_t screen;

  if (failed(get_screen(&screen)))
    {
    trace_error("cannot open the screen\n");
    return e_unexpected;
    }

  // register all of the proton library widget classes.

  handle_t hwnd = 0;
  // not strictly needed as a reset is needed
  menu_handle = nullptr;
  right_menu_handle = nullptr;

  if(failed(result = (create_layout)(screen, memid, create_widgets, layout_wndproc, &hwnd)))
    {
    trace_error("Cannot create the layout\n");
    return result;
    }

  // despatch messages
  canmsg_t msg;
  // returns s_false to stop the queue
  while (true)
    if (succeeded(get_message(&hwnd, &msg)))
      {
      dispatch_message(hwnd, &msg);
      }

  return s_ok;
  }
