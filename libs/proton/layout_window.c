/*
diy-efis
Copyright (C) 2016 Kotuku Aerospace Limited

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

If a file does not contain a copyright header, either because it is incomplete
or a binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice and
the GPL3 are subservient to that copyright notice.

Portions of this repository contain code fragments from the following
providers.


If any file has a copyright notice or portions of code have been used
and the original copyright notice is not yet transcribed to the repository
then the origional copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
#include "../photon/widget.h"
#include "regex.h"
#include "pens.h"

#include "../photon/window.h"

extern result_t create_airspeed_window(handle_t parent, memid_t section, handle_t *hwnd);
extern result_t create_altitude_window(handle_t parent, memid_t section, handle_t *hwnd);
extern result_t create_annunciator_window(handle_t parent, memid_t section, handle_t *hwnd);
extern result_t create_attitude_window(handle_t parent, memid_t section, handle_t *hwnd);
extern result_t create_gauge_window(handle_t parent, memid_t section, handle_t *hwnd);
extern result_t create_gps_window(handle_t parent, memid_t section, handle_t *hwnd);
extern result_t create_hsi_window(handle_t parent, memid_t section, handle_t *hwnd);
extern result_t create_widget(handle_t parent, memid_t section, handle_t *hwnd);

  enum window_type
    {
    airspeed_window,            // vertical ticker-tape airspeed window
    hsi_window,                 // rectangular gyro position and heading
    attitude_window,            // rectangular attitude window
    altitude_window, // vertical ticker tape altitude and vertical speed indicator window
    gauge_window,               // general purpose round or bar gauge
    annunciator_window,         // time, oat etc.
    gps_window,                 // gps route map window
    alert_window,               // vertical alert window
    widget,                     // a generic widget
    last_layout_enum
    };

static const char *layout_names[] = {
    "airspeed",      // vertical ticker-tape airspeed window
    "hsi",           // rectangular gyro position and heading
    "attitude",      // rectangular attitude window
    "altitude",      // vertical ticker tape altitude and vertical speed indicator window
    "gauge",         // general purpose round or bar gauge
    "annunciator",   // time, oat etc.
    "gps",           // gps route map window
    "alert",         // vertical alert window
    "widget",               // a generic widget
};

typedef enum
  {
  mi_menu,      // item is a menu
  mi_cancel,    // item is a cancel option
  mi_enter,     // item is an accept option
  mi_event,     // item is an event generator
  mi_edit,      // item is a property editor
  mi_checklist, // item is a selection item
  } menu_item_type;

typedef enum
  {
  mia_nothing, // no change to menu state
  mia_cancel, // event causes a cancel
  mia_enter, // event causes a done, all menu close
  mia_close_item, // either a popup or edit is closed
  } menu_item_action_result;

struct _layout_window_t;
struct _menu_item_t;
typedef void(*item_paint_fn)(handle_t hwnd, struct _layout_window_t *wnd, const rect_t *wnd_rect, struct _menu_item_t *item, const rect_t *rect);
typedef menu_item_action_result(*item_evaluate_fn)(struct _layout_window_t *wnd, struct _menu_item_t *item, const canmsg_t *msg);
typedef void(*item_edit_fn)(struct _layout_window_t *wnd, struct _menu_item_t *item, const canmsg_t *msg);
typedef bool(*item_is_enabled_fn)(struct _layout_window_t *wnd, struct _menu_item_t *item, const canmsg_t *msg);
typedef void(*item_msg_event_fn)(struct _layout_window_t *wnd, struct _menu_item_t *item, const canmsg_t *msg);

typedef struct _menu_item_t
  {
  menu_item_type item_type;
  char caption[REG_STRING_MAX];

  char enable_regex[REG_STRING_MAX];
  char enable_format[REG_STRING_MAX];

  uint16_t controlling_param;
  canmsg_t controlling_variable;

  // set by parent menu so the item will highlight itself
  bool is_selected;
  bool editor_open; // if true the popup menu editor is active

  TRex *pat_buff;

  item_paint_fn paint;
  item_evaluate_fn evaluate;
  item_edit_fn edit;
  item_is_enabled_fn is_enabled;
  item_msg_event_fn event;
  } menu_item_t;

/**
* Assign the expression that is used to test if this menu item is
* enabled
* @param expr expression in the form <param>:<expression>
*/
static void item_assign_enabler(menu_item_t *item, const char *expr);

/* This describes the handlers for a menu.
*/
typedef struct _keys_t {
  menu_item_t *key0;
  menu_item_t *key1;
  menu_item_t *key2;
  menu_item_t *key3;
  menu_item_t *key4;
  menu_item_t *decka_up;
  menu_item_t *decka_dn;
  menu_item_t *deckb_up;
  menu_item_t *deckb_dn;
  } keys_t;

typedef struct _menu_t
  {
  // a list of items in the menu.  is a vector.
  vector_p menu_items;
  char *name;         // name of the menu
                      // this is the menu item with the current focus.  All ok, cancel is sent
                      // here.  This is not the selected active menu (only a popup)
  uint16_t active_item;

  // bottom menu item displayed.  The menu will expand to fit the
  // vertical height of the screen.
  uint16_t selected_index;

  // if there is a popup active, this is keys handler
  keys_t *keys;
  } menu_t;

struct _layout_window_t;

static menu_item_t *menu_item_at(menu_t *menu, uint16_t index);
static uint16_t menu_count(menu_t *menu);
static void draw_menu_item(handle_t hwnd,
  struct _layout_window_t *wnd,
  const rect_t *wnd_rect,
  menu_item_t *menu,
  const point_t *menu_origin);

typedef struct _layout_window_t
  {
  uint16_t version;
  // this holds the screen window definitions.  Filled in with
  // the attach_window hidden system call
  handle_t window;
  /**
  * This holds the global root menu for the system
  */
  keys_t *root_keys;
  /**
  * this holds the active key handler for the current popup menu.  If there is
  * no popup active the root handler is used.
  */
  keys_t *active_keys;

  /**
  * This holds the current popup menu that can be a property editor or
  * action items
  */
  menu_t *current_menu;

  int16_t menu_timer; // as a press/rotate is given this sets the tick-timeout

  gdi_dim_t menu_rect_x;
  gdi_dim_t menu_rect_y;
  gdi_dim_t menu_start_x;
  gdi_dim_t menu_start_y;

  memid_t key;            // window key
  // map of root menus
  map_p key_mappings;

  // map of menus
  map_p menus;
  // stack of menu's
  vector_p menu_stack;

  // we hold a reference to all loaded menu items as they need to be informed
  // if a controlling variable changed
  vector_p menu_items;

  color_t background_color;
  color_t selected_background_color;
  color_t text_color;              // text color
  color_t border_color;
  color_t selected_color;
  pen_t border_pen;
  handle_t  font;
  } layout_window_t;

