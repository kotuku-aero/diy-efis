#include "../../src/mfdlib/mfd.h"
#include "../../src/proton/include/menu_window.h"
#include "../../src/mfdlib/event_handlers.h"
#include "../../src/mfdlib/menu_handlers.h"

// layout files
#include "au_nz.h"
#include "au_nz_priv.h"

uint32_t num_key_captions = 7;
key_number_t key_captions[7] = { kn_decka | kn_key0, kn_key2, kn_key3, kn_key4, kn_key5, kn_key6, kn_deckb | kn_key1 };

static map_widget_t *nav_widget;

result_t get_range(menu_item_t *edit, variant_t *value)
  {
  uint32_t range;
  map_get_range(nav_widget, &range);

  // range is meters * 1000, convert to NM
  float nm = meters_to_nm(range * 1000);

  create_variant_uint32((uint32_t)nm, value);
  return s_ok;
  }

result_t set_range(menu_item_t *edit, const variant_t *value)
  {
  uint32_t nm;
  coerce_to_uint32(value, &nm);
  float mtrs = nm_to_meters(nm);

  map_set_range(nav_widget, mtrs / 1000);
  }

result_t get_range_max(menu_item_t *edit, variant_t *value) 
  {
  create_variant_uint32(120, value);
  return s_ok;
  }

result_t get_mode(menu_item_t *edit, uint32_t *value)
  {
  map_display_mode mode;
  get_map_mode(nav_widget, &mode);

  *value = (uint32_t)mode;
  return s_ok;
  }

result_t get_water_mode(menu_item_t *edit, uint32_t *value)
  {
  bool visible;
  get_map_water_visible(nav_widget, &visible);

  *value = visible ? 1 : 0;
  return s_ok;
  }

result_t get_contours_mode(menu_item_t *edit, uint32_t *value)
  {
  bool visible;
  get_map_contours_visible(nav_widget, &visible);

  *value = visible ? 1 : 0;
  return s_ok;
  }

result_t get_cities_mode(menu_item_t *edit, uint32_t *value) 
  {
  bool visible;
  get_map_cities_visible(nav_widget, &visible);

  *value = visible ? 1 : 0;
  return s_ok;
  }

result_t get_transport_mode(menu_item_t *edit, uint32_t *value)
  {
  bool visible;
  get_map_transport_visible(nav_widget, &visible);

  *value = visible ? 1 : 0;
  return s_ok;
  }

result_t get_obstacles_mode(menu_item_t *edit, uint32_t *value)
  {
  bool visible;
  get_map_obstacles_visible(nav_widget, &visible);

  *value = visible ? 1 : 0;
  return s_ok;
  }


result_t on_create_nav(handle_t hwnd, widget_t *widget) 
  {
  nav_widget = (map_widget_t *)widget;
  return s_ok;
  }

result_t my_wndproc(handle_t hndl, const canmsg_t* msg, void* wnddata)
  {
  int16_t value;
  switch (get_can_id(msg))
    {
    case id_map_mode:
      if (succeeded(get_param_int16(msg, &value)))
        map_set_mode(nav_widget, (map_display_mode)value);
      break;
    case id_map_water:
      if(succeeded(get_param_int16(msg, &value)))
        set_map_water_visible(nav_widget, value != 0);
      break;
    case id_map_cities:
      if (succeeded(get_param_int16(msg, &value)))
        set_map_cities_visible(nav_widget, value != 0);
      break;
    case id_map_contours:
      if (succeeded(get_param_int16(msg, &value)))
        set_map_contours_visible(nav_widget, value != 0);
      break;
    case id_map_obstacles:
      if (succeeded(get_param_int16(msg, &value)))
        set_map_obstacles_visible(nav_widget, value != 0);
      break;
    case id_map_transport:
      if (succeeded(get_param_int16(msg, &value)))
        set_map_transport_visible(nav_widget, value != 0);
      break;
    }

  return mfd_wndproc(hndl, msg, wnddata);
  }

#ifdef _MSC_VER
// debug arguments:
// -c 4096 -x 1024 -y 600 canfly.cdb
int main(int argc, char** argv)
  {
  if (failed(canfly_init(argc, argv)))
    return -1;
#else
int main()
  {
#endif

  return run_proton(0, create_layout, create_au_nz, my_wndproc);
  }
