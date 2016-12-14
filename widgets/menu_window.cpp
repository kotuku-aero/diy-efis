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
#include "application.h"
#include "menu_window.h"
#include "autopilot.h"
#include "layout_window.h"
#include "can_aerospace.h"
#include <stdio.h>
#include <sys/types.h>
#include "regex.h"
#include "pens.h"
#include "fonts.h"
#include "spatial.h"

struct datatype_lookup_t {
  const char *name;
  uint8_t datatype;
  size_t length;
};

static datatype_lookup_t datatypes[] = {
    { "SHORT", kotuku::msg_t::datatype_short, 2 },
    { "FLOAT", kotuku::msg_t::datatype_float, 4 },
    { 0, kotuku::msg_t::datatype_nodata, 0 }
};

typedef std::vector<std::string> attributes_t;

/**
 * Split a string into its parts
 * @param option      string to split
 * @param attributes  attributes of the string
 * @param delimiter   split character
 */
static void split_tuple(const char *option, attributes_t &attributes, char delimiter = ':')
  {
  const char *ptr = option;
  while(*ptr != 0)
    {
    if(*ptr == '"')      // embedded string
      {
      ptr++;
      while(*ptr != 0 && *ptr != '"')
        {
        if(*ptr == '\\')
            ptr++;        // skip escape char
        ptr++;
        }
      }
    if(*ptr == '{')       // embedded can message, threat as a string
      {
      ptr++;
      while(*ptr != 0 && *ptr != '}')
        {
        if(*ptr == '\\')
            ptr++;        // skip escape char
        ptr++;
        }
      }
    if(*ptr == '\\')
      ptr++;              // skip escape char
    else if(*ptr == delimiter)
      {
      attributes.push_back(std::string(option, ptr));
      option = ptr+1;
      }

    ptr++;
    }

  if(ptr != option)
    attributes.push_back(option);
  }

static kotuku::msg_t parse_msg(const std::string &defn)
  {
  kotuku::msg_t result;
  // a message tuple is {addr,type[values]}
  if(defn[0] != '{' || defn[defn.length()-1] != '}')
    {
    trace_error("Can Definition (%s) is not enclosed in {}\n", defn.c_str());
    return result;
    }

  attributes_t attributes;
  split_tuple(defn.substr(1, defn.length()-2).c_str(), attributes, ',');

  if(attributes.size() < 2)
    {
    trace_error("Definition (%s) must have at least 2 attributes\n", defn.c_str());
    return result;
    }

  const datatype_lookup_t *dt;
  for(dt = datatypes; dt->name != 0; dt++)
    if(strcasecmp(dt->name, attributes[1].c_str())== 0)
      break;

  result.id = atoi(attributes[0].c_str());
  result.length(4 + dt->length);
  result.msg.raw[0] = 0;
  result.msg.raw[1] = dt->datatype;
  result.msg.raw[2] = 0;
  result.msg.raw[3] = 0;
  switch(dt->datatype)
    {
    case kotuku::msg_t::datatype_short :
      if(attributes.size()> 2)
        {
        short num = (short) atoi(attributes[2].c_str());
        result.msg.raw[4] = (uint8_t)(num >> 8);
        result.msg.raw[5] = (uint8_t)num;
        }
      else
        trace_error("CAN Definition (%s) defines a short, but there are not 3 attributes\n", defn.c_str());
      break;
    case kotuku::msg_t::datatype_float :
      if(attributes.size()> 2)
        {
        float num = (float) atof(attributes[2].c_str());
        const uint8_t *np = reinterpret_cast<uint8_t *>(&num);
        result.msg.raw[4] = np[3];
        result.msg.raw[5] = np[2];
        result.msg.raw[6] = np[1];
        result.msg.raw[7] = np[0];
        }
      else
        trace_error("CAN Definition (%s) defines a float, but there are not 3 attributes\n", defn.c_str());
      break;
    }

  return result;
  }

static const short menu_timeout = 25;             // 5-second timeout

