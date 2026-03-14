#ifndef __combobox_widget_h__
#define __combobox_widget_h__

#include "bound_widget.h"
#include "menu_window.h"

typedef struct _combobox_widget_item_t {
  const char *text;
  uint16_t can_id;
  variant_t value;
  } combobox_widget_item_t;

typedef struct _combobox_item_select_event_args_t
  {
  event_args_t base;

  uint32_t index;
  combobox_widget_item_t *item;
  } combobox_item_select_event_args_t;

typedef struct _combobox_widget_t {
  data_bound_widget_t base;

  // these are the added/removed items
  combobox_widget_item_t *items;
  uint32_t num_items;
  int32_t selected_item;

  uint16_t menu_can_id;

  // events.
  on_event_fn on_item_selected;
  } combobox_widget_t;

extern result_t create_combobox_widget(handle_t parent, uint32_t flags, combobox_widget_t* wnd, handle_t* hndl);

#endif
