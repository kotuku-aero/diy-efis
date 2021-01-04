using System;
using System.Collections;

namespace CanFly.Proton
{
  // helper classes
  public class KeysMap : Hashtable
  {
    public KeysMap()
    {

    }

    public bool Contains(string value)
    {
      return base.Contains(value);
    }

    public Keys this[string index]
    {
      get { return base[index] as Keys; }
      set { base[index] = value; }
    }
  }

  public class MenuMap : Hashtable
  {
    public MenuMap()
    {

    }

    public bool Contains(string value)
    {
      return base.Contains(value);
    }

    public Menu this[string index]
    {
      get { return base[index] as Menu; }
      set { base[index] = value; }
    }
  }

  public class LayoutWidget : Widget
  {
    //
    // This holds the global root menu for the system
    //
    private Keys _rootKeys;
    private System.Threading.AutoResetEvent _shutdownEvent;

    public void Run()
    {
      // wait till a menu item requests the loop to exit and the device will reset.
      _shutdownEvent.WaitOne();
    }

    //
    // this holds the active key handler for the current popup menu.  If there is
    // no popup active the root handler is used.
    //
    private Keys _activeKeys;

    //
    // This holds the current popup menu that can be a property editor or
    // action items
    //
    private Menu _currentMenu;

    private int _menuTimer; // as a press/rotate is given this sets the tick-timeout

    private int _menuRectX;
    private int _menuRectY;
    private int _menuStartX;
    private int _menuStartY;

    private uint _key; // window key

    // map of root menus
    private KeysMap _keyMappings;

    // map of menus
    private MenuMap _menus;

    // stack of menu's
    private ArrayList _menuStack;

    // we hold a reference to all loaded menu items as they need to be informed
    // if a controlling variable changed
    private ArrayList _menuItems;

    private uint _backgroundColor;
    private uint _selectedBackgroundColor;
    private uint _textColor; // text color
    private uint _borderColor;
    private uint _selectedColor;
    private Pen _borderPen;
    private Font _font;
    private ushort _orentation;

    private enum WindowType
    {
      AirspeedWindow, // vertical ticker-tape airspeed window
      HsiWindow, // rectangular gyro position and heading
      AttitudeWindow, // rectangular attitude window
      AltitudeWindow, // vertical ticker tape altitude and vertical speed indicator window
      GaugeWindow, // general purpose round or bar gauge
      AnnunciatorWindow, // time, oat etc.
      GpsWindow, // gps route map window
      AlertWindow, // vertical alert window
      Widget, // a generic widget
      LastLayoutEnum
    };

 
    public Pen BorderPen
    {
      get { return _borderPen; }
      set { _borderPen = value; }
    }

    /// <summary>
    /// This is called by the runtime.  It cannot be called directly
    /// </summary>
    /// <param name="parent"></param>
    public LayoutWidget(Widget parent, ushort orientation, ushort id , uint key)
      : base(parent, parent.WindowRect, id)
    {
      _key = key;
      _shutdownEvent = new System.Threading.AutoResetEvent(false);
      
      // load the layout....
    }

    public uint BorderColor
    {
      get { return _borderColor; }
      set { _borderColor = value; }
    }

    public uint BackgroundColor
    {
      get { return _backgroundColor; }
      set { _backgroundColor = value; }
    }

    public uint SelectedBackgroundColor
    {
      get { return _selectedBackgroundColor; }
      set { _selectedBackgroundColor = value; }
    }

    public uint TextColor
    {
      get { return _textColor; }
      set { _textColor = value; }
    }

    public uint SelectedColor
    {
      get { return _selectedColor; }
      set { _selectedColor = value; }
    }

    public Font Font
    {
      get { return _font; }
      set { _font = value; }
    }

    /// <summary>
    /// Assign the expression that is used to test if this menu item is enabled
    /// </summary>
    /// <param name="item">Item to assign to</param>
    /// <param name="expr">xpression in the form <param>:<expression></param>
    private void item_assign_enabler(MenuItem item, string expr)
    {
      
    }

    private void DrawMenuItem(MenuItem menu, Point menuOrigin)
    {

    }

    /// <summary>
    /// Close the current popup menu and restore the previous menu
    /// </summary>
    private void PopMenu()
    {
      if(_menuStack.Count == 0)
      {
        CloseMenu();      // all done
        return;
      }

      Menu prevMenu = (Menu) _menuStack[_menuStack.Count - 1];
      _menuStack.RemoveAt(_menuStack.Count - 1);
      ShowMenu(prevMenu);
    }

//
// Close all menu's.  Called by the cancel: menu item
//
    private void CloseMenu()
    {
      if (_currentMenu != null)
        _currentMenu.SelectedIndex = 0;

      _activeKeys = null;
      _currentMenu = null;
      _menuTimer = 0;

      _activeKeys = _rootKeys;

    }

//
    // Show a popup menu at the left of the menu window.
    // @param Popup           menu to display
    // @param option_selected Option that is selected. Will be the bottom option
//
    private void ShowMenu(Menu menu)
    {
      // assign the menu
      _currentMenu = menu;
      menu.SelectedIndex = 0;         // always start at first item
      // and set the keys
      _activeKeys = menu.Keys;

    }

    /// <summary>
    /// Change the first display item to list item-1 if possible
    /// </summary>
    private void ScrollMenuUp()
    {

    }

    /// <summary>
    /// Change the first display item to list item +1 if possible
    /// </summary>
    private void ScrollMenuDown()
    {

    }

    /// <summary>
    /// Open the popup menu editor and show the edit menu
    /// </summary>
    /// <param name="item"></param>
    private void ShowItemEditor(MenuItem item)
    {
      if (item.ItemType ==  MenuItemType.MiEdit)
      {
        // select the popup menu
        item.EditorOpen = true;

        // TODO: edit the item....
      }

    }

