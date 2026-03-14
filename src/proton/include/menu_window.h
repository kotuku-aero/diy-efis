#ifndef __menu_window_h__
#define __menu_window_h__

#include "widget.h"
#include "regex.h"
#include "converters.h"

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
  mia_show,     // show the menu
  mia_cancel, // event causes a cancel
  mia_enter, // event causes a done, all menu close
  mia_close_item, // either a popup or edit is closed
  } menu_item_action_result;

typedef struct _menu_widget_t menu_widget_t;
typedef struct _menu_item_t menu_item_t;

typedef void(*item_paint_fn)(handle_t hwnd, menu_widget_t* wnd, const rect_t* wnd_rect, menu_item_t * item, const rect_t* rect, bool is_highlighted);
typedef menu_item_action_result(*item_evaluate_fn)(menu_widget_t* wnd, menu_item_t * item, const canmsg_t* msg);
typedef bool(*item_is_enabled_fn)(menu_widget_t* wnd, menu_item_t * item, const canmsg_t* msg);
typedef void(*item_msg_event_fn)(menu_widget_t* wnd, menu_item_t * item, const canmsg_t* msg);


typedef struct _menu_item_t
  {
  menu_item_type item_type;
  const char *caption;

  const char *enable_regex;
  const char *enable_format;

  uint16_t controlling_param;
  canmsg_t controlling_variable;

  // set by parent menu so the item will highlight itself
  bool is_selected;
  bool editor_open; // if true the popup menu editor is active

  TRex* pat_buff;

  item_paint_fn paint;
  item_evaluate_fn evaluate;
  item_is_enabled_fn is_enabled;
  item_msg_event_fn event;
  item_paint_fn edit_paint;
  } menu_item_t;

typedef menu_item_t* menu_itemp;

typedef result_t(*get_value_fn)(menu_item_t * edit, variant_t* value);
typedef result_t(*set_value_fn)(menu_item_t * edit, const variant_t *value);
typedef result_t(*evaluate_fn)(menu_item_t * edit);

/* This describes the handlers for a menu.
*/
typedef struct _keys_t {
  const char* name;     // path of the keys in the registry
  // keys that are pressed for a short time
  menu_item_t* key0;
  const char *key0_caption;
  menu_item_t* key1;
  const char* key1_caption;
  menu_item_t* key2;
  const char* key2_caption;
  menu_item_t* key3;
  const char* key3_caption;
  menu_item_t* key4;
  const char* key4_caption;
  menu_item_t* key5;
  const char* key5_caption;
  menu_item_t* key6;
  const char* key6_caption;
  menu_item_t* key7;
  const char* key7_caption;
  // keys that are held for a long time
  menu_item_t* hold_key0;
  menu_item_t* hold_key1;
  menu_item_t* hold_key2;
  menu_item_t* hold_key3;
  menu_item_t* hold_key4;
  menu_item_t* hold_key5;
  menu_item_t* hold_key6;
  menu_item_t* hold_key7;
  // spin edit keys
  menu_item_t* decka_up;            // deck a CW
  menu_item_t* decka_dn;            // deck a CCW
  menu_item_t* deckb_up;            // deckb CW
  menu_item_t* deckb_dn;            // deckb CCW
  // press and rotate keys
  menu_item_t* decka_press_up;      // decka press and CW
  menu_item_t* decka_press_dn;      // decka press and CCW
  menu_item_t* deckb_press_up;      // deckb press and CW
  menu_item_t* deckb_press_dn;      // deckb press and CCW
  } keys_t;

typedef keys_t* keysp;

vector_t(menu_itemp)

typedef struct _menu_t
  {
  // a list of items in the menu.
  menu_item_t** menu_items;
  size_t num_items;

  const char *name;     // name of the menu
  const char *caption;  // menu caption
  // this is the menu item with the current focus.  All ok, cancel is sent
  // here.  This is not the selected active menu (only a popup)
  uint16_t active_item;

  // bottom menu item displayed.  The menu will expand to fit the
  // vertical height of the screen.
  uint16_t selected_index;

  // if there is a popup active, this is keys handler
  const keys_t* keys;
  } menu_t;

typedef menu_t* menup;

vector_t(menup)

static inline menu_item_t* menu_item_at(menu_t* menu, size_t index)
  {
  if (index >= menu->num_items)
    return 0;

  return  menu->menu_items[index];
  }

static inline size_t menu_count(menu_t* menu)
  {
  return menu->num_items;
  }

extern void draw_menu_item(handle_t hwnd, menu_widget_t* wnd, const rect_t* wnd_rect, menu_item_t * menu,
  const point_t* menu_origin);

typedef struct _menu_item_spin_edit_t menu_item_spin_edit_t;

typedef enum _key_number_t {
  kn_no_caption = 0x0000,
  kn_key0 = 0x0001,
  kn_key1 = 0x0002,
  kn_key2 = 0x0003,
  kn_key3 = 0x0004,
  kn_key4 = 0x0005,
  kn_key5 = 0x0006,
  kn_key6 = 0x0007,
  kn_key7 = 0x0008,
  kn_decka = 0x4001,
  kn_deckb = 0x8002,
  kn_num_captions
  } key_number_t;

extern uint32_t num_key_captions;   // this should match the physical keys or 0 if no captions

typedef struct _button_t {
  key_number_t key;                 // key/encoder this is associated with
  rect_t rect;                      // relative to the parent
  color_t foreground_color;         // text color
  color_t background_color;         // background color
  color_t selected_color;           // pressed color
  color_t highlight_color;          // pressed background_color
  } button_t;