/**
* Close the current popup menu
* and restore the previous menu
*/
static void pop_menu(layout_window_t *wnd);
/**
* Close all menu's.  Called by the cancel: menu item
*/
static void close_menu(layout_window_t *wnd);
/**
* Show a popup menu at the left of the menu window.
* @param Popup           menu to display
* @param option_selected Option that is selected. Will be the bottom option
*/
static void show_menu(layout_window_t *wnd, menu_t *);
/**
* Change the first display item to list item-1 if possible
*/
static void scroll_menu_up(layout_window_t *wnd);
/**
* Change the first display item to list item +1 if possible
*/
static void scroll_menu_down(layout_window_t *wnd);
/**
* Open the popup menu editor and show the edit menu
*/
static void show_item_editor(layout_window_t *wnd, menu_item_t *item);

/**
* Load a menu from the registry and cache it
* @param wnd
* @param name
* @param menu
* @return
*/
static result_t find_keys(layout_window_t *wnd, const char *name, keys_t **menu);
/**
* Find a popup menu id that matches the name given
* @param wnd     Menu window parameters
* @param name    Name of the popup menu
* @param memid   Resulting memid, if found
* @return s_ok if the menu is found, e_not_found if no match
*/
static result_t find_menu(layout_window_t *wnd, const char *name, menu_t **menu);

typedef struct _datatype_lookup_t {
  const char *name;
  uint8_t datatype;
  uint16_t length;
  } datatype_lookup_t;

static datatype_lookup_t datatypes[] = {
    { "SHORT", CANAS_DATATYPE_SHORT, 2 },
    { "FLOAT", CANAS_DATATYPE_FLOAT, 4 },
    { 0, CANAS_DATATYPE_NODATA, 0 }
  };

static result_t load_can_msg(memid_t key, canmsg_t *msg)
  {
  uint16_t id;
  if (succeeded(reg_get_uint16(key, "can-id", &id)))
    {
    char type[REG_STRING_MAX] = { 0 };
    if (succeeded(reg_get_string(key, "can-type", type, 0)))
      {
      char value[REG_STRING_MAX] = { 0 };
      // decode the message
      if (strcmp(type, "NODATA") == 0 ||
        failed(reg_get_string(key, "can-value", value, 0)))
        {
        msg->id = id;
        msg->canas.data_type = CANAS_DATATYPE_NODATA;
        }
      else
        {
        if (strcmp(type, "ERROR") == 0)
          {
          uint32_t v = strtoul(value, 0, 0);
          create_can_msg_uint32(msg, id, 0, v);
          msg->canas.data_type = CANAS_DATATYPE_ERROR;
          }
        else if (strcmp(type, "FLOAT") == 0)
          {
          float v = strtof(value, 0);
          create_can_msg_float(msg, id, 0, v);
          }
        else if (strcmp(type, "LONG") == 0)
          {
          int32_t v = strtol(value, 0, 0);
          create_can_msg_int32(msg, id, 0, v);
          }
        if (strcmp(type, "ULONG") == 0)
          {
          uint32_t v = strtoul(value, 0, 0);
          create_can_msg_uint32(msg, id, 0, v);
          }
        else if (strcmp(type, "SHORT") == 0)
          {
          int16_t v = (int16_t)strtol(value, 0, 0);
          create_can_msg_int16(msg, id, 0, v);
          }
        else if (strcmp(type, "USHORT") == 0)
          {
          uint16_t v = (uint16_t)strtoul(value, 0, 0);
          create_can_msg_uint16(msg, id, 0, v);
          }
        else if (strcmp(type, "CHAR") == 0)
          {
          create_can_msg_uint8(msg, id, 0, (uint8_t)value[0]);
          msg->canas.data_type = CANAS_DATATYPE_UCHAR;
          }
        else if (strcmp(type, "UCHAR") == 0)
          {
          create_can_msg_uint8(msg, id, 0, (uint8_t)value[0]);
          }
        else if (strcmp(type, "CHAR2") == 0)
          {
          create_can_msg_uint8_2(msg, id, 0, (uint8_t)value[0], (uint8_t)value[1]);
          msg->canas.data_type = CANAS_DATATYPE_CHAR2;
          }
        else if (strcmp(type, "UCHAR2") == 0)
          {
          create_can_msg_uint8_2(msg, id, 0, (uint8_t)value[0], (uint8_t)value[1]);
          }
        else if (strcmp(type, "CHAR3") == 0)
          {
          create_can_msg_uint8_3(msg, id, 0, (uint8_t)value[0], (uint8_t)value[1], (uint8_t)value[2]);
          msg->canas.data_type = CANAS_DATATYPE_CHAR3;
          }
        else if (strcmp(type, "UCHAR3") == 0)
          {
          create_can_msg_uint8_3(msg, id, 0, (uint8_t)value[0], (uint8_t)value[1], (uint8_t)value[2]);
          }
        else if (strcmp(type, "CHAR4") == 0)
          {
          create_can_msg_uint8_4(msg, id, 0, (uint8_t)value[0], (uint8_t)value[1], (uint8_t)value[2], (uint8_t)value[3]);
          msg->canas.data_type = CANAS_DATATYPE_CHAR4;
          }
        else if (strcmp(type, "UCHAR4") == 0)
          {
          create_can_msg_uint8_4(msg, id, 0, (uint8_t)value[0], (uint8_t)value[1], (uint8_t)value[2], (uint8_t)value[3]);
          }
        else if (strcmp(type, "SHORT2") == 0)
          {
          int v1;
          int v2;
          sscanf(value, "%d,%d", &v1, &v2);
          create_can_msg_int16_2(msg, id, 0, (int16_t)v1, (int16_t)v2);
          }
        else if (strcmp(type, "USHORT2") == 0)
          {
          unsigned int v1;
          unsigned int v2;
          sscanf(value, "%u,%u", &v1, &v2);
          create_can_msg_uint16_2(msg, id, 0, (uint16_t)v1, (uint16_t)v2);
          }
        else
          {
          msg->id = id;
          msg->canas.data_type = CANAS_DATATYPE_NODATA;
          }
        }
      }
    }

  return s_ok;
  }

static const int16_t menu_timeout = 25;             // 5-second timeout

static void copy_ptr(const void *src, void **dst)
  {
  *dst = (void *)src;
  }

static void paint(handle_t hwnd, layout_window_t *wnd, const rect_t *wnd_rect, menu_item_t *item, const rect_t *rect);

static void update_window(handle_t hwnd, layout_window_t *wnd)
  {
  rect_t wnd_rect;
  get_window_rect(hwnd, &wnd_rect);

  extent_t ex;
  rect_extents(&wnd_rect, &ex);

  rect_t rect;
  if (wnd->current_menu != 0)
    {
    // draw the root menu
    point_t menu_pt = { wnd->menu_start_x, wnd->menu_start_y };
    gdi_dim_t item_width = rect_width(&wnd_rect) / 3;
    gdi_dim_t item_height = 20;

    extent_t item_extents = { item_width, item_height };

    // see if we are displaying a popup menu
    menu_pt.x = wnd->menu_start_x;
    menu_pt.y = wnd->menu_start_y - item_height;

    // determine how menu items to draw from the item
    uint16_t items_avail = wnd->menu_start_y / item_height;

    uint16_t top_item = wnd->current_menu->selected_index - items_avail + 1;

    if (top_item < 0)
      top_item = items_avail;

    uint16_t num_items = menu_count(wnd->current_menu);

    if (wnd->current_menu->selected_index >= num_items)
      wnd->current_menu->selected_index = num_items - 1;

    uint16_t index;
    // draw the visible items
    for (index = wnd->current_menu->selected_index; index >= top_item; index--)
      {
      menu_item_t *item = menu_item_at(wnd->current_menu, index);
      (*item->paint)(hwnd, wnd, &wnd_rect, item,
        make_rect(menu_pt.x, menu_pt.y,
          menu_pt.x + item_extents.dx,
          menu_pt.y + item_extents.dy, &rect));

      point_t pt;
      // now ask the item to draw if it is selected
      if (item->is_selected)
        draw_menu_item(hwnd, wnd, &wnd_rect, item,
          make_point(menu_pt.x + item_width, menu_pt.y, &pt));

      // skip up one.
      menu_pt.y -= item_height;
      }
    }
  }