    /// <summary>
    /// Load a menu from the registry and cache it
    /// </summary>
    /// <param name="name"></param>
    /// <param name="menu"></param>
    private void FindKeys(string name, uint menu)
    {

    }

    /// <summary>
    /// Find a popup menu id that matches the name given
    /// </summary>
    /// <param name="name">Name of the popup menu</param>
    /// <param name="menu"></param>
    private uint FindMenu(string name, Menu menu)
    {
      return 0;
    }

    private bool DefaultEnableHandler(MenuItem item, CanFlyMsg msg)
    {
      if (item.ControllingParam == 0 ||
          item.EnableRegex == null)
        return true;

      // we now determine a match against the controlling regular expression
      return item.Match(item.ControllingVariable.ToString(item.EnableFormat));
    }

    private class DatatypeLookup
    {
      private string _name;
      private CanFlyDataType _datatype;
      private ushort _length;

      public DatatypeLookup(string name, CanFlyDataType dataType, ushort length)
      {
        _name = name;
        _datatype = dataType;
        _length = length;
      }

      public string Name
      {
        get { return _name; }
      }

      public CanFlyDataType DataType
      {
        get { return _datatype; }
      }

      public ushort Length
      {
        get { return _length; }
      }
    };

    private static DatatypeLookup[] _datatypes =
    {
      new DatatypeLookup("SHORT", CanFlyDataType.Short, 2),
      new DatatypeLookup("FLOAT", CanFlyDataType.Float, 4),
    };

    private CanFlyMsg LoadCanMessage(uint key)
    {
      CanFlyMsg result = null;
      ushort id;
      if (TryRegGetUint16(key, "can-id", out id))
      {
        string type;
        if (TryRegGetString(key, "can-type", out type))
        {
          string value;
          string[] values;
          // decode the message
          if (type == "NODATA" ||
              !TryRegGetString(key, "can-value", out value))
          {
            result = new CanFlyMsg(id, CanFlyDataType.NoData, 0);
          }
          else
          {
            switch (type)
            {
              case "ERROR":
                result = new CanFlyMsg(id, CanFlyDataType.Error, Convert.ToUInt32(value));
                break;
              case "FLOAT":
                result = new CanFlyMsg(id, (float) Convert.ToDouble(value));
                break;
              case "LONG":
                result = new CanFlyMsg(id, Convert.ToInt32(value));
                break;
              case "ULONG":
                result = new CanFlyMsg(id, Convert.ToUInt32(value));
                break;
              case "SHORT":
                result = new CanFlyMsg(id, Convert.ToInt16(value));
                break;
              case "USHORT":
                result = new CanFlyMsg(id, Convert.ToUInt16(value));
                break;
              case "CHAR":
                result = new CanFlyMsg(id, value[0]);
                break;
              case "UCHAR":
                result = new CanFlyMsg(id, (byte) DecodeHex(value));
                break;
              case "CHAR2":
                values = value.Split(',');
                result = new CanFlyMsg(id,
                  (char) (values.Length > 0 ? DecodeHex(values[0]) : 0),
                  (char) (values.Length > 1 ? DecodeHex(values[1]) : 0));
                break;
              case "UCHAR2":
                values = value.Split(',');
                result = new CanFlyMsg(id,
                  (byte) (values.Length > 0 ? DecodeHex(values[0]) : 0),
                  (byte) (values.Length > 1 ? DecodeHex(values[1]) : 0));
                break;
              case "CHAR3":
                values = value.Split(',');
                result = new CanFlyMsg(id,
                  (char) (values.Length > 0 ? DecodeHex(values[0]) : 0),
                  (char) (values.Length > 1 ? DecodeHex(values[1]) : 0),
                  (char) (values.Length > 2 ? DecodeHex(values[2]) : 0));
                break;
              case "UCHAR3":
                values = value.Split(',');
                result = new CanFlyMsg(id,
                  (byte) (values.Length > 0 ? DecodeHex(values[0]) : 0),
                  (byte) (values.Length > 1 ? DecodeHex(values[1]) : 0),
                  (byte) (values.Length > 2 ? DecodeHex(values[2]) : 0));
                break;
              case "CHAR4":
                values = value.Split(',');
                result = new CanFlyMsg(id,
                  (char) (values.Length > 0 ? DecodeHex(values[0]) : 0),
                  (char) (values.Length > 1 ? DecodeHex(values[1]) : 0),
                  (char) (values.Length > 2 ? DecodeHex(values[2]) : 0),
                  (char) (values.Length > 3 ? DecodeHex(values[3]) : 0));
                break;
              case "UCHAR4":
                values = value.Split(',');
                result = new CanFlyMsg(id,
                  (byte) (values.Length > 0 ? DecodeHex(values[0]) : 0),
                  (byte) (values.Length > 1 ? DecodeHex(values[1]) : 0),
                  (byte) (values.Length > 2 ? DecodeHex(values[2]) : 0),
                  (byte) (values.Length > 3 ? DecodeHex(values[3]) : 0));
                break;
              case "SHORT2":
                values = value.Split(',');
                result = new CanFlyMsg(id,
                  (short) (values.Length > 0 ? DecodeHex(values[0]) : 0),
                  (short) (values.Length > 1 ? DecodeHex(values[1]) : 0));
                break;
              case "USHORT2":
                values = value.Split(',');
                result = new CanFlyMsg(id,
                  (ushort) (values.Length > 0 ? DecodeHex(values[0]) : 0),
                  (ushort) (values.Length > 1 ? DecodeHex(values[1]) : 0));
                break;
              default:
                result = new CanFlyMsg(id, CanFlyDataType.NoData, 0);
                break;
            }
          }
        }
      }

      return result;
    }

