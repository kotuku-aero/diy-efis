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
*/
using System;
using System.Collections;

namespace CanFly.Proton
{
  public sealed class LayoutWidget : Widget
  {
    //
    // This holds the global root menu for the system
    //
    private Keys _rootKeys;
    private short menuTimeout = 25;             // 5-second timeout

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

    private short _menuRectX;
    private short _menuRectY;
    private short _menuStartX;
    private short _menuStartY;

    private ushort _key; // window key

    // map of root _menus
    private Hashtable _keyMappings;

    // map of _menus
    private Hashtable _menus;

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

    /// <summary>
    /// This is called by the runtime.  It cannot be called directly
    /// </summary>
    /// <param name="parent"></param>
    public LayoutWidget(Widget parent, ushort orientation, ushort id, ushort hive)
      : base(parent, parent.WindowRect, id)
    {
      _key = hive;

      // load the layout....
      // must be 0 on first call
      ushort child = 0;
      ushort nextDefaultId = 0x8000;
      string name;
      while (TryRegEnumKey(hive, ref child, out name))
      {
        string widgetType;
        if (!TryRegGetString(child, "type", out widgetType))
          continue;

        ushort widgetId;

        if (!TryRegGetUint16(child, "id", out widgetId))
          widgetId = nextDefaultId++;

        Rect bounds;

        if (!TryRegGetRect(child, out bounds))
          bounds = new Rect(WindowRect.TopLeft, new Extent(0, 0));

        Widget widget = CreateWidget(parent, child, widgetType, bounds, widgetId);
      }

      // the hive must have series of hives that form windows

      ushort menu;
      if (TryRegOpenKey(hive, "menu", out menu))
      {
        // this stores a cache of loaded keys.
        _keyMappings = new Hashtable();
        _menuStack = new ArrayList();
        _menus = new Hashtable();
        _menuItems = new ArrayList();

        if (!TryRegGetInt16(menu, "menu-rect-x", out _menuRectX))
          _menuRectX = 0;

        if (!TryRegGetInt16(menu, "menu-rect-y", out _menuRectY))
          _menuRectY = (short) WindowRect.Bottom;

        if (!TryRegGetInt16(menu, "menu-start-x", out _menuStartX))
          _menuStartX = 0;

        if (!TryRegGetInt16(menu, "menu-start-y", out _menuStartY))
          _menuStartY = (short)WindowRect.Bottom;

        if (FindKeys(menu, "root-keys", out _activeKeys))
          _menuTimer = 0;


        if (LookupColor(menu, "bk-color", out _backgroundColor))
          _backgroundColor = Colors.Black;

        if (LookupColor(menu, "bk-selected", out _selectedBackgroundColor))
          _selectedBackgroundColor = Colors.White;

        if (LookupColor(menu, "selected-color", out _selectedColor))
          _selectedColor = Colors.Magenta;

        if (LookupColor(menu, "text-color", out _textColor))
          _textColor = Colors.Green;

        if (!LookupPen(menu, "pen", out _borderPen))
          _borderPen = Pens.LightGrayPen;

        // check for the font
        if (!LookupFont(menu, "font", out _font))
          OpenFont("neo", 9, out _font);
      }

      Screen.Instance.AfterPaint += Instance_AfterPaint;
      AddEventListener(PhotonID.id_key0, OnKey0);
      AddEventListener(PhotonID.id_key1, OnKey1);
      AddEventListener(PhotonID.id_key2, OnKey2);
      AddEventListener(PhotonID.id_key3, OnKey3);
      AddEventListener(PhotonID.id_key4, OnKey4);
      AddEventListener(PhotonID.id_key5, OnKey5);
      AddEventListener(PhotonID.id_key6, OnKey6);
      AddEventListener(PhotonID.id_key7, OnKey7);
      AddEventListener(PhotonID.id_decka, OnDeckA);
      AddEventListener(PhotonID.id_deckb, OnDeckB);
      AddEventListener(PhotonID.id_menu_left, OnMenuLeft);
      AddEventListener(PhotonID.id_menu_right, OnMenuRight);
      AddEventListener(PhotonID.id_menu_up, OnMenuUp);
      AddEventListener(PhotonID.id_menu_dn, OnMenuDown);
      AddEventListener(PhotonID.id_menu_cancel, OnMenuCancel);
      AddEventListener(PhotonID.id_menu_ok, OnMenuOk);
    }

