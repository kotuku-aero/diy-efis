#ifndef __dialog_widget_h__
#define __dialog_widget_h__

#include "widget.h"
#include "dialog_theme.h"

typedef struct _dialog_widget_t
  {
  widget_t base;

  // this is the name of the menu that will be selected when
  // the widget gets the focus.
  const char *menu_name;
  // set when the widget has the focus for input messages
  bool focused;
  on_event_fn on_focus;
  } dialog_widget_t;

extern void on_paint_dialog_widget_decorations(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, dialog_widget_t* widget);
extern result_t dialog_wndproc(handle_t hwnd, const canmsg_t* msg, void* wnddata);

#endif
