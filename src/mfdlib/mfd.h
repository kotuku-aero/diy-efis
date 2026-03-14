#ifndef __mfd_h__
#define __mfd_h__

#include "../proton/include/proton.h"
#include "../proton/include/converters.h"
#include "../proton/include/menu_window.h"

typedef struct _mfd_wnd_t {
  widget_t base;
  uint16_t left_fuel_map;
  uint16_t right_fuel_map;
  uint16_t selected_fuel_map;
  // fuel totals are in CC
  uint32_t fuel_total;
  uint32_t max_fuel_available;
  uint16_t autopilot_mode;
  // assigned altitude in meters
  float altitude;
  // assigned vertical speed in m/s
  float vertical_speed;
  memid_t key;

  const selected_unit_t* altitude_converter;
  const selected_unit_t* airspeed_converter;
  const selected_unit_t* distance_converter;
  const selected_unit_t* fuel_converter;
  const selected_unit_t* temp_converter;
  const selected_unit_t* vertical_speed_converter;
  const selected_unit_t* flow_converter;

  // loaded at startup, either from the local configuration database
  // or the kHUB avionics hub
  aircraft_t aircraft;

  } mfd_wnd_t;

extern mfd_wnd_t mfd;

/**
 * @brief Create a layout based on the layout definition
 * @param parent
 * @param key
 * @param aircraft
 * @param layout_fn
 * @return
*/
extern result_t create_layout(handle_t parent, memid_t key, create_widgets_fn layout_fn, wndproc_fn wndproc, handle_t* hndl);
/**
 * @brief Processes a message for a window in the MFD (Multi-Function Display) system.
 * @param wnd A handle to the window receiving the message.
 * @param msg A pointer to the message structure to be processed.
 * @param wnddata A pointer to window-specific data.
 * @return A result code indicating the outcome of the message processing.
 */
extern result_t mfd_wndproc(handle_t wnd, const canmsg_t* msg, void* wnddata);
/**
 * @brief Change the default layout to the given mode
 * @param mode mode to be selected
 * 
 * This should be implemented by the application to change the layout
 */
extern result_t set_selected_layout(uint32_t mode);

// these are provided by the application layer.
/**
 * Return the layout function for a selected layout
 * @param layout_number   Opaque number of the layout
 * @param layout_fn       Window creator
 * @return s_ok if a valid window type
 */
extern result_t get_widget_creator(uint32_t layout_number, create_widgets_fn *layout_fn);

/**
 * Get the selected layout for the menu editor
 * @param edit    Current menu item
 * @param value   Index of the layout
 * @return s_ok if it works
 */
extern result_t get_selected_layout(menu_item_t* edit, uint32_t* value);

#ifdef _MSC_VER
#define __builtin_ctz(x) _tzcnt_u32(x)
#endif

#endif
