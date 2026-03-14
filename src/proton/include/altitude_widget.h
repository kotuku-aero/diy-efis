#ifndef __altitude_widget_h__
#define __altitude_widget_h__

#include "widget.h"
#include "converters.h"

#include <stdio.h>

typedef struct _altitude_widget_t {
  widget_t base;

  //handle_t background_canvas;

  color_t text_color;
  color_t pen;
  const font_t* font;           // neo 9
  const font_t* small_roller;  // neo 12
  const font_t* large_roller;  // neo 15

  int16_t assigned_altitude;
  int16_t assigned_altitude_rate;
  int16_t altitude;
  int16_t vertical_speed;
  float qnh;

  const selected_unit_t* altitude_converter;
  const selected_unit_t* alt_rate_converter;
  const selected_unit_t* qnh_converter;

  } altitude_widget_t;

extern result_t create_altitude_widget(handle_t parent, uint32_t flags, altitude_widget_t* wnd, handle_t* hndl);


#endif
