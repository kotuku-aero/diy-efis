#ifndef __altitude_widget_h__
#define __altitude_widget_h__

#include "widget.h"
#include "../graviton/aircraft.h"

#include <stdio.h>

typedef struct _altitude_widget_t {
  widget_t base;

  handle_t background_canvas;

  color_t text_color;
  color_t pen;
  const font_t* font;           // neo 9
  const font_t* small_roller;  // neo 12
  const font_t* large_roller;  // neo 15

  // runtime values
  aircraft_t *aircraft;
  int16_t altitude;
  int16_t vertical_speed;
  uint16_t qnh;

  } altitude_widget_t;

extern result_t create_altitude_widget(handle_t parent, uint16_t id, aircraft_t *aircraft, altitude_widget_t* wnd, handle_t* hndl);


#endif
