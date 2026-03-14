#ifndef __compass_h__
#define __compass_h__

#include "widget.h"
#include "../mfdlib/mfd.h"

typedef struct _compass_widget_t {
  widget_t base;
  uint16_t magnetic_heading;
  uint16_t true_heading;
  uint16_t course;
  int16_t deviation;
  uint16_t track;
  int16_t distance_to_waypoint;
  int16_t time_to_waypoint;
  uint16_t wind_direction; // always true
  int16_t wind_speed;
  int16_t magnetic_variation;
  uint16_t heading_bug;

  // configuration values
  uint16_t hdg_offset;    // offset to where the compass center is drawn,
                          // Allows for the alignment of the heading bug to
                          // be at the center of the attitude indicator
} compass_widget_t;

extern result_t create_compass_widget(handle_t parent, uint32_t flags, compass_widget_t* wnd, handle_t* out);

#endif
