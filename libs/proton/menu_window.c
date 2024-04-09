#include "menu_window.h"
#include "proton.h"

typedef struct _datatype_lookup_t
  {
  const char *name;
  uint8_t datatype;
  uint16_t length;
  } datatype_lookup_t;

/**
 * Close all menu's.  Called by the cancel: menu item
 */
static void close_menu(menu_widget_t *wnd);
/**
 * Show a popup menu at the left of the menu window.
 * @param Popup           menu to display
 * @param option_selected Option that is selected. Will be the bottom option
 */
static void show_menu(menu_widget_t *wnd, menu_t *);
/**
 * @brief Show a popup menu that is a checklist
 * @param wnd         window to display on
 * @param checklist   checklist to display
*/
static void show_checklist_menu(menu_widget_t *wnd, menu_item_checklist_t *checklist);
/**
 * Change the first display item to list item-1 if possible
 */
static void scroll_menu_up(menu_widget_t *wnd);
/**
 * Change the first display item to list item +1 if possible
 */
static void scroll_menu_down(menu_widget_t *wnd);
/**
 * Open the popup menu editor and show the edit menu
 */
static void show_item_editor(menu_widget_t *wnd, menu_item_t *item);

static void copy_ptr(const void *src, void **dst)
  {
  *dst = (void *) src;
  }

void menu_window_on_paint(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnddata)
  {
  menu_widget_t *wnd = (menu_widget_t *) wnddata;
  // erase the overlay
  rectangle(canvas, wnd_rect, color_hollow, color_hollow, wnd_rect);

  extent_t ex;
  rect_extents(wnd_rect, &ex);

  rect_t rect;
  if (wnd != 0 && wnd->current_menu != 0)
    {
    // draw the root menu
    point_t menu_pt = {wnd->menu_start.x, wnd->menu_start.y};
    int32_t item_width = rect_width(wnd_rect) / 3;
    int32_t item_height = 20;

    extent_t item_extents = {item_width, item_height};

    // see if we are displaying a popup menu
    menu_pt.x = wnd->menu_start.x;
    menu_pt.y = wnd->menu_start.y - item_height;

    // determine how menu items to draw from the item
    uint16_t items_avail = wnd->menu_start.y / item_height;

    size_t num_items = menu_count(wnd->current_menu);
    uint16_t drawing_item = 0;
    size_t index = 0;

    if (num_items > items_avail)
      index = wnd->current_menu->selected_index; // start drawing at the selected index

    // draw the visible items
    for (; index < num_items && drawing_item < items_avail; drawing_item++, index++)
      {
      menu_item_t *item = menu_item_at(wnd->current_menu, index);
      (*item->paint)(canvas, wnd, wnd_rect, item,
                     rect_create(menu_pt.x, menu_pt.y, menu_pt.x + item_extents.dx, menu_pt.y + item_extents.dy, &rect),
                     index == wnd->current_menu->selected_index);

      point_t pt;
      // now ask the item to draw if it is selected
      if (item->is_selected)
        draw_menu_item(canvas, wnd, wnd_rect, item, point_create(menu_pt.x + item_width, menu_pt.y, &pt));

      // skip up one.
      menu_pt.y -= item_height;
      }
    }
  }

static void show_menu(menu_widget_t *wnd, menu_t *menu)
  {
  // assign the menu
  wnd->current_menu = menu;
  menu->selected_index = 0; // always start at first item
  // and set the keys
  wnd->active_keys = menu->keys;
  wnd->menu_timer = wnd->menu_timeout;

  invalidate(wnd->window);
  }

void show_checklist_menu(menu_widget_t* wnd, menu_item_checklist_t* checklist)
  {
  wnd->current_menu = checklist->popup;
  
  // set the selected item
  for (uint16_t i = 0; i < checklist->popup->num_items; i++)
    (*checklist->popup->menu_items)[i].is_selected = i == checklist->selected_item;

  checklist->popup->selected_index = checklist->selected_item;

  // and set the keys
  wnd->active_keys = checklist->popup->keys;
  wnd->menu_timer = wnd->menu_timeout;

  invalidate(wnd->window);
  }

