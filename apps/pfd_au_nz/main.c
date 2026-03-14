#include "../../src/mfdlib/mfd.h"
#include "../../src/proton/include/menu_window.h"
#include "../../src/mfdlib/event_handlers.h"
#include "../../src/mfdlib/menu_handlers.h"

// layout files
#include "en_nz.h"
#include "en_nz_priv.h"

uint32_t num_key_captions = 7;
key_number_t key_captions[7] = { kn_decka | kn_key0, kn_key2, kn_key3, kn_key4, kn_key5, kn_key6, kn_deckb | kn_key1 };

// this is defined in the layout.
static map_widget_t *nav_widget;

result_t on_create_navigator_widget(handle_t hwnd, widget_t *widget)
{
  nav_widget = (map_widget_t *) widget;

  return s_ok;
}

result_t get_range(menu_item_t* edit, variant_t* value)
  {
  int32_t range;
  map_get_range(nav_widget->map, &range);
  create_variant_int32(range, value);

  return s_ok;
  }

result_t set_range(menu_item_t* edit, const variant_t* value)
  {
  int32_t range;
  if (succeeded(coerce_to_int32(value, &range)))
    map_set_range(nav_widget->map, range);

  return s_ok;
  }

result_t get_range_max(menu_item_t* edit, variant_t* value)
  {
  create_variant_int32((int32_t)(100.0f * METERS_PER_NM), value);
  return s_ok;
  }

result_t get_mode(menu_item_t* edit, uint32_t* value)
  {
  map_display_mode mode = mdm_north;
  map_get_mode(nav_widget->map, &mode);

  *value = (uint32_t)mode;

  return s_ok;
  }

result_t get_water_mode(menu_item_t* edit, uint32_t* value)
  {
  result_t result;
  uint32_t layers = 0;
  if (failed(result = map_get_layer_visible(nav_widget->map, &layers)))
    return result;

  *value = (layers & MAP_LAYER_SURFACE_WATER) != 0 ? 1 : 0;

  return s_ok;
  }

result_t get_contours_mode(menu_item_t* edit, uint32_t* value)
  {
  result_t result;
  uint32_t layers = 0;
  if (failed(result = map_get_layer_visible(nav_widget->map, &layers)))
    return result;

  *value = (layers & MAP_LAYER_CONTOURS) != 0 ? 1 : 0;

  return s_ok;
  }

result_t get_cities_mode(menu_item_t* edit, uint32_t* value)
  {
  result_t result;
  uint32_t layers = 0;
  if (failed(result = map_get_layer_visible(nav_widget->map, &layers)))
    return result;

  *value = (layers & MAP_LAYER_CITIES) != 0 ? 1 : 0;

  return s_ok;
  }

result_t get_transport_mode(menu_item_t* edit, uint32_t* value)
  {
  result_t result;
  uint32_t layers = 0;
  if (failed(result = map_get_layer_visible(nav_widget->map, &layers)))
    return result;

  *value = (layers & MAP_LAYER_TRANSPORT) != 0 ? 1 : 0;

  return s_ok;
  }

result_t get_obstacles_mode(menu_item_t* edit, uint32_t* value)
  {
  result_t result;
  uint32_t layers = 0;
  if (failed(result = map_get_layer_visible(nav_widget->map, &layers)))
    return result;

  *value = (layers & MAP_LAYER_OBSTACLES) != 0 ? 1 : 0;

  return s_ok;
  }

static void change_layer_visibility(map_widget_t* wnd, uint32_t layer, bool visible)
  {
  uint32_t layers = map_get_layer_visible(wnd, nullptr);
  if (visible)
    layers |= layer;
  else
    layers &= ~layer;

  map_set_layer_visible(wnd, layers);
  }

static result_t my_wndproc(handle_t hndl, const canmsg_t* msg, void* wnddata)
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
        change_layer_visibility(nav_widget, MAP_LAYER_SURFACE_WATER, value != 0);
      break;
    case id_map_cities:
      if (succeeded(get_param_int16(msg, &value)))
        change_layer_visibility(nav_widget, MAP_LAYER_CITIES, value != 0);
      break;
    case id_map_contours:
      if (succeeded(get_param_int16(msg, &value)))
        change_layer_visibility(nav_widget, MAP_LAYER_CONTOURS, value != 0);
      break;
    case id_map_obstacles:
      if (succeeded(get_param_int16(msg, &value)))
        change_layer_visibility(nav_widget, MAP_LAYER_OBSTACLES, value != 0);
      break;
    case id_map_transport:
      if (succeeded(get_param_int16(msg, &value)))
        change_layer_visibility(nav_widget, MAP_LAYER_TRANSPORT, value != 0);
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

  return run_proton(0, create_layout, create_en_nz, my_wndproc);
  }