void kotuku::menu_window_t::init(const char *section)
  {
  _section = section;
  _menu_timer = 0;
  _popup_menu = 0;
  _active_menu = 0;
  _popup_menu_index = 0;

  int value;
  if(failed(application_t::instance->hal()->get_config_value(section, "menu-rect-x", value)))
    _menu_rect_x = 88;
  else
    _menu_rect_x = value;

  if(failed(application_t::instance->hal()->get_config_value(section, "menu-rect-y", value)))
    _menu_rect_y = 32;
  else
    _menu_rect_y = value;

  if(failed(application_t::instance->hal()->get_config_value(section, "menu-start-x", value)))
    _menu_start_x = 0;
  else
    _menu_start_x = value;

  if(failed(application_t::instance->hal()->get_config_value(section, "menu-start-y", value)))
    _menu_start_y = window_rect().height() - _menu_rect_y;
  else
    _menu_start_y = value;

  load_menus();

  subscribe(id_key0);
  subscribe(id_key1);
  subscribe(id_key2);
  subscribe(id_key3);
  subscribe(id_key4);
  subscribe(id_decka);
  subscribe(id_deckb);
  subscribe(id_menu_up);
  subscribe(id_menu_dn);
  subscribe(id_menu_left);
  subscribe(id_menu_right);
  subscribe(id_menu_ok);
  subscribe(id_menu_cancel);
  subscribe(id_menu_edit);
  subscribe(id_menu_back);
  subscribe(id_menu_select);
  subscribe(id_qnh);
  subscribe(id_heading);
  }

kotuku::menu_window_t::menu_window_t(widget_t &parent, const rect_t &extents, const char *section)
: widget_t("menu", parent, extents)
  {
  init(section);
  }

kotuku::menu_window_t::menu_window_t(screen_t *screen, const char *section)
: widget_t(0, screen)
  {
  init(section);
  }

bool kotuku::menu_window_t::ev_msg(const msg_t &msg)
  {
  bool changed = false;
  uint16_t id = msg.message_id();
  switch(id)
    {
    case id_key0 :
      if(_active_menu != 0)
        {
        if(_active_menu->key0() != 0 && msg.msg_data_16() > 0 && _active_menu->key0()->is_enabled())
          {
          if(_menu_timer != 0)
            _active_menu->key0()->evaluate_action(msg);

          _menu_timer = menu_timeout;
          changed=true;
          }
        }
      else
        {
        show_menu(_root_menu);
        _menu_timer = menu_timeout;
        changed=true;
        }
      break;
    case id_key1 :
      if(_active_menu != 0)
        {
        if(_active_menu->key1() != 0 && msg.msg_data_16() > 0&& _active_menu->key1()->is_enabled())
          {
          if(_menu_timer != 0)
            _active_menu->key1()->evaluate_action(msg);

          _menu_timer = menu_timeout;
          changed=true;
          }
        }
      else
        {
        show_menu(_root_menu);
        _menu_timer = menu_timeout;
        changed=true;
        }
      break;
    case id_key2 :
      if(_active_menu != 0)
        {
        if(_active_menu->key2() != 0 && msg.msg_data_16() > 0&& _active_menu->key2()->is_enabled())
          {
          if(_menu_timer != 0)
            _active_menu->key2()->evaluate_action(msg);

          _menu_timer = menu_timeout;
          changed=true;
          }
        }
      else
        {
        show_menu(_root_menu);
        _menu_timer = menu_timeout;
        changed=true;
        }
      break;
    case id_key3 :
      if(_active_menu != 0)
        {
        if(_active_menu->key3() != 0 && msg.msg_data_16() > 0&& _active_menu->key3()->is_enabled())
          {
          if(_menu_timer != 0)
            _active_menu->key3()->evaluate_action(msg);

          _menu_timer = menu_timeout;
          changed=true;
          }
        }
      else
        {
        show_menu(_root_menu);
        _menu_timer = menu_timeout;
        changed=true;
        }
      break;
    case id_key4 :
      if(_active_menu != 0)
        {
        if(_active_menu->key4() != 0 && msg.msg_data_16() > 0 && _active_menu->key4()->is_enabled())
          {
          if(_menu_timer != 0)
            _active_menu->key4()->evaluate_action(msg);

          _menu_timer = menu_timeout;
          changed=true;
          }
        }
      else
        {
        show_menu(_root_menu);
        _menu_timer = menu_timeout;
        changed=true;
        }
      break;
    case id_deckb :
      {
      short value = msg.msg_data_16();
      const root_menu_t *handler = _active_menu == 0 ? _root_menu : _active_menu;
      if(value > 0)
        {
        if(handler->deckb_up() != 0 && handler->deckb_up()->is_enabled())
          {
          if(_menu_timer != 0)
            _menu_timer = menu_timeout;

          handler->deckb_up()->evaluate_action(msg);
          changed=true;
          }
        }
      else if(value < 0)
        {
        if(handler->deckb_dn() != 0 && handler->deckb_dn()->is_enabled())
          {
          if(_menu_timer != 0)
            _menu_timer = menu_timeout;

          handler->deckb_dn()->evaluate_action(msg);
          changed=true;
          }
        }
      }
      break;
    case id_decka :
      {
      short value = msg.msg_data_16();
      const root_menu_t *handler = _active_menu == 0 ? _root_menu : _active_menu;
      if(value > 0)
        {
        if(handler->decka_up() != 0 && handler->decka_up()->is_enabled())
          {
          if(_menu_timer != 0)
            _menu_timer = menu_timeout;

          handler->decka_up()->evaluate_action(msg);
          changed=true;
          }
        }
      else if(value < 0)
        {
        if(handler->decka_dn() != 0 && handler->decka_dn()->is_enabled())
          {
          if(_menu_timer != 0)
            _menu_timer = menu_timeout;

          handler->decka_dn()->evaluate_action(msg);
          changed=true;
          }
        }
      }
      break;
    case id_menu_up :
      break;
    case id_menu_dn :
      break;
    case id_menu_left :
      break;
    case id_menu_right :
      break;
    case id_menu_ok :
      break;
    case id_menu_cancel :
      close_menu();
      break;
    case id_menu_edit :
      show_popup_menu_editor();
      break;
    case id_menu_back :
      close_popup_menu();
      break;
    case id_menu_select :
      break;
    default :
      return false;
    }

  if(changed)
    invalidate();

  return true;
  }

