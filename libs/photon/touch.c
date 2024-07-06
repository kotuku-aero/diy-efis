#include "window.h"

result_t register_touch_window(handle_t hwnd, uint16_t flags)
  {
  return s_ok;
  }

result_t is_window_tap(handle_t hwnd, const canmsg_t* msg, uint16_t blur)
  {
  return s_false;
  }

result_t get_touch_msg(const canmsg_t* msg, const touch_msg_t** out)
  {
  return s_ok;
  }
