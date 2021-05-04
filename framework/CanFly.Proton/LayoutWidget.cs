/*
diy-efis
Copyright (C) 2021 Kotuku Aerospace Limited

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

If a file does not contain a copyright header, either because it is incomplete
or a binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice is
subservient to that copyright notice.

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
  /// <summary>
  /// A layout widget will load a screen design from the rgistry and
  /// also load a menu to associated with the layout
  /// </summary>
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

    private Color _backgroundColor;
    private Color _selectedBackgroundColor;
    private Color _textColor; // text color
    private Color _borderColor;
    private Color _selectedColor;
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
          bounds = Rect.Create(WindowRect.TopLeft, Extent.Create(0, 0));

        // this window lays out the windows, but the screen owns the children.
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
          MenuRectX = 0;

        if (!TryRegGetInt16(menu, "menu-rect-y", out _menuRectY))
          MenuRectY = (short) WindowRect.Bottom;

        if (!TryRegGetInt16(menu, "menu-start-x", out _menuStartX))
          MenuStartX = 0;

        if (!TryRegGetInt16(menu, "menu-start-y", out _menuStartY))
          MenuStartY = (short)WindowRect.Bottom;

        string rootKeysName;
        if(TryRegGetString(menu, "root-keys", out rootKeysName))
        {
          _rootKeys = LoadKeys(rootKeysName);
          _activeKeys = _rootKeys;
        }

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
      AddCanFlyEvent(PhotonID.id_key0, OnKey0);
      AddCanFlyEvent(PhotonID.id_key1, OnKey1);
      AddCanFlyEvent(PhotonID.id_key2, OnKey2);
      AddCanFlyEvent(PhotonID.id_key3, OnKey3);
      AddCanFlyEvent(PhotonID.id_key4, OnKey4);
      AddCanFlyEvent(PhotonID.id_key5, OnKey5);
      AddCanFlyEvent(PhotonID.id_key6, OnKey6);
      AddCanFlyEvent(PhotonID.id_key7, OnKey7);
      AddCanFlyEvent(PhotonID.id_decka, OnDeckA);
      AddCanFlyEvent(PhotonID.id_deckb, OnDeckB);
      AddCanFlyEvent(PhotonID.id_menu_left, OnMenuLeft);
      AddCanFlyEvent(PhotonID.id_menu_right, OnMenuRight);
      AddCanFlyEvent(PhotonID.id_menu_up, OnMenuUp);
      AddCanFlyEvent(PhotonID.id_menu_dn, OnMenuDown);
      AddCanFlyEvent(PhotonID.id_menu_cancel, OnMenuCancel);
      AddCanFlyEvent(PhotonID.id_menu_ok, OnMenuOk);
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

    private void OnMenuOk(CanFlyMsg msg)
    {
      // this is sent when an item is selected.  Only ever one
      if (CurrentMenu != null)
      {
        MenuItem item = CurrentMenu[CurrentMenu.SelectedIndex];
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
      if (CurrentMenu != null && CurrentMenu.SelectedIndex > 0)
      {
        CurrentMenu.SelectedIndex--;
        InvalidateRect();
      }
    }

    private void OnMenuUp(CanFlyMsg msg)
    {
      if (CurrentMenu != null && CurrentMenu.SelectedIndex < CurrentMenu.MenuItems.Count - 1)
      {
        CurrentMenu.SelectedIndex++;
        InvalidateRect();
      }
    }

    private void OnMenuLeft(CanFlyMsg msg)
    {
      if (CurrentMenu != null)
      {
        CloseMenu();
        InvalidateRect();
      }
    }

    private void OnMenuRight(CanFlyMsg msg)
    {
      if (CurrentMenu != null)
      {
        MenuItem item = CurrentMenu[CurrentMenu.SelectedIndex];
        if (item is MenuItemPopup)
        {
          MenuItemPopup mi = item as MenuItemPopup;
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
      Keys handler = ActiveKeys ?? RootKeys;
      short value = msg.GetInt16();

      if (value > 0)
      {
        if (handler.DeckbUp != null && handler.DeckbUp.Enabled(msg))
        {
          if (_menuTimer != 0)
            _menuTimer = MenuTimeout;

          handler.DeckbUp.Evaluate(msg);
          InvalidateRect();
        }
      }
      else if (value < 0)
      {
        if (handler.DeckbDn != null && handler.DeckbDn.Enabled(msg))
        {
          if (_menuTimer != 0)
            _menuTimer = MenuTimeout;

          handler.DeckbDn.Evaluate(msg);
          InvalidateRect();
        }
      }
    }

    private void OnDeckA(CanFlyMsg msg)
    {
      Keys handler = ActiveKeys ?? RootKeys;
      short value = msg.GetInt16();

      if (value > 0)
      {
        if (handler.DeckaUp != null && handler.DeckaUp.Enabled(msg))
        {
          if (_menuTimer != 0)
            _menuTimer = MenuTimeout;

          handler.DeckaUp.Evaluate(msg);
          InvalidateRect();
        }
      }
      else if (value < 0)
      {
        if (handler.DeckaDn != null && handler.DeckaDn.Enabled(msg))
        {
          if (_menuTimer != 0)
            _menuTimer = MenuTimeout;

          handler.DeckaDn.Evaluate(msg);
          InvalidateRect();
        }
      }
    }

    private void OnKey7(CanFlyMsg msg)
    {
      HandleKey(msg, ActiveKeys == null ? null : ActiveKeys.Key7);
    }

    private void OnKey6(CanFlyMsg msg)
    {
      HandleKey(msg, ActiveKeys == null ? null : ActiveKeys.Key6);
    }

    private void OnKey5(CanFlyMsg msg)
    {
      HandleKey(msg, ActiveKeys == null ? null : ActiveKeys.Key5);
    }

    private void OnKey4(CanFlyMsg msg)
    {
      HandleKey(msg, ActiveKeys == null ? null : ActiveKeys.Key4);
    }

    private void OnKey3(CanFlyMsg msg)
    {
      HandleKey(msg, ActiveKeys == null ? null : ActiveKeys.Key3);
    }

    private void OnKey2(CanFlyMsg msg)
    {
      HandleKey(msg, ActiveKeys == null ? null : ActiveKeys.Key2);
    }


    private void OnKey1(CanFlyMsg msg)
    {
      HandleKey(msg, ActiveKeys == null ? null : ActiveKeys.Key1);
    }

    private void OnKey0(CanFlyMsg msg)
    {
      HandleKey(msg, ActiveKeys == null ? null : ActiveKeys.Key0);
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

          _menuTimer = MenuTimeout;
        }
      }
      else
      {
        _activeKeys = RootKeys;
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

    public Color BorderColor
    {
      get { return _borderColor; }
      set { _borderColor = value; }
    }

    public Color BackgroundColor
    {
      get { return _backgroundColor; }
      set { _backgroundColor = value; }
    }

    public Color SelectedBackgroundColor
    {
      get { return _selectedBackgroundColor; }
      set { _selectedBackgroundColor = value; }
    }

    public Color TextColor
    {
      get { return _textColor; }
      set { _textColor = value; }
    }

    public Color SelectedColor
    {
      get { return _selectedColor; }
      set { _selectedColor = value; }
    }

    public Font Font
    {
      get { return _font; }
      set { _font = value; }
    }

    public Keys RootKeys
    {
      get { return _rootKeys; }
      set { _rootKeys = value; }
    }
    public short MenuTimeout 
    {
       get { return menuTimeout; } 
    }
    public Keys ActiveKeys
    {
      get { return _activeKeys; }
    }

    public Menu CurrentMenu
    {
      get { return _currentMenu; }
    }

    public short MenuRectX
    {
      get { return _menuRectX; }
      set { _menuRectX = value; }
    }
    public short MenuRectY
    {
      get { return _menuRectY; }
      set { _menuRectY = value; }
    }
    public short MenuStartX
    {
      get { return _menuStartX; }
      set { _menuStartX = value; }
    }
    public short MenuStartY
    {
      get { return _menuStartY; }
      set { _menuStartY = value; }
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

    /// <summary>
    /// Close all menu's.  Called by the cancel: menu item
    /// </summary>
    private void CloseMenu()
    {
      if (CurrentMenu != null)
        CurrentMenu.SelectedIndex = 0;

      _activeKeys = null;
      _currentMenu = null;
      _menuTimer = 0;

      // assign the root keys as none can be active
      _activeKeys = RootKeys;
    }
    /// <summary>
    /// Show a popup menu at the left of the menu window.
    /// </summary>
    /// <param name="menu">menu to display</param>
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
      item.Edit(null);
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

    private void UpdateWindow()
    {
      Rect wndRect = WindowRect;

      Extent ex = wndRect.Extent;

      if (CurrentMenu != null)
      {
        // draw the root menu
        Point menuPt = Point.Create(MenuStartX, MenuStartY);
        int itemWidth = wndRect.Width / 3;
        int itemHeight = 20;

        Extent itemExtents = Extent.Create(itemWidth, itemHeight);

        // see if we are displaying a popup menu
        menuPt = menuPt.Add(0, -itemHeight);

        // determine how menu items to draw from the item
        int itemsAvail = MenuStartY / itemHeight;

        int numItems = CurrentMenu.MenuItems.Count;
        int drawingItem = 0;
        int index = 0;

        if (numItems > itemsAvail)
          index = CurrentMenu.SelectedIndex;  // start drawing at the selected index

        // draw the visible items
        for (; index < numItems && drawingItem < itemsAvail; drawingItem++, index++)
        {
          MenuItem item = (MenuItem)CurrentMenu.MenuItems[index];
          item.Paint(Rect.Create(menuPt.X, menuPt.Y, menuPt.X + itemExtents.Dx, menuPt.Y + itemExtents.Dy),
            index == CurrentMenu.SelectedIndex);

          // now ask the item to draw if it is selected
          if (item.Selected)
            item.Paint(Rect.Create(menuPt.X, menuPt.Y, menuPt.X + 80, menuPt.Y + 20), true);


          // skip up one.
          menuPt = menuPt.Add(0, -itemHeight);
        }
      }
    }

    protected override void OnPaint(CanFlyMsg msg)
    {
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

    /// <summary>
    /// Load a set of keys, if it exists then returned cached version
    /// </summary>
    /// <param name="name">Key mapping to load</param>
    /// <returns>Set of keys</returns>
    public Keys LoadKeys(string name)
    {
      Keys theKeys = null;
      if (_keyMappings.Contains(name))
        theKeys = (Keys)_keyMappings[name];
      else
      {
        // see if the key is found
        ushort key;
        if (!TryRegOpenKey(_key, name, out key))
          return null;

        theKeys = Keys.Parse(this, key);
      
        _keyMappings.Add(name, theKeys);
      }

      return theKeys;
    }

    /// <summary>
    /// Load a menu, by name.
    /// </summary>
    /// <param name="name">Menu name</param>
    /// <returns>Locaded or cached menu</returns>
    public Menu LoadMenu(string name)
    {
      Menu popup;

      if (_menus.Contains(name))
        popup = (Menu)_menus[name];
      else
      {
        ushort key;
        if (!TryRegOpenKey(_key, name, out key))
          return null;

        popup = new Menu(this, key);

        // store the cached menu
        _menus.Add(name, popup);
      }

      return popup;
    }
  }
}
