#ifndef __listbox_widget_h__
#define __listbox_widget_h__

#include "bound_widget.h"

typedef struct _listbox_widget_t {
  data_bound_widget_t base;

  int32_t selected_item;
} listbox_widget_t;

extern result_t create_listbox_widget(handle_t parent, uint32_t flags, listbox_widget_t *wnd, handle_t *hndl);

#endif