    private byte DecodeHex(string value)
    {
      return 0;
    }

    private void UpdateWindow()
    {
      Rect wndRect = WindowRect;

      Extent ex = wndRect.Extent;

      Rect rect;
      if (_currentMenu != null)
      {
        // draw the root menu
        Point menuPt = new Point(_menuStartX, _menuStartY);
        int itemWidth = wndRect.Width / 3;
        int itemHeight = 20;

        Extent itemExtents = new Extent(itemWidth, itemHeight);

        // see if we are displaying a popup menu
        menuPt.X = _menuStartX;
        menuPt.Y -= itemHeight;

        // determine how menu items to draw from the item
        int itemsAvail = _menuStartY / itemHeight;

        int numItems = _currentMenu.MenuItems.Count;
        int drawingItem = 0;
        int index = 0;

        if (numItems > itemsAvail)
          index = _currentMenu.SelectedIndex;  // start drawing at the selected index

        // draw the visible items
        for (; index < numItems && drawingItem < itemsAvail; drawingItem++, index++)
        {
          MenuItem item = (MenuItem) _currentMenu.MenuItems[index];
          item.Paint(new Rect(menuPt.X, menuPt.Y, menuPt.X + itemExtents.Dx, menuPt.Y + itemExtents.Dy),
            index == _currentMenu.SelectedIndex);

          // now ask the item to draw if it is selected
          if (item.Selected)
            DrawMenuItem(item, menuPt);

          // skip up one.
          menuPt.Y -= itemHeight;
        }
      }
    }

    protected override void OnPaint()
    {
      throw new NotImplementedException();
    }
  }

}
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
then the original copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.

#include "../photon/widget.h"
#include "regex.h"
#include "pens.h"

#include "../photon/window.h"

extern result_t create_airspeed_window(handle_t parent, uint section, handle_t *hwnd);
extern result_t create_altitude_window(handle_t parent, uint section, handle_t *hwnd);
extern result_t create_annunciator_window(handle_t parent, uint section, handle_t *hwnd);
extern result_t create_attitude_window(handle_t parent, uint section, handle_t *hwnd);
extern result_t create_gauge_window(handle_t parent, uint section, handle_t *hwnd);
extern result_t create_gps_window(handle_t parent, uint section, handle_t *hwnd);
extern result_t create_hsi_window(handle_t parent, uint section, handle_t *hwnd);
extern result_t create_widget(handle_t parent, uint section, handle_t *hwnd);



static const short Menuimeout = 25;             // 5-second timeout


//
* Called after a popup menu closes or a back option is selected
//
static void PopMenu(LayoutWindow wnd)
  {
  }

////////////////////////////////////////////////////////////////////////////////
//
// Generic handlers
//



static void default_msg_handler(LayoutWidget wnd, MenuItem item, CanFlyMsg msg)
  {
  if (item.controlling_param == get_can_id(msg))
    memcpy(&item.controlling_variable, msg, sizeof(CanFlyMsg));
  }

static void load_item_defaults(LayoutWindow wnd, MenuItem item, uint key)
  {
  item.event = default_msg_handler;
  item.paint = default_paint_handler;
  item.is_enabled = DefaultEnableHandler;

  reg_get_string(key, "caption", item.caption, 0);
  uint enable_key;
  if (succeeded(reg_open_key(key, "enable", &enable_key)))
    {
    reg_get_uint16(enable_key, "id", &item.controlling_param);
    reg_get_string(enable_key, "regex", item.enable_regex, 0);
    reg_get_string(enable_key, "format", item.enable_format, 0);

    if (item.enable_regex[0] != 0)
      {
      string error;
      // compile the pattern
      item.pat_buff = trex_compile(item.enable_regex, &error);
      }
    }
  }

static MenuItem MenuItemAt(Menu menu, ushort index)
  {
  MenuItem mi;
  if (failed(vector_at(menu.menu_items, index, &mi)))
    return 0;

  return mi;
  }

static ushort menu_count(Menu menu)
  {
  ushort count = 0;
  vector_count(menu.menu_items, &count);

  return count;
  }

////////////////////////////////////////////////////////////////////////////////
//
// Item handlers for a POPUP item
//
typedef struct _menu_item_Menu {
  menu_item_t item;

  // if this is a checklist popup then this holds the ushort value
  // that is matched to create the checklist option.
  ushort value;

  // if a checklist item then this has no value
  char caption;
  Menu menu;
  } menu_item_Menu;

static menu_item_action_result item_menu_evaluate(LayoutWindow wnd,
  MenuItem item,
  CanFlyMsg msg)
  {
  menu_item_Menu menu = (menu_item_Menu )item;

  ShowMenu(wnd, menu.menu);

  return mia_nothing;
  }

static MenuItem item_menu_load(LayoutWindow wnd,
  uint key,
  bool is_checklist)
  {
  menu_item_Menu item = (menu_item_Menu )neutron_malloc(sizeof(menu_item_Menu));
  memset(item, 0, sizeof(menu_item_Menu));

  load_item_defaults(wnd, &item.item, key);

  item.item.evaluate = item_menu_evaluate;
  item.item.item_type = mi_menu;

  load_item_defaults(wnd, &item.item, key);

  reg_get_string(key, "caption", item.caption, 0);
  char popup_name;
  reg_get_string(key, "menu", popup_name, 0);
  // load the popup menu
  FindMenu(wnd, popup_name, &item.menu);

  if (is_checklist)
    reg_get_uint16(key, "value", &item.value);

  // save the new item
  vector_push_back(menu_items, &item);
  return &item.item;
  }