    private bool TryRegEnumKey(ushort key, ref ushort child, out string name)
    {
      name = null;
      try
      {
        Syscall.RegEnumKey(key, ref child, out name);
      }
      catch
      {
        return false;
      }

      return true;
    }

    public delegate Widget CreateCustomWidget(Widget parent, ushort hive, string widgetType, Rect bounds, ushort id);

    private static CreateCustomWidget customWidgetConstructor = null;

    public void SetCustomWidgetConstructor(CreateCustomWidget creator)
    {
      customWidgetConstructor = creator;
    }

    private Widget CreateWidget(Widget parent, ushort hive, string widgetType, Rect bounds, ushort id)
    {
      if (customWidgetConstructor != null)
      {
        Widget result = customWidgetConstructor(parent, hive, widgetType, bounds, id);
        if (result != null)
          return null;
      }

      switch (widgetType)
      {
        case "airspeed":
          return new AirspeedWidget(parent, bounds, id, hive);
        case "hsi":
          return new HSIWidget(parent, bounds, id, hive);
        case "attitude":
          return new AttitudeWidget(parent, bounds, id, hive);
        case "altitude":
          return new AltitudeWidget(parent, bounds, id, hive);
        case "gauge":
          return new GaugeWidget(parent, bounds, id, hive);
        case "annunciator":
          return new AnnunciatorWidget(parent, bounds, id, hive);
        case "gps":
          return new GpsWidget(parent, bounds, id, hive);
      }

      return null;
    }

    private bool FindKeys(ushort menu, string keys_name, out Keys activeKeys)
    {
      activeKeys = null;
      ushort hive;
      if(TryRegOpenKey(menu, keys_name, out hive))
      {
        // open the key
        LoadFromRegistry(keys_name, hive);
        return true;
      }

      return false;
    }

    private void OnMenuOk(CanFlyMsg msg)
    {
      // this is sent when an item is selected.  Only ever one
      if (_currentMenu != null)
      {
        MenuItem item = _currentMenu[_currentMenu.SelectedIndex];
        if (item != null)
          item.Evaluate(msg);
      }
    }

    private void OnMenuCancel(CanFlyMsg msg)
    {
      CloseMenu();
    }

    private void OnMenuDown(CanFlyMsg msg)
    {
      if (_currentMenu != null && _currentMenu.SelectedIndex > 0)
      {
        _currentMenu.SelectedIndex--;
        InvalidateRect();
      }
    }

    private void OnMenuUp(CanFlyMsg msg)
    {
      if (_currentMenu != null && _currentMenu.SelectedIndex < _currentMenu.MenuItems.Count - 1)
      {
        _currentMenu.SelectedIndex++;
        InvalidateRect();
      }
    }

    private void OnMenuLeft(CanFlyMsg msg)
    {
      if (_currentMenu != null)
      {
        CloseMenu();
        InvalidateRect();
      }
    }

    private void OnMenuRight(CanFlyMsg msg)
    {
      if (_currentMenu != null)
      {
        MenuItem item = _currentMenu[_currentMenu.SelectedIndex];
        if (item is MenuItemMenu)
        {
          MenuItemMenu mi = item as MenuItemMenu;
          ShowMenu(mi.Menu);
        }
        else if (item is MenuItemEdit)
        {
          ShowItemEditor(item);
        }

        InvalidateRect();
      }
    }

    private void OnDeckB(CanFlyMsg msg)
    {
      Keys handler = _activeKeys ?? _rootKeys;
      short value = msg.GetInt16();

      if (value > 0)
      {
        if (handler.DeckbUp != null && handler.DeckbUp.Enabled(msg))
        {
          if (_menuTimer != 0)
            _menuTimer = menuTimeout;

          handler.DeckbUp.Evaluate(msg);
          InvalidateRect();
        }
      }
      else if (value < 0)
      {
        if (handler.DeckbDn != null && handler.DeckbDn.Enabled(msg))
        {
          if (_menuTimer != 0)
            _menuTimer = menuTimeout;

          handler.DeckbDn.Evaluate(msg);
          InvalidateRect();
        }
      }
    }