static void show_menu(layout_window_t *wnd, menu_t *menu)
  {
  // assign the menu
  wnd->current_menu = menu;
  // and set the keys
  wnd->active_keys = menu->keys;
  }

static void show_item_editor(layout_window_t *wnd, menu_item_t *item)
  {
  if (item->item_type == mi_edit)
    {
    // select the popup menu
    item->editor_open = true;

    // TODO: edit the item....
    }
  }

static void close_menu(layout_window_t *wnd)
  {
  if (wnd->current_menu != 0)
    wnd->current_menu->selected_index = 0;

  wnd->active_keys = 0;
  wnd->current_menu = 0;
  wnd->menu_timer = 0;

  wnd->active_keys = wnd->root_keys;
  }

/**
* Called after a popup menu closes or a back option is selected
*/
static void pop_menu(layout_window_t *wnd)
  {
  menu_t *prev_menu = 0;
  if (succeeded(vector_empty(wnd->menu_stack)))
    {
    close_menu(wnd);      // all done
    return;
    }

  if (succeeded(vector_pop_back(wnd->menu_stack, &prev_menu)))
    show_menu(wnd, prev_menu);
  }

////////////////////////////////////////////////////////////////////////////////
//
// Generic handlers
//

static const char *to_string(const canmsg_t *msg, const char *format, char *buffer, int len)
  {
  switch (msg->canas.data_type)
    {
    case CANAS_DATATYPE_ERROR:
      strcpy(buffer, "error");
      break;;
    case CANAS_DATATYPE_FLOAT:
    {
    float value;
    get_param_float(msg, &value);
    snprintf(buffer, len, format, value);
    }
    break;
    case CANAS_DATATYPE_INT32:
    {
    int32_t value;
    get_param_int32(msg, &value);
    snprintf(buffer, len, format, value);
    }
    break;
    case CANAS_DATATYPE_UINT32:
    {
    uint32_t value;
    get_param_uint32(msg, &value);
    snprintf(buffer, len, format, value);
    }
    break;
    case CANAS_DATATYPE_SHORT:
    {
    int16_t value;
    get_param_int16(msg, 0, &value);
    snprintf(buffer, len, format, value);
    }
    break;
    case CANAS_DATATYPE_USHORT:
    {
    int16_t value;
    get_param_uint16(msg, 0, &value);
    snprintf(buffer, len, format, value);
    }
    break;
    case CANAS_DATATYPE_CHAR:
    {
    int8_t value;
    get_param_int8(msg, 0, &value);
    snprintf(buffer, len, format, value);
    }
    break;
    case CANAS_DATATYPE_UCHAR:
    {
    uint8_t value;
    get_param_uint8(msg, 0, &value);
    snprintf(buffer, len, format, value);
    }
    break;
    case CANAS_DATATYPE_SHORT2:
    {
    int16_t v1;
    get_param_int16(msg, 0, &v1);
    int16_t v2;
    get_param_int16(msg, 1, &v2);
    snprintf(buffer, len, format, v1, v2);
    }
    break;
    case CANAS_DATATYPE_USHORT2:
    {
    uint16_t v1;
    get_param_uint16(msg, 0, &v1);
    uint16_t v2;
    get_param_uint16(msg, 1, &v2);
    snprintf(buffer, len, format, v1, v2);
    }
    break;
    case CANAS_DATATYPE_CHAR4:
    {
    int8_t v1;
    get_param_int8(msg, 0, &v1);
    int8_t v2;
    get_param_int8(msg, 1, &v2);
    int8_t v3;
    get_param_int8(msg, 2, &v3);
    int8_t v4;
    get_param_int8(msg, 3, &v4);
    snprintf(buffer, len, format, v1, v2, v3, v4);
    }
    break;
    case CANAS_DATATYPE_UCHAR4:
    {
    uint8_t v1;
    get_param_uint8(msg, 0, &v1);
    uint8_t v2;
    get_param_uint8(msg, 1, &v2);
    uint8_t v3;
    get_param_uint8(msg, 2, &v3);
    uint8_t v4;
    get_param_uint8(msg, 3, &v4);
    snprintf(buffer, len, format, v1, v2, v3, v4);
    }
    break;
    case CANAS_DATATYPE_CHAR2:
    {
    int8_t v1;
    get_param_int8(msg, 0, &v1);
    int8_t v2;
    get_param_int8(msg, 1, &v2);
    snprintf(buffer, len, format, v1, v2);
    }
    break;
    case CANAS_DATATYPE_UCHAR2:
    {
    uint8_t v1;
    get_param_uint8(msg, 0, &v1);
    uint8_t v2;
    get_param_uint8(msg, 1, &v2);
    snprintf(buffer, len, format, v1, v2);
    }
    break;
    case CANAS_DATATYPE_CHAR3:
    {
    int8_t v1;
    get_param_int8(msg, 0, &v1);
    int8_t v2;
    get_param_int8(msg, 1, &v2);
    int8_t v3;
    get_param_int8(msg, 2, &v3);
    snprintf(buffer, len, format, v1, v2, v3);
    }
    break;
    case CANAS_DATATYPE_UCHAR3:
    {
    uint8_t v1;
    get_param_uint8(msg, 0, &v1);
    uint8_t v2;
    get_param_uint8(msg, 1, &v2);
    uint8_t v3;
    get_param_uint8(msg, 2, &v3);
    snprintf(buffer, len, format, v1, v2, v3);
    }
    break;
    }

  return buffer;
  }

static bool default_enable_handler(layout_window_t *wnd,
  menu_item_t *item,
  const canmsg_t *msg)
  {
  if (item->controlling_param == 0 ||
    item->enable_regex == 0)
    return true;

  // get a string that is the format of the variable
  char buffer[REG_STRING_MAX] = { 0 };

  // print the can message we listen on in a standard format
  to_string(&item->controlling_variable, item->enable_format, buffer, REG_STRING_MAX);

  // we now determine a match against the controlling regular expression
  return trex_match(item->pat_buff, buffer) != 0;
  }