typedef struct _menu_widget_t {
  widget_t base;

  // the menu captions rectangle
  rect_t menu_caption_rect;
  // these hold the indexes of the keys to have captions shown for
  const button_t *key_captions;
  // this is provided by the implementation to define if any key captions
  // are to be shown.
  uint32_t num_key_captions;
  // this is opened on the id_create message if an id_setup_menu is received
  // before the widget is created  Means the button is pressed while power
  // is applied.
  menu_t *setup_menu;

  point_t menu_rect;
  point_t menu_start;
  color_t selected_background_color;
  color_t text_color;              // text color
  color_t menu_border_color;
  color_t selected_color;
  int32_t menu_timeout;     // 5-second timeout = 25 (20hz wm_timer message)
  const font_t* font;

  bool right_align; // if true the menu is right aligned, otherwise left aligned 

  size_t num_menu_items;
  size_t num_menus;
  // we hold a reference to all loaded menu items as they need to be informed
  // if a controlling variable changed
  menu_item_t ** menu_items;
  menu_t** menus;

  /**
  * This holds the global root menu for the system
  */
  const keys_t* root_keys;
  const keys_t *alarm_keys;

  handle_t window;
  /**
  * this holds the active key handler for the current popup menu.  If there is
  * no popup active the root handler is used.
  */
  const keys_t* active_keys;

  /**
  * This holds the current popup menu that can be a property editor or
  * action items
  */
  menu_t* current_menu;

  int32_t menu_timer;       // as a press/rotate is given this sets the tick-timeout

  menu_item_spin_edit_t* edit;   // currently displayed edit.
  } menu_widget_t;

extern void default_msg_handler(menu_widget_t* wnd, menu_item_t * item, const canmsg_t* msg);
extern void menu_widget_on_paint(handle_t canvas, const rect_t *wnd_rect, const canmsg_t* msg, void* wnddata);
/**
 * @brief Used to over-ride the popup menu system when an alarm is being processed
 * @param is_alarm 
*/
extern void use_alarm_keys(bool is_alarm);

////////////////////////////////////////////////////////////////////////////////
//
// Item handlers for a POPUP item
//
typedef struct _menu_item_menu_t {
  menu_item_t base;

  // if this is a checklist popup then this holds the short value
  // that is matched to create the checklist option.
  int16_t value;
  menu_t* menu;
  } menu_item_menu_t;
extern menu_item_action_result item_menu_evaluate(menu_widget_t* wnd, menu_item_t * item, const canmsg_t* msg);

////////////////////////////////////////////////////////////////////////////////
//
// Item handlers for a EVENT item
//
typedef struct _menu_item_event_t {
  menu_item_t base;
  uint16_t can_id;
  variant_t value;
  // called when menu opened.  This is to allow the menu to set the
  // value of the event item based on the current value of the controlling
  // variable.
  get_value_fn get_value;
  const selected_unit_t* converter; // the converter to use for the value
  evaluate_fn evaluate;
  } menu_item_event_t;

extern menu_item_action_result item_event_evaluate(menu_widget_t* wnd, menu_item_t * item, const canmsg_t* msg);


////////////////////////////////////////////////////////////////////////////////
//
// Item handlers for a CHECKLIST item
//
// A checklist is a popup menu with the events being sent to the
// checklist which then does a message.  This is to allow the current value
// to be displayed as the popup menu selected item
//
typedef struct _menu_item_checklist_t menu_item_checklist_t;
typedef result_t (*get_selected_fn)(menu_item_t * checklist, uint32_t *value);

typedef struct _menu_item_checklist_t {
  menu_item_t base;

  get_selected_fn get_selected;
  // item that is selected in the list.
  uint32_t selected_item;

  // this holds the checklist items
  menu_t* popup;
  } menu_item_checklist_t;

extern menu_item_action_result item_checklist_evaluate(menu_widget_t* wnd, menu_item_t * item, const canmsg_t* msg);
extern void item_checklist_event(menu_widget_t* wnd, menu_item_t * item, const canmsg_t* msg);


////////////////////////////////////////////////////////////////////////////////
//
// Item handlers for a spin edit item
//
typedef struct _menu_item_spin_edit_t {
  menu_item_t base;

  get_value_fn get_value;
  set_value_fn set_value;
  get_value_fn get_max_value;
  get_value_fn get_min_value;
  keys_t *keys;             // keys to use when the edit is active
  bool circular;

  // only one of these is defined, converter is preferred
  const selected_unit_t *converter; // the converter to use for the value

  const char* format;       // defaults to %d

  float min_value;        // minimum value
  float max_value;        // maximum value

  // runtime values
  float value;
  } menu_item_spin_edit_t;

extern void item_spin_edit_paint(handle_t canvas, menu_widget_t* wnd, const rect_t* wnd_rect, menu_item_t * item, const rect_t* rect, bool is_selected);
extern menu_item_action_result item_spin_edit_evaluate(menu_widget_t* wnd, menu_item_t * item, const canmsg_t* msg);
extern void item_spin_edit_event(menu_widget_t* wnd, menu_item_t * item, const canmsg_t* msg);

extern result_t find_menu(menu_widget_t *wnd, const char *name, const menu_t **menu);


extern void default_paint_handler(handle_t canvas, menu_widget_t* wnd, const rect_t* wnd_rect, menu_item_t * item, const rect_t* area, bool is_selected);
extern bool default_enable_handler(menu_widget_t* wnd, menu_item_t * item, const canmsg_t* msg);

extern result_t create_menu_window(handle_t parent, uint32_t flags, menu_widget_t* wnd, handle_t* hndl);

/**
 * Close all menu's.  Called by the cancel: menu item
 */
extern void close_menu(menu_widget_t* wnd);

#endif
