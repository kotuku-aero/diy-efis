#ifndef __menu_handlers_h__
#define __menu_handlers_h__

#include "../proton/include/menu_window.h"
#include "mfd.h"

extern mfd_wnd_t mfd;

extern result_t get_direct_to_waypoint(menu_item_t* edit, variant_t* value);
extern result_t get_nearest_airport_name(menu_item_t* edit, variant_t* value);
extern result_t get_nearest_vor_name(menu_item_t* edit, variant_t* value);
extern result_t get_nearest_ndb_name(menu_item_t* edit, variant_t* value);
extern result_t get_current_flight_plan(menu_item_t* edit, uint32_t* value);

typedef enum
  {
  id_hnav_mode = id_app_action_id_first,
  id_vnav_mode,
  id_toggle_hs_mode,
  id_toggle_vs_mode,
  id_vs_up,
  id_vs_dn,
  id_set_nodeid,
  id_cancel_alarm,
  id_snooze_alarm,
  id_map_water,
  id_map_contours,
  id_map_cities,
  id_map_transport,
  id_map_obstacles,
  id_map_mode,
  id_tab_next,
  id_tab_prev,
  id_tab_up,
  id_tab_down,
  id_set_config_view,
  id_decal_orientation,
  id_air_data_ports,
  id_cylinder_count,
  id_fuel_sensor_type,
  } action_id_t;

#endif
