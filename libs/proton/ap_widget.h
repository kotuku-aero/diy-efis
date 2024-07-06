#ifndef __ap_widget_h__
#define __ap_widget_h__

#include "widget.h"
#include "../graviton/aircraft.h"

#include <stdio.h>

typedef enum {
  ap_off,         // no autopilot
  ap_hdg_alt,     // altitude and heading
  ap_crs_alt,     // altitude and gps course
  ap_hdg,         // heading hold
  ap_crs,         // gps course
  ap_alt,         // alt hold
  ap_180_left,    // alt hold, 180 reversing turn left
  ap_180_right,   // alt hold, 180 reversing turn right
  } ap_mode;

typedef struct _ap_widget_t {
  widget_t base;
  handle_t background_canvas;

  extent_t font_cell_size;
  aircraft_t* aircraft;
  ap_mode mode;

  // current waypoint we are heading to.
  uint16_t dist_to_waypoint;    // distance to next (should use converter!)
  int16_t time_to_next;         // time_to_next
  uint16_t altitude_set;        // current selected (use converter)
  uint16_t vertical_speed;      // current vertical speed (use converter)

  // text to display as waypoint
  char waypoint[16];

  uint16_t button_height; // hight of the mode button
  uint16_t height;      // hight of a ap parameter (includes gutter)
  uint16_t gutter;      // pixels between values

  color_t info_color;
  color_t active_color;
  color_t standby_color;
  
  // these are calculated
  rect_t mode_button;
  rect_t wpt_label;
  rect_t wpt_value;
  rect_t dst_label;
  rect_t dst_value;
  rect_t ete_label;
  rect_t ete_value;
  rect_t alt_label;
  rect_t alt_value;
  rect_t vs_label;
  rect_t vs_value;
  } ap_widget_t;

extern result_t create_autopilot_widget(handle_t parent, uint16_t id, aircraft_t* aircraft, ap_widget_t* wnd, handle_t* hndl);

#endif