////////////////////////////////////////////////////////////////////////////////
//
// Item handlers for a CANCEL item
//

typedef struct _menu_item_cancel_t {
  menu_item_t item;
  ushort value;     // cancel value
  } menu_item_cancel_t;

static const CanFlyMsg cancel_msg =
{
  .flags = id_menu_cancel
};

static menu_item_action_result item_cancel_evaluate(LayoutWindow wnd, MenuItem item, CanFlyMsg msg)
  {
  // send the menu cancel message to the window
  send_message(window, &cancel_msg);

  return mia_cancel;
  }

static MenuItem item_cancel_load(LayoutWindow wnd, uint key)
  {
  menu_item_cancel_t *item = (menu_item_cancel_t *)neutron_malloc(sizeof(menu_item_cancel_t));
  memset(item, 0, sizeof(menu_item_cancel_t));
  item.item.item_type = mi_cancel;

  load_item_defaults(wnd, &item.item, key);
  item.item.evaluate = item_cancel_evaluate;

  if (failed(reg_get_uint16(key, "value", &item.value)))
    item.value = id_menu_cancel;

  // save the new item
  vector_push_back(menu_items, &item);
  return &item.item;
  }

////////////////////////////////////////////////////////////////////////////////
//
// Item handlers for a ENTER item
//

typedef struct _menu_item_enter_t {
  menu_item_t item;
  ushort value;     // enter value
  } menu_item_enter_t;

static menu_item_action_result item_enter_evaluate(LayoutWindow wnd,
  MenuItem item,
  CanFlyMsg msg)
  {
  // get the current menu and select the item
  MenuItem mi;
  if (succeeded(vector_at(_currentMenu.menu_items, _currentMenu.SelectedIndex, &mi)) &&
    (*mi.is_enabled)(wnd, mi, msg))
    (*mi.evaluate)(wnd, mi, msg);

  return mia_enter;
  }

static MenuItem item_enter_load(LayoutWindow wnd,
  uint key)
  {
  menu_item_enter_t *item = (menu_item_enter_t *)neutron_malloc(sizeof(menu_item_enter_t));
  memset(item, 0, sizeof(menu_item_enter_t));

  item.item.item_type = mi_enter;

  load_item_defaults(wnd, &item.item, key);
  item.item.evaluate = item_enter_evaluate;

  if (failed(reg_get_uint16(key, "value", &item.value)))
    item.value = id_menu_ok;

  // save the new item
  vector_push_back(menu_items, &item);
  return &item.item;
  }

////////////////////////////////////////////////////////////////////////////////
//
// Item handlers for a EVENT item
//
typedef struct _menu_item_event_t {
  menu_item_t item;
  CanFlyMsg msg;
  } menu_item_event_t;

static menu_item_action_result item_event_evaluate(LayoutWindow wnd, MenuItem item, CanFlyMsg msg)
  {
  menu_item_event_t *mi = (menu_item_event_t *)item;

  // see if the event is an internal menu one
  if (get_can_id(msg) >= id_first_internal_msg && get_can_id(msg) <= id_last_internal_msg)
    send_message(window, &mi.msg);
  else
    can_send(&mi.msg);

  return mia_nothing;
  }

static MenuItem item_event_load(LayoutWindow wnd,
  uint key)
  {
  menu_item_event_t *item = (menu_item_event_t *)neutron_malloc(sizeof(menu_item_event_t));
  memset(item, 0, sizeof(menu_item_event_t));

  item.item.item_type = mi_event;

  load_item_defaults(wnd, &item.item, key);
  item.item.evaluate = item_event_evaluate;

  // read the event message from the registry
  LoadCanMessage(key, &item.msg);

  // save the new item
  vector_push_back(menu_items, &item);
  return &item.item;
  }

////////////////////////////////////////////////////////////////////////////////
//
// Item handlers for a EDIT item
//
typedef struct _menu_item_edit_t {
  menu_item_t item;

  ushort value_id;
  CanFlyMsg value;

  ushort set_id;
  bool integer_type;

  ushort precision;       // number of digits after decimal point to edit
  ushort digits;          // number of digits to the left of the decimal point

  float min_value;
  float max_value;
  float decka_increment;
  float deckb_increment;
  } menu_item_edit_t;

static void item_edit_paint(Widget widget,
  LayoutWindow wnd,
  Rect wnd_rect,
  MenuItem item,
  Rect rect,
  bool is_selected)
  {

  }

static menu_item_action_result item_edit_evaluate(LayoutWindow wnd,
  MenuItem item,
  CanFlyMsg msg)
  {
  return mia_nothing;
  }

static void item_edit_edit(LayoutWindow wnd,
  MenuItem item,
  CanFlyMsg msg)
  {

  }

static void item_edit_event(LayoutWindow wnd,
  MenuItem item,
  CanFlyMsg msg)
  {
  // check the enabler
  DefaultEnableHandler(wnd, item, msg);

  menu_item_edit_t *edit = (menu_item_edit_t *)item;
  // see if we update our value
  if (edit.value_id == get_can_id(msg))
    memcpy(&edit.value, msg, sizeof(CanFlyMsg));
  }


