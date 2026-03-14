#ifndef __airspeed_widget_h__
#define __airspeed_widget_h__

#include "widget.h"
#include "converters.h"

typedef struct _airspeed_widget_t {
  widget_t base;

  handle_t background_canvas;

  // color to use for text
  color_t text_color;
  // pen color for lines
  color_t pen;
  // text font
  const font_t *font;
  // font to use for the large roller
  const font_t* large_roller;
  // font to use for the small rollwe
  const font_t* small_roller;

  ////////////////////////////////////////////////////////////
  //
  // Calculated data

  // vertical pixels
  uint16_t pixels_per_unit;

  // this is updated on each window message
  uint16_t airspeed;

  const selected_unit_t* converter;
  } airspeed_widget_t;

extern result_t create_airspeed_widget(handle_t parent, uint32_t flags, airspeed_widget_t* wnd, handle_t* hndl);

#endif