void kotuku::menu_window_t::update_background(canvas_t &background_canvas)
  {
  }

void kotuku::menu_window_t::update_window()
  {
  rect_t draw_area(0, 0, window_rect().width(), window_rect().height());
  clipping_rectangle(draw_area);

  if(_active_menu != 0)
    {
    // draw the root menu
    point_t menu_pt(_menu_start_x, _menu_start_y);
    gdi_dim_t item_width = draw_area.width() / 5;
    gdi_dim_t item_height = 20;

    extent_t item_extents(item_width, item_height);

    if(_active_menu->key0() != 0)
      _active_menu->key0()->paint(rect_t(menu_pt, item_extents), false);

    menu_pt.x += item_width;

    if(_active_menu->key1() != 0)
      _active_menu->key1()->paint(rect_t(menu_pt, item_extents), false);

    menu_pt.x += item_width;

    if(_active_menu->key2() != 0)
      _active_menu->key2()->paint(rect_t(menu_pt, item_extents), false);

    menu_pt.x += item_width;

    if(_active_menu->key3() != 0)
      _active_menu->key3()->paint(rect_t(menu_pt, item_extents), false);

    menu_pt.x += item_width;

    if(_active_menu->key4() != 0)
      _active_menu->key4()->paint(rect_t(menu_pt, item_extents), false);

    if(_popup_menu != 0)
      {
      menu_pt.x = _menu_start_x;
      menu_pt.y = _menu_start_y - item_height;

      // determine how menu items to draw from the item
      size_t items_avail = _menu_start_y / item_height;

      // scroll to ensure the item is visible in the menu
      if(_popup_menu_option_selected > _popup_menu_index)
        _popup_menu_index = _popup_menu_option_selected;

      size_t top_item = _popup_menu_index - items_avail +1;

      if(top_item < 0)
        top_item = items_avail;

      if(top_item > _popup_menu_option_selected)
        {
        // scroll menu to make the top item the selected item
        top_item = _popup_menu_option_selected;
        _popup_menu_index = top_item + items_avail -1;
        }

      if(_popup_menu_index >= _popup_menu->size())
        _popup_menu_index = _popup_menu->size()-1;

      // draw the visible items
      for(size_t item = _popup_menu_index; item >= top_item; item--)
        {
        bool is_selected = item == _popup_menu_option_selected;
        (*_popup_menu)[item]->paint(rect_t(menu_pt, item_extents), is_selected);
        // now ask the item to draw if it is selected
        if(is_selected)
          _popup_menu->draw_popup(item, this, point_t(menu_pt.x + item_width, menu_pt.y));

        // skip up one.
        menu_pt.y -= item_height;
        }
      }
    }
  }

