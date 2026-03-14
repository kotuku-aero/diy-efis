#ifndef __tab_widget_h__
#define __tab_widget_h__

#include "dialog_widget.h"

#define TAB_HORIZONTAL       0x00000000
#define TAB_VERTICAL         0x01000000

typedef struct _tab_widget_t tab_widget_t;
typedef struct _panel_widget_t panel_widget_t;

typedef void (*on_tab_changed_fn)(tab_widget_t *tab, panel_widget_t *item);

typedef struct _tab_widget_t
  {
  dialog_widget_t base;

  rect_t tab_page_rect;     // the rectangle that describes the tab
  extent_t tab_name_extent;
  uint32_t num_tabs;       // number of tabs
  panel_widget_t **tabs;  // list of tabs to display
  uint32_t selected_index;  // index of the currently selected tab
  color_t selected_color;
  uint32_t focused_index;
  on_tab_changed_fn on_tab_changed;   // called when a new tab is selected
  } tab_widget_t;

extern result_t create_tab_widget(handle_t parent, uint32_t flags, tab_widget_t* wnd, handle_t* hndl);
extern result_t select_tab_page(handle_t widget, uint32_t index);

#endif