/**
* Generic draw routine for boxed menu items
* @param hwnd
* @param wnd
* @param wnd_rect
* @param area
* @param is_selected
* handle_t hwnd, struct _layout_window_t *wnd, const rect_t *wnd_rect, struct _menu_item_t *item, const rect_t *rect
*/
static void default_paint_handler(handle_t hwnd,
  layout_window_t *wnd,
  const rect_t *wnd_rect,
  menu_item_t *item,
  const rect_t *area)
  {
  point_t center_pt = {
    (rect_width(area) >> 1) + area->left,
    (rect_height(area) >> 1) + area->top
    };

  rectangle(hwnd, wnd_rect, &wnd->border_pen,
    item->is_selected ? wnd->selected_background_color : wnd->background_color, area);

  // calculate the text extents
  extent_t ex;

  text_extent(hwnd, wnd->font, item->caption, 0, &ex);

  center_pt.x -= ex.dx >> 1;
  center_pt.y -= ex.dy >> 1;

  draw_text(hwnd, wnd_rect, wnd->font,
    item->is_selected ? wnd->selected_color : wnd->text_color,
    wnd->background_color,
    item->caption, 0, &center_pt, 0, 0, 0);
  }

static void default_msg_handler(struct _layout_window_t *wnd, struct _menu_item_t *item, const canmsg_t *msg)
  {
  if (item->controlling_param == msg->id)
    memcpy(&item->controlling_variable, msg, sizeof(canmsg_t));
  }

static void load_item_defaults(layout_window_t *wnd, menu_item_t *item, memid_t key)
  {
  item->event = default_msg_handler;
  item->paint = default_paint_handler;
  item->is_enabled = default_enable_handler;

  reg_get_string(key, "caption", item->caption, 0);
  memid_t enable_key;
  if (succeeded(reg_open_key(key, "enable", &enable_key)))
    {
    reg_get_uint16(enable_key, "id", &item->controlling_param);
    reg_get_string(enable_key, "regex", item->enable_regex, 0);
    reg_get_string(enable_key, "format", item->enable_format, 0);

    if (item->enable_regex[0] != 0)
      {
      const char *error;
      // compile the pattern
      item->pat_buff = trex_compile(item->enable_regex, &error);
      }
    }
  }

static menu_item_t *menu_item_at(menu_t *menu, uint16_t index)
  {
  menu_item_t *mi;
  if (failed(vector_at(menu->menu_items, index, &mi)))
    return 0;

  return mi;
  }

static uint16_t menu_count(menu_t *menu)
  {
  uint16_t count = 0;
  vector_count(menu->menu_items, &count);

  return count;
  }

////////////////////////////////////////////////////////////////////////////////
//
// Item handlers for a POPUP item
//
typedef struct _menu_item_menu_t {
  menu_item_t item;

  // if this is a checklist popup then this holds the ushort value
  // that is matched to create the checklist option.
  uint16_t value;

  // if a checklist item then this has no value
  char menu_name[REG_STRING_MAX];
  menu_t *menu;
  } menu_item_menu_t;

static menu_item_action_result item_menu_evaluate(layout_window_t *wnd,
  struct _menu_item_t *item,
  const canmsg_t *msg)
  {
  menu_item_menu_t *menu = (menu_item_menu_t *)item;

  show_menu(wnd, menu->menu);

  return mia_nothing;

  return mia_nothing;
  }

static menu_item_t *item_menu_load(layout_window_t *wnd,
  memid_t key,
  bool is_checklist)
  {
  menu_item_menu_t *item = (menu_item_menu_t *)neutron_malloc(sizeof(menu_item_menu_t));
  memset(item, 0, sizeof(menu_item_menu_t));

  load_item_defaults(wnd, &item->item, key);

  item->item.evaluate = item_menu_evaluate;
  item->item.item_type = mi_menu;

  load_item_defaults(wnd, &item->item, key);

  reg_get_string(key, "menu", item->menu_name, 0);
  // load the popup menu
  find_menu(wnd, item->menu_name, &item->menu);

  if (is_checklist)
    reg_get_uint16(key, "value", &item->value);

  // save the new item
  vector_push_back(wnd->menu_items, item);
  return &item->item;
  }

////////////////////////////////////////////////////////////////////////////////
//
// Item handlers for a CANCEL item
//

typedef struct _menu_item_cancel_t {
  menu_item_t item;
  uint16_t value;     // cancel value
  } menu_item_cancel_t;

static menu_item_action_result item_cancel_evaluate(layout_window_t *wnd,
  menu_item_t *item,
  const canmsg_t *msg)
  {
  /*
  msg_t cancel_msg(id_menu_cancel);
  cancel_msg.flags |= LOOPBACK_MESSAGE;

  reinterpret_cast<application_t *>(application_t::instance)->publish(cancel_msg);
  * */

  return mia_cancel;
  }

static menu_item_t *item_cancel_load(layout_window_t *wnd, memid_t key)
  {
  menu_item_cancel_t *item = (menu_item_cancel_t *)neutron_malloc(sizeof(menu_item_cancel_t));
  memset(item, 0, sizeof(menu_item_cancel_t));
  item->item.item_type = mi_cancel;

  load_item_defaults(wnd, &item->item, key);
  item->item.evaluate = item_cancel_evaluate;

  if (failed(reg_get_uint16(key, "value", &item->value)))
    item->value = id_menu_cancel;

  // save the new item
  vector_push_back(wnd->menu_items, item);
  return &item->item;
  }

////////////////////////////////////////////////////////////////////////////////
//
// Item handlers for a ENTER item
//

typedef struct _menu_item_enter_t {
  menu_item_t item;
  uint16_t value;     // enter value
  } menu_item_enter_t;

static menu_item_action_result item_enter_evaluate(layout_window_t *wnd,
  menu_item_t *item,
  const canmsg_t *msg)
  {
  return mia_enter;
  }

static menu_item_t *item_enter_load(layout_window_t *wnd,
  memid_t key)
  {
  menu_item_enter_t *item = (menu_item_enter_t *)neutron_malloc(sizeof(menu_item_enter_t));
  memset(item, 0, sizeof(menu_item_enter_t));

  item->item.item_type = mi_enter;

  load_item_defaults(wnd, &item->item, key);
  item->item.evaluate = item_enter_evaluate;

  if (failed(reg_get_uint16(key, "value", &item->value)))
    item->value = id_menu_ok;

  // save the new item
  vector_push_back(wnd->menu_items, item);
  return &item->item;
  }

////////////////////////////////////////////////////////////////////////////////
//
// Item handlers for a EVENT item
//
typedef struct _menu_item_event_t {
  menu_item_t item;
  canmsg_t msg;
  } menu_item_event_t;

static menu_item_action_result item_event_evaluate(layout_window_t *wnd,
  menu_item_t *item,
  const canmsg_t *msg)
  {
  menu_item_event_t *mi = (menu_item_event_t *)item;
  can_send(&mi->msg);

  return mia_nothing;
  }

static menu_item_t *item_event_load(layout_window_t *wnd,
  memid_t key)
  {
  menu_item_event_t *item = (menu_item_event_t *)neutron_malloc(sizeof(menu_item_event_t));
  memset(item, 0, sizeof(menu_item_event_t));

  item->item.item_type = mi_event;

  load_item_defaults(wnd, &item->item, key);
  item->item.evaluate = item_event_evaluate;

  // read the event message from the registry
  load_can_msg(key, &item->msg);

  // save the new item
  vector_push_back(wnd->menu_items, item);
  return &item->item;
  }

