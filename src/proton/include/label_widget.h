#ifndef __label_widget_h__
#define __label_widget_h__

#include "bound_widget.h"

typedef struct _label_widget_t
  {
  widget_t base;

  } label_widget_t;

extern result_t create_label_widget(handle_t parent, uint32_t flags, label_widget_t* wnd, handle_t* hndl);

#endif
