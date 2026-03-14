#ifndef __gauge_widget_h__
#define __gauge_widget_h__

#include "widget.h"

#include "../../include/type_vector.h"
#include "../../include/vector.h"

#include "converters.h"

#include <stdio.h>

typedef enum
  {
  gs_pointer,         // a simple line
  gs_sweep,           // a pie sweep
  gs_bar,             // line sweep
  gs_point,           // simple-pointer
  gs_pointer_minmax,  // a pointer with min-max markers
  gs_point_minmax,    // simple-pointer with min-max markers
  gs_hbar,            // small horizonatal bar
  // bar graph styles
  bgs_point, // simple-pointer
  bgs_pointer_minmax, // a pointer with min-max markers
  bgs_pointer_max, // simple-pointer with max markers
  bgs_pointer_min,
  bgs_small,            // vertical bars with tick marks
  } gauge_style;

typedef struct
  {
  int32_t value;
  color_t pen;
  color_t color;
  } step_t;

typedef struct
  {
  int32_t value;
  const char *text;
  color_t color;
  uint16_t width;
  uint16_t length;
  } tick_mark_t;

typedef struct _gauge_widget_t {
  widget_t base;
  handle_t background_canvas; // canvas to draw the background on
  gauge_style style;
  const font_t* font;
  const selected_unit_t* converter;
  uint16_t can_id;
  uint16_t reset_id;
  bool incremental;           // used when drawing a gauge with 3 pointers

  // this draws a filled pie
  point_t center;
  int32_t gauge_radii;
  int32_t arc_width;
  uint16_t arc_begin;
  uint16_t arc_range;
  float reset_value;

  uint16_t bar_width;          // pointer or sweep width

  bool draw_value;         // draw the value
  bool draw_value_box;      // if false then text is centered
  const font_t* value_font;       // what font to draw a value in
  rect_t value_rect;

  const step_t *steps;
  size_t num_steps;
  const tick_mark_t *ticks;
  size_t num_ticks;


  // this is updated on each message
  float value;
  float min_value;
  float max_value;
  } gauge_widget_t;

/**
 * @brief Create a gauge window
 * @param wnd 
 * @param hndl 
 * @return 
*/
extern result_t create_gauge_widget(handle_t parent, uint32_t flags, gauge_widget_t *wnd, handle_t *hndl);

extern void on_paint_gauge_background(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnddata);
extern void on_paint_gauge(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnddata);

#endif
