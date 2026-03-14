#ifndef __edit_widget_h__
#define __edit_widget_h__

#include "bound_widget.h"

typedef struct _edit_widget_t {
  data_bound_widget_t base;

  variant_t value;

  // events
  on_event_fn on_text_changed;
  } edit_widget_t;

extern result_t create_edit_widget(handle_t parent, uint32_t flags, edit_widget_t* wnd, handle_t* hndl);

#endif