void kotuku::menu_window_t::show_menu(const root_menu_t *menu)
  {
  if(_active_menu != 0)
    _menu_stack.push_back(_active_menu);

  _active_menu = menu;
  invalidate();
  }

void kotuku::menu_window_t::show_popup_menu(const popup_menu_t *menu, size_t option_selected)
  {
  _popup_menu = menu;
  _popup_menu_option_selected = option_selected;
  }

void kotuku::menu_window_t::show_popup_menu_editor()
  {
  const menu_item_t *item = (*_popup_menu)[_popup_menu_option_selected];

  if(item->item_type() == mi_edit)
    {
    // select the popup menu
    _editor_open = true;
    menu_item_edit_t *item_editor =
        const_cast<menu_item_edit_t *>(reinterpret_cast<const menu_item_edit_t *>(item));

    }
  }

void kotuku::menu_window_t::close_menu()
  {
  _active_menu = 0;
  _popup_menu = 0;
  _menu_timer = 0;

  invalidate();
  widget_t *mainframe = reinterpret_cast<widget_t *>(parent());

  mainframe->invalidate();
  }

void kotuku::menu_window_t::close_popup_menu()
  {
  _popup_menu = 0;
  // restore the previous root menu
  pop_menu();

  invalidate();
  }

/**
 * Called after a popup menu closes or a back option is selected
 */
void kotuku::menu_window_t::pop_menu()
  {
  root_menu_t *prev_menu = 0;
  if(_menu_stack.empty())
    {
    close_menu();      // all done
    return;
    }

  show_menu(_menu_stack.back());
  _menu_stack.pop_back();
  }

void kotuku::menu_window_t::load_menus()
  {
  // we get the root menu
  std::string menu_name;
  if(failed(application_t::instance->hal()->get_config_value(_section.c_str(), "root-menu", menu_name)))
    menu_name = "root-menu";

  // set the root menu
  _root_menu = load_menu(menu_name);
  }

kotuku::root_menu_t *kotuku::menu_window_t::load_menu(const std::string &name)
  {
  menus_t::iterator it = _menus.find(name);
  root_menu_t *result = 0;
  if(it == _menus.end())
    {
    result = new root_menu_t(this, name);
    _menus.insert(std::make_pair(name, result));
    }
  else
    {
    if(it->second->menu_type() != mt_root)
      return 0;

    result = reinterpret_cast<root_menu_t *>(it->second);
    }

  return result;
  }

kotuku::popup_menu_t *kotuku::menu_window_t::load_popup(const std::string &name)
  {
  menus_t::iterator it = _menus.find(name);
  popup_menu_t *result = 0;
  if(it == _menus.end())
    {
    result = new popup_menu_t(this, name);
    _menus.insert(std::make_pair(name, result));
    }
  else
    {
    if(it->second->menu_type() != mt_popup)
      return 0;

    result = reinterpret_cast<popup_menu_t *>(it->second);
    }

  return result;
  }

/**
 * Split a string based on the syntax of item_type(expr)
 * @param value     String to parse
 * @returns The created menu item if it exists
 */
static kotuku::menu_item_t *parse_item(kotuku::menu_window_t *parent, const std::string value)
  {
  kotuku::menu_item_t *result = 0;

  attributes_t attributes;
  split_tuple(value.c_str(), attributes);

  if(attributes.size() == 0)
    {
    trace_error("Menu item must be in the form <name>:[<options>]");
    return 0;
    }

  std::string item_type;
  std::string str_expr;
  std::string::size_type it = attributes[0].find_first_of('(');
  if(it != std::string::npos)
    {
    item_type = attributes[0].substr(0, it);
    str_expr = attributes[0].substr(it+1, attributes[0].length()-it -2);
    }
  else
    item_type = attributes[0];

  const char *options = attributes.size()== 1 ? 0 : attributes[1].c_str();
  const char *expr = str_expr.c_str();

  if(item_type == "menu")
    result = new kotuku::menu_item_menu_t(parent, options, expr);
  else if(item_type == "popup")
    result = new kotuku::menu_item_popup_t(parent, options, expr);
  else if(item_type == "cancel")
    result = new kotuku::menu_item_cancel_t(parent, options, expr);
  else if(item_type == "enter")
    result = new kotuku::menu_item_enter_t(parent, options, expr);
  else if(item_type == "event")
    result = new kotuku::menu_item_event_t(parent, options, expr);
  else if(item_type == "edit")
    result = new kotuku::menu_item_edit_t(parent, options, expr);
  else if(item_type == "checklist")
    result = new kotuku::menu_item_checklist_t(parent, options, expr);

  return result;
  }