static MenuItem item_edit_load(LayoutWindow wnd,
  uint key)
  {
  menu_item_edit_t *item = (menu_item_edit_t *)neutron_malloc(sizeof(menu_item_edit_t));
  memset(item, 0, sizeof(menu_item_edit_t));

  item.item.item_type = MenuItemType.MiEdit;

  load_item_defaults(wnd, &item.item, key);
  item.item.paint = item_edit_paint;
  item.item.evaluate = item_edit_evaluate;
  item.item.edit = item_edit_edit;
  item.item.event = item_edit_event;

  reg_get_uint16(key, "value-id", &item.value_id);

  // load the set-value message
  reg_get_uint16(key, "set-id", &item.set_id);

  // get the float point flag
  reg_get_bool(key, "send-short", &item.integer_type);

  // get the values
  reg_get_float(key, "min-value", &item.min_value);
  reg_get_float(key, "max-value", &item.max_value);
  reg_get_float(key, "decka-incr", &item.decka_increment);
  reg_get_float(key, "deckb-incr", &item.deckb_increment);

  reg_get_uint16(key, "precision", &item.precision);
  reg_get_uint16(key, "digits", &item.digits);

  // save the new item
  vector_push_back(menu_items, &item);
  return &item.item;
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
//

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

  ushort value_id;
  CanFlyMsg value;

  // item that is selected in the list.
  short selected_item;

  // this holds the checklist items
  menu_item_Menu popup;
  } menu_item_checklist_t;

static menu_item_action_result item_checklist_evaluate(LayoutWindow wnd,
  MenuItem item,
  CanFlyMsg msg)
  {
  return mia_nothing;
  }

static void item_checklist_event(LayoutWindow wnd, MenuItem item, CanFlyMsg msg)
  {
  // check the enabler
  DefaultEnableHandler(wnd, item, msg);

  menu_item_checklist_t *checklist = (menu_item_checklist_t *)item;
  // see if we update our value
  if (checklist.value_id == get_can_id(msg))
    {
    memcpy(&checklist.value, msg, sizeof(CanFlyMsg));

    ushort value;
    get_param_uint16(msg, 0, &value);

    ushort count;
    vector_count(checklist.popup.menu.menu_items, &count);

    ushort i;
    for (i = 0; i < count; i++)
      {
      MenuItem item;
      vector_at(checklist.popup.menu.menu_items, i, &item);
      if (i == value)
        checklist.popup.menu.SelectedIndex = i;
      }
    }
  }

static MenuItem item_checklist_load(LayoutWindow wnd,
  uint key)
  {
  menu_item_checklist_t *item = (menu_item_checklist_t *)neutron_malloc(sizeof(menu_item_checklist_t));
  memset(item, 0, sizeof(menu_item_checklist_t));

  item.item.item_type = mi_checklist;

  load_item_defaults(wnd, &item.item, key);
  item.item.evaluate = item_checklist_evaluate;
  item.item.event = item_checklist_event;

  reg_get_uint16(key, "value-id", &item.value_id);

  // load the popup menu next.
  item.popup = (menu_item_Menu )item_menu_load(wnd, key, true);

  // save the new item
  vector_push_back(menu_items, &item);
  return &item.item;
  }

