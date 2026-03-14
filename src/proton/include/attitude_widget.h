#ifndef __attitude_widget_h__
#define __attitude_widget_h__

#include "widget.h"

static const int32_t pixels_per_degree = 49;

typedef struct _attitude_widget_t {
  widget_t base;

  int16_t pitch;
  int16_t roll;
  int16_t yaw;

  uint16_t aoa_degrees;

  float aoa_pixels_per_degree;
  float aoa_degrees_per_mark;

  int16_t glideslope;
  int16_t localizer;
  bool glideslope_aquired;
  bool localizer_aquired;

  bool show_aoa;
  bool show_glideslope;

  extent_t aoa_warning_extent;
  rect_t skid_indicator;

  gdi_dim_t yaw_scale;

  point_t median;
  } attitude_widget_t;

extern result_t create_attitude_widget(handle_t parent, uint32_t flags, attitude_widget_t* wnd, handle_t* hndl);

#endif