kotuku::menu_t::menu_t(menu_window_t *parent)
: _parent(parent),
  _parent_menu(0)
  {
  }

kotuku::menu_t::~menu_t()
  {
  }

///////////////////////////////////////////////////////////////////////////////
//
// Root menu
kotuku::root_menu_t::root_menu_t(menu_window_t *parent, const std::string &name)
: menu_t(parent),
  _key0(0),
  _key1(0),
  _key2(0),
  _key3(0),
  _key4(0),
  _decka_up(0),
  _decka_dn(0),
  _deckb_up(0),
  _deckb_dn(0)
  {
  std::string value;
  std::string expr;

  if(succeeded(application_t::instance->hal()->get_config_value(name.c_str(), "key0", value)))
    _key0 = parse_item(parent, value);

  if(succeeded(application_t::instance->hal()->get_config_value(name.c_str(), "key1", value)))
    _key1 = parse_item(parent, value);

  if(succeeded(application_t::instance->hal()->get_config_value(name.c_str(), "key2", value)))
    _key2 = parse_item(parent, value);

  if(succeeded(application_t::instance->hal()->get_config_value(name.c_str(), "key3", value)))
    _key3 = parse_item(parent, value);

  if(succeeded(application_t::instance->hal()->get_config_value(name.c_str(), "key4", value)))
    _key4 = parse_item(parent, value);

  if(succeeded(application_t::instance->hal()->get_config_value(name.c_str(), "decka-up", value)))
    _decka_up = parse_item(parent, value);

  if(succeeded(application_t::instance->hal()->get_config_value(name.c_str(), "decka-dn", value)))
    _decka_dn = parse_item(parent, value);

  if(succeeded(application_t::instance->hal()->get_config_value(name.c_str(), "deckb-up", value)))
    _deckb_up = parse_item(parent, value);

  if(succeeded(application_t::instance->hal()->get_config_value(name.c_str(), "deckb-dn", value)))
    _deckb_dn = parse_item(parent, value);
  }

kotuku::root_menu_t::~root_menu_t()
  {
  }

size_t kotuku::root_menu_t::size() const
  {
  return 9;
  }

const kotuku::menu_item_t *kotuku::root_menu_t::at(int index) const
  {
  switch(index)
    {
  case 0 :
    return _key0;
  case 1 :
    return _key1;
  case 2 :
    return _key2;
  case 3 :
    return _key3;
  case 4 :
    return _key4;
  case 5 :
    return _decka_up;
  case 6 :
    return _decka_dn;
  case 7 :
    return _deckb_up;
  case 8 :
    return _deckb_dn;
    }

  return 0;
  }

kotuku::e_menu_type kotuku::root_menu_t::menu_type() const
  {
  return mt_root;
  }

///////////////////////////////////////////////////////////////////////////////
//
// Popup menu
kotuku::popup_menu_t::popup_menu_t(menu_window_t *parent, const std::string &name)
: menu_t(parent)
  {
  char buffer[64];
  std::string value;
  for(int i = 0; i < 128; i++)
    {
    sprintf(buffer, "item%d", i);
    if(succeeded(application_t::instance->hal()->get_config_value(name.c_str(), buffer, value)))
      {
      menu_item_t *item =  parse_item(parent, value);
      if(item != 0)
        _menu_items.push_back(item);
      }
    else
      break;
    }
  }

kotuku::popup_menu_t::~popup_menu_t()
  {
  }

size_t kotuku::popup_menu_t::size() const
  {
  return _menu_items.size();
  }

const kotuku::menu_item_t *kotuku::popup_menu_t::at(int index) const
  {
  return _menu_items[index];
  }

kotuku::e_menu_type kotuku::popup_menu_t::menu_type() const
  {
  return mt_popup;
  }

void kotuku::popup_menu_t::draw_popup(size_t selected_item, menu_window_t *parent, const point_t &popup_item_origin) const
  {

  }