    private void OnDeckA(CanFlyMsg msg)
    {
      Keys handler = _activeKeys ?? _rootKeys;
      short value = msg.GetInt16();

      if (value > 0)
      {
        if (handler.DeckaUp != null && handler.DeckaUp.Enabled(msg))
        {
          if (_menuTimer != 0)
            _menuTimer = menuTimeout;

          handler.DeckaUp.Evaluate(msg);
          InvalidateRect();
        }
      }
      else if (value < 0)
      {
        if (handler.DeckaDn != null && handler.DeckaDn.Enabled(msg))
        {
          if (_menuTimer != 0)
            _menuTimer = menuTimeout;

          handler.DeckaDn.Evaluate(msg);
          InvalidateRect();
        }
      }
    }

    private void OnKey7(CanFlyMsg msg)
    {
      HandleKey(msg, _activeKeys?.Key7);
    }

    private void OnKey6(CanFlyMsg msg)
    {
      HandleKey(msg, _activeKeys?.Key6);
    }

    private void OnKey5(CanFlyMsg msg)
    {
      HandleKey(msg, _activeKeys?.Key5);
    }

    private void OnKey4(CanFlyMsg msg)
    {
      HandleKey(msg, _activeKeys?.Key4);
    }

    private void OnKey3(CanFlyMsg msg)
    {
      HandleKey(msg, _activeKeys?.Key3);
    }

    private void OnKey2(CanFlyMsg msg)
    {
      HandleKey(msg, _activeKeys?.Key2);
    }


    private void OnKey1(CanFlyMsg msg)
    {
      HandleKey(msg, _activeKeys?.Key1);
    }

    private void OnKey0(CanFlyMsg msg)
    {
      HandleKey(msg, _activeKeys?.Key0);
    }

    private void HandleKey(CanFlyMsg msg, MenuItem key)
    {
      short value = msg.GetInt16();
      if (key != null)
      {
        if (value > 0 && key.Enabled(msg))
        {
          if (_menuTimer != 0)
            key.Evaluate(msg);

          _menuTimer = menuTimeout;
        }
      }
      else
      {
        _activeKeys = _rootKeys;
        _menuTimer = 0;
      }

      InvalidateRect();
    }

    private void Instance_AfterPaint(CanFlyMsg msg)
    {
      // handle the layout widget has painted, display the menu if
      // it is active

      if (_menuItems != null)
      {
        // we go through all of the menu items and check to see if they are
        // listening, and if their state has changed
        foreach (MenuItem mi in _menuItems)
        {
          // update the controlling variable if it is being watched.
          mi.Evaluate(msg);
        }
      }

      // finally update the menu if it is there so it draws on top
      UpdateWindow();
    }