////////////////////////////////////////////////////////////////////////////////
//
// Item handlers for a EDIT item
//
typedef struct _menu_item_edit_t {
  menu_item_t item;

  uint16_t value_id;
  canmsg_t value;

  uint16_t set_id;
  bool integer_type;

  uint16_t precision;       // number of digits after decimal point to edit
  uint16_t digits;          // number of digits to the left of the decimal point

  float min_value;
  float max_value;
  float decka_increment;
  float deckb_increment;
  } menu_item_edit_t;

static void item_edit_paint(handle_t hwnd,
  layout_window_t *wnd,
  const rect_t *wnd_rect,
  menu_item_t *item,
  const rect_t *rect)
  {

  }

static menu_item_action_result item_edit_evaluate(layout_window_t *wnd,
  menu_item_t *item,
  const canmsg_t *msg)
  {
  return mia_nothing;
  }

static void item_edit_edit(layout_window_t *wnd,
  struct _menu_item_t *item,
  const canmsg_t *msg)
  {

  }

static void item_edit_event(layout_window_t *wnd,
  menu_item_t *item,
  const canmsg_t *msg)
  {
  // check the enabler
  default_enable_handler(wnd, item, msg);

  menu_item_edit_t *edit = (menu_item_edit_t *)item;
  // see if we update our value
  if (edit->value_id == msg->id)
    memcpy(&edit->value, msg, sizeof(canmsg_t));
  }


static menu_item_t *item_edit_load(layout_window_t *wnd,
  memid_t key)
  {
  menu_item_edit_t *item = (menu_item_edit_t *)neutron_malloc(sizeof(menu_item_edit_t));
  memset(item, 0, sizeof(menu_item_edit_t));

  item->item.item_type = mi_edit;

  load_item_defaults(wnd, &item->item, key);
  item->item.paint = item_edit_paint;
  item->item.evaluate = item_edit_evaluate;
  item->item.edit = item_edit_edit;
  item->item.event = item_edit_event;

  reg_get_uint16(key, "value-id", &item->value_id);

  // load the set-value message
  reg_get_uint16(key, "set-id", &item->set_id);

  // get the float point flag
  reg_get_bool(key, "send-int16_t", &item->integer_type);

  // get the values
  reg_get_float(key, "min-value", &item->min_value);
  reg_get_float(key, "max-value", &item->max_value);
  reg_get_float(key, "decka-incr", &item->decka_increment);
  reg_get_float(key, "deckb-incr", &item->deckb_increment);

  reg_get_uint16(key, "precision", &item->precision);
  reg_get_uint16(key, "digits", &item->digits);

  // save the new item
  vector_push_back(wnd->menu_items, item);
  return &item->item;
  }
/*

#	NUMERIC					A number editor
#		Params are:
#			type=<type>		Can aerospace type, usually SHORT or FLOAT
#			value=id		A canerospace value the will be the current value
#			set_value=id		Message to set the value
#			precision=num		Optional precision, if ommitted 0 is assumed and an integer value
#			digits=num		Number of digits, if precision is included will be <digits>.<precision>
#			min_value=num
#			max_value=num
#			circualar=<true/false>	If provided the editor will swap to min on overflow and max on underflow
#			decka_increment=<value>	Normally decka increments the digits, this sets by how much.  defaults to 1
#			deckb_increment=<value>	If a precision is set then this sets how much the minor value changes, otherwise allows for
#						incrementing the digits.
#  	example for a frequency:	type=FLOAT,value=nnn,set_value=nnn,precision=3,digits=2,min_value=108.0,max_value=136.75,circular=true,decka_increment=1,deckb_increment=0.25
*/

////////////////////////////////////////////////////////////////////////////////
//
// Item handlers for a CHECKLIST item
//
// A checklist is a popup menu with the events being sent to the
// checklist which then does a message.  This is to allow the current value
// to be displayed as the popup menu selected item
//
typedef struct _menu_item_checklist_t {
  menu_item_t item;

  uint16_t value_id;
  canmsg_t value;

  // item that is selected in the list.
  int16_t selected_item;

  // this holds the checklist items
  menu_item_menu_t *popup;
  } menu_item_checklist_t;

static menu_item_action_result item_checklist_evaluate(layout_window_t *wnd,
  menu_item_t *item,
  const canmsg_t *msg)
  {
  return mia_nothing;
  }

static void item_checklist_event(layout_window_t *wnd,
  menu_item_t *item,
  const canmsg_t *msg)
  {
  // check the enabler
  default_enable_handler(wnd, item, msg);

  menu_item_checklist_t *checklist = (menu_item_checklist_t *)item;
  // see if we update our value
  if (checklist->value_id == msg->id)
    {
    memcpy(&checklist->value, msg, sizeof(canmsg_t));

    uint16_t value;
    get_param_uint16(msg, 0, &value);

    uint16_t count;
    vector_count(checklist->popup->menu->menu_items, &count);

    uint16_t i;
    for (i = 0; i < count; i++)
      {
      menu_item_t *item;
      vector_at(checklist->popup->menu->menu_items, i, &item);
      if (i == value)
        checklist->popup->menu->selected_index = i;
      }
    }
  }

static menu_item_t *item_checklist_load(layout_window_t *wnd,
  memid_t key)
  {
  menu_item_checklist_t *item = (menu_item_checklist_t *)neutron_malloc(sizeof(menu_item_checklist_t));
  memset(item, 0, sizeof(menu_item_checklist_t));


  item->item.item_type = mi_checklist;

  load_item_defaults(wnd, &item->item, key);
  item->item.evaluate = item_checklist_evaluate;
  item->item.event = item_checklist_event;

  reg_get_uint16(key, "value-id", &item->value_id);

  // load the popup menu next.
  item->popup = (menu_item_menu_t *)item_menu_load(wnd, key, true);

  // save the new item
  vector_push_back(wnd->menu_items, item);
  return &item->item;
  }
/*
menu_item_checklist_t::menu_item_checklist_t(layout_window_t *parent, const char *options, const char *expr)
: menu_item_t(parent)
{
// if the parser determines we have an enable expression in the form menu(<expr>): then
// pass it to the base class.
if(expr != 0)
assign_enabler(expr);

// the string is composed of a tuple of 3 attributes.  The first is the
//
attributes_t attributes;
split_tuple(options, attributes, ',');

if(attributes[0][0] == '"')
caption(attributes[0].substr(1, attributes[0][attributes[0].size()-1]== '"' ? attributes[0].size()-2 : attributes[0].size()-1));
else
caption(attributes[0]);
}

menu_item_checklist_t::~menu_item_checklist_t()
{
}

menu_item_type menu_item_checklist_t::item_type() const
{
return mi_checklist;
}

menu_item_action_result menu_item_checklist_t::evaluate(const canmsg_t &) const
{
return mia_nothing;
}
*/
////////////////////////////////////////////////////////////////////////////////
//
//
/**
* Load a menu item from the registry
* @param value     String to parse
* @returns The created menu item if it exists
*/
static menu_item_t *parse_item(layout_window_t *parent, memid_t key)
  {
  char item_type[REG_STRING_MAX + 1];
  uint16_t len = REG_STRING_MAX + 1;

  if (failed(reg_get_string(key, "type", item_type, &len)))
    return 0;

  if (strcmp(item_type, "menu") == 0)
    return item_menu_load(parent, key, false);

  if (strcmp(item_type, "cancel") == 0)
    return item_cancel_load(parent, key);

  if (strcmp(item_type, "enter") == 0)
    return item_enter_load(parent, key);

  if (strcmp(item_type, "event") == 0)
    return item_event_load(parent, key);

  if (strcmp(item_type, "edit") == 0)
    return item_edit_load(parent, key);

  if (strcmp(item_type, "checklist") == 0)
    return item_checklist_load(parent, key);

  return 0;
  }