static void show_item_editor(menu_widget_t *wnd, menu_item_t *item)
  {
  if (item->item_type == mi_edit)
    {
    // select the popup menu
    item->editor_open = true;

    // TODO: edit the item....
    }
  }

static void close_menu(menu_widget_t *wnd)
  {
  if (wnd->current_menu != 0)
    wnd->current_menu->selected_index = 0;

  wnd->active_keys = 0;
  wnd->current_menu = 0;
  wnd->menu_timer = 0;

  wnd->active_keys = wnd->root_keys;

  invalidate(wnd->window);
  }

////////////////////////////////////////////////////////////////////////////////
//
// Generic handlers
//

static const char *to_string(const canmsg_t *msg, const char *format, char *buffer, int len)
  {
  // the format specifier is used to work this out
  if (strcmp(format, "%int8") == 0)
    {
    snprintf(buffer, len, "%d", msg->data[0]);
    }
  else
    strncpy(buffer, "not implemented", len);


  return buffer;
  }

bool default_enable_handler(menu_widget_t *wnd, menu_item_t *item, const canmsg_t *msg)
  {
  if (item->controlling_param == 0 ||
      item->enable_regex == 0)
    return true;

  // get a string that is the format of the variable
  char buffer[REG_STRING_MAX] = {0};

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
 * handle_t hwnd, menu_widget_t *wnd, const rect_t *wnd_rect, struct _menu_item_t *item, const rect_t *rect
 */
void default_paint_handler(handle_t canvas,
                           menu_widget_t *wnd,
                           const rect_t *wnd_rect,
                           menu_item_t*item,
                           const rect_t *area,
                           bool is_selected)
  {
  point_t center_pt = {
                       (rect_width(area) >> 1) + area->left,
                       (rect_height(area) >> 1) + area->top
    };

  rectangle(canvas, wnd_rect, wnd->border_color,
            is_selected ? wnd->selected_background_color : wnd->base.background_color, area);

  // calculate the text extents
  extent_t ex;
  uint16_t len = (uint16_t) strlen(item->caption);
  text_extent(wnd->font, len, item->caption, &ex);

  center_pt.x -= ex.dx >> 1;
  center_pt.y -= ex.dy >> 1;

  draw_text(canvas, wnd_rect, wnd->font,
            is_selected ? wnd->selected_color : wnd->text_color,
            wnd->base.background_color,
            len, item->caption, &center_pt, 0, 0, 0);
  }

void default_msg_handler(menu_widget_t *wnd, menu_item_t*item, const canmsg_t *msg)
  {
  if (item->controlling_param == get_can_id(msg))
    memcpy(& ((menu_item_t *) item)->controlling_variable, msg, sizeof (canmsg_t));
  }

menu_item_action_result item_menu_evaluate(menu_widget_t *wnd,
                                           menu_item_t *item,
                                           const canmsg_t *msg)
  {
  menu_item_menu_t *menu = (menu_item_menu_t *) item;

  show_menu(wnd, menu->menu);

  return mia_show;
  }

menu_item_action_result item_event_evaluate(menu_widget_t *wnd, menu_item_t*item, const canmsg_t *_msg)
  {
  menu_item_event_t *mi = (menu_item_event_t *) item;

  canmsg_t msg;
  variant_to_msg(&mi->value, mi->can_id, can_type_from_variant(mi->value.vt), &msg);
  // see if the event is an internal menu one
  if (mi->can_id >= id_first_internal_msg && mi->can_id <= id_last_internal_msg)
    {
    handle_t root_window;
    if (succeeded(get_screen(&root_window)))
      send_message(root_window, &msg);
    }
  else
    can_send(&msg, DEFAULT_CAN_TIMEOUT);

  return mia_close_item;
  }

menu_item_action_result item_checklist_evaluate(menu_widget_t *wnd,
                                                menu_item_t *item,
                                                const canmsg_t *msg)
  {
  menu_item_checklist_t *checklist_menu = (menu_item_checklist_t *) item;

  if(succeeded((*checklist_menu->get_selected)(checklist_menu, &checklist_menu->selected_item)))
    show_checklist_menu(wnd, checklist_menu);

  return mia_show;
  }

void item_checklist_event(menu_widget_t *wnd, menu_item_t *item, const canmsg_t *msg)
  {
  // check the enabler
  default_enable_handler(wnd, item, msg);

  menu_item_checklist_t *checklist = (menu_item_checklist_t *) item;
  }

menu_item_action_result item_spin_edit_evaluate(menu_widget_t *wnd,
                                                menu_item_t *item,
                                                const canmsg_t *msg)
  {
  menu_item_spin_edit_t* edit = (menu_item_spin_edit_t*) item;

  wnd->edit = edit;
  wnd->active_keys = edit->keys;

  (*edit->get_value)(edit, &edit->value);

  return mia_show;
  }

void item_spin_edit_event(menu_widget_t *wnd,
                          menu_item_t *item,
                          const canmsg_t *msg)
  {
  // check the enabler
  default_enable_handler(wnd, item, msg);

  menu_item_spin_edit_t *edit = (menu_item_spin_edit_t*) item;

  // see if this changes the edit
  switch (get_can_id(msg))
    {
    case id_menu_ok:
      (*edit->set_value)(edit, edit->value);
      break;
    case id_menu_cancel:
      wnd->edit = 0;
      close_menu(wnd);
      break;
    case id_spin_edit:
    {
      int16_t value;
      get_param_int16(msg, &value);

      edit->value += value;
      if (edit->value < edit->min_value)
        {
        if (edit->circular)
          edit->value = edit->max_value - (edit->min_value - edit->value);
        else
          edit->value = edit->min_value;
        }

      if (edit->value > edit->max_value)
        {
        if (edit->circular)
          edit->value = edit->min_value + (edit->value - edit->max_value);
        else
          edit->value = edit->max_value;
        }
    }
      break;
    }
  }

void draw_menu_item(handle_t hwnd,
                    menu_widget_t *wnd,
                    const rect_t *wnd_rect,
                    menu_item_t*selected_item,
                    const point_t *menu_origin)
  {
  rect_t menu_rect;
  // depending on the type of item the rendering will be different
  // TODO: this is a hack....
  (*selected_item->paint)(hwnd, wnd, wnd_rect, selected_item,
                          rect_create(menu_origin->x, menu_origin->y, menu_origin->x + 80, menu_origin->y + 20, &menu_rect), true);
  }

static bool evaluate_item(handle_t hwnd, menu_widget_t *wnd, const canmsg_t *msg, menu_item_t*item)
  {
  bool changed = false;

  if (item != 0 && (*item->is_enabled)(wnd, item, msg))
    {
    switch ((*item->evaluate)(wnd, item, msg))
      {
      case mia_cancel:
        wnd->active_keys = wnd->root_keys;
        wnd->menu_timer = 0;
        hide_window(hwnd);
        changed = true;
        break;
      case mia_close_item:
        break;
      case mia_enter:
        break;
      case mia_nothing:
        changed = true;
        break;
      case mia_show:
      {
        menu_item_menu_t *mi = (menu_item_menu_t *) item;
        wnd->current_menu = mi->menu;
        wnd->menu_timer = wnd->menu_timeout;
        show_window(hwnd);
        changed = true;
      }
        break;
      }
    }
  return changed;
  }

// these are the attached event handlers

static result_t on_key0(handle_t hwnd, const canmsg_t *msg, void *wnddata)
  {
  menu_widget_t *wnd = (menu_widget_t *) wnddata;

  const keys_t *handler = wnd->active_keys == 0 ? wnd->root_keys : wnd->active_keys;
  if (evaluate_item(hwnd, wnd, msg, handler->key0))
    invalidate(hwnd);

  return s_ok;
  }

static result_t on_key1(handle_t hwnd, const canmsg_t *msg, void *wnddata)
  {
  menu_widget_t *wnd = (menu_widget_t *) wnddata;

  const keys_t *handler = wnd->active_keys == 0 ? wnd->root_keys : wnd->active_keys;
  if (evaluate_item(hwnd, wnd, msg, handler->key1))
    invalidate(hwnd);

  return s_ok;
  }

static result_t on_key2(handle_t hwnd, const canmsg_t *msg, void *wnddata)
  {
  menu_widget_t *wnd = (menu_widget_t *) wnddata;

  const keys_t *handler = wnd->active_keys == 0 ? wnd->root_keys : wnd->active_keys;
  if (evaluate_item(hwnd, wnd, msg, handler->key2))
    invalidate(hwnd);

  return s_ok;
  }

static result_t on_key3(handle_t hwnd, const canmsg_t *msg, void *wnddata)
  {
  menu_widget_t *wnd = (menu_widget_t *) wnddata;

  const keys_t *handler = wnd->active_keys == 0 ? wnd->root_keys : wnd->active_keys;
  if (evaluate_item(hwnd, wnd, msg, handler->key3))
    invalidate(hwnd);

  return s_ok;
  }

static result_t on_key4(handle_t hwnd, const canmsg_t *msg, void *wnddata)
  {
  menu_widget_t *wnd = (menu_widget_t *) wnddata;

  const keys_t *handler = wnd->active_keys == 0 ? wnd->root_keys : wnd->active_keys;
  if (evaluate_item(hwnd, wnd, msg, handler->key4))
    invalidate(hwnd);

  return s_ok;
  }

static result_t on_key5(handle_t hwnd, const canmsg_t *msg, void *wnddata)
  {
  menu_widget_t *wnd = (menu_widget_t *) wnddata;

  const keys_t *handler = wnd->active_keys == 0 ? wnd->root_keys : wnd->active_keys;
  if (evaluate_item(hwnd, wnd, msg, handler->key5))
    invalidate(hwnd);

  return s_ok;
  }

static result_t on_key6(handle_t hwnd, const canmsg_t *msg, void *wnddata)
  {
  menu_widget_t *wnd = (menu_widget_t *) wnddata;

  const keys_t *handler = wnd->active_keys == 0 ? wnd->root_keys : wnd->active_keys;
  if (evaluate_item(hwnd, wnd, msg, handler->key6))
    invalidate(hwnd);

  return s_ok;
  }

static result_t on_key7(handle_t hwnd, const canmsg_t *msg, void *wnddata)
  {
  menu_widget_t *wnd = (menu_widget_t *) wnddata;

  const keys_t *handler = wnd->active_keys == 0 ? wnd->root_keys : wnd->active_keys;
  if (evaluate_item(hwnd, wnd, msg, handler->key7))
    invalidate(hwnd);

  return s_ok;
  }

static result_t on_decka(handle_t hwnd, const canmsg_t *msg, void *wnddata)
  {
  menu_widget_t *wnd = (menu_widget_t *) wnddata;

  int16_t value;
  get_param_int16(msg, &value);

  const keys_t *handler = wnd->active_keys == 0 ? wnd->root_keys : wnd->active_keys;
  if (value > 0)
    {
    if (evaluate_item(hwnd, wnd, msg, handler->decka_up))
      invalidate(hwnd);
    }
  else if (value < 0)
    {
    if (evaluate_item(hwnd, wnd, msg, handler->decka_dn))
      invalidate(hwnd);
    }

  return s_ok;
  }

static result_t on_deckb(handle_t hwnd, const canmsg_t *msg, void *wnddata)
  {
  menu_widget_t *wnd = (menu_widget_t *) wnddata;
  int16_t value;
  get_param_int16(msg, &value);

  const keys_t *handler = wnd->active_keys == 0 ? wnd->root_keys : wnd->active_keys;
  if (value > 0)
    {
    if (evaluate_item(hwnd, wnd, msg, handler->deckb_up))
      invalidate(hwnd);
    }
  else if (value < 0)
    {
    if (evaluate_item(hwnd, wnd, msg, handler->deckb_dn))
      invalidate(hwnd);
    }

  return s_ok;
  }

static result_t on_press_decka(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  menu_widget_t* wnd = (menu_widget_t*) wnddata;

  int16_t value;
  get_param_int16(msg, &value);

  const keys_t* handler = wnd->active_keys == 0 ? wnd->root_keys : wnd->active_keys;
  if (value > 0)
    {
    if (evaluate_item(hwnd, wnd, msg, handler->decka_press_up))
      invalidate(hwnd);
    }
  else if (value < 0)
    {
    if (evaluate_item(hwnd, wnd, msg, handler->decka_press_dn))
      invalidate(hwnd);
    }

  return s_ok;
  }

static result_t on_press_deckb(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  menu_widget_t* wnd = (menu_widget_t*) wnddata;
  int16_t value;
  get_param_int16(msg, &value);

  const keys_t* handler = wnd->active_keys == 0 ? wnd->root_keys : wnd->active_keys;
  if (value > 0)
    {
    if (evaluate_item(hwnd, wnd, msg, handler->deckb_press_up))
      invalidate(hwnd);
    }
  else if (value < 0)
    {
    if (evaluate_item(hwnd, wnd, msg, handler->deckb_press_dn))
      invalidate(hwnd);
    }

  return s_ok;
  }

static result_t on_menu_up(handle_t hwnd, const canmsg_t *msg, void *wnddata)
  {
  bool changed = false;
  menu_widget_t *wnd = (menu_widget_t *) wnddata;
  rect_t wnd_rect;
  window_rect(hwnd, &wnd_rect);

  uint16_t id = get_can_id(msg);
  int16_t value;
  get_param_int16(msg, &value);

  // used to change the current popup menu index
  if (wnd->current_menu != 0)
    {
    size_t count = menu_count(wnd->current_menu);
    if (wnd->current_menu->selected_index < count - 1)
      {
      wnd->current_menu->selected_index++;
      changed = true;
      }
    }

  if (changed)
    invalidate(hwnd);

  return s_ok;
  }

static result_t on_menu_dn(handle_t hwnd, const canmsg_t *msg, void *wnddata)
  {
  bool changed = false;
  menu_widget_t *wnd = (menu_widget_t *) wnddata;
  rect_t wnd_rect;
  window_rect(hwnd, &wnd_rect);

  uint16_t id = get_can_id(msg);
  int16_t value;
  get_param_int16(msg, &value);

  // used to change the current popup menu index
  if (wnd->current_menu != 0 && wnd->current_menu->selected_index > 0)
    {
    wnd->current_menu->selected_index--;
    changed = true;
    }

  if (changed)
    invalidate(hwnd);

  return s_ok;
  }

static result_t on_menu_left(handle_t hwnd, const canmsg_t *msg, void *wnddata)
  {
  bool changed = false;
  menu_widget_t *wnd = (menu_widget_t *) wnddata;
  rect_t wnd_rect;
  window_rect(hwnd, &wnd_rect);

  uint16_t id = get_can_id(msg);
  int16_t value;
  get_param_int16(msg, &value);

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
    invalidate(hwnd);

  return s_ok;
  }

static result_t on_menu_right(handle_t hwnd, const canmsg_t *msg, void *wnddata)
  {
  bool changed = false;
  menu_widget_t *wnd = (menu_widget_t *) wnddata;
  rect_t wnd_rect;
  window_rect(hwnd, &wnd_rect);

  uint16_t id = get_can_id(msg);
  int16_t value;
  get_param_int16(msg, &value);

  if (wnd->current_menu != 0)
    {
    menu_item_t*item = menu_item_at(wnd->current_menu, wnd->current_menu->selected_index);
    if (item->item_type == mi_menu)
      {
      menu_item_menu_t *mi = (menu_item_menu_t *) item;
      show_menu(wnd, mi->menu); // show the popup
      }
    else if (item->item_type == mi_edit)
      show_item_editor(wnd, item);
    changed = true;
    }

  if (changed)
    invalidate(hwnd);

  return s_ok;
  }

static result_t on_ok(handle_t hwnd, const canmsg_t *msg, void *wnddata)
  {
  bool changed = false;
  menu_widget_t *wnd = (menu_widget_t *) wnddata;
  rect_t wnd_rect;
  window_rect(hwnd, &wnd_rect);

  uint16_t id = get_can_id(msg);
  int16_t value;
  get_param_int16(msg, &value);

  // this is sent when an item is selected.  Only ever one
  if (wnd->current_menu != 0)
    {
    menu_item_t*item = menu_item_at(wnd->current_menu, wnd->current_menu->selected_index);
    if (item != 0)
      switch ((*item->evaluate)(wnd, item, msg))
        {
        case mia_cancel:
        case mia_close_item:
        case mia_enter:
          close_menu(wnd);
          changed = true;
          break;
        case mia_show:
          changed = true;
          break;
        case mia_nothing:
          break;
        }
    }

  if (changed)
    invalidate(hwnd);

  return s_ok;
  }

static result_t on_cancel(handle_t hwnd, const canmsg_t *msg, void *wnddata)
  {
  menu_widget_t *wnd = (menu_widget_t *) wnddata;

  close_menu(wnd);
  hide_window(hwnd);
  invalidate(hwnd);

  return s_ok;
  }

result_t menu_wndproc(handle_t hwnd, const canmsg_t *msg, void *wnddata)
  {
  bool changed = false;
  menu_widget_t *wnd = (menu_widget_t *) wnddata;

  size_t item;

  size_t count = num_menu_items;
  if (count != 0)
    {
    // we go through all of the menu items and check to see if they are
    // listening, and if their state has changed
    for (item = 0; item < count; item++)
      {
      menu_item_t *mi = menu_items[item];

      // update the controlling variable if it is being watched.
      if (mi != 0 && mi->event != 0)
        (*mi->event)(wnd, mi, msg);
      }
    }

  switch (get_can_id(msg))
    {
    case id_timer:
#ifndef _DEBUG
      if (wnd->menu_timer > 0)
        {
        wnd->menu_timer--;

        if (wnd->menu_timer == 0)
          {
          close_menu(wnd);
          hide_window(hwnd);
          invalidate(hwnd);
          }
        }
#endif
      break;
    case id_paint:
      on_paint_widget(hwnd, msg, wnddata);
      break;
    case id_key0:
      on_key0(hwnd, msg, wnddata);
      break;
    case id_key1:
      on_key1(hwnd, msg, wnddata);
      break;
    case id_key2:
      on_key2(hwnd, msg, wnddata);
      break;
    case id_key3:
      on_key3(hwnd, msg, wnddata);
      break;
    case id_key4:
      on_key4(hwnd, msg, wnddata);
      break;
    case id_key5:
      on_key5(hwnd, msg, wnddata);
      break;
    case id_key6:
      on_key6(hwnd, msg, wnddata);
      break;
    case id_key7:
      on_key7(hwnd, msg, wnddata);
      break;
    case id_decka:
      on_decka(hwnd, msg, wnddata);
      break;
    case id_deckb:
      on_deckb(hwnd, msg, wnddata);
      break;
    case id_press_decka:
      on_press_decka(hwnd, msg, wnddata);
      break;
    case id_press_deckb:
      on_press_deckb(hwnd, msg, wnddata);
      break;
    case id_menu_left:
      on_menu_left(hwnd, msg, wnddata);
      break;
    case id_menu_right:
      on_menu_right(hwnd, msg, wnddata);
      break;
    case id_menu_up:
      on_menu_up(hwnd, msg, wnddata);
      break;
    case id_menu_dn:
      on_menu_dn(hwnd, msg, wnddata);
      break;
    case id_menu_cancel:
      on_cancel(hwnd, msg, wnddata);
      break;
    case id_menu_ok:
      on_ok(hwnd, msg, wnddata);
      break;
    }

  return defwndproc(hwnd, msg, wnddata);
  }

result_t find_menu(menu_widget_t* wnd, const char* name, menu_t** menu)
  {
  for (uint16_t i = 0; i < num_menus; i++)
    {
    menu_t *item = menus[i];
    if (strcmp(item->name, name) == 0)
      {
      *menu = item;
      return s_ok;
      }
    }

  return e_not_found;
  }

static handle_t menu_wnd = 0;

result_t create_menu_window(handle_t parent, uint16_t id, aircraft_t* aircraft, menu_widget_t* wnd, handle_t* out)
  {
  result_t result;
  handle_t hndl;
  if (failed(result = create_widget(parent, id, menu_wndproc, &wnd->base, &hndl)))
    return result;

  menu_wnd = hndl; // this is globally used

  // hidden so only show on popup
  hide_window(hndl);

  wnd->window = hndl;

  wnd->base.on_paint = menu_window_on_paint;

  if (out != 0)
    *out = hndl;

  return s_ok;
  }

void use_alarm_keys(bool is_alarm)
  {
  if (menu_wnd == 0)
    return;

  menu_widget_t* wnd;
  get_wnddata(menu_wnd, (void **) &wnd);

  close_menu(wnd);

  if (is_alarm)
    wnd->active_keys = wnd->alarm_keys;
  else
    wnd->active_keys = wnd->root_keys;
  }