///////////////////////////////////////////////////////////////////////////////
//
// Menu item
kotuku::menu_item_t::menu_item_t(menu_window_t *parent)
  : _parent(parent),
    _bitmap(0)
  {
  }

kotuku::menu_item_t::~menu_item_t()
  {
  }


void kotuku::menu_item_t::assign_enabler(const char *expr)
  {
  if(expr == 0 || strlen(expr)== 0)
    return;

  // the expression is in the form id:regex:format
  // where id is an integer, regex is a string and format is an
  // optional value format string.
  attributes_t attributes;
  split_tuple(expr, attributes);

  if(attributes.size()<2)
    {
    trace_error("menu_item_t(enabler) does not contain at least 2 attributes\n");
    return;
    }

  if(attributes.size()>3)
    {
    trace_error("menu_item_t(enabler) contains more than 3 attributes\n");
    return;
    }

  _controlling_param = atoi(attributes[0].c_str());
  if(_controlling_param == 0 || _controlling_param > 8192)
    {
    trace_error("menu_item_t(enabler) can_id must be 1..8192\n");
    return;
    }

  _enable_regex = attributes[1];
  if(attributes.size()>2)
    _enable_format = attributes[2];

  // listen to the message
  _parent->subscribe(_controlling_param, this);

  const char *error;
  // compile the pattern
  _pat_buff = trex_compile(_enable_regex.c_str(), &error);
  }

bool kotuku::menu_item_t::is_enabled() const
  {
  if(_controlling_param == 0 ||
      _enable_regex.empty())
    return true;

  // get a string that is the format of the variable
  std::string value = _controlling_variable.to_string(_enable_format.c_str());

  // we now determine a match against the controlling regular expression
  return trex_match(_pat_buff, value.c_str())!= 0;
  }

void kotuku::menu_item_t::receive_parameter(canaerospace_provider_t *prov, const msg_t &msg)
  {
  if(msg.message_id() == _controlling_param)
    _controlling_variable = msg;
  }

bool kotuku::menu_item_t::is_equal(const can_parameter_handler_t &other) const
  {
  return false;
  }

void kotuku::menu_item_t::caption(const std::string &str)
  {
  _caption = str;

  // TODO: load resources
  }

void kotuku::menu_item_t::paint(const rect_t &area, bool is_selected) const
  {
  point_t center_pt((area.width() >> 1) + area.top_left().x, (area.height() >> 1) + area.top_left().y);

  if(_bitmap != 0)
    {
    /*
    center_pt.x -= _bitmap->bitmap_width >> 1;
    center_pt.y -= _bitmap->bitmap_height >> 1;

    parent()->pattern_blt(rect_t(center_pt, extent_t(_bitmap->bitmap_width, _bitmap->bitmap_height)), *_bitmap);
    */
    }
  else
    {
    parent()->pen(&light_gray_pen_3);
    parent()->background_color(is_selected ? color_white : color_black);
    parent()->font(&arial_9_font);

    parent()->rectangle(area);
    // calculate the text extents
    extent_t ex = parent()->text_extent(_caption.c_str(), _caption.length());

    center_pt.x -= ex.cx >> 1;
    center_pt.y -= ex.cy >> 1;

    if(is_selected)
      parent()->text_color(color_magenta);
    else
      parent()->text_color(color_green);

    parent()->draw_text(_caption.c_str(), _caption.length(), center_pt);
    }

  }

const bitmap_t *load_resource(const std::string &name)
  {
  const bitmap_t *result = 0;
  // determine the bitmap name and builtin types

  return result;
  }

///////////////////////////////////////////////////////////////////////////////
//
//    Menu item
//
kotuku::menu_item_menu_t::menu_item_menu_t(menu_window_t *parent, const char *option, const char *expr)
: menu_item_t(parent)
  {
  // if the parser determines we have an enable expression in the form menu(<expr>): then
  // pass it to the base class.
  if(expr != 0)
    assign_enabler(expr);

  // the string is composed of a tuple of 2 attributes.  The first is the
  //
  attributes_t attributes;
  split_tuple(option, attributes, ',');

  if(attributes.size() != 2)
    {
    trace_error("menu_item_t(%s) does not have <text>:<value>\n", option);
    return;
    }

  // the option is split into the following
  _menu = parent->load_menu(attributes[1]);

  if(attributes[0][0] == '"')
    caption(attributes[0].substr(1, attributes[0][attributes[0].size()-1]== '"' ? attributes[0].size()-2 : attributes[0].size()-1));
  else
    caption(attributes[0]);
  }

