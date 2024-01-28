#ifndef __pancake_indicator_widget_h__
#define __pancake_indicator_widget_h__

#include "widget.h"

#include "converters.h"

#include <stdio.h>

/**
 * @brief A pancake indicator is made of 9 bars that
 * can display a converted can value.
 * The min_value is displayed at the left or top of the
 * indicator, while the max forms the other end.
*/
typedef struct _pancake_widget_t {
  widget_t base;

  // this is updated on each window message
  int16_t value;            // converted value
  int16_t min_value;        // minimum value
  int16_t max_value;        // maximum value
  uint16_t can_id;          // can-id of value

  rect_t indicator_rect;    // calculcated rectangle to draw the indicator in
  color_t outline_color;    // color of the indicator box
  color_t indicator_color;

  uint16_t bar_width;       // width of indicator (or height)
  uint16_t bar_height;      // height of indicator (or width)
  uint16_t bar_gutter;      // gutter width of indicator

  bool draw_value;          // draw the value
  const font_t *value_font; // what font to draw a value in
  rect_t value_rect;
  bool draw_value_box;      // if false then text is centered

  bool horizontal;          // horizontal indicator

  /**
   * @brief this should convert the can value to
   * the range of 0--100
  */
  const selected_unit_t* converter;

  } pancake_widget_t;

/**
 * @brief Create a package gauge window
 * @param wnd 
 * @param hndl 
 * @return 
*/
extern result_t create_pancake_widget(handle_t parent, uint16_t id, aircraft_t* aircraft, pancake_widget_t*wnd, handle_t *hndl);

#endif
