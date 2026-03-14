#ifndef __panel_widget_h__
#define __panel_widget_h__


#include "bound_widget.h"

typedef struct _panel_widget_t
  {
  data_bound_widget_container_t base;

  } panel_widget_t;

extern result_t create_panel_widget(handle_t parent, uint32_t flags, panel_widget_t* wnd, handle_t* hndl);

#endif
