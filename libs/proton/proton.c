#include "fonts.h"

#include "../neutron/bsp.h"

/**
 * Load a registry hive that describes a series of windows
 * @param canvas    Parent window to create child windows within
 * @param hive      list of keys that describe each window
 * @return s_ok if loaded ok
 */
extern result_t load_layout(handle_t canvas, memid_t hive);

handle_t main_window = 0;

void run_proton(void *parg)
  {
  result_t result;
  memid_t key;

  if(succeeded(reg_open_key(0, "proton", &key)))
    {
    // create the root window
    uint16_t orientation;

    if(failed(reg_get_uint16(key, "orientation", &orientation)))
      orientation = 0;

    char layout_name[REG_STRING_MAX];

    memid_t layout_key;

    if (failed(result = reg_get_string(key, "layout", layout_name, 0)) ||
      failed(result = reg_open_key(0, layout_name, &layout_key)))
      return result;

    if(succeeded(result = open_screen(orientation, defwndproc, 0, &main_window)))
      load_layout(main_window, layout_key);
    }
  
  // run the message queue
  // despatch messages
  window_msg_t msg;
  // returns s_false to stop the queue
  while (true)
    if(succeeded(get_message(main_window, &msg)))
      dispatch_message(&msg);
  }

