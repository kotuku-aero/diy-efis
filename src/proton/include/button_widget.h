#ifndef __button_widget_h__
#define __button_widget_h__

#include "bound_widget.h"

typedef struct _button_widget_t button_widget_t;
typedef void (*on_pressed_fn)(button_widget_t *btn);

typedef struct _button_widget_t {
  data_bound_widget_t base;

  const char *text;             // text to display on the button
  const bitmap_t *bitmap;       // optional bitmap to draw on the button
  
  on_pressed_fn on_pressed;
  } button_widget_t;

extern result_t create_button_widget(handle_t parent, uint32_t flags, button_widget_t* wnd, handle_t* hndl);

#endif