    public Pen BorderPen
    {
      get { return _borderPen; }
      set { _borderPen = value; }
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
    /// <param name="expr">expression in the form <param>:<expression></param>
    private void item_assign_enabler(MenuItem item, string expr)
    {

    }

    /// <summary>
    /// Close the current popup menu and restore the previous menu
    /// </summary>
    private void PopMenu()
    {
      if (_menuStack.Count == 0)
      {
        CloseMenu();      // all done
        return;
      }

      Menu prevMenu = (Menu)_menuStack[_menuStack.Count - 1];
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
      if (item.ItemType == MenuItemType.MiEdit)
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

    private CanFlyMsg LoadCanMessage(ushort key)
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
                result = new CanFlyMsg(id, (float)Convert.ToDouble(value));
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
                result = new CanFlyMsg(id, (byte)DecodeHex(value));
                break;
              case "CHAR2":
                values = value.Split(',');
                result = new CanFlyMsg(id,
                  (char)(values.Length > 0 ? DecodeHex(values[0]) : 0),
                  (char)(values.Length > 1 ? DecodeHex(values[1]) : 0));
                break;
              case "UCHAR2":
                values = value.Split(',');
                result = new CanFlyMsg(id,
                  (byte)(values.Length > 0 ? DecodeHex(values[0]) : 0),
                  (byte)(values.Length > 1 ? DecodeHex(values[1]) : 0));
                break;
              case "CHAR3":
                values = value.Split(',');
                result = new CanFlyMsg(id,
                  (char)(values.Length > 0 ? DecodeHex(values[0]) : 0),
                  (char)(values.Length > 1 ? DecodeHex(values[1]) : 0),
                  (char)(values.Length > 2 ? DecodeHex(values[2]) : 0));
                break;
              case "UCHAR3":
                values = value.Split(',');
                result = new CanFlyMsg(id,
                  (byte)(values.Length > 0 ? DecodeHex(values[0]) : 0),
                  (byte)(values.Length > 1 ? DecodeHex(values[1]) : 0),
                  (byte)(values.Length > 2 ? DecodeHex(values[2]) : 0));
                break;
              case "CHAR4":
                values = value.Split(',');
                result = new CanFlyMsg(id,
                  (char)(values.Length > 0 ? DecodeHex(values[0]) : 0),
                  (char)(values.Length > 1 ? DecodeHex(values[1]) : 0),
                  (char)(values.Length > 2 ? DecodeHex(values[2]) : 0),
                  (char)(values.Length > 3 ? DecodeHex(values[3]) : 0));
                break;
              case "UCHAR4":
                values = value.Split(',');
                result = new CanFlyMsg(id,
                  (byte)(values.Length > 0 ? DecodeHex(values[0]) : 0),
                  (byte)(values.Length > 1 ? DecodeHex(values[1]) : 0),
                  (byte)(values.Length > 2 ? DecodeHex(values[2]) : 0),
                  (byte)(values.Length > 3 ? DecodeHex(values[3]) : 0));
                break;
              case "SHORT2":
                values = value.Split(',');
                result = new CanFlyMsg(id,
                  (short)(values.Length > 0 ? DecodeHex(values[0]) : 0),
                  (short)(values.Length > 1 ? DecodeHex(values[1]) : 0));
                break;
              case "USHORT2":
                values = value.Split(',');
                result = new CanFlyMsg(id,
                  (ushort)(values.Length > 0 ? DecodeHex(values[0]) : 0),
                  (ushort)(values.Length > 1 ? DecodeHex(values[1]) : 0));
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
          MenuItem item = (MenuItem)_currentMenu.MenuItems[index];
          item.Paint(new Rect(menuPt.X, menuPt.Y, menuPt.X + itemExtents.Dx, menuPt.Y + itemExtents.Dy),
            index == _currentMenu.SelectedIndex);

          // now ask the item to draw if it is selected
          if (item.Selected)
            item.Paint(new Rect(menuPt.X, menuPt.Y, menuPt.X + 80, menuPt.Y + 20), true);


          // skip up one.
          menuPt.Y -= itemHeight;
        }
      }
    }

    protected override void OnPaint(CanFlyMsg msg)
    {
    }

    private void DefaultMsgHandler(MenuItem menuItem, CanFlyMsg msg)
    {
      if (menuItem.ControllingParam == msg.CanID)
        menuItem.ControllingVariable = msg;
    }

    private void LoadItemDefaults(ushort key, MenuItem menuItem)
    {
      menuItem.EventHandler(DefaultMsgHandler);

      string strValue;
      ushort ushortValue;
      TryRegGetString(key, "caption", out strValue);
      menuItem.Caption = strValue;

      ushort enable_key;
      if (TryRegOpenKey(key, "enable", out enable_key))
      {
        if (TryRegGetUint16(enable_key, "id", out ushortValue))
          menuItem.ControllingParam = ushortValue;

        if (TryRegGetString(enable_key, "regex", out strValue))
          menuItem.EnableRegex = strValue;

        if (TryRegGetString(enable_key, "format", out strValue))
          menuItem.EnableFormat = strValue;
      }
    }

    private MenuItem MenuItemAt(Menu menu, ushort index)
    {
      /*
static MenuItem MenuItemAt(Menu menu, ushort index)
  {
  MenuItem mi;
  if (failed(vector_at(menu._menuItems, index, &mi)))
    return 0;

  return mi;
  }
       */
      return null;
    }

    private int MenuCount(Menu menu)
    {
      return menu.MenuItems.Count;
    }

    internal MenuItem ParseItem(ushort key)
    {
      string itemType;
      if (!TryRegGetString(key, "type", out itemType))
        return null;

      switch(itemType)
      {
        case "menu":
          return LoadMenuItem(key);
        case "cancel":
          return LoadMenuCancel(key);
        case "enter":
          return LoadMenuEnter(key);
        case "event":
          return LoadMenuEvent(key);
        case "edit":
          return LoadMenuEdit(key);
        case "checklist":
          return LoadMenuChecklist(key);
        case "popup":
          return LoadMenuPopup(key);
      }

      return null;
    }

