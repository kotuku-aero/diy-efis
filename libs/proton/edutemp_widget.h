#ifndef __edutemp_widget_h__
#define __edutemp_widget_h__

#include "../photon/photon.h"
#include "../atomdb/spatial.h"
#include "widget.h"
#include "../graviton/aircraft.h"

#include <stdio.h>

typedef struct _edu_bar_t {
  uint16_t can_id;
  color_t default_color;
  uint16_t alarm_id;
  color_t alarm_color;
  const char *bar_name;
  uint16_t sensor_id;

  bool sensor_fail;
  bool is_alarm;
  bool show_max;
  uint16_t max_value;
  uint16_t value;
  } edu_bar_t;

typedef struct _edu_temp_widget_t {
  widget_t base;

  // the edu support up to 12 sensors (6 egt, 6 cht)
  uint16_t num_cylinders;
  edu_bar_t cht[6];
  edu_bar_t egt[6];

  uint16_t cht_red_line;
  uint16_t egt_red_line;
  uint16_t cht_min;
  uint16_t egt_min;
  int32_t left_gutter;
  int32_t right_gutter;
  int32_t cylinder_draw_width;
  int32_t egt_line;
  int32_t cht_line;
  int32_t cht_red_line_pos;
  int32_t bar_top;
  int32_t bar_bottom;
  int32_t status_top;
  color_t cht_color;
  color_t egt_color;
  const font_t* font;
  } edu_temp_widget_t;

extern result_t create_edutemps_widget(handle_t parent, uint16_t id, aircraft_t *aircraft, edu_temp_widget_t* wnd, handle_t* hndl);

#endif