kotuku::menu_item_menu_t::~menu_item_menu_t()
  {

  }

kotuku::menu_item_type kotuku::menu_item_menu_t::item_type() const
  {
  return mi_menu;
  }

kotuku::menu_item_action_result kotuku::menu_item_menu_t::evaluate_action(const msg_t &msg) const
  {
  parent()->show_menu(_menu);

  return mia_nothing;
  }

///////////////////////////////////////////////////////////////////////////////
//
//  Popup Menu
//
kotuku::menu_item_popup_t::menu_item_popup_t(menu_window_t *parent, const char *option, const char *expr)
: menu_item_t(parent)
  {
  // if the parser determines we have an enable expression in the form menu(<expr>): then
  // pass it to the base class.
  if(expr != 0)
    assign_enabler(expr);

  // the string is composed of a tuple of 3 attributes.  The first is the
  //
  attributes_t attributes;
  split_tuple(option, attributes, ',');

  if(attributes.size() != 3)
    {
    trace_error("popup_item_t(%s) does not have <text>:<value>:<value>\n", option);
    return;
    }

  if(attributes[0][0] == '"')
    caption(attributes[0].substr(1, attributes[0][attributes[0].size()-1]== '"' ? attributes[0].size()-2 : attributes[0].size()-1));
  else
    caption(attributes[0]);

  _menu = parent->load_menu(attributes[2]);
  _popup = parent->load_popup(attributes[1]);
  }

kotuku::menu_item_popup_t::~menu_item_popup_t()
  {

  }

kotuku::menu_item_type kotuku::menu_item_popup_t::item_type() const
  {
  return mi_popup;
  }

kotuku::menu_item_action_result kotuku::menu_item_popup_t::evaluate_action(const msg_t &msg) const
  {
  parent()->show_popup_menu(_popup, 0);

  parent()->show_menu(_menu);
  return mia_nothing;
  }

///////////////////////////////////////////////////////////////////////////////
//
// Cancel item
kotuku::menu_item_cancel_t::menu_item_cancel_t(menu_window_t *parent, const char *options, const char *expr)
: menu_item_t(parent)
  {
  // if the parser determines we have an enable expression in the form menu(<expr>): then
  // pass it to the base class.
  if(expr != 0)
    assign_enabler(expr);

  if(options == 0)
    options = "id_cancel";          // resource id.

  caption(options);
  }

kotuku::menu_item_cancel_t::~menu_item_cancel_t()
  {
  }
   
kotuku::menu_item_type kotuku::menu_item_cancel_t::item_type() const
  {
  return mi_cancel;
  }

kotuku::menu_item_action_result kotuku::menu_item_cancel_t::evaluate_action(const msg_t &) const
  {
  msg_t cancel_msg(id_menu_cancel);
  cancel_msg.flags |= LOOPBACK_MESSAGE;

  reinterpret_cast<application_t *>(application_t::instance)->publish(cancel_msg);

  return mia_cancel;
  }

///////////////////////////////////////////////////////////////////////////////
//
// Enter Item
kotuku::menu_item_enter_t::menu_item_enter_t(menu_window_t *parent, const char *options, const char *expr)
: menu_item_t(parent)
  {
  // if the parser determines we have an enable expression in the form menu(<expr>): then
  // pass it to the base class.
  if(expr != 0)
    assign_enabler(expr);

  if(options == 0)
    options = "id_ok";          // resource id.

  caption(options);
  }

kotuku::menu_item_enter_t::~menu_item_enter_t()
  {
  }

kotuku::menu_item_type kotuku::menu_item_enter_t::item_type() const
  {
  return mi_enter;
  }

kotuku::menu_item_action_result kotuku::menu_item_enter_t::evaluate_action(const msg_t &) const
  {
  msg_t enter_msg(id_menu_ok);
  enter_msg.flags |= LOOPBACK_MESSAGE;

  reinterpret_cast<application_t *>(application_t::instance)->publish(enter_msg);
  return mia_nothing;
  }

