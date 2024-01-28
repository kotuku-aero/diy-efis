#ifndef __airspeed_widget_h__
#define __airspeed_widget_h__

#include "../photon/photon.h"
#include "widget.h"
#include "../graviton/aircraft.h"

#include <stdio.h>

typedef struct _airspeed_widget_t {
  widget_t base;
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
  // performance details of the aircraft
  aircraft_t* aircraft;

  ////////////////////////////////////////////////////////////
  //
  // Calculated data

  // vertical pixels
  uint16_t pixels_per_unit;

  // this is updated on each window message
  uint16_t airspeed;


  } airspeed_widget_t;

extern result_t create_airspeed_widget(handle_t parent, uint16_t id, aircraft_t *aircraft, airspeed_widget_t* wnd, handle_t* hndl);

#endif