//
* Load a menu item from the registry
* @param value     String to parse
* @returns The created menu item if it exists
//
static MenuItem parse_item(LayoutWindow parent, uint key)
  {
  char item_type[REG_STRING_MAX + 1];
  ushort len = REG_STRING_MAX + 1;

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

static result_t FindKeys(LayoutWindow wnd, string name, uint pmenu)
  {
  if (pmenu == 0 ||
    wnd == 0 ||
    name == 0)
    return e_bad_parameter;

  result_t result;
  keys_t menu;

  if (failed(map_find(key_mappings, name, (void **)&menu)))
    {
    // see if the key is found
    uint menu_key;
    if (failed(result = reg_open_key(key, name, &menu_key)))
      return result;

    menu = (keys_t )neutron_malloc(sizeof(keys_t));
    *pmenu = menu;

    memset(menu, 0, sizeof(keys_t));

    // must be done before we load any items so if they recurse we don't get
    // created more than once!
    if (failed(result = map_add(key_mappings, name, menu)))
      {
      neutron_free(menu);
      return result;
      }

    uint child;
    if (succeeded(reg_open_key(menu_key, "key0", &child)))
      menu.key0 = parse_item(wnd, child);

    if (succeeded(reg_open_key(menu_key, "key1", &child)))
      menu.key1 = parse_item(wnd, child);

    if (succeeded(reg_open_key(menu_key, "key2", &child)))
      menu.key2 = parse_item(wnd, child);

    if (succeeded(reg_open_key(menu_key, "key3", &child)))
      menu.key3 = parse_item(wnd, child);

    if (succeeded(reg_open_key(menu_key, "key4", &child)))
      menu.key4 = parse_item(wnd, child);

    if (succeeded(reg_open_key(menu_key, "decka-up", &child)))
      menu.decka_up = parse_item(wnd, child);

    if (succeeded(reg_open_key(menu_key, "decka-dn", &child)))
      menu.decka_dn = parse_item(wnd, child);

    if (succeeded(reg_open_key(menu_key, "deckb-up", &child)))
      menu.deckb_up = parse_item(wnd, child);

    if (succeeded(reg_open_key(menu_key, "deckb-dn", &child)))
      menu.deckb_dn = parse_item(wnd, child);
    }

  *pmenu = menu;

  return s_ok;
  }

static result_t FindMenu(LayoutWindow wnd, string name, Menu *menu)
  {
  if (wnd == 0 ||
    name == 0 ||
    menu == 0)
    return e_bad_parameter;

  result_t result;
  Menu popup;

  if (failed(map_find(menus, name, (void **)&popup)))
    {
    uint key;
    if (failed(result = reg_open_key(key, name, &key)))
      return result;

    // we have the memid, load it.
    popup = (Menu )neutron_malloc(sizeof(Menu));
    memset(popup, 0, sizeof(Menu));


    // must be done before we load any items so if they recurse we don't get
    // created more than once!
    if (failed(result = map_add(menus, name, popup)))
      {
      neutron_free(popup);
      return result;
      }

    popup.name = neutron_strdup(name);
    vector_create(sizeof(MenuItem ), &popup.menu_items);

    uint item_key = 0;
    field_datatype dt = field_key;

    while (succeeded(reg_enum_key(key, &dt, 0, 0, 0, 0, &item_key)))
      {
      // the protocol assumes all child keys are menu items
      MenuItem item = parse_item(wnd, item_key);
      if(item != 0)
        // add the menu item to the mix
        vector_push_back(popup.menu_items, &item);
      }

    // we now load the keys
    char keys;
    if (succeeded(reg_get_string(key, "keys", keys, 0)))
      FindKeys(wnd, keys, &popup.keys);
    }

  *menu = popup;

  return s_ok;
  }


result_t layout_wndproc(Widget widget, CanFlyMsg msg)
  {
  bool changed = false;
  LayoutWindow wnd;
  ushort count;
  ushort item;

  get_wnddata(hwnd, (void **)&wnd);

  // do the default processing (calls event handlers)
  defwndproc(hwnd, msg);

  if(wnd != 0 && menu_items != 0)
    {
    // we go through all of the menu items and check to see if they are
    // listening, and if their state has changed
    vector_count(menu_items, &count);
    for (item = 0; item < count; item++)
      {
      MenuItem mi;
      vector_at(menu_items, item, &mi);

      // update the controlling variable if it is being watched.
      if (mi != 0 && mi.event != 0)
        (*mi.event)(wnd, mi, msg);
      }
    }

  // finally update the menu if it is there so it draws on top
  UpdateWindow(hwnd, wnd);

  return s_ok;
  }

static void DrawMenuItem(Widget widget,
  LayoutWindow wnd,
  Rect wnd_rect,
  MenuItem selected_item,
  Point menu_origin)
  {
  Rect menu_rect;
  // depending on the type of item the rendering will be different
  // TODO: this is a hack....
  (*selected_item.paint)(hwnd, wnd, wnd_rect, selected_item,
    new Rect(menu_origin.X, menu_origin.Y, menu_origin.X + 80, menu_origin.Y + 20, &menu_rect), true);
  }

// these are the attached event handlers
static result_t on_key0(Widget widget, event_proxy_t *proxy, CanFlyMsg msg)
  {
  bool changed = false;
  LayoutWindow wnd = (LayoutWindow )proxy.parg;
  Rect wnd_rect;
  get_window_rect(hwnd, &wnd_rect);

  ushort id = get_can_id(msg);
  short value;
  get_param_int16(msg, 0, &value);
  if (_activeKeys != 0)
    {
    if (_activeKeys.key0 != 0 &&
      value > 0 &&
      (*_activeKeys.key0.is_enabled)(wnd, _activeKeys.key0, msg))
      {
      (*_activeKeys.key0.evaluate)(wnd, _activeKeys.key0, msg);
      Menuimer = Menuimeout;
      changed = true;
      }
    }
  else
    {
    _activeKeys = _rootKeys;
    Menuimer = 0;
    changed = true;
    }

  if(changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

static result_t on_key1(Widget widget, event_proxy_t *proxy, CanFlyMsg msg)
  {
  bool changed = false;
  LayoutWindow wnd = (LayoutWindow )proxy.parg;
  Rect wnd_rect;
  get_window_rect(hwnd, &wnd_rect);

  ushort id = get_can_id(msg);
  short value;
  get_param_int16(msg, 0, &value);

  if (_activeKeys != 0)
    {
    if (_activeKeys.key1 != 0 &&
      value > 0 &&
      (*_activeKeys.key1.is_enabled)(wnd, _activeKeys.key1, msg))
      {
      (*_activeKeys.key1.evaluate)(wnd, _activeKeys.key1, msg);

      Menuimer = Menuimeout;
      changed = true;
      }
    }
  else
    {
    _activeKeys = _rootKeys;
    Menuimer = 0;
    changed = true;
    }

  if (changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

static result_t on_key2(Widget widget, event_proxy_t *proxy, CanFlyMsg msg)
  {
  bool changed = false;
  LayoutWindow wnd = (LayoutWindow )proxy.parg;
  Rect wnd_rect;
  get_window_rect(hwnd, &wnd_rect);

  ushort id = get_can_id(msg);
  short value;
  get_param_int16(msg, 0, &value);

  if (_activeKeys != 0)
    {
    if (_activeKeys.key2 != 0 &&
      value > 0 &&
      (*_activeKeys.key2.is_enabled)(wnd, _activeKeys.key2, msg))
      {
      if (Menuimer != 0)
        (*_activeKeys.key2.evaluate)(wnd, _activeKeys.key2, msg);

      Menuimer = Menuimeout;
      changed = true;
      }
    }
  else
    {
    _activeKeys = _rootKeys;
    Menuimer = 0;
    changed = true;
    }

  if (changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

static result_t on_key3(Widget widget, event_proxy_t *proxy, CanFlyMsg msg)
  {
  bool changed = false;
  LayoutWindow wnd = (LayoutWindow )proxy.parg;
  Rect wnd_rect;
  get_window_rect(hwnd, &wnd_rect);

  ushort id = get_can_id(msg);
  short value;
  get_param_int16(msg, 0, &value);

  if (_activeKeys != 0)
    {
    if (_activeKeys.key3 != 0 &&
      value > 0 &&
      (*_activeKeys.key3.is_enabled)(wnd, _activeKeys.key3, msg))
      {
      if (Menuimer != 0)
        (*_activeKeys.key3.evaluate)(wnd, _activeKeys.key3, msg);

      Menuimer = Menuimeout;
      changed = true;
      }
    }
  else
    {
    _activeKeys = _rootKeys;
    Menuimer = 0;
    changed = true;
    }

  if (changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

static result_t on_key4(Widget widget, event_proxy_t *proxy, CanFlyMsg msg)
  {
  bool changed = false;
  LayoutWindow wnd = (LayoutWindow )proxy.parg;
  Rect wnd_rect;
  get_window_rect(hwnd, &wnd_rect);

  ushort id = get_can_id(msg);
  short value;
  get_param_int16(msg, 0, &value);
  if (_activeKeys != 0)
    {
    if (_activeKeys.key4 != 0 &&
      value > 0 &&
      (*_activeKeys.key4.is_enabled)(wnd, _activeKeys.key4, msg))
      {
      if (Menuimer != 0)
        (*_activeKeys.key4.evaluate)(wnd, _activeKeys.key4, msg);

      Menuimer = Menuimeout;
      changed = true;
      }
    }
  else
    {
    _activeKeys = _rootKeys;
    Menuimer = 0;
    changed = true;
    }

  if (changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

static result_t on_decka(Widget widget, event_proxy_t *proxy, CanFlyMsg msg)
  {
  bool changed = false;
  LayoutWindow wnd = (LayoutWindow )proxy.parg;
  Rect wnd_rect;
  get_window_rect(hwnd, &wnd_rect);

  ushort id = get_can_id(msg);
  short value;
  get_param_int16(msg, 0, &value);

  const keys_t handler = _activeKeys == 0 ? _rootKeys : _activeKeys;
  if (value > 0)
    {
    if (handler.decka_up != 0 && (*handler.decka_up.is_enabled)(wnd, handler.decka_up, msg))
      {
      if (Menuimer != 0)
        Menuimer = Menuimeout;

      (*handler.decka_up.evaluate)(wnd, handler.decka_up, msg);
      changed = true;
      }
    }
  else if (value < 0)
    {
    if (handler.decka_dn != 0 && (*handler.decka_dn.is_enabled)(wnd, handler.decka_dn, msg))
      {
      if (Menuimer != 0)
        Menuimer = Menuimeout;

      (*handler.decka_dn.evaluate)(wnd, handler.decka_dn, msg);
      changed = true;
      }
    }

  if (changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

static result_t on_deckb(Widget widget, event_proxy_t *proxy, CanFlyMsg msg)
  {
  bool changed = false;
  LayoutWindow wnd = (LayoutWindow )proxy.parg;
  Rect wnd_rect;
  get_window_rect(hwnd, &wnd_rect);

  ushort id = get_can_id(msg);
  short value;
  get_param_int16(msg, 0, &value);

  const keys_t handler = _activeKeys == 0 ? _rootKeys : _activeKeys;
  if (value > 0)
    {
    if (handler.deckb_up != 0 && (*handler.deckb_up.is_enabled)(wnd, handler.deckb_up, msg))
      {
      if (Menuimer != 0)
        Menuimer = Menuimeout;

      (*handler.deckb_up.evaluate)(wnd, handler.deckb_up, msg);
      changed = true;
      }
    }
  else if (value < 0)
    {
    if (handler.deckb_dn != 0 && (*handler.deckb_dn.is_enabled)(wnd, handler.deckb_dn, msg))
      {
      if (Menuimer != 0)
        Menuimer = Menuimeout;

      (*handler.deckb_dn.evaluate)(wnd, handler.deckb_dn, msg);
      changed = true;
      }
    }

  if (changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

static result_t on_menu_up(Widget widget, event_proxy_t *proxy, CanFlyMsg msg)
  {
  bool changed = false;
  LayoutWindow wnd = (LayoutWindow )proxy.parg;
  Rect wnd_rect;
  get_window_rect(hwnd, &wnd_rect);

  ushort id = get_can_id(msg);
  short value;
  get_param_int16(msg, 0, &value);

  // used to change the current popup menu index
  if (_currentMenu != 0)
    {
    ushort count = _currentMenu.MenuItems.Count;
    if (_currentMenu.SelectedIndex < count - 1)
      {
      _currentMenu.SelectedIndex++;
      changed = true;
      }
    }

  if (changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

static result_t on_menu_dn(Widget widget, event_proxy_t *proxy, CanFlyMsg msg)
  {
  bool changed = false;
  LayoutWindow wnd = (LayoutWindow )proxy.parg;
  Rect wnd_rect;
  get_window_rect(hwnd, &wnd_rect);

  ushort id = get_can_id(msg);
  short value;
  get_param_int16(msg, 0, &value);

  // used to change the current popup menu index
  if (_currentMenu != 0 && _currentMenu.SelectedIndex > 0)
    {
    _currentMenu.SelectedIndex--;
    changed = true;
    }

  if (changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

static result_t on_menu_left(Widget widget, event_proxy_t *proxy, CanFlyMsg msg)
  {
  bool changed = false;
  LayoutWindow wnd = (LayoutWindow )proxy.parg;
  Rect wnd_rect;
  get_window_rect(hwnd, &wnd_rect);

  ushort id = get_can_id(msg);
  short value;
  get_param_int16(msg, 0, &value);

  if (_currentMenu != 0)
    {
    // close the popup and open the parent if any
    CloseMenu(wnd);
    changed = true;
    }
  else
    {
    }

  if (changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

static result_t on_menu_right(Widget widget, event_proxy_t *proxy, CanFlyMsg msg)
  {
  bool changed = false;
  LayoutWindow wnd = (LayoutWindow )proxy.parg;
  Rect wnd_rect;
  get_window_rect(hwnd, &wnd_rect);

  ushort id = get_can_id(msg);
  short value;
  get_param_int16(msg, 0, &value);

  if (_currentMenu != 0)
    {
    MenuItem item = MenuItemAt(_currentMenu, _currentMenu.SelectedIndex);
    if (item.item_type == mi_menu)
      {
      menu_item_Menu mi = (menu_item_Menu )item;
      ShowMenu(wnd, mi.menu);     // show the popup
      }
    else if (item.item_type == MenuItemType.MiEdit)
      ShowItemEditor(wnd, item);
    changed = true;
    }

  if (changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

static result_t on_ok(Widget widget, event_proxy_t *proxy, CanFlyMsg msg)
  {
  bool changed = false;
  LayoutWindow wnd = (LayoutWindow )proxy.parg;
  Rect wnd_rect;
  get_window_rect(hwnd, &wnd_rect);

  ushort id = get_can_id(msg);
  short value;
  get_param_int16(msg, 0, &value);

  // this is sent when an item is selected.  Only ever one
  if (_currentMenu != 0)
    {
    MenuItem item = MenuItemAt(_currentMenu, _currentMenu.SelectedIndex);
    if (item != 0)
      (*item.evaluate)(wnd, item, msg);
    }

  if (changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

static result_t on_cancel(Widget widget, event_proxy_t *proxy, CanFlyMsg msg)
  {
  LayoutWindow wnd = (LayoutWindow )proxy.parg;

  CloseMenu(wnd);

  return s_ok;
  }

result_t release_layout(LayoutWindow wnd)
  {
  // clear the layout window
  return s_ok;
  }

result_t build_layout(LayoutWindow wnd, uint hive)
  {
  return s_ok;
  }

result_t load_layout(handle_t parent, uint hive)
  {
  // if the window exists then release all resources
  LayoutWindow wnd = 0;
  if (succeeded(get_wnddata(parent, (void **)&wnd)) &&
    wnd != 0)
    {
    release_layout(wnd);
    }
  else
    {
    // first call, create an empty layout
    wnd = (LayoutWindow )neutron_malloc(sizeof(layout_window_t));
    memset(wnd, 0, sizeof(layout_window_t));

    // attach the window to the screen
    window = parent;
    version = sizeof(layout_window_t);
    }

  // the hive must have series of hives that form windows
  result_t result;
  char name[REG_NAME_MAX + 1];
  ushort length = REG_NAME_MAX + 1;
  field_datatype type = field_key;


  uint menu = 0;
  if (succeeded(reg_open_key(hive, "menu", &menu)))
    {
      // this stores a cache of loaded keys.
    map_create_nv(copy_ptr, 0, &key_mappings);
    // this is the undo stack for the menu
    vector_create(sizeof(Menu ), &menu_stack);
    // these are the popup menus
    map_create_nv(copy_ptr, 0, &menus);
    // all menu items are kept here
    vector_create(sizeof(MenuItem ), &menu_items);

    short value;
    if (succeeded(reg_get_int16(menu, "menu-rect-x", &value)))
      menu_rect_x = value;

    if (succeeded(reg_get_int16(menu, "menu-rect-y", &value)))
      menu_rect_y = value;

    if (succeeded(reg_get_int16(menu, "menu-start-x", &value)))
      _menuStartX = value;

    if (succeeded(reg_get_int16(menu, "menu-start-y", &value)))
      _menuStartY = value;
      }

  key = menu;

  uint root_keys_key;

  static string root_keys_s = "root-keys";

  if (succeeded(reg_open_key(menu, root_keys_s, &root_keys_key)))
    {
    FindKeys(wnd, root_keys_s, &_rootKeys);
    _activeKeys = _rootKeys;
    Menuimer = 0;
    }

  if (failed(lookup_color(menu, "bk-color", &_backgroundColor)))
    _backgroundColor = color_black;

  if (failed(lookup_color(menu, "bk-selected", &_selectedBackgroundColor)))
    _selectedBackgroundColor = color_white;

  if(failed(lookup_color(menu, "selected-color", &SelectedColor)))
    SelectedColor = color_magenta;

  if(failed(lookup_color(menu, "text-color", &TextColor)))
    TextColor = color_green;

  memcpy(&_borderPen, &light_gray_pen_3, sizeof(Pen));
  uint pen_key;
  if(succeeded(reg_open_key(menu, "pen", &pen_key)))
    lookup_pen(pen_key, &_borderPen);

  // check for the font
  if (failed(lookup_font(menu, "font",  &font)))
    {
    // we always have the neo font.
    if(failed(result = open_font("neo", 9, &font)))
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
  uint child = 0;

  while (succeeded(result = reg_enum_key(hive, &type, 0, 0, REG_NAME_MAX, name, &child)))
    {
    char widget_type[REG_NAME_MAX];
    if (failed(reg_get_string(child, "type", widget_type, 0)))
      continue;

    Widget widget;
    // TODO: these will go
    if(strcmp(widget_type, "airspeed")== 0)
      create_airspeed_window(parent, child, &hwnd);
    else if(strcmp(widget_type, "hsi")== 0)
      create_hsi_window(parent, child, &hwnd);
    else if(strcmp(widget_type, "attitude")== 0)
      create_attitude_window(parent, child, &hwnd);
    else if(strcmp(widget_type, "altitude")== 0)
      create_altitude_window(parent, child, &hwnd);
    else if(strcmp(widget_type, "gauge")== 0)
      create_gauge_window(parent, child, &hwnd);
    else if(strcmp(widget_type, "annunciator")== 0)
      create_annunciator_window(parent, child, &hwnd);
    else if(strcmp(widget_type, "gps")== 0)
      create_gps_window(parent, child, &hwnd);
    else
      create_widget(parent, child, &hwnd);

    // field_datatype has the field type, name is the child name
    type = field_key;
    }

  // store the parameters for the window
  set_wnddata(parent, wnd);

  Rect rect;
  get_window_rect(parent, &rect);
  invalidate_rect(parent, &rect);

  return s_ok;
  }
*/