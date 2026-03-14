#ifndef __bound_widget_h__
#define __bound_widget_h__

#include "dialog_widget.h"
#include "converters.h"

typedef struct _data_bound_widget_container_t data_bound_widget_container_t;
typedef struct _data_bound_widget_t data_bound_widget_t;


/*
 * These are called when a data bound widget needs data from a data source
 */
typedef result_t(*on_get_value_fn)(data_bound_widget_t *widget, variant_t *value);
typedef result_t(*on_set_value_fn)(data_bound_widget_t *widget, const variant_t *value);

// the node bindings are specific to the physical node types.
// located as atom/board_id.h
// these are psuedo bindings that allow the client to work.
#define local_node_id 16
#define mag_node_id 17

/**
 * @brief A widget bound to a registry key
  */
typedef struct _data_bound_widget_t
  {
  dialog_widget_t base;

  uint16_t node_id;           // node the setting is stored in (16 for local)
  const char *path;           // name of the setting
  // this is Canfly__Neutron__EFieldDatatype in the protobuf, check that it matches
  field_datatype datatype;    // datatype of the setting
  data_bound_widget_container_t *container; // reference to the container.

  variant_t default_value;    // default value to use if the setting is not found
  variant_t value;            // current value of the setting
  variant_t min_value;        // minimum value of the setting (optional)
  variant_t max_value;        // maximum value of the setting (optional)

  const selected_unit_t *converter;     // optional variant conversion

  on_get_value_fn get_value;
  on_set_value_fn set_value;
  } data_bound_widget_t;

// these are called if the get/set functions are not provided.  They will read the values
// using the node_id and path to access registry values.
extern result_t default_data_bound_widget_get_value(data_bound_widget_t *widget, variant_t *value);
extern result_t default_data_bound_widget_set_value(data_bound_widget_t *widget, const variant_t *value);

extern result_t data_bound_widget_wndproc(handle_t hwnd, const canmsg_t *msg, void *wnddata);

typedef struct _data_bound_widget_container_t
  {
  dialog_widget_t base;

  uint16_t node_id;           // node the setting is stored in (16 for local)
  const char *path;           // key to read/write the setting from/to

  data_bound_widget_t **contents;
  uint32_t size;

  } data_bound_widget_container_t;


#endif
