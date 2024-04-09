#ifndef __hsi_widget_h__
#define __hsi_widget_h__

#include "widget.h"
#include "../atomdb/spatial.h"
#include "../graviton/aircraft.h"
#include <stdio.h>

enum {
  ds_course,
  ds_heading,
  };


typedef struct _hsi_widget_t {
  widget_t base;
  int16_t direction;
  int16_t course;
  int16_t deviation;
  int16_t track;
  int16_t distance_to_waypoint;
  int16_t time_to_waypoint;
  int16_t wind_direction; // always true
  int16_t magnetic_variation;
  int16_t wind_speed;
  char waypoint_name[17];
  int16_t heading_bug;
  int16_t heading;
  aircraft_t *aircraft;

  handle_t background_canvas;
  } hsi_widget_t;

extern result_t create_hsi_widget(handle_t parent, uint16_t id, aircraft_t *aircraft, hsi_widget_t* wnd, handle_t* hndl);



#endif // !__hsi_window_h__