    internal MenuItem LoadMenuPopup(ushort key)
    {
      MenuItemPopup result = new MenuItemPopup(this, key);
      LoadItemDefaults(key, result);

      string menuName;
      if (TryRegGetString(key, "popup", out menuName))
        result.PopupMenu = FindMenu(menuName);

      return result;
    }

    internal MenuItem LoadMenuChecklist(ushort key)
    {
      MenuItemChecklist result = new MenuItemChecklist(this, key);
      LoadItemDefaults(key, result);

      return result;
    }

    internal MenuItem LoadMenuEdit(ushort key)
    {
      MenuItemEdit result = new MenuItemEdit(this, key);
      LoadItemDefaults(key, result);

      return result;
    }

    internal MenuItem LoadMenuEvent(ushort key)
    {
      MenuItemEvent result = new MenuItemEvent(this, key);
      LoadItemDefaults(key, result);

      return result;
    }

    internal MenuItem LoadMenuEnter(ushort key)
    {
      MenuItemEnter result = new MenuItemEnter(this, key);
      LoadItemDefaults(key, result);

      return result;
    }

    internal MenuItem LoadMenuCancel(ushort key)
    {
      MenuItemCancel result = new MenuItemCancel(this, key);
      LoadItemDefaults(key, result);

      return result;
    }

    internal MenuItem LoadMenuItem(ushort key)
    {
      MenuItemItem result = new MenuItemItem(this, key);
      LoadItemDefaults(key, result);

      return result;
    }

    private Keys LoadFromRegistry(string name, ushort keys_key)
    {
      Keys theKeys = new Keys();
      _keyMappings.Add(name, theKeys);


      // must be done before we load any items so if they recurse we don't get
      // created more than once!

      ushort child;
      if (TryRegOpenKey(keys_key, "key0", out child))
        theKeys.Key0 = ParseItem(child);

      if (TryRegOpenKey(keys_key, "key1", out child))
        theKeys.Key1 = ParseItem(child);

      if (TryRegOpenKey(keys_key, "key2", out child))
        theKeys.Key2 = ParseItem(child);

      if (TryRegOpenKey(keys_key, "key3", out child))
        theKeys.Key3 = ParseItem(child);

      if (TryRegOpenKey(keys_key, "key4", out child))
        theKeys.Key4 = ParseItem(child);

      if (TryRegOpenKey(keys_key, "key5", out child))
        theKeys.Key5 = ParseItem(child);

      if (TryRegOpenKey(keys_key, "key6", out child))
        theKeys.Key6 = ParseItem(child);

      if (TryRegOpenKey(keys_key, "key7", out child))
        theKeys.Key7 = ParseItem(child);

      if (TryRegOpenKey(keys_key, "decka-up", out child))
        theKeys.DeckaUp = ParseItem(child);

      if (TryRegOpenKey(keys_key, "decka-dn", out child))
        theKeys.DeckaDn = ParseItem(child);

      if (TryRegOpenKey(keys_key, "deckb-up", out child))
        theKeys.DeckbUp = ParseItem(child);

      if (TryRegOpenKey(keys_key, "deckb-dn", out child))
        theKeys.DeckbDn = ParseItem(child);

      return theKeys;
    }

    private Keys LoadKeys(string name)
    {
      Keys theKeys = null;
      if (_keyMappings.Contains(name))
        theKeys = (Keys)_keyMappings[name];
      else
      {
        // see if the key is found
        ushort keys_key;
        if (!TryRegOpenKey(_key, name, out keys_key))
          return null;

        theKeys = LoadFromRegistry(name, keys_key);
      }

      return theKeys;
    }

    private Menu FindMenu(string name)
    {
      Menu popup;

      if (_menus.Contains(name))
        popup = (Menu)_menus[name];
      else
      {
        ushort key;
        if (!TryRegOpenKey(_key, name, out key))
          return null;

        popup = new Menu();

        _menus.Add(name, popup);

        ushort child = 0;
        string itemName;
        while (TryRegEnumKey(key, ref child, out itemName))
        {
          // the protocol assumes all child keys are menu items
          MenuItem item = ParseItem(child);
          if (item != null)
            popup.MenuItems.Add(item);
        }

        // we now load the keys
        string keys;
        if (TryRegGetString(key, "keys", out keys))
          popup.Keys = LoadKeys(keys);
      }

      return popup;
    }
  }
}