static result_t find_keys(layout_window_t *wnd, const char *name, keys_t **pmenu)
  {
  if (pmenu == 0 ||
    wnd == 0 ||
    name == 0)
    return e_bad_parameter;

  result_t result;
  keys_t *menu;

  if (failed(map_find(wnd->key_mappings, name, (void **)&menu)))
    {
    menu = (keys_t *)neutron_malloc(sizeof(keys_t));
    *pmenu = menu;

    memset(menu, 0, sizeof(keys_t));

    // must be done before we load any items so if they recurse we don't get
    // created more than once!
    if (failed(result = map_add(wnd->key_mappings, name, menu)))
      {
      neutron_free(menu);
      return result;
      }

    memid_t child;
    if (succeeded(reg_open_key(wnd->key, "key0", &child)))
      menu->key0 = parse_item(wnd, child);

    if (succeeded(reg_open_key(wnd->key, "key1", &child)))
      menu->key1 = parse_item(wnd, child);

    if (succeeded(reg_open_key(wnd->key, "key2", &child)))
      menu->key2 = parse_item(wnd, child);

    if (succeeded(reg_open_key(wnd->key, "key3", &child)))
      menu->key3 = parse_item(wnd, child);

    if (succeeded(reg_open_key(wnd->key, "key4", &child)))
      menu->key4 = parse_item(wnd, child);

    if (succeeded(reg_open_key(wnd->key, "decka-up", &child)))
      menu->decka_up = parse_item(wnd, child);

    if (succeeded(reg_open_key(wnd->key, "decka-dn", &child)))
      menu->decka_dn = parse_item(wnd, child);

    if (succeeded(reg_open_key(wnd->key, "deckb-up", &child)))
      menu->deckb_up = parse_item(wnd, child);

    if (succeeded(reg_open_key(wnd->key, "deckb-dn", &child)))
      menu->deckb_dn = parse_item(wnd, child);
    }

  *pmenu = menu;

  return s_ok;
  }

static result_t find_menu(layout_window_t *wnd, const char *name, menu_t **menu)
  {
  if (wnd == 0 ||
    name == 0 ||
    menu == 0)
    return e_bad_parameter;

  result_t result;
  menu_t *popup;

  if (failed(map_find(wnd->menus, name, (void **)&popup)))
    {
    memid_t key;
    if (failed(result = reg_open_key(wnd->key, name, &key)))
      return result;

    // we have the memid, load it.
    popup = (menu_t *)neutron_malloc(sizeof(menu_t));
    memset(popup, 0, sizeof(menu_t));


    // must be done before we load any items so if they recurse we don't get
    // created more than once!
    if (failed(result = map_add(wnd->menus, name, popup)))
      {
      neutron_free(popup);
      return result;
      }

    popup->name = neutron_strdup(name);
    vector_create(sizeof(menu_item_t *), &popup->menu_items);

    memid_t item_key = 0;
    field_datatype dt = field_key;

    while (succeeded(reg_enum_key(key, &dt, 0, 0, 0, 0, &item_key)))
      {
      // the protocol assumes all child keys are menu items
      menu_item_t *item = parse_item(wnd, item_key);
      // add the menu item to the mix
      vector_push_back(popup->menu_items, item);
      }
    }

  *menu = popup;

  return s_ok;
  }


result_t layout_wndproc(handle_t hwnd, const canmsg_t *msg)
  {
  bool changed = false;
  layout_window_t *wnd;
  uint16_t count;
  uint16_t item;

  get_wnddata(hwnd, (void **)&wnd);

  // do the default processing (calls event handlers)
  defwndproc(hwnd, msg);

  if(wnd != 0 && wnd->menu_items != 0)
    {
    // we go through all of the menu items and check to see if they are
    // listening, and if their state has changed
    vector_count(wnd->menu_items, &count);
    for (item = 0; item < count; item++)
      {
      menu_item_t *mi;
      vector_at(wnd->menu_items, item, &mi);

      // update the controlling variable if it is being watched.
      if (mi->event != 0)
        (*mi->event)(wnd, mi, msg);
      }
    }

  return s_ok;
  }

static void draw_menu_item(handle_t hwnd,
  struct _layout_window_t *wnd,
  const rect_t *wnd_rect,
  menu_item_t *selected_item,
  const point_t *menu_origin)
  {

  }

