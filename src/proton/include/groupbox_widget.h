#ifndef __groupbox_widget_h__
#define __groupbox_widget_h__

#include "bound_widget.h"

typedef struct _groupbox_widget_t
  {
  data_bound_widget_container_t base;

  } groupbox_widget_t;

extern result_t create_groupbox_widget(handle_t parent, uint32_t flags, groupbox_widget_t* wnd, handle_t* hndl);

#endif
