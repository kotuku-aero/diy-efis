#ifndef __hsi_widget_h__
#define __hsi_widget_h__

#include "widget.h"
#include "menu_window.h"
#include <stdio.h>

typedef struct _hsi_widget_t {
  widget_t base;
  uint16_t direction;
  uint16_t course;
  int16_t deviation;
  uint16_t track;
  int16_t distance_to_waypoint;
  int16_t time_to_waypoint;
  uint16_t wind_direction; // always true
  int16_t magnetic_variation;
  int16_t wind_speed;
  char waypoint_name[17];
  uint16_t heading_bug;
  int16_t heading;

  // reported by the kHUB
  uint16_t autopilot_mode;
  float altitude;

  const selected_unit_t* converter;
  } hsi_widget_t;

extern result_t create_hsi_widget(handle_t parent, uint32_t flags, hsi_widget_t* wnd, handle_t* hndl);

// menu handlers
extern void on_toggle_hs_mode(menu_widget_t *menu, widget_t *hsi);
extern void on_toggle_vs_mode(menu_widget_t* menu, widget_t *hsi);
extern result_t on_create_hsi_widget(handle_t hwnd, widget_t* widget);

#endif // !__hsi_window_h__
