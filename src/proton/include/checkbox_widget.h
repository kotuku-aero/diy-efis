#ifndef __checkbox_widget_h__
#define __checkbox_widget_h__

#include "bound_widget.h"

typedef struct _checkbox_widget_t
  {
  data_bound_widget_t base;
  } checkbox_widget_t;

extern result_t create_checkbox_widget(handle_t parent, uint32_t flags, checkbox_widget_t* wnd, handle_t* hndl);

#endif
