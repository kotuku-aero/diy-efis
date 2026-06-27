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

  int32_t course;             // course (is true)
  int32_t track;              // current track (is true)
  int32_t magnetic_heading;   // current heading is magnetic
  int32_t mag_var;            // magnetic variation.
  int32_t true_heading;
  int16_t distance_to_waypoint;
  int16_t time_to_waypoint;
  uint16_t wind_direction; // always true
  int16_t wind_speed;
  char waypoint_name[8];
  // reported by the kHUB
  uint16_t autopilot_mode;
  // current assigned altitude in feed AMSL
  uint16_t ap_altitude;
  // current vertical speed assigned
  uint16_t ap_vs;

  // this is created by the yehudi compiler from the
  // color definitions
  map_theme_t day_theme;
  map_theme_t night_theme;

  const char *config_path;
  const font_t *font;

  map_display_mode map_mode;
  // this is the create map canvas that forms the background
  // of the widget
  handle_t map;
  } map_widget_t;

/**
 * @brief Create a widget that is a moving map display
 * @param parent      Owner parent
 * @param spatial_db  Handle of the previously opened spatial database
 * @param flags       Widget flags
 * @param wnd         Widget creation parameters
 * @param hndl        Resulting widget
 * @return s_ok if the map was created
 */
extern result_t create_map_widget(handle_t parent, handle_t spatial_db, uint32_t flags, map_widget_t* wnd, handle_t* hndl);

extern void set_gps_position(map_widget_t *widget, const lla_t *position);
extern void set_map_range(map_widget_t *widget, int32_t range);
extern void get_map_range(map_widget_t *widget, int32_t *range);
extern void set_map_mode(map_widget_t *widget, map_display_mode mode);
extern void get_map_mode(map_widget_t *widget, map_display_mode *mode);
extern void get_map_contours_visible(map_widget_t *wnd, bool *visible);
extern void set_map_contours_visible(map_widget_t *widget, bool visible);
extern void get_map_cities_visible(map_widget_t *wnd, bool *visible);
extern void set_map_cities_visible(map_widget_t *widget, bool visible);
extern void get_map_water_visible(map_widget_t *widget, bool *visible);
extern void set_map_water_visible(map_widget_t *widget, bool visible);
extern void get_map_transport_visible(map_widget_t *widget, bool *visible);
extern void set_map_transport_visible(map_widget_t *widget, bool visible);
extern void get_map_obstacles_visible(map_widget_t *widget, bool *visible);
extern void set_map_obstacles_visible(map_widget_t *widget, bool visible);


#endif
