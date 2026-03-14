#include "../../include/bound_widget.h"

result_t default_data_bound_widget_get_value(data_bound_widget_t *widget, variant_t *value)
  {
  return e_not_implemented;
  }

result_t default_data_bound_widget_set_value(data_bound_widget_t *widget, const variant_t *value)
  {
  return e_not_implemented;
  }

result_t data_bound_widget_wndproc(handle_t hwnd, const canmsg_t *msg, void *wnddata)
  {
  uint16_t id = get_can_id(msg);

  switch (id)
    {
    case id_create:
      // read the binding

      break;
    }

  return dialog_wndproc(hwnd, msg, wnddata);
  }