///////////////////////////////////////////////////////////////////////////////
//
// event Item
//
// event:<text/resourceid>,<id>,<datatype>,[param],[<options>] sends a canaerospace message

kotuku::menu_item_event_t::menu_item_event_t(menu_window_t *parent, const char *options, const char *expr)
: menu_item_t(parent)
  {
  assign_enabler(expr);

  // the string is composed of a tuple of 3 attributes.  The first is the
  //
  attributes_t attributes;
  split_tuple(options, attributes, ',');

  if(attributes.size() < 2)
    {
    trace_error("event_item_t(%s) does not have <text>,{<msg>}[,{<msg>}]\n", options);
    return;
    }
  if(attributes[0][0] == '"')
    caption(attributes[0].substr(1, attributes[0][attributes[0].size()-1]== '"' ? attributes[0].size()-2 : attributes[0].size()-1));
  else
    caption(attributes[0]);

  // we now work over the can messages
  for(size_t i = 1; i < attributes.size(); i++)
    {
    msg_t msg = parse_msg(attributes[i]);
    _events.push_back(msg);
    if(msg.id >= id_key0 && msg.id <= id_menu_select)
      _events.back().flags |= LOOPBACK_MESSAGE;
    }
  }

kotuku::menu_item_event_t::~menu_item_event_t()
  {
  }

kotuku::menu_item_type kotuku::menu_item_event_t::item_type() const
  {
  return mi_event;
  }

kotuku::menu_item_action_result kotuku::menu_item_event_t::evaluate_action(const msg_t &msg) const
  {
  for(size_t i = 0; i < _events.size(); i++)
    reinterpret_cast<application_t *>(application_t::instance)->publish(_events[i]);

  return mia_nothing;
  }

///////////////////////////////////////////////////////////////////////////////
//
// Numeric editor
namespace kotuku {
class numeric_editor_t : public msg_editor_t
  {
  public:
    numeric_editor_t(const attributes_t &);
    virtual menu_item_action_result evaluate_action(const msg_t &) const;
    virtual void receive_parameter(canaerospace_provider_t *, const msg_t &);
    virtual bool is_equal(const can_parameter_handler_t &) const;
  private:
    msg_t _value;
    msg_t _set_value;
    float _min_value;
    float _max_value;
    float _decka_increment;
    float _deckb_increment;
  };
};

///////////////////////////////////////////////////////////////////////////////
//
// edit Item
kotuku::menu_item_edit_t::menu_item_edit_t(menu_window_t *parent, const char *options, const char *expr)
: menu_item_t(parent),
  _menu(0),
  _editor(0)
  {
  // if the parser determines we have an enable expression in the form menu(<expr>): then
  // pass it to the base class.
  if(expr != 0)
    assign_enabler(expr);

  // the string is composed of a tuple of 3 attributes.  The first is the
  //
  attributes_t attributes;
  split_tuple(options, attributes, ',');

  if(attributes.size() < 3)
    {
    trace_error("An edit option should have at least 3 attributes (%s)\n", options);
    return;
    }

  if(attributes[0][0] == '"')
    caption(attributes[0].substr(1, attributes[0][attributes[0].size()-1]== '"' ? attributes[0].size()-2 : attributes[0].size()-1));
  else
    caption(attributes[0]);

  _menu = parent->load_menu(attributes[1]);

  if(strcasecmp("NUMERIC", attributes[2].c_str()))
    {

    }
  }

kotuku::menu_item_edit_t::~menu_item_edit_t()
  {
  }

kotuku::menu_item_type kotuku::menu_item_edit_t::item_type() const
  {
  return mi_event;
  }

kotuku::menu_item_action_result kotuku::menu_item_edit_t::evaluate_action(const msg_t &) const
  {
  return mia_nothing;
  }

///////////////////////////////////////////////////////////////////////////////
//
// edit Item
kotuku::menu_item_checklist_t::menu_item_checklist_t(menu_window_t *parent, const char *options, const char *expr)
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

kotuku::menu_item_checklist_t::~menu_item_checklist_t()
  {
  }

kotuku::menu_item_type kotuku::menu_item_checklist_t::item_type() const
  {
  return mi_checklist;
  }

kotuku::menu_item_action_result kotuku::menu_item_checklist_t::evaluate_action(const msg_t &) const
  {
  return mia_nothing;
  }