// these are the attached event handlers
static result_t on_key0(handle_t hwnd, event_proxy_t *proxy, const canmsg_t *msg)
  {
  bool changed = false;
  layout_window_t *wnd = (layout_window_t *)proxy->parg;
  rect_t wnd_rect;
  get_window_rect(hwnd, &wnd_rect);

  uint16_t id = msg->id;
  int16_t value;
  get_param_int16(msg, 0, &value);
  if (wnd->active_keys != 0)
    {
    if (wnd->active_keys->key0 != 0 &&
      value > 0 &&
      (*wnd->active_keys->key0->is_enabled)(wnd, wnd->active_keys->key0, msg))
      {
      if (wnd->menu_timer != 0)
        (*wnd->active_keys->key0->evaluate)(wnd, wnd->active_keys->key0, msg);
      wnd->menu_timer = menu_timeout;
      changed = true;
      }
    }
  else
    {
    wnd->active_keys = wnd->root_keys;
    wnd->menu_timer = 0;
    changed = true;
    }

  if(changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

static result_t on_key1(handle_t hwnd, event_proxy_t *proxy, const canmsg_t *msg)
  {
  bool changed = false;
  layout_window_t *wnd = (layout_window_t *)proxy->parg;
  rect_t wnd_rect;
  get_window_rect(hwnd, &wnd_rect);

  uint16_t id = msg->id;
  int16_t value;
  get_param_int16(msg, 0, &value);

  if (wnd->active_keys != 0)
    {
    if (wnd->active_keys->key1 != 0 &&
      value > 0 &&
      (*wnd->active_keys->key1->is_enabled)(wnd, wnd->active_keys->key1, msg))
      {
      if (wnd->menu_timer != 0)
        (*wnd->active_keys->key1->evaluate)(wnd, wnd->active_keys->key1, msg);

      wnd->menu_timer = menu_timeout;
      changed = true;
      }
    }
  else
    {
    wnd->active_keys = wnd->root_keys;
    wnd->menu_timer = 0;
    changed = true;
    }

  if (changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

static result_t on_key2(handle_t hwnd, event_proxy_t *proxy, const canmsg_t *msg)
  {
  bool changed = false;
  layout_window_t *wnd = (layout_window_t *)proxy->parg;
  rect_t wnd_rect;
  get_window_rect(hwnd, &wnd_rect);

  uint16_t id = msg->id;
  int16_t value;
  get_param_int16(msg, 0, &value);

  if (wnd->active_keys != 0)
    {
    if (wnd->active_keys->key2 != 0 &&
      value > 0 &&
      (*wnd->active_keys->key2->is_enabled)(wnd, wnd->active_keys->key2, msg))
      {
      if (wnd->menu_timer != 0)
        (*wnd->active_keys->key2->evaluate)(wnd, wnd->active_keys->key2, msg);

      wnd->menu_timer = menu_timeout;
      changed = true;
      }
    }
  else
    {
    wnd->active_keys = wnd->root_keys;
    wnd->menu_timer = 0;
    changed = true;
    }

  if (changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

static result_t on_key3(handle_t hwnd, event_proxy_t *proxy, const canmsg_t *msg)
  {
  bool changed = false;
  layout_window_t *wnd = (layout_window_t *)proxy->parg;
  rect_t wnd_rect;
  get_window_rect(hwnd, &wnd_rect);

  uint16_t id = msg->id;
  int16_t value;
  get_param_int16(msg, 0, &value);

  if (wnd->active_keys != 0)
    {
    if (wnd->active_keys->key3 != 0 &&
      value > 0 &&
      (*wnd->active_keys->key3->is_enabled)(wnd, wnd->active_keys->key3, msg))
      {
      if (wnd->menu_timer != 0)
        (*wnd->active_keys->key3->evaluate)(wnd, wnd->active_keys->key3, msg);

      wnd->menu_timer = menu_timeout;
      changed = true;
      }
    }
  else
    {
    wnd->active_keys = wnd->root_keys;
    wnd->menu_timer = 0;
    changed = true;
    }

  if (changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

static result_t on_key4(handle_t hwnd, event_proxy_t *proxy, const canmsg_t *msg)
  {
  bool changed = false;
  layout_window_t *wnd = (layout_window_t *)proxy->parg;
  rect_t wnd_rect;
  get_window_rect(hwnd, &wnd_rect);

  uint16_t id = msg->id;
  int16_t value;
  get_param_int16(msg, 0, &value);
  if (wnd->active_keys != 0)
    {
    if (wnd->active_keys->key4 != 0 &&
      value > 0 &&
      (*wnd->active_keys->key4->is_enabled)(wnd, wnd->active_keys->key4, msg))
      {
      if (wnd->menu_timer != 0)
        (*wnd->active_keys->key4->evaluate)(wnd, wnd->active_keys->key4, msg);

      wnd->menu_timer = menu_timeout;
      changed = true;
      }
    }
  else
    {
    wnd->active_keys = wnd->root_keys;
    wnd->menu_timer = 0;
    changed = true;
    }

  if (changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

static result_t on_decka(handle_t hwnd, event_proxy_t *proxy, const canmsg_t *msg)
  {
  bool changed = false;
  layout_window_t *wnd = (layout_window_t *)proxy->parg;
  rect_t wnd_rect;
  get_window_rect(hwnd, &wnd_rect);

  uint16_t id = msg->id;
  int16_t value;
  get_param_int16(msg, 0, &value);

  const keys_t *handler = wnd->active_keys == 0 ? wnd->root_keys : wnd->active_keys;
  if (value > 0)
    {
    if (handler->decka_up != 0 && (*handler->decka_up->is_enabled)(wnd, handler->decka_up, msg))
      {
      if (wnd->menu_timer != 0)
        wnd->menu_timer = menu_timeout;

      (*handler->decka_up->evaluate)(wnd, handler->decka_up, msg);
      changed = true;
      }
    }
  else if (value < 0)
    {
    if (handler->decka_dn != 0 && (*handler->decka_dn->is_enabled)(wnd, handler->decka_dn, msg))
      {
      if (wnd->menu_timer != 0)
        wnd->menu_timer = menu_timeout;

      (*handler->decka_dn->evaluate)(wnd, handler->decka_dn, msg);
      changed = true;
      }
    }

  if (changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

static result_t on_deckb(handle_t hwnd, event_proxy_t *proxy, const canmsg_t *msg)
  {
  bool changed = false;
  layout_window_t *wnd = (layout_window_t *)proxy->parg;
  rect_t wnd_rect;
  get_window_rect(hwnd, &wnd_rect);

  uint16_t id = msg->id;
  int16_t value;
  get_param_int16(msg, 0, &value);

  const keys_t *handler = wnd->active_keys == 0 ? wnd->root_keys : wnd->active_keys;
  if (value > 0)
    {
    if (handler->deckb_up != 0 && (*handler->deckb_up->is_enabled)(wnd, handler->deckb_up, msg))
      {
      if (wnd->menu_timer != 0)
        wnd->menu_timer = menu_timeout;

      (*handler->deckb_up->evaluate)(wnd, handler->deckb_up, msg);
      changed = true;
      }
    }
  else if (value < 0)
    {
    if (handler->deckb_dn != 0 && (*handler->deckb_dn->is_enabled)(wnd, handler->deckb_dn, msg))
      {
      if (wnd->menu_timer != 0)
        wnd->menu_timer = menu_timeout;

      (*handler->deckb_dn->evaluate)(wnd, handler->deckb_dn, msg);
      changed = true;
      }
    }

  if (changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

static result_t on_menu_up(handle_t hwnd, event_proxy_t *proxy, const canmsg_t *msg)
  {
  bool changed = false;
  layout_window_t *wnd = (layout_window_t *)proxy->parg;
  rect_t wnd_rect;
  get_window_rect(hwnd, &wnd_rect);

  uint16_t id = msg->id;
  int16_t value;
  get_param_int16(msg, 0, &value);

  // used to change the current popup menu index
  if (wnd->current_menu != 0)
    {
    uint16_t count = menu_count(wnd->current_menu);
    if (wnd->current_menu->selected_index < count - 1)
      {
      wnd->current_menu->selected_index++;
      changed = true;
      }
    }

  if (changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

static result_t on_menu_dn(handle_t hwnd, event_proxy_t *proxy, const canmsg_t *msg)
  {
  bool changed = false;
  layout_window_t *wnd = (layout_window_t *)proxy->parg;
  rect_t wnd_rect;
  get_window_rect(hwnd, &wnd_rect);

  uint16_t id = msg->id;
  int16_t value;
  get_param_int16(msg, 0, &value);

  // used to change the current popup menu index
  if (wnd->current_menu != 0 && wnd->current_menu->selected_index > 0)
    {
    wnd->current_menu->selected_index--;
    changed = true;
    }

  if (changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

static result_t on_menu_left(handle_t hwnd, event_proxy_t *proxy, const canmsg_t *msg)
  {
  bool changed = false;
  layout_window_t *wnd = (layout_window_t *)proxy->parg;
  rect_t wnd_rect;
  get_window_rect(hwnd, &wnd_rect);

  uint16_t id = msg->id;
  int16_t value;
  get_param_int16(msg, 0, &value);

  if (wnd->current_menu != 0)
    {
    // close the popup and open the parent if any
    close_menu(wnd);
    changed = true;
    }
  else
    {
    }

  if (changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

static result_t on_menu_right(handle_t hwnd, event_proxy_t *proxy, const canmsg_t *msg)
  {
  bool changed = false;
  layout_window_t *wnd = (layout_window_t *)proxy->parg;
  rect_t wnd_rect;
  get_window_rect(hwnd, &wnd_rect);

  uint16_t id = msg->id;
  int16_t value;
  get_param_int16(msg, 0, &value);

  if (wnd->current_menu != 0)
    {
    menu_item_t *item = menu_item_at(wnd->current_menu, wnd->current_menu->selected_index);
    if (item->item_type == mi_menu)
      {
      menu_item_menu_t *mi = (menu_item_menu_t *)item;
      show_menu(wnd, mi->menu);     // show the popup
      }
    else if (item->item_type == mi_edit)
      show_item_editor(wnd, item);
    changed = true;
    }

  if (changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

static result_t on_ok(handle_t hwnd, event_proxy_t *proxy, const canmsg_t *msg)
  {
  bool changed = false;
  layout_window_t *wnd = (layout_window_t *)proxy->parg;
  rect_t wnd_rect;
  get_window_rect(hwnd, &wnd_rect);

  uint16_t id = msg->id;
  int16_t value;
  get_param_int16(msg, 0, &value);

  // this is sent when an item is selected.  Only ever one
  if (wnd->current_menu != 0)
    {
    menu_item_t *item = menu_item_at(wnd->current_menu, wnd->current_menu->selected_index);
    if (item != 0)
      (*item->evaluate)(wnd, item, msg);
    }

  if (changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

static result_t on_cancel(handle_t hwnd, event_proxy_t *proxy, const canmsg_t *msg)
  {
  layout_window_t *wnd = (layout_window_t *)proxy->parg;

  close_menu(wnd);

  return s_ok;
  }

result_t load_layout(handle_t parent, memid_t hive)
  {
  // the hive must have series of hives that form windows
  result_t result;
  char name[REG_NAME_MAX +1];
  uint16_t length = REG_NAME_MAX + 1;
  field_datatype type = field_key;

  layout_window_t *wnd = (layout_window_t *)neutron_malloc(sizeof(layout_window_t));
  memset(wnd, 0, sizeof(layout_window_t));

  // attach the window to the screen
  wnd->window = parent;
  wnd->version = sizeof(layout_window_t);

  memid_t menu = 0;
  if (succeeded(reg_open_key(hive, "menu", &menu)))
    {
      // this stores a cache of loaded keys.
    map_create_nv(copy_ptr, 0, &wnd->key_mappings);
    // this is the undo stack for the menu
    vector_create(sizeof(menu_t *), &wnd->menu_stack);
    // these are the popup menus
    map_create_nv(copy_ptr, 0, &wnd->menus);
    // all menu items are kept here
    vector_create(sizeof(menu_item_t *), &wnd->menu_items);

    int16_t value;
    if (succeeded(reg_get_int16(menu, "menu-rect-x", &value)))
      wnd->menu_rect_x = value;

    if (succeeded(reg_get_int16(menu, "menu-rect-y", &value)))
      wnd->menu_rect_y = value;

    if (succeeded(reg_get_int16(menu, "menu-start-x", &value)))
      wnd->menu_start_x = value;

    if (succeeded(reg_get_int16(menu, "menu-start-y", &value)))
      wnd->menu_start_y = value;
      }

  wnd->key = menu;

  char menu_name[REG_STRING_MAX];

  if (succeeded(reg_get_string(menu, "root-keys", menu_name, 0)))
    find_keys(wnd, menu_name, &wnd->root_keys);

  if (failed(lookup_color(menu, "bk-color", &wnd->background_color)))
    wnd->background_color = color_black;

  if (failed(lookup_color(menu, "bk-selected", &wnd->selected_background_color)))
    wnd->selected_background_color = color_white;

  if(failed(lookup_color(menu, "selected-color", &wnd->selected_color)))
    wnd->selected_color = color_magenta;

  if(failed(lookup_color(menu, "text-color", &wnd->text_color)))
    wnd->text_color = color_green;

  memcpy(&wnd->border_pen, &light_gray_pen_3, sizeof(pen_t));
  memid_t pen_key;
  if(succeeded(reg_open_key(menu, "pen", &pen_key)))
    lookup_pen(pen_key, &wnd->border_pen);

  // check for the font
  if (failed(lookup_font(menu, "font",  &wnd->font)))
    {
    // we always have the neo font.
    if(failed(result = open_font("neo", 9, &wnd->font)))
      return result;
    }

  // attach all of our event handlers now
  add_event(parent, id_key0, wnd, 0, on_key0);
  add_event(parent, id_key1, wnd, 0, on_key1);
  add_event(parent, id_key2, wnd, 0, on_key2);
  add_event(parent, id_key3, wnd, 0, on_key3);
  add_event(parent, id_key4, wnd, 0, on_key4);
  add_event(parent, id_decka, wnd, 0, on_decka);
  add_event(parent, id_deckb, wnd, 0, on_deckb);
  add_event(parent, id_menu_left, wnd, 0, on_menu_left);
  add_event(parent, id_menu_right, wnd, 0, on_menu_right);
  add_event(parent, id_menu_up, wnd, 0, on_menu_up);
  add_event(parent, id_menu_dn, wnd, 0, on_menu_dn);
  add_event(parent, id_menu_cancel, wnd, 0, on_cancel);
  add_event(parent, id_menu_ok, wnd, 0, on_ok);

  // must be 0 on first call
  memid_t child = 0;

  while (succeeded(result = reg_enum_key(hive, &type, 0, 0, REG_NAME_MAX, name, &child)))
    {
    // we create a widget and pickup the widget defined settings.
    // these are:
    // font -> name of the font to load
    // color -> foreground color
    // background -> back
    int ordinal;
    if (failed(lookup_enum(child, "type", layout_names, last_layout_enum, &ordinal)))
      {
      continue;
      }

    trace_info("Create widget %s\n", name);

    handle_t hwnd;

    switch (ordinal)
      {
      case airspeed_window:
        create_airspeed_window(parent, child, &hwnd);
        break;
      case hsi_window:
        create_hsi_window(parent, child, &hwnd);
        break;
      case attitude_window:
        create_attitude_window(parent, child, &hwnd);
        break;
      case altitude_window:
        create_altitude_window(parent, child, &hwnd);
        break;
      case gauge_window:
        create_gauge_window(parent, child, &hwnd);
        break;
      case annunciator_window:
        create_annunciator_window(parent, child, &hwnd);
        break;
      case gps_window:
        create_gps_window(parent, child, &hwnd);
        break;
      case alert_window:

        break;
      case widget:
        create_widget(parent, child, &hwnd);
        break;

      default:
        break;
      }

    // field_datatype has the field type, name is the child name
    type = field_key;
    }

  // store the parameters for the window
  set_wnddata(parent, wnd);

  rect_t rect;
  get_window_rect(parent, &rect);
  invalidate_rect(parent, &rect);

  return s_ok;
  }
