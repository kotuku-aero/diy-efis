#ifndef __map_widget_h__
#define __map_widget_h__

#include "widget.h"

typedef struct _map_widget_t map_widget_t;

extern void set_gps_position(map_widget_t *widget, const lla_t *position);

typedef struct _map_widget_t {
  widget_t base;
  // actual display window
  handle_t hwnd;
  extent_t font_cell_size;

  int32_t course;             // course
  int32_t track;              // current track    
  int32_t heading;            // current heading
  lla_t gps_position;

  // this is created by the yehudi compiler from the
  // color definitions
  map_theme_t day_theme;
  map_theme_t night_theme;

  point_t map_center;
  const char *db_path;
  const char *config_path;
  const font_t *font;

  map_display_mode map_mode;

  handle_t map;
  } map_widget_t;

extern result_t create_map_widget(handle_t parent, uint32_t flags, map_widget_t* wnd, handle_t* hndl);

#endif
